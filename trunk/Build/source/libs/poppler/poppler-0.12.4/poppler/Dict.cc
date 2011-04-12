//========================================================================
//
// Dict.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2005 Kristian Høgsberg <krh@redhat.com>
// Copyright (C) 2006 Krzysztof Kowalczyk <kkowalczyk@gmail.com>
// Copyright (C) 2007-2008 Julien Rebetez <julienr@svn.gnome.org>
// Copyright (C) 2008 Albert Astals Cid <aacid@kde.org>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include <config.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stddef.h>
#include <string.h>
#include "goo/gmem.h"
#include "Object.h"
#include "XRef.h"
#include "Dict.h"

//------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------

Dict::Dict(XRef *xrefA) {
  xref = xrefA;
  entries = NULL;
  size = length = 0;
  ref = 1;
}

Dict::Dict(Dict* dictA) {
  xref = dictA->xref;
  size = length = dictA->length;
  ref = 1;

  entries = (DictEntry *)gmallocn(size, sizeof(DictEntry));
  for (int i=0; i<length; i++) {
    entries[i].key = strdup(dictA->entries[i].key);
    dictA->entries[i].val.copy(&entries[i].val);
  }
}

Dict::~Dict() {
  int i;

  for (i = 0; i < length; ++i) {
    gfree(entries[i].key);
    entries[i].val.free();
  }
  gfree(entries);
}

void Dict::add(char *key, Object *val) {
  if (length == size) {
    if (length == 0) {
      size = 8;
    } else {
      size *= 2;
    }
    entries = (DictEntry *)greallocn(entries, size, sizeof(DictEntry));
  }
  entries[length].key = key;
  entries[length].val = *val;
  ++length;
}

inline DictEntry *Dict::find(char *key) {
  int i;

  for (i = length - 1; i >=0; --i) {
    if (!strcmp(key, entries[i].key))
      return &entries[i];
  }
  return NULL;
}

void Dict::remove(char *key) {
  int i; 
  bool found = false;
  DictEntry tmp;
  if(length == 0) return;

  for(i=0; i<length; i++) {
    if (!strcmp(key, entries[i].key)) {
      found = true;
      break;
    }
  }
  if(!found) return;
  //replace the deleted entry with the last entry
  length -= 1;
  tmp = entries[length];
  if (i!=length) //don't copy the last entry if it is deleted 
    entries[i] = tmp;
}

void Dict::set(char *key, Object *val) {
  DictEntry *e;
  e = find (key);
  if (e) {
    e->val.free();
    e->val = *val;
  } else {
    add (copyString(key), val);
  }
}


GBool Dict::is(char *type) {
  DictEntry *e;

  return (e = find("Type")) && e->val.isName(type);
}

Object *Dict::lookup(char *key, Object *obj) {
  DictEntry *e;

  return (e = find(key)) ? e->val.fetch(xref, obj) : obj->initNull();
}

Object *Dict::lookupNF(char *key, Object *obj) {
  DictEntry *e;

  return (e = find(key)) ? e->val.copy(obj) : obj->initNull();
}

GBool Dict::lookupInt(const char *key, const char *alt_key, int *value)
{
  Object obj1;
  GBool success = gFalse;
  
  lookup ((char *) key, &obj1);
  if (obj1.isNull () && alt_key != NULL) {
    obj1.free ();
    lookup ((char *) alt_key, &obj1);
  }
  if (obj1.isInt ()) {
    *value = obj1.getInt ();
    success = gTrue;
  }

  obj1.free ();

  return success;
}

char *Dict::getKey(int i) {
  return entries[i].key;
}

Object *Dict::getVal(int i, Object *obj) {
  return entries[i].val.fetch(xref, obj);
}

Object *Dict::getValNF(int i, Object *obj) {
  return entries[i].val.copy(obj);
}
