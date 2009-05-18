/*****
 * glrender.h
 * Render 3D Bezier paths and surfaces.
 *****/

#ifndef GLRENDER_H
#define GLRENDER_H

#include "common.h"

#ifdef HAVE_LIBGL

#include <csignal>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#ifdef GLU_TESS_CALLBACK_TRIPLEDOT
typedef GLvoid (* _GLUfuncptr)(...);
#else
typedef GLvoid (* _GLUfuncptr)();
#endif
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

namespace camp {
class picture;

inline void store(GLfloat *f, double *C)
{
  f[0]=C[0];
  f[1]=C[1];
  f[2]=C[2];
}

inline void store(GLfloat *control, const camp::triple& v)
{
  control[0]=v.getx();
  control[1]=v.gety();
  control[2]=v.getz();
}
}

namespace gl {
void glrender(const string& prefix, const camp::picture* pic,
              const string& format, double width, double height,
              double angle, const camp::triple& m, const camp::triple& M,
              double *t, double *background, size_t nlights,
              camp::triple *lights, double *diffuse, double *ambient,
              double *specular, bool viewportlighting, bool view, int oldpid=0);
}

#else
typedef void GLUnurbs;
typedef float GLfloat;
#endif

#endif


