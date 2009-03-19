/*
 *  $Id: mixedform.c,v 1.7 2008/06/21 12:11:42 tom Exp $
 *
 *  formbox.c -- implements the form (i.e, some pairs label/editbox)
 *
 *  Copyright 2007,2008	Thomas E. Dickey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License, version 2.1
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to
 *	Free Software Foundation, Inc.
 *	51 Franklin St., Fifth Floor
 *	Boston, MA 02110, USA.
 *
 *  This is inspired by a patch from Kiran Cherupally
 *  (but different interface design).
 */

#include <dialog.h>

#define LLEN(n) ((n) * MIXEDFORM_TAGS)

#define ItemName(i)     items[LLEN(i) + 0]
#define ItemNameY(i)    items[LLEN(i) + 1]
#define ItemNameX(i)    items[LLEN(i) + 2]
#define ItemText(i)     items[LLEN(i) + 3]
#define ItemTextY(i)    items[LLEN(i) + 4]
#define ItemTextX(i)    items[LLEN(i) + 5]
#define ItemTextFLen(i) items[LLEN(i) + 6]
#define ItemTextILen(i) items[LLEN(i) + 7]
#define ItemTypep(i)    items[LLEN(i) + 8]
#define ItemHelp(i)     (dialog_vars.item_help ? items[LLEN(i) + 9] : dlg_strempty())

int
dialog_mixedform(const char *title,
		 const char *cprompt,
		 int height,
		 int width,
		 int form_height,
		 int item_no,
		 char **items)
{
    int result;
    int choice;
    int i;
    DIALOG_FORMITEM *listitems;
    DIALOG_VARS save_vars;
    bool show_status = FALSE;

    dlg_save_vars(&save_vars);
    dialog_vars.separate_output = TRUE;

    listitems = dlg_calloc(DIALOG_FORMITEM, item_no + 1);
    assert_ptr(listitems, "dialog_mixedform");

    for (i = 0; i < item_no; ++i) {
	listitems[i].type = dialog_vars.formitem_type;
	listitems[i].name = ItemName(i);
	listitems[i].name_len = strlen(ItemName(i));
	listitems[i].name_y = dlg_ordinate(ItemNameY(i));
	listitems[i].name_x = dlg_ordinate(ItemNameX(i));
	listitems[i].text = ItemText(i);
	listitems[i].text_len = strlen(ItemText(i));
	listitems[i].text_y = dlg_ordinate(ItemTextY(i));
	listitems[i].text_x = dlg_ordinate(ItemTextX(i));
	listitems[i].text_flen = atoi(ItemTextFLen(i));
	listitems[i].text_ilen = atoi(ItemTextILen(i));
	listitems[i].help = (dialog_vars.item_help ? ItemHelp(i) :
			     dlg_strempty());
	listitems[i].type = atoi(ItemTypep(i));
    }

    result = dlg_form(title,
		      cprompt,
		      height,
		      width,
		      form_height,
		      item_no,
		      listitems,
		      &choice);

    switch (result) {
    case DLG_EXIT_OK:		/* FALLTHRU */
    case DLG_EXIT_EXTRA:
	show_status = TRUE;
	break;
    case DLG_EXIT_HELP:
	dlg_add_result("HELP ");
	show_status = dialog_vars.help_status;
	if (USE_ITEM_HELP(listitems[choice].help)) {
	    dlg_add_string(listitems[choice].help);
	    result = DLG_EXIT_ITEM_HELP;
	} else {
	    dlg_add_string(listitems[choice].name);
	}
	if (show_status)
	    dlg_add_separator();
	break;
    }
    if (show_status) {
	for (i = 0; i < item_no; i++) {
	    if (listitems[i].text_flen > 0) {
		dlg_add_string(listitems[i].text);
		dlg_add_separator();
	    }
	}
    }

    dlg_free_formitems(listitems);
    dlg_restore_vars(&save_vars);

    return result;
}
