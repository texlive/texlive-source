/* primitive.c

   Copyright 2008-2009 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

#include "luatex-api.h"
#include <ptexlib.h>

#include "commands.h"
#include "primitive.h"
#include "tokens.h"

static const char _svn_version[] =
    "$Id: primitive.c 2448 2009-06-08 07:43:50Z taco $ $URL: http://foundry.supelec.fr/svn/luatex/tags/beta-0.40.6/source/texk/web2c/luatexdir/tex/primitive.c $";

/* as usual, the file starts with a bunch of #defines that mimic pascal @ds */

#define level_one 1
#define flush_string() do { decr(str_ptr); pool_ptr=str_start_macro(str_ptr); } while (0)
#define cur_length (pool_ptr - str_start_macro(str_ptr))
#define append_char(a) str_pool[pool_ptr++]=(a)

#define next(a) hash[(a)].lhfield       /* link for coalesced lists */
#define text(a) hash[(a)].rh    /* string number for control sequence name */
#define hash_is_full (hash_used==hash_base)     /* test if all positions are occupied */
#define hash_size 65536

#define span_code 1114113
#define unless_code 32          /* amount added for `\.{\\unless}' prefix */
#define protected_token 0x1C00001       /* $2^{21}\cdot|end_match|+1$ */
#define offset_ocp_name 1
#define ocp_name(A) ocp_tables[(A)][offset_ocp_name]

#define skip_base      get_skip_base()
#define mu_skip_base   get_mu_skip_base()
#define glue_base      static_glue_base
#define toks_base      get_toks_base()
#define count_base     get_count_base()
#define int_base       static_int_base
#define attribute_base get_attribute_base()
#define scaled_base    get_scaled_base()
#define dimen_base     get_dimen_base()



/* \primitive support needs a few extra variables and definitions */

#define prim_base 1

/* The arrays |prim| and |prim_eqtb| are used for name -> cmd,chr lookups.
 * 
 * The are  modelled after |hash| and |eqtb|, except that primitives do not 
 *  have an |eq_level|, that field is replaced by |origin|.
 */

#define prim_next(a) prim[(a)].lhfield  /* link for coalesced lists */
#define prim_text(a) prim[(a)].rh       /* string number for control sequence name */
#define prim_is_full (prim_used==prim_base)     /* test if all positions are occupied */

#define prim_origin_field(a) (a).hh.b1
#define prim_eq_type_field(a)  (a).hh.b0
#define prim_equiv_field(a) (a).hh.rh
#define prim_origin(a) prim_origin_field(prim_eqtb[(a)])        /* level of definition */
#define prim_eq_type(a) prim_eq_type_field(prim_eqtb[(a)])      /* command code for equivalent */
#define prim_equiv(a) prim_equiv_field(prim_eqtb[(a)])  /* equivalent value */

static pointer prim_used;       /* allocation pointer for |prim| */
static two_halves prim[(prim_size + 1)];        /* the primitives table */
static memory_word prim_eqtb[(prim_size + 1)];

/* The array |prim_data| works the other way around, it is used for
   cmd,chr -> name lookups. */

typedef struct prim_info {
    halfword subids;            /* number of name entries */
    halfword offset;            /* offset to be used for |chr_code|s */
    str_number *names;          /* array of names */
} prim_info;

static prim_info prim_data[(last_cmd + 1)];

/* initialize the memory arrays */

void init_primitives(void)
{
    int k;
    memset(prim_data, 0, (sizeof(prim_info) * (last_cmd + 1)));
    memset(prim, 0, (sizeof(two_halves) * (prim_size + 1)));
    memset(prim_eqtb, 0, (sizeof(memory_word) * (prim_size + 1)));
    for (k = 0; k <= prim_size; k++)
        prim_eq_type(k) = undefined_cs_cmd;
}

void ini_init_primitives(void)
{
    prim_used = prim_size;      /* nothing is used */
}


/* The value of |hash_prime| should be roughly 85\pct! of |hash_size|, and it
   should be a prime number.  The theory of hashing tells us to expect fewer
   than two table probes, on the average, when the search is successful.
   [See J.~S. Vitter, {\sl Journal of the ACM\/ \bf30} (1983), 231--258.]
   @^Vitter, Jeffrey Scott@>
*/

static halfword compute_hash(const char *j, pool_pointer l, halfword prime_number)
{
    pool_pointer k;
    halfword h = (unsigned char) *j;
    for (k = 1; k <= l - 1; k++) {
        h = h + h + (unsigned char) *(j + k);
        while (h >= prime_number)
            h = h - prime_number;
    }
    return h;
}


/*  Here is the subroutine that searches the primitive table for an identifier */

pointer prim_lookup(str_number s)
{
    integer h;                  /* hash code */
    pointer p;                  /* index in |hash| array */
    pool_pointer j, l;
    if (s < string_offset) {
        p = s;
        if ((p < 0) || (get_prim_eq_type(p) == undefined_cs_cmd)) {
            p = undefined_primitive;
        }
    } else {
        j = str_start_macro(s);
        if (s == str_ptr)
            l = cur_length;
        else
            l = length(s);
        h = compute_hash((char *) (str_pool + j), l, prim_prime);
        p = h + prim_base;      /* we start searching here; note that |0<=h<hash_prime| */
        while (1) {
            if (prim_text(p) > 0)
                if (length(prim_text(p)) == l)
                    if (str_eq_str(prim_text(p), s))
                        goto FOUND;
            if (prim_next(p) == 0) {
                if (no_new_control_sequence) {
                    p = undefined_primitive;
                } else {
                    /* Insert a new primitive after |p|, then make |p| point to it */
                    if (prim_text(p) > 0) {
                        do {    /* search for an empty location in |prim| */
                            if (prim_is_full)
                                overflow_string("primitive size", prim_size);
                            decr(prim_used);
                        } while (prim_text(prim_used) != 0);
                        prim_next(p) = prim_used;
                        p = prim_used;
                    }
                    prim_text(p) = s;
                }
                goto FOUND;
            }
            p = prim_next(p);
        }
    }
  FOUND:
    return p;
}

/* how to test a csname for primitive-ness */

boolean is_primitive(str_number csname)
{
    integer n, m;
    m = prim_lookup(csname);
    n = string_lookup(makecstring(csname), length(csname));
    return ((n != undefined_cs_cmd) &&
            (m != undefined_primitive) &&
            (eq_type(n) == prim_eq_type(m)) && (equiv(n) == prim_equiv(m)));
}


/* a few simple accessors */

quarterword get_prim_eq_type(integer p)
{
    return prim_eq_type(p);
}

quarterword get_prim_origin(integer p)
{
    return prim_origin(p);
}

halfword get_prim_equiv(integer p)
{
    return prim_equiv(p);
}

str_number get_prim_text(integer p)
{
    return prim_text(p);
}


/* dumping and undumping */

void dump_primitives(void)
{
    int p, q;
    for (p = 0; p <= prim_size; p++)
        dump_hh(prim[p]);
    for (p = 0; p <= prim_size; p++)
        dump_wd(prim_eqtb[p]);
    for (p = 0; p <= last_cmd; p++) {
        dump_int(prim_data[p].offset);
        dump_int(prim_data[p].subids);
        for (q = 0; q < prim_data[p].subids; q++) {
            dump_int(prim_data[p].names[q]);
        }
    }
}

void undump_primitives(void)
{
    int p, q;
    for (p = 0; p <= prim_size; p++)
        undump_hh(prim[p]);
    for (p = 0; p <= prim_size; p++)
        undump_wd(prim_eqtb[p]);

    for (p = 0; p <= last_cmd; p++) {
        undump_int(prim_data[p].offset);
        undump_int(prim_data[p].subids);
        if (prim_data[p].subids > 0) {
            prim_data[p].names =
                (str_number *) xcalloc((prim_data[p].subids),
                                       sizeof(str_number *));
        }
        for (q = 0; q < prim_data[p].subids; q++) {
            undump_int(prim_data[p].names[q]);
        }
    }
}

/* 
   We need to put \TeX's ``primitive'' control sequences into the hash
   table, together with their command code (which will be the |eq_type|)
   and an operand (which will be the |equiv|). The |primitive| procedure
   does this, in a way that no \TeX\ user can. The global value |cur_val|
   contains the new |eqtb| pointer after |primitive| has acted.
*/

/* Because the definitions of the actual user-accessible name of a
   primitive can be postponed until runtime, the function |primitive_def|
   is needed that does nothing except creating the control sequence name. 
*/

void primitive_def(char *s, size_t l, quarterword c, halfword o)
{
    int nncs = no_new_control_sequence;
    no_new_control_sequence = false;
    cur_val = string_lookup(s, l);      /* this creates the |text()| string */
    no_new_control_sequence = nncs;
    eq_level(cur_val) = level_one;
    eq_type(cur_val) = c;
    equiv(cur_val) = o;
}

/* The function |store_primitive_name| sets up the bookkeeping for the
   reverse lookup. It is quite paranoid, because it is easy to mess this up
   accidentally.

   The |offset| is needed because sometimes character codes (in |o|)
   are indices into |eqtb| or are offset by a magical value to make
   sure they do not conflict with something else. We don't want the
   |prim_data[c].names| to have too many entries as it will just be
   wasted room, so |offset| is substracted from |o| because creating
   or accessing the array. The |assert(idx<=0xFFFF)| is not strictly
   needed, but it helps catch errors of this kind.
*/

static void
store_primitive_name(str_number s, quarterword c, halfword o, halfword offset)
{
    int idx;
    if (prim_data[c].offset != 0 && prim_data[c].offset != offset) {
        assert(false);
    }
    prim_data[c].offset = offset;
    idx = ((int) o - offset);
    assert(idx >= 0);
    assert(idx <= 0xFFFF);
    if (prim_data[c].subids < (idx + 1)) {
        str_number *new =
            (str_number *) xcalloc((idx + 1), sizeof(str_number *));
        if (prim_data[c].names != NULL) {
            assert(prim_data[c].subids);
            memcpy(new, (prim_data[c].names),
                   (prim_data[c].subids * sizeof(str_number)));
            free(prim_data[c].names);
        }
        prim_data[c].names = new;
        prim_data[c].subids = idx + 1;
    }
    prim_data[c].names[idx] = s;
}

/* Compared to tex82, |primitive| has two extra parameters. The |off| is an offset 
   that will be passed on to |store_primitive_name|, the |cmd_origin| is the bit
   that is used to group primitives by originator.
*/

void
primitive(str_number ss, quarterword c, halfword o, halfword off,
          int cmd_origin)
{
    str_number s;               /* actual |str_number| used */
    integer prim_val;           /* needed to fill |prim_eqtb| */
    char *thes;
    assert(o >= off);
    if (ss < string_offset) {
        if (ss > 127)
            tconfusion("prim"); /* should be ASCII */
        append_char(ss);
        s = make_string();
    } else {
        s = ss;
    }
    thes = makecstring(s);
    if (cmd_origin == tex_command || cmd_origin == core_command) {
        primitive_def(thes, strlen(thes), c, o);
    }
    prim_val = prim_lookup(s);
    prim_origin(prim_val) = cmd_origin;
    prim_eq_type(prim_val) = c;
    prim_equiv(prim_val) = o;
    store_primitive_name(s, c, o, off);
}


/* 
 * Here is a helper that does the actual hash insertion.
 */

static halfword insert_id(halfword p, const unsigned char *j, pool_pointer l)
{
    integer d;
    const unsigned char *k;
    /* This code far from ideal: the existance of |hash_extra| changes
       all the potential (short) coalesced lists into a single (long)
       one. This will create a slowdown. */
    if (text(p) > 0) {
        if (hash_high < hash_extra) {
            incr(hash_high);
            /* can't use eqtb_top here (perhaps because that is not finalized 
               yet when called from |primitive|?) */
            next(p) = hash_high + get_eqtb_size();
            p = next(p);
        } else {
            do {
                if (hash_is_full)
                    overflow_string("hash size", hash_size + hash_extra);
                decr(hash_used);
            } while (text(hash_used) != 0);     /* search for an empty location in |hash| */
            next(p) = hash_used;
            p = hash_used;
        }
    }
    check_pool_overflow((pool_ptr + l));
    d = cur_length;
    while (pool_ptr > str_start_macro(str_ptr)) {
        /* move current string up to make room for another */
        decr(pool_ptr);
        str_pool[pool_ptr + l] = str_pool[pool_ptr];
    }
    for (k = j; k <= j + l - 1; k++)
        append_char(*k);
    text(p) = make_string();
    pool_ptr = pool_ptr + d;
    incr(cs_count);
    return p;
}


/*
 Here is the subroutine that searches the hash table for an identifier
 that matches a given string of length |l>1| appearing in |buffer[j..
 (j+l-1)]|. If the identifier is found, the corresponding hash table address
 is returned. Otherwise, if the global variable |no_new_control_sequence|
 is |true|, the dummy address |undefined_control_sequence| is returned.
 Otherwise the identifier is inserted into the hash table and its location
 is returned.
*/


pointer id_lookup(integer j, integer l)
{                               /* search the hash table */
    integer h;                  /* hash code */
    pointer p;                  /* index in |hash| array */

    h = compute_hash((char *) (buffer + j), l, hash_prime);
    p = h + hash_base;          /* we start searching here; note that |0<=h<hash_prime| */
    while (1) {
        if (text(p) > 0)
            if (length(text(p)) == l)
                if (str_eq_buf(text(p), j))
                    goto FOUND;
        if (next(p) == 0) {
            if (no_new_control_sequence) {
                p = static_undefined_control_sequence;
            } else {
                p = insert_id(p, (buffer + j), l);
            }
            goto FOUND;
        }
        p = next(p);
    }
  FOUND:
    return p;
}

/*
 * Here is a similar subroutine for finding a primitive in the hash.
 * This one is based on a C string.
 */


pointer string_lookup(const char *s, size_t l)
{                               /* search the hash table */
    integer h;                  /* hash code */
    pointer p;                  /* index in |hash| array */
    h = compute_hash(s, l, hash_prime);
    p = h + hash_base;          /* we start searching here; note that |0<=h<hash_prime| */
    while (1) {
        if (text(p) > 0)
            if (str_eq_cstr(text(p), s, l))
                goto FOUND;
        if (next(p) == 0) {
            if (no_new_control_sequence) {
                p = static_undefined_control_sequence;
            } else {
                p = insert_id(p, (const unsigned char *) s, l);
            }
            goto FOUND;
        }
        p = next(p);
    }
  FOUND:
    return p;
}

/* The |print_cmd_chr| routine prints a symbolic interpretation of a
   command code and its modifier. This is used in certain `\.{You can\'t}'
   error messages, and in the implementation of diagnostic routines like
   \.{\\show}.

   The body of |print_cmd_chr| use to be  a rather tedious listing of print
   commands, and most of it was essentially an inverse to the |primitive|
   routine that enters a \TeX\ primitive into |eqtb|. 

   Thanks to |prim_data|, there is no need for all that tediousness. What 
   is left of |primt_cnd_chr| are just the exceptions to the general rule
   that the  |cmd,chr_code| pair represents in a single primitive command.
*/

#define chr_cmd(A) do { tprint(A); print(chr_code); } while (0)

static void prim_cmd_chr(quarterword cmd, halfword chr_code)
{
    int idx = chr_code - prim_data[cmd].offset;
    if (cmd <= last_cmd &&
        idx >= 0 && idx < prim_data[cmd].subids &&
        prim_data[cmd].names != NULL && prim_data[cmd].names[idx] != 0) {
        tprint("\\");
        print(prim_data[cmd].names[idx]);
    } else {
        /* TEX82 didn't print the |cmd,idx| information, but it may be useful */
        tprint("[unknown command code! (");
        print_int(cmd);
        tprint(", ");
        print_int(idx);
        tprint(")]");
    }
}

void print_cmd_chr(quarterword cmd, halfword chr_code)
{
    integer n;                  /* temp variable */
    switch (cmd) {
    case left_brace_cmd:
        chr_cmd("begin-group character ");
        break;
    case right_brace_cmd:
        chr_cmd("end-group character ");
        break;
    case math_shift_cmd:
        chr_cmd("math shift character ");
        break;
    case mac_param_cmd:
        chr_cmd("macro parameter character ");
        break;
    case sup_mark_cmd:
        chr_cmd("superscript character ");
        break;
    case sub_mark_cmd:
        chr_cmd("subscript character ");
        break;
    case endv_cmd:
        tprint("end of alignment template");
        break;
    case spacer_cmd:
        chr_cmd("blank space ");
        break;
    case letter_cmd:
        chr_cmd("the letter ");
        break;
    case other_char_cmd:
        chr_cmd("the character ");
        break;
    case tab_mark_cmd:
        if (chr_code == span_code)
            tprint_esc("span");
        else
            chr_cmd("alignment tab character ");
        break;
    case if_test_cmd:
        if (chr_code >= unless_code)
            tprint_esc("unless");
        prim_cmd_chr(cmd, (chr_code % unless_code));
        break;
    case char_given_cmd:
        tprint_esc("char");
        print_hex(chr_code);
        break;
    case math_given_cmd:
        tprint_esc("mathchar");
        show_mathcode_value(mathchar_from_integer(chr_code, tex_mathcode));
        break;
    case omath_given_cmd:
        tprint_esc("omathchar");
        show_mathcode_value(mathchar_from_integer(chr_code, aleph_mathcode));
        break;
    case xmath_given_cmd:
        tprint_esc("Umathchar");
        show_mathcode_value(mathchar_from_integer(chr_code, xetex_mathcode));
        break;
    case set_font_cmd:
        tprint("select font ");
        tprint(font_name(chr_code));
        if (font_size(chr_code) != font_dsize(chr_code)) {
            tprint(" at ");
            print_scaled(font_size(chr_code));
            tprint("pt");
        }
        break;
    case undefined_cs_cmd:
        tprint("undefined");
        break;
    case call_cmd:
    case long_call_cmd:
    case outer_call_cmd:
    case long_outer_call_cmd:
        n = cmd - call_cmd;
        if (info(link(chr_code)) == protected_token)
            n = n + 4;
        if (odd(n / 4))
            tprint_esc("protected");
        if (odd(n))
            tprint_esc("long");
        if (odd(n / 2))
            tprint_esc("outer");
        if (n > 0)
            tprint(" ");
        tprint("macro");
        break;
    case extension_cmd:
        if (chr_code < prim_data[cmd].subids &&
            prim_data[cmd].names[chr_code] != 0) {
            prim_cmd_chr(cmd, chr_code);
        } else {
            tprint("[unknown extension! (");
            print_int(chr_code);
            tprint(")]");

        }
        break;
    case set_ocp_cmd:
        tprint("select ocp ");
        slow_print(ocp_name(chr_code));
        break;
    case set_ocp_list_cmd:
        tprint("select ocp list ");
        break;
    case assign_glue_cmd:
    case assign_mu_glue_cmd:
        if (chr_code < skip_base) {
            print_skip_param(chr_code - glue_base);
        } else if (chr_code < mu_skip_base) {
            tprint_esc("skip");
            print_int(chr_code - skip_base);
        } else {
            tprint_esc("muskip");
            print_int(chr_code - mu_skip_base);
        }
        break;
    case assign_toks_cmd:
        if (chr_code >= toks_base) {
            tprint_esc("toks");
            print_int(chr_code - toks_base);
        } else {
            prim_cmd_chr(cmd, chr_code);
        }
        break;
    case assign_int_cmd:
        if (chr_code < count_base) {
            print_param(chr_code - int_base);
        } else {
            tprint_esc("count");
            print_int(chr_code - count_base);
        }
        break;
    case assign_attr_cmd:
        tprint_esc("attribute");
        print_int(chr_code - attribute_base);
        break;
    case assign_dimen_cmd:
        if (chr_code < scaled_base) {
            print_length_param(chr_code - dimen_base);
        } else {
            tprint_esc("dimen");
            print_int(chr_code - scaled_base);
        }
        break;
    default:
        /* these are most commands, actually */
        prim_cmd_chr(cmd, chr_code);
        break;
    }
}
