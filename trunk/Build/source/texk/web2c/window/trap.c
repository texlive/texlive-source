/* trap.c -- generate output for the trap test.  See comments in
   lib/texmf.c for what each routine does.  */

#define EXTERN extern
#include "../mfd.h"


/* No #ifdef for the whole file, because we always want to support this.  */

boolean mf_trap_initscreen(void);
void mf_trap_updatescreen(void);
void mf_trap_blankrectangle(screencol left,
                            screencol right,
                            screenrow top,
                            screenrow bottom);
void mf_trap_paintrow(screenrow row,
                      pixelcolor init_color,
                      transspec transition_vector,
                      screencol vector_size);

/* This returns true if we can do window operations, else false.  */

boolean
mf_trap_initscreen(void)
{
  return 1;
}

void
mf_trap_updatescreen(void)
{
  fputs ("Calling UPDATESCREEN\n", logfile);
}

void
mf_trap_blankrectangle(screencol left,
                       screencol right,
                       screenrow top,
                       screenrow bottom)
{
  fprintf (logfile, "\nCalling BLANKRECTANGLE(%ld,%ld,%ld,%ld)\n",
           (long)left, (long)right, (long)top, (long)bottom);
}

void
mf_trap_paintrow(screenrow row,
                 pixelcolor init_color,
                 transspec transition_vector,
                 screencol vector_size)
{
  unsigned k;

  fprintf (logfile, "Calling PAINTROW(%ld,%ld;", (long)row, (long)init_color);
  for (k = 0; k <= vector_size; k++) {
    fprintf (logfile, "%ld", (long)transition_vector[k]);
    if (k != vector_size)
      putc (',', logfile);
    }
  fputs (")\n", logfile);
}
