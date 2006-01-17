/* newer - true if any source file is newer than the target file.
 *
 * Public domain.
 *
 * This program used to be Copyright 1990 - 1995 by AT&T Bell Laboratories,
 * and the original version was written by John Hobby.  It has since been
 * placed in the public domain.
 */

#ifdef HAVE_CONFIG_H
#include "c-auto.h"     /* In case we need, e.g., _POSIX_SOURCE */
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

/* Exit codes are 0 for success ("true") and 1 for failure ("false"). */
#define exit_false 1
#define exit_true  0

/* We may have high-res timers in struct stat.  If we do, use them.  */
#if HAVE_ST_MTIM
#define NEWER(S,T) (S.st_mtim.tv_sec > T.st_mtim.tv_sec ||              \
                    (S.st_mtim.tv_sec  == T.st_mtim.tv_sec &&           \
                     S.st_mtim.tv_nsec >= T.st_mtim.tv_nsec))
#else
#define NEWER(S,T) (S.st_mtime >= T.st_mtime)
#endif

int i;
int verbose = 1;
int missing_source = 0;
int missing_target = 0;
int result = exit_false;

/*
 *      newer src1 [ src2 ... srcN ] target
 *
 *      returns 0 if files `src1' ... `srcN' exist and 
 *      at least one of them is not older than `target'
 *      or if `target' doesn't exist.
 */

int
main(int argc, char **argv)
{
    struct stat source_stat, target_stat;

    while ((argc > 1) && (argv[1][0] == '-')) {
        if (strcmp(argv[1], "-v") == 0 ||
            strcmp(argv[1], "-verbose") == 0 ||
            strcmp(argv[1], "--verbose") == 0) {

            verbose = 1;
            argv++;
            argc--;

        } else if (strcmp(argv[1], "-q") == 0 ||
                   strcmp(argv[1], "-quiet") == 0 ||
                   strcmp(argv[1], "--quiet") == 0) {

            verbose = 0;
            argv++;
            argc--;

        } else if (strcmp(argv[1], "-help") == 0 ||
                   strcmp(argv[1], "--help") == 0) {

            fputs("Usage: newer [OPTION] src1 [ src2 ... srcN ] target\n\
  Exit successfully if `src1' ... `srcN' exist and at least\n\
  one of them is not older than `target'.\n\
  Also exit successfully if `target' doesn't exist.\n\
\n\
--help      display this help and exit\n\
--quiet     do not print anything\n\
--verbose   list missing input files (default)\n\
--version   output version information and exit\n\n", stdout);
            fputs ("Email bug reports to metapost@tug.org.\n", stdout);
            exit(0);

        } else if (strcmp(argv[1], "-version") == 0 ||
                   strcmp(argv[1], "--version") == 0) {

            fputs("newer 0.901\n\
This program is in the public domain.\n\
Primary author of newer: John Hobby.\n", stdout);
            exit(0);

        } else {

            fprintf(stderr, "newer: Unknown option \"%s\"\n", argv[1]);
            fputs("Try `newer --help' for more information.\n", stderr);
            exit(1);

        }
    }
    
    /* do we have at least two arguments? */
    if (argc < 3) {
        fputs ("newer: Too few arguments.\n\
Try `newer --help' for more information.\n", stderr);
        exit(1);
    }
    
    /* check the target file */
    if (stat(argv[argc-1], &target_stat) < 0) {
        if (verbose) {
            fprintf(stderr, "newer: target file `%s' doesn't exist.\n",
                    argv[argc-1]);
        }
        missing_target = 1;
    }
    
    /* check the source files */
    for (i = 1; i < (argc-1); i++) {
        if (stat(argv[i], &source_stat) < 0) {
	    if (verbose) {
                fprintf (stderr, "newer: source file `%s' doesn't exist.\n", 
                         argv[i]);
            }
	    missing_source = 1;
        } else if (!missing_target) {
            if (NEWER(source_stat, target_stat)) {
                result = exit_true;
            }
        }
    }

    /* Adjust result for cases when source or target is missing:
     *   missing source => exit_false
     *   missing target => exit_true
     */
    if (missing_source) {
        result = exit_false;
    } else if (missing_target) {
        result = exit_true;
    }

    return result;
}
