/* sfd.c */

/************************************************************************

  Part of the dvipng distribution

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA.

  Copyright (C) 2006 Jan-�ke Larsson

************************************************************************/

#include "dvipng.h"
struct subfont* subfontp=NULL;

struct subfont* ReadSubfont(char* sfdname, char *infix) 
{
  char *pos,*max,*sfdfile=NULL;
  struct subfont* sfdp=NULL;
  struct filemmap fmmap;
  
  /* OK, find subfont and look for correct infix */
#ifdef HAVE_KPSE_ENC_FORMATS
  TEMPSTR(sfdfile,kpse_find_file(sfdname,kpse_sfd_format,false));
#endif
  if (sfdfile == NULL) {
    Warning("subfont file %s could not be found",sfdname);
    return(NULL);
  }
  DEBUG_PRINT((DEBUG_FT|DEBUG_ENC),("\n  OPEN SUBFONT:\t'%s'", sfdfile));
  if (MmapFile(sfdfile,&fmmap)) return(NULL);
  pos=fmmap.dp_mmap;
  max=fmmap.dp_mmap+fmmap.size;
  while(pos<max && (*pos==' ' || *pos=='\n' || *pos=='\t')) pos++;
  while (pos<max && *pos=='#') {
    while(pos<max && *pos!='\n') pos++;
    while(pos<max && (*pos==' ' || *pos=='\n' || *pos=='\t')) pos++;
  }
  while(pos+strlen(infix)<max 
	&& (strncmp(pos,infix,strlen(infix))!=0
	    || (pos[strlen(infix)]!=' ' && pos[strlen(infix)]!='\t'))) {
    while(pos+1<max && (*(pos+1)!='\n' || *pos=='\\'))
      pos++;
    pos++;
    while(pos<max && (*pos==' ' || *pos=='\n' || *pos=='\t')) pos++;
    while (pos<max && *pos=='#') {
      while(pos<max && *pos!='\n') pos++;
      while(pos<max && (*pos==' ' || *pos=='\n' || *pos=='\t')) pos++;
    }
  }
  pos=pos+strlen(infix);
  if (pos<max) {
    int number,range,codepoint=0;

    if ((sfdp = calloc(sizeof(struct subfont)+strlen(sfdname)+1
		       +strlen(infix)+1,1))==NULL) {
      Warning("cannot alloc space for subfont",sfdfile);
      UnMmapFile(&fmmap);
      return(NULL);
    }
    sfdp->name=(char*)sfdp+sizeof(struct subfont);
    strcpy(sfdp->name,sfdname);
    sfdp->infix=(char*)sfdp+sizeof(struct subfont)+strlen(sfdname)+1;
    strcpy(sfdp->infix,infix);
    sfdp->encoding=FT_ENCODING_UNICODE;
    while (pos<max && *pos != '\n') {
      number=strtol(pos,&pos,0);
      while(pos<max && (*pos==' ' || *pos=='\t')) pos++;
      switch(*pos) {
      case ':': 
	codepoint=number;
	pos++;
	break;
      case '_':
	range=strtol(pos+1,&pos,0);
	while(codepoint<256 && number<range) {
	  sfdp->charindex[codepoint]=number;
	  DEBUG_PRINT(DEBUG_ENC,("\n  SUBFONT MAP %d %d",codepoint,number)); 
	  number++;
	  codepoint++;
	}
      default:
	if (codepoint<256)
	  sfdp->charindex[codepoint]=number;
	DEBUG_PRINT(DEBUG_ENC,("\n  SUBFONT MAP %d %d",codepoint,number)); 
      }
      while(pos<max && (*pos==' ' || *pos=='\t')) pos++;
      while(pos<max && *pos=='\\' && pos+1<max && *(pos+1)=='\n') {
	pos+=2;
	while(pos<max && (*pos==' ' || *pos=='\t')) pos++;
      }
    }
  }
  return (sfdp);
}

struct psfontmap* FindSubFont(struct psfontmap* entry, char* fontname) 
{
  struct subfont *temp=subfontp;
  char *sfdspec=entry->tfmname,*sfdwant=fontname,
    *sfdname,*infix,*postfix;

  while (*sfdspec!='\0' && *sfdspec==*sfdwant) {
    sfdspec++;
    sfdwant++;
  }
  /* Find delimiter */
  if (*sfdspec!='@') 
    return(NULL);
  sfdspec++;
  postfix=sfdspec;
  while (*postfix!='\0' && *postfix!='@')
    postfix++;
  if (*postfix!='@')
    return(NULL);
  /* Extract subfont name */
  sfdname=alloca(postfix-sfdspec+1);
  strncpy(sfdname,sfdspec,postfix-sfdspec);
  sfdname[postfix-sfdspec]='\0';
  /* Check postfix */
  postfix++;
  if (strcmp(sfdwant+strlen(sfdwant)-strlen(postfix),postfix)!=0)
    return(NULL);
  /* Extract infix */
  infix=alloca(strlen(sfdwant)-strlen(postfix)+1);
  strncpy(infix,sfdwant,strlen(sfdwant)-strlen(postfix));
  infix[strlen(sfdwant)-strlen(postfix)]='\0';
  DEBUG_PRINT(DEBUG_ENC,("\n  SUBFONT %s %s %s",fontname,sfdname,infix)); 
  /* Find subfont */
  while(temp!=NULL 
	&& (strcmp(sfdname,temp->name)!=0 || strcmp(infix,temp->infix)!=0))
    temp=temp->next;
  if (temp==NULL) {
    temp=ReadSubfont(sfdname,infix);
    if (temp!=NULL) {
      temp->next=subfontp;
      subfontp=temp;
    }
  }
  entry=NewPSFont(entry);
  if (entry!=NULL) {
    entry->tfmname=copyword(fontname);
    entry->subfont=temp;
  }
  return(entry);
}

void ClearSubfont(void)
{
  struct subfont *temp=subfontp;

  while(temp!=NULL) {
    subfontp=subfontp->next;
    free(temp);
    temp=subfontp;
  }
}

