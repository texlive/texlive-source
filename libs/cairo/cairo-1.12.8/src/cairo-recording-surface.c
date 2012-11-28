/* -*- Mode: c; tab-width: 8; c-basic-offset: 4; indent-tabs-mode: t; -*- */
/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2005 Red Hat, Inc
 * Copyright © 2007 Adrian Johnson
 *
 * This library is free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 *
 * The Original Code is the cairo graphics library.
 *
 * The Initial Developer of the Original Code is Red Hat, Inc.
 *
 * Contributor(s):
 *	Kristian Høgsberg <krh@redhat.com>
 *	Carl Worth <cworth@cworth.org>
 *	Adrian Johnson <ajohnson@redneon.com>
 */

/**
 * SECTION:cairo-recording
 * @Title: Recording Surfaces
 * @Short_Description: Records all drawing operations
 * @See_Also: #cairo_surface_t
 *
 * A recording surface is a surface that records all drawing operations at
 * the highest level of the surface backend interface, (that is, the
 * level of paint, mask, stroke, fill, and show_text_glyphs). The recording
 * surface can then be "replayed" against any target surface by using it
 * as a source surface.
 *
 * If you want to replay a surface so that the results in target will be
 * identical to the results that would have been obtained if the original
 * operations applied to the recording surface had instead been applied to the
 * target surface, you can use code like this:
 * <informalexample><programlisting>
 * cairo_t *cr;
 *
 * cr = cairo_create (target);
 * cairo_set_source_surface (cr, recording_surface, 0.0, 0.0);
 * cairo_paint (cr);
 * cairo_destroy (cr);
 * </programlisting></informalexample>
 *
 * A recording surface is logically unbounded, i.e. it has no implicit constraint
 * on the size of the drawing surface. However, in practice this is rarely
 * useful as you wish to replay against a particular target surface with
 * known bounds. For this case, it is more efficient to specify the target
 * extents to the recording surface upon creation.
 *
 * The recording phase of the recording surface is careful to snapshot all
 * necessary objects (paths, patterns, etc.), in order to achieve
 * accurate replay. The efficiency of the recording surface could be
 * improved by improving the implementation of snapshot for the
 * various objects. For example, it would be nice to have a
 * copy-on-write implementation for _cairo_surface_snapshot.
 **/

#include "cairoint.h"

#include "cairo-array-private.h"
#include "cairo-analysis-surface-private.h"
#include "cairo-clip-private.h"
#include "cairo-combsort-inline.h"
#include "cairo-composite-rectangles-private.h"
#include "cairo-default-context-private.h"
#include "cairo-error-private.h"
#include "cairo-image-surface-private.h"
#include "cairo-recording-surface-inline.h"
#include "cairo-surface-wrapper-private.h"
#include "cairo-traps-private.h"

typedef enum {
    CAIRO_RECORDING_REPLAY,
    CAIRO_RECORDING_CREATE_REGIONS
} cairo_recording_replay_type_t;

static const cairo_surface_backend_t cairo_recording_surface_backend;

/**
 * CAIRO_HAS_RECORDING_SURFACE:
 *
 * Defined if the recording surface backend is available.
 * The recording surface backend is always built in.
 * This macro was added for completeness in cairo 1.10.
 *
 * Since: 1.10
 **/

/* Currently all recording surfaces do have a size which should be passed
 * in as the maximum size of any target surface against which the
 * recording-surface will ever be replayed.
 *
 * XXX: The naming of "pixels" in the size here is a misnomer. It's
 * actually a size in whatever device-space units are desired (again,
 * according to the intended replay target).
 */

static int bbtree_left_or_right (struct bbtree *bbt,
				 const cairo_box_t *box)
{
    int left, right;

    if (bbt->left) {
	cairo_box_t *e = &bbt->left->extents;
	cairo_box_t b;

	b.p1.x = MIN (e->p1.x, box->p1.x);
	b.p1.y = MIN (e->p1.y, box->p1.y);
	b.p2.x = MAX (e->p2.x, box->p2.x);
	b.p2.y = MAX (e->p2.y, box->p2.y);

	left = _cairo_fixed_integer_part (b.p2.x - b.p1.x) * _cairo_fixed_integer_part (b.p2.y - b.p1.y);
	left -= _cairo_fixed_integer_part (e->p2.x - e->p1.x) * _cairo_fixed_integer_part (e->p2.y - e->p1.y);
    } else
	left = 0;

    if (bbt->right) {
	cairo_box_t *e = &bbt->right->extents;
	cairo_box_t b;

	b.p1.x = MIN (e->p1.x, box->p1.x);
	b.p1.y = MIN (e->p1.y, box->p1.y);
	b.p2.x = MAX (e->p2.x, box->p2.x);
	b.p2.y = MAX (e->p2.y, box->p2.y);

	right = _cairo_fixed_integer_part (b.p2.x - b.p1.x) * _cairo_fixed_integer_part (b.p2.y - b.p1.y);
	right -= _cairo_fixed_integer_part (e->p2.x - e->p1.x) * _cairo_fixed_integer_part (e->p2.y - e->p1.y);
    } else
	right = 0;

    return left <= right;
}

#define INVALID_CHAIN ((cairo_command_header_t *)-1)

static struct bbtree *
bbtree_new (const cairo_box_t *box, cairo_command_header_t *chain)
{
    struct bbtree *bbt = malloc (sizeof (*bbt));
    if (bbt == NULL)
	return NULL;
    bbt->extents = *box;
    bbt->left = bbt->right = NULL;
    bbt->chain = chain;
    return bbt;
}

static void
bbtree_init (struct bbtree *bbt, cairo_command_header_t *header)
{
    _cairo_box_from_rectangle (&bbt->extents, &header->extents);
    bbt->chain = header;
}

static cairo_status_t
bbtree_add (struct bbtree *bbt,
	    cairo_command_header_t *header,
	    const cairo_box_t *box)
{
    if (box->p1.x < bbt->extents.p1.x || box->p1.y < bbt->extents.p1.y ||
	box->p2.x > bbt->extents.p2.x || box->p2.y > bbt->extents.p2.y)
    {
	if (bbt->chain) {
	    if (bbtree_left_or_right (bbt, &bbt->extents)) {
		if (bbt->left == NULL) {
		    bbt->left = bbtree_new (&bbt->extents, bbt->chain);
		    if (unlikely (bbt->left == NULL))
			return _cairo_error (CAIRO_STATUS_NO_MEMORY);
		} else
		    bbtree_add (bbt->left, bbt->chain, &bbt->extents);
	    } else {
		if (bbt->right == NULL) {
		    bbt->right = bbtree_new (&bbt->extents, bbt->chain);
		    if (unlikely (bbt->right == NULL))
			return _cairo_error (CAIRO_STATUS_NO_MEMORY);
		} else
		    bbtree_add (bbt->right, bbt->chain, &bbt->extents);
	    }

	    bbt->chain = NULL;
	}

	bbt->extents.p1.x = MIN (bbt->extents.p1.x, box->p1.x);
	bbt->extents.p1.y = MIN (bbt->extents.p1.y, box->p1.y);
	bbt->extents.p2.x = MAX (bbt->extents.p2.x, box->p2.x);
	bbt->extents.p2.y = MAX (bbt->extents.p2.y, box->p2.y);
    }

    if (box->p1.x == bbt->extents.p1.x && box->p1.y == bbt->extents.p1.y &&
	box->p2.x == bbt->extents.p2.x && box->p2.y == bbt->extents.p2.y)
    {
	header->chain = bbt->chain;
	bbt->chain = header;
	return CAIRO_STATUS_SUCCESS;
    }

    if (bbtree_left_or_right (bbt, box)) {
	if (bbt->left == NULL) {
	    bbt->left = bbtree_new (box, header);
	    if (unlikely (bbt->left == NULL))
		return _cairo_error (CAIRO_STATUS_NO_MEMORY);
	} else
	    return bbtree_add (bbt->left, header, box);
    } else {
	if (bbt->right == NULL) {
	    bbt->right = bbtree_new (box, header);
	    if (unlikely (bbt->right == NULL))
		return _cairo_error (CAIRO_STATUS_NO_MEMORY);
	} else
	    return bbtree_add (bbt->right, header, box);
    }

    return CAIRO_STATUS_SUCCESS;
}

static void bbtree_del (struct bbtree *bbt)
{
    if (bbt->left)
	bbtree_del (bbt->left);
    if (bbt->right)
	bbtree_del (bbt->right);

    free (bbt);
}

static cairo_bool_t box_outside (const cairo_box_t *a, const cairo_box_t *b)
{
    return
	a->p1.x >= b->p2.x || a->p1.y >= b->p2.y ||
	a->p2.x <= b->p1.x || a->p2.y <= b->p1.y;
}

static void
bbtree_foreach_mark_visible (struct bbtree *bbt,
			     const cairo_box_t *box,
			     int **indices)
{
    cairo_command_header_t *chain;

    for (chain = bbt->chain; chain; chain = chain->chain)
	*(*indices)++ = chain->index;

    if (bbt->left && ! box_outside (box, &bbt->left->extents))
	bbtree_foreach_mark_visible (bbt->left, box, indices);
    if (bbt->right && ! box_outside (box, &bbt->right->extents))
	bbtree_foreach_mark_visible (bbt->right, box, indices);
}

static inline int intcmp (const int a, const int b)
{
    return a - b;
}
CAIRO_COMBSORT_DECLARE (sort_indices, int, intcmp)

static inline int sizecmp (int a, int b, cairo_command_header_t **elements)
{
    const cairo_rectangle_int_t *r;

    r = &elements[a]->extents;
    a = r->width * r->height;

    r = &elements[b]->extents;
    b = r->width * r->height;

    return b - a;
}
CAIRO_COMBSORT_DECLARE_WITH_DATA (sort_commands, int, sizecmp)

static void
_cairo_recording_surface_destroy_bbtree (cairo_recording_surface_t *surface)
{
    cairo_command_t **elements;
    int i, num_elements;

    if (surface->bbtree.chain == INVALID_CHAIN)
	return;

    if (surface->bbtree.left) {
	bbtree_del (surface->bbtree.left);
	surface->bbtree.left = NULL;
    }
    if (surface->bbtree.right) {
	bbtree_del (surface->bbtree.right);
	surface->bbtree.right = NULL;
    }

    elements = _cairo_array_index (&surface->commands, 0);
    num_elements = surface->commands.num_elements;
    for (i = 0; i < num_elements; i++)
	elements[i]->header.chain = NULL;

    surface->bbtree.chain = INVALID_CHAIN;
}

static cairo_status_t
_cairo_recording_surface_create_bbtree (cairo_recording_surface_t *surface)
{
    cairo_command_t **elements = _cairo_array_index (&surface->commands, 0);
    cairo_status_t status;
    int i, count;
    int *indices;

    count = surface->commands.num_elements;
    if (count > surface->num_indices) {
	free (surface->indices);
	surface->indices = _cairo_malloc_ab (count, sizeof (int));
	if (unlikely (surface->indices == NULL))
	    return _cairo_error (CAIRO_STATUS_NO_MEMORY);

	surface->num_indices = count;
    }

    indices = surface->indices;
    for (i = 0; i < count; i++)
	indices[i] = i;

    sort_commands (indices, count, elements);

    bbtree_init (&surface->bbtree, &elements[indices[0]]->header);
    for (i = 1; i < count; i++) {
	cairo_command_header_t *header = &elements[indices[i]]->header;
	cairo_box_t box;

	_cairo_box_from_rectangle (&box, &header->extents);
	status = bbtree_add (&surface->bbtree, header, &box);
	if (unlikely (status))
	    goto cleanup;
    }

    return CAIRO_STATUS_SUCCESS;

cleanup:
    bbtree_del (&surface->bbtree);
    return status;
}

/**
 * cairo_recording_surface_create:
 * @content: the content of the recording surface
 * @extents: the extents to record in pixels, can be %NULL to record
 *           unbounded operations.
 *
 * Creates a recording-surface which can be used to record all drawing operations
 * at the highest level (that is, the level of paint, mask, stroke, fill
 * and show_text_glyphs). The recording surface can then be "replayed" against
 * any target surface by using it as a source to drawing operations.
 *
 * The recording phase of the recording surface is careful to snapshot all
 * necessary objects (paths, patterns, etc.), in order to achieve
 * accurate replay.
 *
 * Return value: a pointer to the newly created surface. The caller
 * owns the surface and should call cairo_surface_destroy() when done
 * with it.
 *
 * Since: 1.10
 **/
cairo_surface_t *
cairo_recording_surface_create (cairo_content_t		 content,
				const cairo_rectangle_t	*extents)
{
    cairo_recording_surface_t *surface;

    surface = malloc (sizeof (cairo_recording_surface_t));
    if (unlikely (surface == NULL))
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));

    _cairo_surface_init (&surface->base,
			 &cairo_recording_surface_backend,
			 NULL, /* device */
			 content);


    surface->unbounded = TRUE;

    /* unbounded -> 'infinite' extents */
    if (extents != NULL) {
	surface->extents_pixels = *extents;

	/* XXX check for overflow */
	surface->extents.x = floor (extents->x);
	surface->extents.y = floor (extents->y);
	surface->extents.width = ceil (extents->x + extents->width) - surface->extents.x;
	surface->extents.height = ceil (extents->y + extents->height) - surface->extents.y;

	surface->unbounded = FALSE;
    }

    _cairo_array_init (&surface->commands, sizeof (cairo_command_t *));

    surface->base.is_clear = TRUE;

    surface->bbtree.left = surface->bbtree.right = NULL;
    surface->bbtree.chain = INVALID_CHAIN;

    surface->indices = NULL;
    surface->num_indices = 0;
    surface->optimize_clears = TRUE;

    return &surface->base;
}
slim_hidden_def (cairo_recording_surface_create);

static cairo_surface_t *
_cairo_recording_surface_create_similar (void		       *abstract_surface,
					 cairo_content_t	content,
					 int			width,
					 int			height)
{
    cairo_rectangle_t extents;
    extents.x = extents.y = 0;
    extents.width = width;
    extents.height = height;
    return cairo_recording_surface_create (content, &extents);
}

static cairo_status_t
_cairo_recording_surface_finish (void *abstract_surface)
{
    cairo_recording_surface_t *surface = abstract_surface;
    cairo_command_t **elements;
    int i, num_elements;

    num_elements = surface->commands.num_elements;
    elements = _cairo_array_index (&surface->commands, 0);
    for (i = 0; i < num_elements; i++) {
	cairo_command_t *command = elements[i];

	switch (command->header.type) {
	case CAIRO_COMMAND_PAINT:
	    _cairo_pattern_fini (&command->paint.source.base);
	    break;

	case CAIRO_COMMAND_MASK:
	    _cairo_pattern_fini (&command->mask.source.base);
	    _cairo_pattern_fini (&command->mask.mask.base);
	    break;

	case CAIRO_COMMAND_STROKE:
	    _cairo_pattern_fini (&command->stroke.source.base);
	    _cairo_path_fixed_fini (&command->stroke.path);
	    _cairo_stroke_style_fini (&command->stroke.style);
	    break;

	case CAIRO_COMMAND_FILL:
	    _cairo_pattern_fini (&command->fill.source.base);
	    _cairo_path_fixed_fini (&command->fill.path);
	    break;

	case CAIRO_COMMAND_SHOW_TEXT_GLYPHS:
	    _cairo_pattern_fini (&command->show_text_glyphs.source.base);
	    free (command->show_text_glyphs.utf8);
	    free (command->show_text_glyphs.glyphs);
	    free (command->show_text_glyphs.clusters);
	    cairo_scaled_font_destroy (command->show_text_glyphs.scaled_font);
	    break;

	default:
	    ASSERT_NOT_REACHED;
	}

	_cairo_clip_destroy (command->header.clip);
	free (command);
    }

    _cairo_array_fini (&surface->commands);

    if (surface->bbtree.left)
	bbtree_del (surface->bbtree.left);
    if (surface->bbtree.right)
	bbtree_del (surface->bbtree.right);

    free (surface->indices);

    return CAIRO_STATUS_SUCCESS;
}

struct proxy {
    cairo_surface_t base;
    cairo_surface_t *image;
};

static cairo_status_t
proxy_acquire_source_image (void			 *abstract_surface,
			    cairo_image_surface_t	**image_out,
			    void			**image_extra)
{
    struct proxy *proxy = abstract_surface;
    return _cairo_surface_acquire_source_image (proxy->image, image_out, image_extra);
}

static void
proxy_release_source_image (void			*abstract_surface,
			    cairo_image_surface_t	*image,
			    void			*image_extra)
{
    struct proxy *proxy = abstract_surface;
    _cairo_surface_release_source_image (proxy->image, image, image_extra);
}

static cairo_status_t
proxy_finish (void *abstract_surface)
{
    return CAIRO_STATUS_SUCCESS;
}

static const cairo_surface_backend_t proxy_backend  = {
    CAIRO_INTERNAL_SURFACE_TYPE_NULL,
    proxy_finish,
    NULL,

    NULL, /* create similar */
    NULL, /* create similar image */
    NULL, /* map to image */
    NULL, /* unmap image */

    _cairo_surface_default_source,
    proxy_acquire_source_image,
    proxy_release_source_image,
};

static cairo_surface_t *
attach_proxy (cairo_surface_t *source,
	      cairo_surface_t *image)
{
    struct proxy *proxy;

    proxy = malloc (sizeof (*proxy));
    if (unlikely (proxy == NULL))
	return _cairo_surface_create_in_error (CAIRO_STATUS_NO_MEMORY);

    _cairo_surface_init (&proxy->base, &proxy_backend, NULL, image->content);

    proxy->image = image;
    _cairo_surface_attach_snapshot (source, &proxy->base, NULL);

    return &proxy->base;
}

static void
detach_proxy (cairo_surface_t *source,
	      cairo_surface_t *proxy)
{
    cairo_surface_finish (proxy);
    cairo_surface_destroy (proxy);
}

static cairo_surface_t *
get_proxy (cairo_surface_t *proxy)
{
    return ((struct proxy *)proxy)->image;
}

static cairo_status_t
_cairo_recording_surface_acquire_source_image (void			 *abstract_surface,
					       cairo_image_surface_t	**image_out,
					       void			**image_extra)
{
    cairo_recording_surface_t *surface = abstract_surface;
    cairo_surface_t *image, *proxy;
    cairo_status_t status;

    proxy = _cairo_surface_has_snapshot (abstract_surface, &proxy_backend);
    if (proxy != NULL) {
	*image_out = (cairo_image_surface_t *)
	    cairo_surface_reference (get_proxy (proxy));
	*image_extra = NULL;
	return CAIRO_STATUS_SUCCESS;
    }

    assert (! surface->unbounded);
    image = _cairo_image_surface_create_with_content (surface->base.content,
						      surface->extents.width,
						      surface->extents.height);
    if (unlikely (image->status))
	return image->status;

    /* Handle recursion by returning future reads from the current image */
    proxy = attach_proxy (abstract_surface, image);
    status = _cairo_recording_surface_replay (&surface->base, image);
    detach_proxy (abstract_surface, proxy);

    if (unlikely (status)) {
	cairo_surface_destroy (image);
	return status;
    }

    *image_out = (cairo_image_surface_t *) image;
    *image_extra = NULL;
    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_recording_surface_release_source_image (void			*abstract_surface,
					       cairo_image_surface_t	*image,
					       void			*image_extra)
{
    cairo_surface_destroy (&image->base);
}

static cairo_status_t
_command_init (cairo_recording_surface_t *surface,
	       cairo_command_header_t *command,
	       cairo_command_type_t type,
	       cairo_operator_t op,
	       cairo_composite_rectangles_t *composite)
{
    cairo_status_t status = CAIRO_STATUS_SUCCESS;

    command->type = type;
    command->op = op;
    command->region = CAIRO_RECORDING_REGION_ALL;

    command->extents = composite->unbounded;
    command->chain = NULL;
    command->index = surface->commands.num_elements;

    /* steal the clip */
    command->clip = NULL;
    if (! _cairo_composite_rectangles_can_reduce_clip (composite,
						       composite->clip))
    {
	command->clip = composite->clip;
	composite->clip = NULL;
    }

    return status;
}

static void
_cairo_recording_surface_break_self_copy_loop (cairo_recording_surface_t *surface)
{
    cairo_surface_flush (&surface->base);
}

static cairo_status_t
_cairo_recording_surface_commit (cairo_recording_surface_t *surface,
				 cairo_command_header_t *command)
{
    _cairo_recording_surface_break_self_copy_loop (surface);
    return _cairo_array_append (&surface->commands, &command);
}

static void
_cairo_recording_surface_reset (cairo_recording_surface_t *surface)
{
    /* Reset the commands and temporaries */
    _cairo_recording_surface_finish (surface);

    surface->bbtree.left = surface->bbtree.right = NULL;
    surface->bbtree.chain = INVALID_CHAIN;

    surface->indices = NULL;
    surface->num_indices = 0;

    _cairo_array_init (&surface->commands, sizeof (cairo_command_t *));
}

static cairo_bool_t
is_identity_recording_pattern (const cairo_pattern_t *pattern)
{
    cairo_surface_t *surface;

    if (pattern->type != CAIRO_PATTERN_TYPE_SURFACE)
	return FALSE;

    if (!_cairo_matrix_is_identity(&pattern->matrix))
	return FALSE;

    surface = ((cairo_surface_pattern_t *)pattern)->surface;
    return surface->backend->type == CAIRO_SURFACE_TYPE_RECORDING;
}

static cairo_int_status_t
_cairo_recording_surface_paint (void			  *abstract_surface,
				cairo_operator_t	   op,
				const cairo_pattern_t	  *source,
				const cairo_clip_t	  *clip)
{
    cairo_status_t status;
    cairo_recording_surface_t *surface = abstract_surface;
    cairo_command_paint_t *command;
    cairo_composite_rectangles_t composite;

    TRACE ((stderr, "%s: surface=%d\n", __FUNCTION__, surface->base.unique_id));

    if (op == CAIRO_OPERATOR_CLEAR && clip == NULL) {
	if (surface->optimize_clears) {
	    _cairo_recording_surface_reset (surface);
	    return CAIRO_STATUS_SUCCESS;
	}
    }

    if (clip == NULL && surface->optimize_clears &&
	(op == CAIRO_OPERATOR_SOURCE ||
	 (op == CAIRO_OPERATOR_OVER &&
	  (surface->base.is_clear || _cairo_pattern_is_opaque_solid (source)))))
    {
	_cairo_recording_surface_reset (surface);
	if (is_identity_recording_pattern (source)) {
	    cairo_surface_t *src = ((cairo_surface_pattern_t *)source)->surface;
	    return _cairo_recording_surface_replay (src, &surface->base);
	}
    }

    status = _cairo_composite_rectangles_init_for_paint (&composite,
							 &surface->base,
							 op, source,
							 clip);
    if (unlikely (status))
	return status;

    command = malloc (sizeof (cairo_command_paint_t));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto CLEANUP_COMPOSITE;
    }

    status = _command_init (surface,
			    &command->header, CAIRO_COMMAND_PAINT, op,
			    &composite);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_pattern_init_snapshot (&command->source.base, source);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto CLEANUP_SOURCE;

    _cairo_recording_surface_destroy_bbtree (surface);

    _cairo_composite_rectangles_fini (&composite);
    return CAIRO_STATUS_SUCCESS;

  CLEANUP_SOURCE:
    _cairo_pattern_fini (&command->source.base);
  CLEANUP_COMMAND:
    _cairo_clip_destroy (command->header.clip);
    free (command);
CLEANUP_COMPOSITE:
    _cairo_composite_rectangles_fini (&composite);
    return status;
}

static cairo_int_status_t
_cairo_recording_surface_mask (void			*abstract_surface,
			       cairo_operator_t		 op,
			       const cairo_pattern_t	*source,
			       const cairo_pattern_t	*mask,
			       const cairo_clip_t	*clip)
{
    cairo_status_t status;
    cairo_recording_surface_t *surface = abstract_surface;
    cairo_command_mask_t *command;
    cairo_composite_rectangles_t composite;

    TRACE ((stderr, "%s: surface=%d\n", __FUNCTION__, surface->base.unique_id));

    status = _cairo_composite_rectangles_init_for_mask (&composite,
							&surface->base,
							op, source, mask,
							clip);
    if (unlikely (status))
	return status;

    command = malloc (sizeof (cairo_command_mask_t));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto CLEANUP_COMPOSITE;
    }

    status = _command_init (surface,
			    &command->header, CAIRO_COMMAND_MASK, op,
			    &composite);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_pattern_init_snapshot (&command->source.base, source);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_pattern_init_snapshot (&command->mask.base, mask);
    if (unlikely (status))
	goto CLEANUP_SOURCE;

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto CLEANUP_MASK;

    _cairo_recording_surface_destroy_bbtree (surface);

    _cairo_composite_rectangles_fini (&composite);
    return CAIRO_STATUS_SUCCESS;

  CLEANUP_MASK:
    _cairo_pattern_fini (&command->mask.base);
  CLEANUP_SOURCE:
    _cairo_pattern_fini (&command->source.base);
  CLEANUP_COMMAND:
    _cairo_clip_destroy (command->header.clip);
    free (command);
CLEANUP_COMPOSITE:
    _cairo_composite_rectangles_fini (&composite);
    return status;
}

static cairo_int_status_t
_cairo_recording_surface_stroke (void			*abstract_surface,
				 cairo_operator_t	 op,
				 const cairo_pattern_t	*source,
				 const cairo_path_fixed_t	*path,
				 const cairo_stroke_style_t	*style,
				 const cairo_matrix_t		*ctm,
				 const cairo_matrix_t		*ctm_inverse,
				 double			 tolerance,
				 cairo_antialias_t	 antialias,
				 const cairo_clip_t	*clip)
{
    cairo_status_t status;
    cairo_recording_surface_t *surface = abstract_surface;
    cairo_command_stroke_t *command;
    cairo_composite_rectangles_t composite;

    TRACE ((stderr, "%s: surface=%d\n", __FUNCTION__, surface->base.unique_id));

    status = _cairo_composite_rectangles_init_for_stroke (&composite,
							  &surface->base,
							  op, source,
							  path, style, ctm,
							  clip);
    if (unlikely (status))
	return status;

    command = malloc (sizeof (cairo_command_stroke_t));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto CLEANUP_COMPOSITE;
    }

    status = _command_init (surface,
			    &command->header, CAIRO_COMMAND_STROKE, op,
			    &composite);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_pattern_init_snapshot (&command->source.base, source);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_path_fixed_init_copy (&command->path, path);
    if (unlikely (status))
	goto CLEANUP_SOURCE;

    status = _cairo_stroke_style_init_copy (&command->style, style);
    if (unlikely (status))
	goto CLEANUP_PATH;

    command->ctm = *ctm;
    command->ctm_inverse = *ctm_inverse;
    command->tolerance = tolerance;
    command->antialias = antialias;

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto CLEANUP_STYLE;

    _cairo_recording_surface_destroy_bbtree (surface);

    _cairo_composite_rectangles_fini (&composite);
    return CAIRO_STATUS_SUCCESS;

  CLEANUP_STYLE:
    _cairo_stroke_style_fini (&command->style);
  CLEANUP_PATH:
    _cairo_path_fixed_fini (&command->path);
  CLEANUP_SOURCE:
    _cairo_pattern_fini (&command->source.base);
  CLEANUP_COMMAND:
    _cairo_clip_destroy (command->header.clip);
    free (command);
CLEANUP_COMPOSITE:
    _cairo_composite_rectangles_fini (&composite);
    return status;
}

static cairo_int_status_t
_cairo_recording_surface_fill (void			*abstract_surface,
			       cairo_operator_t		 op,
			       const cairo_pattern_t	*source,
			       const cairo_path_fixed_t	*path,
			       cairo_fill_rule_t	 fill_rule,
			       double			 tolerance,
			       cairo_antialias_t	 antialias,
			       const cairo_clip_t	*clip)
{
    cairo_status_t status;
    cairo_recording_surface_t *surface = abstract_surface;
    cairo_command_fill_t *command;
    cairo_composite_rectangles_t composite;

    TRACE ((stderr, "%s: surface=%d\n", __FUNCTION__, surface->base.unique_id));

    status = _cairo_composite_rectangles_init_for_fill (&composite,
							&surface->base,
							op, source, path,
							clip);
    if (unlikely (status))
	return status;

    command = malloc (sizeof (cairo_command_fill_t));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto CLEANUP_COMPOSITE;
    }

    status =_command_init (surface,
			   &command->header, CAIRO_COMMAND_FILL, op,
			   &composite);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_pattern_init_snapshot (&command->source.base, source);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_path_fixed_init_copy (&command->path, path);
    if (unlikely (status))
	goto CLEANUP_SOURCE;

    command->fill_rule = fill_rule;
    command->tolerance = tolerance;
    command->antialias = antialias;

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto CLEANUP_PATH;

    _cairo_recording_surface_destroy_bbtree (surface);

    _cairo_composite_rectangles_fini (&composite);
    return CAIRO_STATUS_SUCCESS;

  CLEANUP_PATH:
    _cairo_path_fixed_fini (&command->path);
  CLEANUP_SOURCE:
    _cairo_pattern_fini (&command->source.base);
  CLEANUP_COMMAND:
    _cairo_clip_destroy (command->header.clip);
    free (command);
CLEANUP_COMPOSITE:
    _cairo_composite_rectangles_fini (&composite);
    return status;
}

static cairo_bool_t
_cairo_recording_surface_has_show_text_glyphs (void *abstract_surface)
{
    return TRUE;
}

static cairo_int_status_t
_cairo_recording_surface_show_text_glyphs (void				*abstract_surface,
					   cairo_operator_t		 op,
					   const cairo_pattern_t	*source,
					   const char			*utf8,
					   int				 utf8_len,
					   cairo_glyph_t		*glyphs,
					   int				 num_glyphs,
					   const cairo_text_cluster_t	*clusters,
					   int				 num_clusters,
					   cairo_text_cluster_flags_t	 cluster_flags,
					   cairo_scaled_font_t		*scaled_font,
					   const cairo_clip_t		*clip)
{
    cairo_status_t status;
    cairo_recording_surface_t *surface = abstract_surface;
    cairo_command_show_text_glyphs_t *command;
    cairo_composite_rectangles_t composite;

    TRACE ((stderr, "%s: surface=%d\n", __FUNCTION__, surface->base.unique_id));

    status = _cairo_composite_rectangles_init_for_glyphs (&composite,
							  &surface->base,
							  op, source,
							  scaled_font,
							  glyphs, num_glyphs,
							  clip,
							  NULL);
    if (unlikely (status))
	return status;

    command = malloc (sizeof (cairo_command_show_text_glyphs_t));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto CLEANUP_COMPOSITE;
    }

    status = _command_init (surface,
			    &command->header, CAIRO_COMMAND_SHOW_TEXT_GLYPHS,
			    op, &composite);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    status = _cairo_pattern_init_snapshot (&command->source.base, source);
    if (unlikely (status))
	goto CLEANUP_COMMAND;

    command->utf8 = NULL;
    command->utf8_len = utf8_len;
    command->glyphs = NULL;
    command->num_glyphs = num_glyphs;
    command->clusters = NULL;
    command->num_clusters = num_clusters;

    if (utf8_len) {
	command->utf8 = malloc (utf8_len);
	if (unlikely (command->utf8 == NULL)) {
	    status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    goto CLEANUP_ARRAYS;
	}
	memcpy (command->utf8, utf8, utf8_len);
    }
    if (num_glyphs) {
	command->glyphs = _cairo_malloc_ab (num_glyphs, sizeof (glyphs[0]));
	if (unlikely (command->glyphs == NULL)) {
	    status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    goto CLEANUP_ARRAYS;
	}
	memcpy (command->glyphs, glyphs, sizeof (glyphs[0]) * num_glyphs);
    }
    if (num_clusters) {
	command->clusters = _cairo_malloc_ab (num_clusters, sizeof (clusters[0]));
	if (unlikely (command->clusters == NULL)) {
	    status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    goto CLEANUP_ARRAYS;
	}
	memcpy (command->clusters, clusters, sizeof (clusters[0]) * num_clusters);
    }

    command->cluster_flags = cluster_flags;

    command->scaled_font = cairo_scaled_font_reference (scaled_font);

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto CLEANUP_SCALED_FONT;

    _cairo_composite_rectangles_fini (&composite);
    return CAIRO_STATUS_SUCCESS;

  CLEANUP_SCALED_FONT:
    cairo_scaled_font_destroy (command->scaled_font);
  CLEANUP_ARRAYS:
    free (command->utf8);
    free (command->glyphs);
    free (command->clusters);

    _cairo_pattern_fini (&command->source.base);
  CLEANUP_COMMAND:
    _cairo_clip_destroy (command->header.clip);
    free (command);
CLEANUP_COMPOSITE:
    _cairo_composite_rectangles_fini (&composite);
    return status;
}

static void
_command_init_copy (cairo_recording_surface_t *surface,
		    cairo_command_header_t *dst,
		    const cairo_command_header_t *src)
{
    dst->type = src->type;
    dst->op = src->op;
    dst->region = CAIRO_RECORDING_REGION_ALL;

    dst->extents = src->extents;
    dst->chain = NULL;
    dst->index = surface->commands.num_elements;

    dst->clip = _cairo_clip_copy (src->clip);
}

static cairo_status_t
_cairo_recording_surface_copy__paint (cairo_recording_surface_t *surface,
				      const cairo_command_t *src)
{
    cairo_command_paint_t *command;
    cairo_status_t status;

    command = malloc (sizeof (*command));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto err;
    }

    _command_init_copy (surface, &command->header, &src->header);

    status = _cairo_pattern_init_copy (&command->source.base,
				       &src->paint.source.base);
    if (unlikely (status))
	goto err_command;

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto err_source;

    return CAIRO_STATUS_SUCCESS;

err_source:
    _cairo_pattern_fini (&command->source.base);
err_command:
    free(command);
err:
    return status;
}

static cairo_status_t
_cairo_recording_surface_copy__mask (cairo_recording_surface_t *surface,
				     const cairo_command_t *src)
{
    cairo_command_mask_t *command;
    cairo_status_t status;

    command = malloc (sizeof (*command));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto err;
    }

    _command_init_copy (surface, &command->header, &src->header);

    status = _cairo_pattern_init_copy (&command->source.base,
				       &src->mask.source.base);
    if (unlikely (status))
	goto err_command;

    status = _cairo_pattern_init_copy (&command->mask.base,
				       &src->mask.source.base);
    if (unlikely (status))
	goto err_source;

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto err_mask;

    return CAIRO_STATUS_SUCCESS;

err_mask:
    _cairo_pattern_fini (&command->mask.base);
err_source:
    _cairo_pattern_fini (&command->source.base);
err_command:
    free(command);
err:
    return status;
}

static cairo_status_t
_cairo_recording_surface_copy__stroke (cairo_recording_surface_t *surface,
				     const cairo_command_t *src)
{
    cairo_command_stroke_t *command;
    cairo_status_t status;

    command = malloc (sizeof (*command));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto err;
    }

    _command_init_copy (surface, &command->header, &src->header);

    status = _cairo_pattern_init_copy (&command->source.base,
				       &src->stroke.source.base);
    if (unlikely (status))
	goto err_command;

    status = _cairo_path_fixed_init_copy (&command->path, &src->stroke.path);
    if (unlikely (status))
	goto err_source;

    status = _cairo_stroke_style_init_copy (&command->style,
					    &src->stroke.style);
    if (unlikely (status))
	goto err_path;

    command->ctm = src->stroke.ctm;
    command->ctm_inverse = src->stroke.ctm_inverse;
    command->tolerance = src->stroke.tolerance;
    command->antialias = src->stroke.antialias;

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto err_style;

    return CAIRO_STATUS_SUCCESS;

err_style:
    _cairo_stroke_style_fini (&command->style);
err_path:
    _cairo_path_fixed_fini (&command->path);
err_source:
    _cairo_pattern_fini (&command->source.base);
err_command:
    free(command);
err:
    return status;
}

static cairo_status_t
_cairo_recording_surface_copy__fill (cairo_recording_surface_t *surface,
				     const cairo_command_t *src)
{
    cairo_command_fill_t *command;
    cairo_status_t status;

    command = malloc (sizeof (*command));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto err;
    }

    _command_init_copy (surface, &command->header, &src->header);

    status = _cairo_pattern_init_copy (&command->source.base,
				       &src->fill.source.base);
    if (unlikely (status))
	goto err_command;

    status = _cairo_path_fixed_init_copy (&command->path, &src->fill.path);
    if (unlikely (status))
	goto err_source;

    command->fill_rule = src->fill.fill_rule;
    command->tolerance = src->fill.tolerance;
    command->antialias = src->fill.antialias;

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto err_path;

    return CAIRO_STATUS_SUCCESS;

err_path:
    _cairo_path_fixed_fini (&command->path);
err_source:
    _cairo_pattern_fini (&command->source.base);
err_command:
    free(command);
err:
    return status;
}

static cairo_status_t
_cairo_recording_surface_copy__glyphs (cairo_recording_surface_t *surface,
				       const cairo_command_t *src)
{
    cairo_command_show_text_glyphs_t *command;
    cairo_status_t status;

    command = malloc (sizeof (*command));
    if (unlikely (command == NULL)) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto err;
    }

    _command_init_copy (surface, &command->header, &src->header);

    status = _cairo_pattern_init_copy (&command->source.base,
				       &src->show_text_glyphs.source.base);
    if (unlikely (status))
	goto err_command;

    command->utf8 = NULL;
    command->utf8_len = src->show_text_glyphs.utf8_len;
    command->glyphs = NULL;
    command->num_glyphs = src->show_text_glyphs.num_glyphs;
    command->clusters = NULL;
    command->num_clusters = src->show_text_glyphs.num_clusters;

    if (command->utf8_len) {
	command->utf8 = malloc (command->utf8_len);
	if (unlikely (command->utf8 == NULL)) {
	    status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    goto err_arrays;
	}
	memcpy (command->utf8, src->show_text_glyphs.utf8, command->utf8_len);
    }
    if (command->num_glyphs) {
	command->glyphs = _cairo_malloc_ab (command->num_glyphs,
					    sizeof (command->glyphs[0]));
	if (unlikely (command->glyphs == NULL)) {
	    status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    goto err_arrays;
	}
	memcpy (command->glyphs, src->show_text_glyphs.glyphs,
		sizeof (command->glyphs[0]) * command->num_glyphs);
    }
    if (command->num_clusters) {
	command->clusters = _cairo_malloc_ab (command->num_clusters,
					      sizeof (command->clusters[0]));
	if (unlikely (command->clusters == NULL)) {
	    status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    goto err_arrays;
	}
	memcpy (command->clusters, src->show_text_glyphs.clusters,
		sizeof (command->clusters[0]) * command->num_clusters);
    }

    command->cluster_flags = src->show_text_glyphs.cluster_flags;

    command->scaled_font =
	cairo_scaled_font_reference (src->show_text_glyphs.scaled_font);

    status = _cairo_recording_surface_commit (surface, &command->header);
    if (unlikely (status))
	goto err_arrays;

    return CAIRO_STATUS_SUCCESS;

err_arrays:
    free (command->utf8);
    free (command->glyphs);
    free (command->clusters);
    _cairo_pattern_fini (&command->source.base);
err_command:
    free(command);
err:
    return status;
}

static cairo_status_t
_cairo_recording_surface_copy (cairo_recording_surface_t *dst,
			       cairo_recording_surface_t *src)
{
    cairo_command_t **elements;
    int i, num_elements;
    cairo_status_t status;

    elements = _cairo_array_index (&src->commands, 0);
    num_elements = src->commands.num_elements;
    for (i = 0; i < num_elements; i++) {
	const cairo_command_t *command = elements[i];

	switch (command->header.type) {
	case CAIRO_COMMAND_PAINT:
	    status = _cairo_recording_surface_copy__paint (dst, command);
	    break;

	case CAIRO_COMMAND_MASK:
	    status = _cairo_recording_surface_copy__mask (dst, command);
	    break;

	case CAIRO_COMMAND_STROKE:
	    status = _cairo_recording_surface_copy__stroke (dst, command);
	    break;

	case CAIRO_COMMAND_FILL:
	    status = _cairo_recording_surface_copy__fill (dst, command);
	    break;

	case CAIRO_COMMAND_SHOW_TEXT_GLYPHS:
	    status = _cairo_recording_surface_copy__glyphs (dst, command);
	    break;

	default:
	    ASSERT_NOT_REACHED;
	}

	if (unlikely (status))
	    return status;
    }

    return CAIRO_STATUS_SUCCESS;
}

/**
 * _cairo_recording_surface_snapshot:
 * @surface: a #cairo_surface_t which must be a recording surface
 *
 * Make an immutable copy of @surface. It is an error to call a
 * surface-modifying function on the result of this function.
 *
 * The caller owns the return value and should call
 * cairo_surface_destroy() when finished with it. This function will not
 * return %NULL, but will return a nil surface instead.
 *
 * Return value: The snapshot surface.
 **/
static cairo_surface_t *
_cairo_recording_surface_snapshot (void *abstract_other)
{
    cairo_recording_surface_t *other = abstract_other;
    cairo_recording_surface_t *surface;
    cairo_status_t status;

    surface = malloc (sizeof (cairo_recording_surface_t));
    if (unlikely (surface == NULL))
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));

    _cairo_surface_init (&surface->base,
			 &cairo_recording_surface_backend,
			 NULL, /* device */
			 other->base.content);

    surface->extents_pixels = other->extents_pixels;
    surface->extents = other->extents;
    surface->unbounded = other->unbounded;

    surface->base.is_clear = other->base.is_clear;

    surface->bbtree.left = surface->bbtree.right = NULL;
    surface->bbtree.chain = INVALID_CHAIN;

    surface->indices = NULL;
    surface->num_indices = 0;
    surface->optimize_clears = TRUE;

    _cairo_array_init (&surface->commands, sizeof (cairo_command_t *));
    status = _cairo_recording_surface_copy (surface, other);
    if (unlikely (status)) {
	cairo_surface_destroy (&surface->base);
	return _cairo_surface_create_in_error (status);
    }

    return &surface->base;
}

static cairo_bool_t
_cairo_recording_surface_get_extents (void		    *abstract_surface,
				      cairo_rectangle_int_t *rectangle)
{
    cairo_recording_surface_t *surface = abstract_surface;

    if (surface->unbounded)
	return FALSE;

    *rectangle = surface->extents;
    return TRUE;
}

static const cairo_surface_backend_t cairo_recording_surface_backend = {
    CAIRO_SURFACE_TYPE_RECORDING,
    _cairo_recording_surface_finish,

    _cairo_default_context_create,

    _cairo_recording_surface_create_similar,
    NULL, /* create similar image */
    NULL, /* map to image */
    NULL, /* unmap image */

    _cairo_surface_default_source,
    _cairo_recording_surface_acquire_source_image,
    _cairo_recording_surface_release_source_image,
    _cairo_recording_surface_snapshot,

    NULL, /* copy_page */
    NULL, /* show_page */

    _cairo_recording_surface_get_extents,
    NULL, /* get_font_options */

    NULL, /* flush */
    NULL, /* mark_dirty_rectangle */

    /* Here are the 5 basic drawing operations, (which are in some
     * sense the only things that cairo_recording_surface should need to
     * implement).  However, we implement the more generic show_text_glyphs
     * instead of show_glyphs.  One or the other is eough. */

    _cairo_recording_surface_paint,
    _cairo_recording_surface_mask,
    _cairo_recording_surface_stroke,
    _cairo_recording_surface_fill,
    NULL, /* fill-stroke */
    NULL,
    _cairo_recording_surface_has_show_text_glyphs,
    _cairo_recording_surface_show_text_glyphs,
};

cairo_int_status_t
_cairo_recording_surface_get_path (cairo_surface_t    *abstract_surface,
				   cairo_path_fixed_t *path)
{
    cairo_recording_surface_t *surface;
    cairo_command_t **elements;
    int i, num_elements;
    cairo_int_status_t status;

    if (unlikely (abstract_surface->status))
	return abstract_surface->status;

    surface = (cairo_recording_surface_t *) abstract_surface;
    status = CAIRO_STATUS_SUCCESS;

    num_elements = surface->commands.num_elements;
    elements = _cairo_array_index (&surface->commands, 0);
    for (i = 0; i < num_elements; i++) {
	cairo_command_t *command = elements[i];

	switch (command->header.type) {
	case CAIRO_COMMAND_PAINT:
	case CAIRO_COMMAND_MASK:
	    status = CAIRO_INT_STATUS_UNSUPPORTED;
	    break;

	case CAIRO_COMMAND_STROKE:
	{
	    cairo_traps_t traps;

	    _cairo_traps_init (&traps);

	    /* XXX call cairo_stroke_to_path() when that is implemented */
	    status = _cairo_path_fixed_stroke_to_traps (&command->stroke.path,
							&command->stroke.style,
							&command->stroke.ctm,
							&command->stroke.ctm_inverse,
							command->stroke.tolerance,
							&traps);

	    if (status == CAIRO_INT_STATUS_SUCCESS)
		status = _cairo_traps_path (&traps, path);

	    _cairo_traps_fini (&traps);
	    break;
	}
	case CAIRO_COMMAND_FILL:
	{
	    status = _cairo_path_fixed_append (path,
					       &command->fill.path,
					       0, 0);
	    break;
	}
	case CAIRO_COMMAND_SHOW_TEXT_GLYPHS:
	{
	    status = _cairo_scaled_font_glyph_path (command->show_text_glyphs.scaled_font,
						    command->show_text_glyphs.glyphs,
						    command->show_text_glyphs.num_glyphs,
						    path);
	    break;
	}

	default:
	    ASSERT_NOT_REACHED;
	}

	if (unlikely (status))
	    break;
    }

    return status;
}

static int
_cairo_recording_surface_get_visible_commands (cairo_recording_surface_t *surface,
					       const cairo_rectangle_int_t *extents)
{
    int num_visible, *indices;
    cairo_box_t box;

    _cairo_box_from_rectangle (&box, extents);

    if (surface->bbtree.chain == INVALID_CHAIN)
	_cairo_recording_surface_create_bbtree (surface);

    indices = surface->indices;
    bbtree_foreach_mark_visible (&surface->bbtree, &box, &indices);
    num_visible = indices - surface->indices;
    if (num_visible > 1)
	sort_indices (surface->indices, num_visible);

    return num_visible;
}

static cairo_status_t
_cairo_recording_surface_replay_internal (cairo_recording_surface_t	*surface,
					  const cairo_rectangle_int_t *surface_extents,
					  const cairo_matrix_t *surface_transform,
					  cairo_surface_t	     *target,
					  const cairo_clip_t *target_clip,
					  cairo_recording_replay_type_t type,
					  cairo_recording_region_type_t region)
{
    cairo_surface_wrapper_t wrapper;
    cairo_command_t **elements;
    cairo_bool_t replay_all =
	type == CAIRO_RECORDING_REPLAY &&
	region == CAIRO_RECORDING_REGION_ALL;
    cairo_int_status_t status = CAIRO_STATUS_SUCCESS;
    cairo_rectangle_int_t extents;
    cairo_bool_t use_indices = FALSE;
    const cairo_rectangle_int_t *r;
    int i, num_elements;

    if (unlikely (surface->base.status))
	return surface->base.status;

    if (unlikely (target->status))
	return target->status;

    if (unlikely (surface->base.finished))
	return _cairo_error (CAIRO_STATUS_SURFACE_FINISHED);

    if (surface->base.is_clear)
	return CAIRO_STATUS_SUCCESS;

    assert (_cairo_surface_is_recording (&surface->base));

    _cairo_surface_wrapper_init (&wrapper, target);
    if (surface_extents)
	_cairo_surface_wrapper_intersect_extents (&wrapper, surface_extents);
    r = &_cairo_unbounded_rectangle;
    if (! surface->unbounded) {
	_cairo_surface_wrapper_intersect_extents (&wrapper, &surface->extents);
	r = &surface->extents;
    }
    _cairo_surface_wrapper_set_inverse_transform (&wrapper, surface_transform);
    _cairo_surface_wrapper_set_clip (&wrapper, target_clip);

    /* Compute the extents of the target clip in recorded device space */
    if (! _cairo_surface_wrapper_get_target_extents (&wrapper, &extents))
	goto done;

    num_elements = surface->commands.num_elements;
    elements = _cairo_array_index (&surface->commands, 0);
    if (extents.width < r->width || extents.height < r->height) {
	num_elements =
	    _cairo_recording_surface_get_visible_commands (surface, &extents);
	use_indices = TRUE;
    }

    for (i = 0; i < num_elements; i++) {
	cairo_command_t *command = elements[use_indices ? surface->indices[i] : i];

	if (! replay_all && command->header.region != region)
	    continue;

	if (! _cairo_rectangle_intersects (&extents, &command->header.extents))
	    continue;

	switch (command->header.type) {
	case CAIRO_COMMAND_PAINT:
	    status = _cairo_surface_wrapper_paint (&wrapper,
						   command->header.op,
						   &command->paint.source.base,
						   command->header.clip);
	    break;

	case CAIRO_COMMAND_MASK:
	    status = _cairo_surface_wrapper_mask (&wrapper,
						  command->header.op,
						  &command->mask.source.base,
						  &command->mask.mask.base,
						  command->header.clip);
	    break;

	case CAIRO_COMMAND_STROKE:
	    status = _cairo_surface_wrapper_stroke (&wrapper,
						    command->header.op,
						    &command->stroke.source.base,
						    &command->stroke.path,
						    &command->stroke.style,
						    &command->stroke.ctm,
						    &command->stroke.ctm_inverse,
						    command->stroke.tolerance,
						    command->stroke.antialias,
						    command->header.clip);
	    break;

	case CAIRO_COMMAND_FILL:
	    status = CAIRO_INT_STATUS_UNSUPPORTED;
	    if (_cairo_surface_wrapper_has_fill_stroke (&wrapper)) {
		cairo_command_t *stroke_command;

		stroke_command = NULL;
		if (type != CAIRO_RECORDING_CREATE_REGIONS && i < num_elements - 1)
		    stroke_command = elements[i + 1];

		if (stroke_command != NULL &&
		    type == CAIRO_RECORDING_REPLAY &&
		    region != CAIRO_RECORDING_REGION_ALL)
		{
		    if (stroke_command->header.region != region)
			stroke_command = NULL;
		}

		if (stroke_command != NULL &&
		    stroke_command->header.type == CAIRO_COMMAND_STROKE &&
		    _cairo_path_fixed_equal (&command->fill.path,
					     &stroke_command->stroke.path) &&
		    _cairo_clip_equal (command->header.clip,
				       stroke_command->header.clip))
		{
		    status = _cairo_surface_wrapper_fill_stroke (&wrapper,
								 command->header.op,
								 &command->fill.source.base,
								 command->fill.fill_rule,
								 command->fill.tolerance,
								 command->fill.antialias,
								 &command->fill.path,
								 stroke_command->header.op,
								 &stroke_command->stroke.source.base,
								 &stroke_command->stroke.style,
								 &stroke_command->stroke.ctm,
								 &stroke_command->stroke.ctm_inverse,
								 stroke_command->stroke.tolerance,
								 stroke_command->stroke.antialias,
								 command->header.clip);
		    i++;
		}
	    }
	    if (status == CAIRO_INT_STATUS_UNSUPPORTED) {
		status = _cairo_surface_wrapper_fill (&wrapper,
						      command->header.op,
						      &command->fill.source.base,
						      &command->fill.path,
						      command->fill.fill_rule,
						      command->fill.tolerance,
						      command->fill.antialias,
						      command->header.clip);
	    }
	    break;

	case CAIRO_COMMAND_SHOW_TEXT_GLYPHS:
	    status = _cairo_surface_wrapper_show_text_glyphs (&wrapper,
							      command->header.op,
							      &command->show_text_glyphs.source.base,
							      command->show_text_glyphs.utf8, command->show_text_glyphs.utf8_len,
							      command->show_text_glyphs.glyphs, command->show_text_glyphs.num_glyphs,
							      command->show_text_glyphs.clusters, command->show_text_glyphs.num_clusters,
							      command->show_text_glyphs.cluster_flags,
							      command->show_text_glyphs.scaled_font,
							      command->header.clip);
	    break;

	default:
	    ASSERT_NOT_REACHED;
	}

	if (type == CAIRO_RECORDING_CREATE_REGIONS) {
	    if (status == CAIRO_INT_STATUS_SUCCESS) {
		command->header.region = CAIRO_RECORDING_REGION_NATIVE;
	    } else if (status == CAIRO_INT_STATUS_IMAGE_FALLBACK) {
		command->header.region = CAIRO_RECORDING_REGION_IMAGE_FALLBACK;
		status = CAIRO_INT_STATUS_SUCCESS;
	    } else {
		assert (_cairo_int_status_is_error (status));
	    }
	}

	if (unlikely (status))
	    break;
    }

done:
    _cairo_surface_wrapper_fini (&wrapper);
    return _cairo_surface_set_error (&surface->base, status);
}

cairo_status_t
_cairo_recording_surface_replay_one (cairo_recording_surface_t	*surface,
				     long unsigned index,
				     cairo_surface_t	     *target)
{
    cairo_surface_wrapper_t wrapper;
    cairo_command_t **elements, *command;
    cairo_int_status_t status;

    if (unlikely (surface->base.status))
	return surface->base.status;

    if (unlikely (target->status))
	return target->status;

    if (unlikely (surface->base.finished))
	return _cairo_error (CAIRO_STATUS_SURFACE_FINISHED);

    assert (_cairo_surface_is_recording (&surface->base));

    /* XXX
     * Use a surface wrapper because we may want to do transformed
     * replay in the future.
     */
    _cairo_surface_wrapper_init (&wrapper, target);

    if (index > surface->commands.num_elements)
	return _cairo_error (CAIRO_STATUS_READ_ERROR);

    elements = _cairo_array_index (&surface->commands, 0);
    command = elements[index];
    switch (command->header.type) {
    case CAIRO_COMMAND_PAINT:
	status = _cairo_surface_wrapper_paint (&wrapper,
					       command->header.op,
					       &command->paint.source.base,
					       command->header.clip);
	break;

    case CAIRO_COMMAND_MASK:
	status = _cairo_surface_wrapper_mask (&wrapper,
					      command->header.op,
					      &command->mask.source.base,
					      &command->mask.mask.base,
					      command->header.clip);
	break;

    case CAIRO_COMMAND_STROKE:
	status = _cairo_surface_wrapper_stroke (&wrapper,
						command->header.op,
						&command->stroke.source.base,
						&command->stroke.path,
						&command->stroke.style,
						&command->stroke.ctm,
						&command->stroke.ctm_inverse,
						command->stroke.tolerance,
						command->stroke.antialias,
						command->header.clip);
	break;

    case CAIRO_COMMAND_FILL:
	status = _cairo_surface_wrapper_fill (&wrapper,
					      command->header.op,
					      &command->fill.source.base,
					      &command->fill.path,
					      command->fill.fill_rule,
					      command->fill.tolerance,
					      command->fill.antialias,
					      command->header.clip);
	break;

    case CAIRO_COMMAND_SHOW_TEXT_GLYPHS:
	status = _cairo_surface_wrapper_show_text_glyphs (&wrapper,
							  command->header.op,
							  &command->show_text_glyphs.source.base,
							  command->show_text_glyphs.utf8, command->show_text_glyphs.utf8_len,
							  command->show_text_glyphs.glyphs, command->show_text_glyphs.num_glyphs,
							  command->show_text_glyphs.clusters, command->show_text_glyphs.num_clusters,
							  command->show_text_glyphs.cluster_flags,
							  command->show_text_glyphs.scaled_font,
							  command->header.clip);
	break;

    default:
	ASSERT_NOT_REACHED;
    }

    _cairo_surface_wrapper_fini (&wrapper);
    return _cairo_surface_set_error (&surface->base, status);
}
/**
 * _cairo_recording_surface_replay:
 * @surface: the #cairo_recording_surface_t
 * @target: a target #cairo_surface_t onto which to replay the operations
 * @width_pixels: width of the surface, in pixels
 * @height_pixels: height of the surface, in pixels
 *
 * A recording surface can be "replayed" against any target surface,
 * after which the results in target will be identical to the results
 * that would have been obtained if the original operations applied to
 * the recording surface had instead been applied to the target surface.
 **/
cairo_status_t
_cairo_recording_surface_replay (cairo_surface_t *surface,
				 cairo_surface_t *target)
{
    return _cairo_recording_surface_replay_internal ((cairo_recording_surface_t *) surface, NULL, NULL,
						     target, NULL,
						     CAIRO_RECORDING_REPLAY,
						     CAIRO_RECORDING_REGION_ALL);
}

cairo_status_t
_cairo_recording_surface_replay_with_clip (cairo_surface_t *surface,
					   const cairo_matrix_t *surface_transform,
					   cairo_surface_t *target,
					   const cairo_clip_t *target_clip)
{
    return _cairo_recording_surface_replay_internal ((cairo_recording_surface_t *) surface, NULL, surface_transform,
						     target, target_clip,
						     CAIRO_RECORDING_REPLAY,
						     CAIRO_RECORDING_REGION_ALL);
}

/* Replay recording to surface. When the return status of each operation is
 * one of %CAIRO_STATUS_SUCCESS, %CAIRO_INT_STATUS_UNSUPPORTED, or
 * %CAIRO_INT_STATUS_FLATTEN_TRANSPARENCY the status of each operation
 * will be stored in the recording surface. Any other status will abort the
 * replay and return the status.
 */
cairo_status_t
_cairo_recording_surface_replay_and_create_regions (cairo_surface_t *surface,
						    cairo_surface_t *target)
{
    return _cairo_recording_surface_replay_internal ((cairo_recording_surface_t *) surface, NULL, NULL,
						     target, NULL,
						     CAIRO_RECORDING_CREATE_REGIONS,
						     CAIRO_RECORDING_REGION_ALL);
}

cairo_status_t
_cairo_recording_surface_replay_region (cairo_surface_t          *surface,
					const cairo_rectangle_int_t *surface_extents,
					cairo_surface_t          *target,
					cairo_recording_region_type_t  region)
{
    return _cairo_recording_surface_replay_internal ((cairo_recording_surface_t *) surface,
						     surface_extents, NULL,
						     target, NULL,
						     CAIRO_RECORDING_REPLAY,
						     region);
}

static cairo_status_t
_recording_surface_get_ink_bbox (cairo_recording_surface_t *surface,
				 cairo_box_t *bbox,
				 const cairo_matrix_t *transform)
{
    cairo_surface_t *null_surface;
    cairo_surface_t *analysis_surface;
    cairo_status_t status;

    null_surface = _cairo_null_surface_create (surface->base.content);
    analysis_surface = _cairo_analysis_surface_create (null_surface);
    cairo_surface_destroy (null_surface);

    status = analysis_surface->status;
    if (unlikely (status))
	return status;

    if (transform != NULL)
	_cairo_analysis_surface_set_ctm (analysis_surface, transform);

    status = _cairo_recording_surface_replay (&surface->base, analysis_surface);
    _cairo_analysis_surface_get_bounding_box (analysis_surface, bbox);
    cairo_surface_destroy (analysis_surface);

    return status;
}

/**
 * cairo_recording_surface_ink_extents:
 * @surface: a #cairo_recording_surface_t
 * @x0: the x-coordinate of the top-left of the ink bounding box
 * @y0: the y-coordinate of the top-left of the ink bounding box
 * @width: the width of the ink bounding box
 * @height: the height of the ink bounding box
 *
 * Measures the extents of the operations stored within the recording-surface.
 * This is useful to compute the required size of an image surface (or
 * equivalent) into which to replay the full sequence of drawing operations.
 *
 * Since: 1.10
 **/
void
cairo_recording_surface_ink_extents (cairo_surface_t *surface,
				     double *x0,
				     double *y0,
				     double *width,
				     double *height)
{
    cairo_status_t status;
    cairo_box_t bbox;

    memset (&bbox, 0, sizeof (bbox));

    if (surface->status || ! _cairo_surface_is_recording (surface)) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	goto DONE;
    }

    status = _recording_surface_get_ink_bbox ((cairo_recording_surface_t *) surface,
					 &bbox,
					 NULL);
    if (unlikely (status))
	status = _cairo_surface_set_error (surface, status);

DONE:
    if (x0)
	*x0 = _cairo_fixed_to_double (bbox.p1.x);
    if (y0)
	*y0 = _cairo_fixed_to_double (bbox.p1.y);
    if (width)
	*width = _cairo_fixed_to_double (bbox.p2.x - bbox.p1.x);
    if (height)
	*height = _cairo_fixed_to_double (bbox.p2.y - bbox.p1.y);
}

cairo_status_t
_cairo_recording_surface_get_bbox (cairo_recording_surface_t *surface,
				   cairo_box_t *bbox,
				   const cairo_matrix_t *transform)
{
    if (! surface->unbounded) {
	_cairo_box_from_rectangle (bbox, &surface->extents);
	if (transform != NULL)
	    _cairo_matrix_transform_bounding_box_fixed (transform, bbox, NULL);

	return CAIRO_STATUS_SUCCESS;
    }

    return _recording_surface_get_ink_bbox (surface, bbox, transform);
}

cairo_status_t
_cairo_recording_surface_get_ink_bbox (cairo_recording_surface_t *surface,
				       cairo_box_t *bbox,
				       const cairo_matrix_t *transform)
{
    return _recording_surface_get_ink_bbox (surface, bbox, transform);
}

/**
 * cairo_recording_surface_get_extents:
 * @surface: a #cairo_recording_surface_t
 * @extents: the #cairo_rectangle_t to be assigned the extents
 *
 * Get the extents of the recording-surface.
 *
 * Return value: %TRUE if the surface is bounded, of recording type, and
 * not in an error state, otherwise %FALSE
 *
 * Since: 1.12
 **/
cairo_bool_t
cairo_recording_surface_get_extents (cairo_surface_t *surface,
				     cairo_rectangle_t *extents)
{
    cairo_recording_surface_t *record;

    if (surface->status || ! _cairo_surface_is_recording (surface)) {
	_cairo_error_throw (CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return FALSE;
    }

    record = (cairo_recording_surface_t *)surface;
    if (record->unbounded)
	return FALSE;

    *extents = record->extents_pixels;
    return TRUE;
}
