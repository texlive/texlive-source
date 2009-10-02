/*****
 * picture.h
 * Andy Hamerlindl 2002/06/06
 *
 * Stores a picture as a list of drawElements and handles its output to
 * PostScript.
 *****/

#ifndef PICTURE_H
#define PICTURE_H

#include <sstream>
#include <iostream>

#include "drawelement.h"

namespace camp {

class picture : public gc {
private:
  bool labels;
  size_t lastnumber;
  size_t lastnumber3;
  transform T; // Keep track of accumulative picture transform
  bbox b;
  bbox b_cached;   // Cached bounding box
  boxvector labelbounds;
  bboxlist bboxstack;
  bool transparency;
  
  static bool epsformat,pdfformat,svgformat,xobject,pdf,Labels;
  static double paperWidth,paperHeight;

public:
  bbox3 b3; // 3D bounding box
  
  typedef mem::list<drawElement*> nodelist;
  nodelist nodes;
  
  picture() : labels(false), lastnumber(0), lastnumber3(0), T(identity),
              transparency(false) {}
  
  // Destroy all of the owned picture objects.
  ~picture();

  // Prepend an object to the picture.
  void prepend(drawElement *p);
  
  // Append an object to the picture.
  void append(drawElement *p);

  // Enclose each layer with begin and end.
  void enclose(drawElement *begin, drawElement *end);
  
  // Add the content of another picture.
  void add(picture &pic);
  void prepend(picture &pic);
  
  bool havelabels();
  bool have3D();

  bbox bounds();
  bbox3 bounds3();

  // Compute bounds on ratio (x,y)/z for 3d picture (not cached).
  pair ratio(double (*m)(double, double));
  
  bool Transparency() {
    return transparency;
  }
  
  int epstopdf(const string& epsname, const string& pdfname);
  
  bool texprocess(const string& texname, const string& tempname,
                  const string& prefix, const pair& bboxshift); 
    
  bool postprocess(const string& prename, const string& outname, 
                   const string& outputformat, double magnification,
                   bool wait, bool view);
    
  // Ship the picture out to PostScript & TeX files.
  bool shipout(picture* preamble, const string& prefix,
               const string& format, double magnification=0.0,
               bool wait=false, bool view=true);
 
  void render(GLUnurbs *nurb, double size2,
              const triple &Min, const triple& Max, double perspective,
              bool transparent) const;
  bool shipout3(const string& prefix, const string& format,
                double width, double height, double angle, double zoom,
                const triple& m, const triple& M, const pair& shift, double *t,
                double *background, size_t nlights, triple *lights,
                double *diffuse, double *ambient, double *specular,
                bool viewportlighting, bool view);
  
  // PRC output
  bool shipout3(const string& prefix, vm::array *index, vm::array *center);
  
  bool reloadPDF(const string& Viewer, const string& outname) const;
  
  picture *transformed(const transform& t);
  picture *transformed(const vm::array& t);
  
  bool null() {
    return nodes.empty();
  }
  
};

inline picture *transformed(const transform& t, picture *p)
{
  return p->transformed(t);
}

inline picture *transformed(const vm::array& t, picture *p)
{
  return p->transformed(t);
}

void texinit();
int opentex(const string& texname, const string& prefix);

const char *texpathmessage();
  
} //namespace camp

#endif
