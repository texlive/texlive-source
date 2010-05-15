/* readable.c: check if a filename is a readable non-directory file.

   Copyright 1993, 1995, 1996, 2008 Karl Berry.
   Copyright 1998, 1999, 2000, 2001, 2005 Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/readable.h>
#include <kpathsea/tex-hush.h>
#include <kpathsea/truncate.h>


/* If access can read FN, run stat (assigning to stat buffer ST) and
   check that fn is not a directory.  Don't check for just being a
   regular file, as it is potentially useful to read fifo's or some
   kinds of devices.  */

#ifdef __DJGPP__
/* `stat' is way too expensive for such a simple job.  */
#define READABLE(fn, st) \
  (access (fn, R_OK) == 0 && access (fn, D_OK) == -1)
#elif WIN32
/* Warning: st must be an unsigned int under Win32 */
static boolean
READABLE(const_string fn, unsigned int st)
{
  if ((st = GetFileAttributes(fn)) != 0xFFFFFFFF) {
      /* succeeded */
      errno = 0;
  } else {
      switch(GetLastError()) {
      case ERROR_BUFFER_OVERFLOW:
	  errno = ENAMETOOLONG;
	  break;
      case ERROR_ACCESS_DENIED:
	  errno = EACCES;
	  break;
      default :
          errno = EIO;		/* meaningless, will make ret=NULL later */
	  break;
      }
  }
  return ((st != 0xFFFFFFFF) &&
		  !(st & FILE_ATTRIBUTE_DIRECTORY));
}
#else
#define READABLE(fn, st) \
  (access (fn, R_OK) == 0 && stat (fn, &(st)) == 0 && !S_ISDIR (st.st_mode))
#endif

/* POSIX invented the brain-damage of not necessarily truncating
   filename components; the system's behavior is defined by the value of
   the symbol _POSIX_NO_TRUNC, but you can't change it dynamically!

   Generic const return warning.  See extend-fname.c.  */

string
kpathsea_readable_file (kpathsea kpse, const_string name)
{
  string ret;

#ifdef WIN32
  unsigned int st = 0;
#else /* ! WIN32 */
  struct stat st;
#endif

  kpathsea_normalize_path(kpse, (string)name);
  if (READABLE (name, st)) {
      ret = (string) name;
#ifdef ENAMETOOLONG
  } else if (errno == ENAMETOOLONG) {
      ret = kpathsea_truncate_filename (kpse, name);

      /* Perhaps some other error will occur with the truncated name, so
         let's call access again.  */
      if (!READABLE (ret, st)) { /* Failed.  */
          if (ret != name) free (ret);
          ret = NULL;
      }
#endif /* ENAMETOOLONG */
  } else { /* Some other error.  */
      if (errno == EACCES) { /* Maybe warn them if permissions are bad.  */
          if (!kpathsea_tex_hush (kpse, "readable")) {
              perror (name);
          }
      }
      ret = NULL;
  }
  return ret;
}

#if defined (KPSE_COMPAT_API)
string
kpse_readable_file (const_string name)
{
    return kpathsea_readable_file (kpse_def, name);
}
#endif

