/*****
 * pen.h
 * John Bowman 2003/03/23
 *
 *****/

#ifndef PEN_H
#define PEN_H

#include "transform.h"
#include "settings.h"
#include "bbox.h"
#include "common.h"
#include "path.h"

namespace camp {

static const double tex2ps=72.0/72.27;
static const double ps2tex=1.0/tex2ps;
  
static const string DEFPAT="<default>";
static const string DEFLATEXFONT="\\usefont{\\ASYencoding}{\\ASYfamily}{\\ASYseries}{\\ASYshape}";
static const string DEFCONTEXTFONT="modern";
static const string DEFTEXFONT="cmr12";
static const double DEFWIDTH=-1;
static const Int DEFCAP=-1;
static const Int DEFJOIN=-1;
static const double DEFMITER=0;
  
static const struct invisiblepen_t {} invisiblepen={};
static const struct setlinewidth_t {} setlinewidth={};
static const struct setfont_t {} setfont={};
static const struct setfontsize_t {} setfontsize={};
static const struct setpattern_t {} setpattern={};
static const struct setlinecap_t {} setlinecap={};
static const struct setlinejoin_t {} setlinejoin={};
static const struct setmiterlimit_t {} setmiterlimit={};
static const struct setoverwrite_t {} setoverwrite={};
static const struct initialpen_t {} initialpen={};
static const struct resolvepen_t {} resolvepen={};
  
static const string Cap[]={"square","round","extended"};
static const string Join[]={"miter","round","bevel"};
const Int nCap=sizeof(Cap)/sizeof(string);
const Int nJoin=sizeof(Join)/sizeof(string);
  
enum overwrite_t {DEFWRITE=-1,ALLOW,SUPPRESS,SUPPRESSQUIET,MOVE,MOVEQUIET};
static const string OverwriteTag[]={"Allow","Suppress","SupressQuiet",
                                    "Move","MoveQuiet"};
const Int nOverwrite=sizeof(OverwriteTag)/sizeof(string);
  
enum FillRule {DEFFILL=-1,ZEROWINDING,EVENODD};
static const string FillRuleTag[]={"ZeroWinding","EvenOdd"};

const Int nFill=sizeof(FillRuleTag)/sizeof(string);
  
enum BaseLine {DEFBASE=-1,NOBASEALIGN,BASEALIGN};
static const string BaseLineTag[]={"NoAlign","Align"};
const Int nBaseLine=sizeof(BaseLineTag)/sizeof(string);
  
enum ColorSpace {DEFCOLOR=0,INVISIBLE,GRAYSCALE,RGB,CMYK,PATTERN};
extern const size_t ColorComponents[];
static const string ColorDeviceSuffix[]={"","","Gray","RGB","CMYK",""};
const unsigned nColorSpace=sizeof(ColorDeviceSuffix)/sizeof(string);
  
  
  
class LineType
{
public:  
  string pattern;       // The string for the PostScript style line pattern.
  double offset;        // The offset in the pattern at which to start drawing.
  bool scale;           // Scale the line type values by the pen width?
  bool adjust;          // Adjust the line type values to fit the arclength?
  
  LineType(string pattern, double offset, bool scale, bool adjust) : 
    pattern(pattern), offset(offset), scale(scale), adjust(adjust) {}
};
  
static const LineType DEFLINE("default",0,true,true);
  
inline bool operator == (LineType a, LineType b) {
  return a.pattern == b.pattern && a.offset == b.offset && 
    a.scale == b.scale && a.adjust == b.adjust;
}
  
class Transparency
{
public:  
  string blend;
  double opacity;
  Transparency(string blend, double opacity) :
    blend(blend), opacity(opacity) {}
};
  
static const Transparency DEFTRANSP("Compatible",1.0);
  
inline bool operator == (Transparency a, Transparency b) {
  return a.blend == b.blend && a.opacity == b.opacity;
}
  
static const string BlendMode[]={"Compatible","Normal","Multiply","Screen",
                                 "Overlay","SoftLight","HardLight",
                                 "ColorDodge","ColorBurn","Darken",
                                 "Lighten","Difference","Exclusion",
                                 "Hue","Saturation","Color","Luminosity"};
const Int nBlendMode=sizeof(BlendMode)/sizeof(string);

static const transform nullTransform=transform(0.0,0.0,0.0,0.0,0.0,0.0);  
  
class pen;
pen& defaultpen();

class pen : public gc { 
  LineType line;

  // Width of line, in PS units.
  double linewidth;
  path P;               // A polygonal path defining a custom pen nib;
                        // nullpath means the default (typically circular) nib.
  string font;
  double fontsize;  
  double lineskip;  
  
  ColorSpace color;
  double r,g,b;         // RGB or CMY value
  double grey;          // grayscale or K value
  
  string pattern;       // The name of the user-defined fill/draw pattern
  FillRule fillrule;    // Zero winding-number (default) or even-odd rule
  BaseLine baseline;    // Align to TeX baseline?
  Transparency transparency;
  Int linecap;
  Int linejoin;
  double miterlimit;
  overwrite_t overwrite;
  
  // The transformation applied to the pen nib for calligraphic effects.
  // nullTransform means the default (typically identity) transformation.
  transform t;
  
public:
  static double pos0(double x) {return x >= 0 ? x : 0;}
  
  void greyrange() {if(grey > 1.0) grey=1.0;}
  
  void rgbrange() {
    double sat=rgbsaturation();
    if(sat > 1.0) {
      double scale=1.0/sat;
      r *= scale;
      g *= scale;
      b *= scale;
    }
  }
  
  void cmykrange() {
    double sat=cmyksaturation();
    if(sat > 1.0) {
      double scale=1.0/sat;
      r *= scale;
      g *= scale;
      b *= scale;
      grey *= scale;
    }
  }
  
  void colorless() {
    r=g=b=grey=0.0;
    color=DEFCOLOR;
  }
  
  pen() :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}

  pen(const LineType& line, double linewidth, const path& P,
      const string& font, double fontsize, double lineskip,
      ColorSpace color, double r, double g, double b,  double grey,
      const string& pattern, FillRule fillrule, BaseLine baseline,
      const Transparency& transparency,
      Int linecap, Int linejoin, double miterlimit,
      overwrite_t overwrite, const transform& t) :
    line(line), linewidth(linewidth), P(P),
    font(font), fontsize(fontsize), lineskip(lineskip), color(color),
    r(r), g(g), b(b), grey(grey),
    pattern(pattern), fillrule(fillrule), baseline(baseline),
    transparency(transparency),
    linecap(linecap), linejoin(linejoin), miterlimit(miterlimit),
    overwrite(overwrite), t(t) {}
      
  pen(invisiblepen_t) : 
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(INVISIBLE),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(setlinewidth_t, double linewidth) : 
    line(DEFLINE), linewidth(linewidth), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(path P) : 
    line(DEFLINE), linewidth(DEFWIDTH), P(P),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(const LineType& line) :
    line(line), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(setfont_t, string font) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(font), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(setfontsize_t, double fontsize, double lineskip) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(fontsize), lineskip(lineskip), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(setpattern_t, const string& pattern) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(PATTERN),
    r(0), g(0), b(0), grey(0),
    pattern(pattern), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(FillRule fillrule) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(fillrule), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(BaseLine baseline) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(baseline),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(const Transparency& transparency) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(transparency),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(setlinecap_t, Int linecap) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(linecap), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(setlinejoin_t, Int linejoin) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(linejoin), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(setmiterlimit_t, double miterlimit) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(miterlimit),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  pen(setoverwrite_t, overwrite_t overwrite) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(DEFCOLOR),
    r(0), g(0), b(0), grey(0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(overwrite), t(nullTransform) {}
  
  explicit pen(double grey) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(GRAYSCALE),
    r(0.0), g(0.0), b(0.0), grey(pos0(grey)),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform)
  {greyrange();}
  
  pen(double r, double g, double b) : 
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(RGB),
    r(pos0(r)), g(pos0(g)), b(pos0(b)),  grey(0.0), 
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform)
  {rgbrange();}
  
  pen(double c, double m, double y, double k) :
    line(DEFLINE), linewidth(DEFWIDTH), P(nullpath),
    font(""), fontsize(0.0), lineskip(0.0), color(CMYK),
    r(pos0(c)), g(pos0(m)), b(pos0(y)), grey(pos0(k)),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(DEFBASE),
    transparency(DEFTRANSP),
    linecap(DEFCAP), linejoin(DEFJOIN), miterlimit(DEFMITER),
    overwrite(DEFWRITE), t(nullTransform)
  {cmykrange();}
  
  // Construct one pen from another, resolving defaults
  pen(resolvepen_t, const pen& p) : 
    line(LineType(p.stroke(),p.line.offset,p.line.scale,p.line.adjust)),
    linewidth(p.width()), P(p.Path()),
    font(p.Font()), fontsize(p.size()), lineskip(p.Lineskip()),
    color(p.colorspace()),
    r(p.red()), g(p.green()), b(p.blue()), grey(p.gray()),
    pattern(""), fillrule(p.Fillrule()), baseline(p.Baseline()),
    transparency(Transparency(p.blend(), p.opacity())),
    linecap(p.cap()), linejoin(p.join()), miterlimit(p.miter()),
    overwrite(p.Overwrite()), t(p.getTransform()) {}
  
  static pen initialpen() {
    return pen(LineType("",0,true,true),0.5,nullpath,"",12.0*tex2ps,
               12.0*1.2*tex2ps,GRAYSCALE,
               0.0,0.0,0.0,0.0,"",ZEROWINDING,NOBASEALIGN,
               DEFTRANSP,1,1,10.0,ALLOW,identity);
  }
  
  pen(initialpen_t) : 
    line(DEFLINE), linewidth(-2.0), P(nullpath),
    font("<invalid>"), fontsize(-1.0), lineskip(-1.0), color(INVISIBLE),
    r(0.0), g(0.0), b(0.0), grey(0.0),
    pattern(DEFPAT), fillrule(DEFFILL), baseline(NOBASEALIGN),
    transparency(DEFTRANSP),linecap(-2), linejoin(-2), miterlimit(-1.0),
    overwrite(DEFWRITE), t(nullTransform) {}
  
  double width() const {
    return linewidth == DEFWIDTH ? defaultpen().linewidth : linewidth;
  }
  
  path Path() const {
    return P.empty() ? defaultpen().P : P;
  }
  
  double size() const {
    return fontsize == 0.0 ? defaultpen().fontsize : fontsize;
  }
  
  string Font() const {
    if(font.empty()) {
      if(defaultpen().font.empty()) {
        string texengine=settings::getSetting<string>("tex");
        if(settings::latex(texengine))
          return DEFLATEXFONT;
        else if(texengine == "none")
          return settings::getSetting<string>("textinitialfont");
        else {
          ostringstream buf;
  // Work around misalignment in ConTeXt switchtobodyfont if font is not found.
          if(texengine == "context")
            buf << "\\switchtobodyfont[" 
                << DEFCONTEXTFONT << "," << size()*ps2tex 
                << "pt]\\removeunwantedspaces%" << newl;
          else
            buf << "\\font\\ASYfont=" << DEFTEXFONT
              << " at " << size()*ps2tex << "pt\\ASYfont";
          return buf.str();
        }
      }
      else return defaultpen().font;
    }
    return font;
  }
  
  double Lineskip() const {
    return lineskip == 0.0 ? defaultpen().lineskip : lineskip;
  }
  
  string stroke() const {
    return line == DEFLINE ? defaultpen().line.pattern : line.pattern;
  }
  
  LineType linetype() const {
    return line == DEFLINE ? defaultpen().line : line;
  }
  
  void setstroke(const string& s) {line.pattern=s;}
  void setoffset(const double& offset) {line.offset=offset;}
  
  string fillpattern() const {
    return pattern == DEFPAT ? (string)"" : pattern;
  }
  
  FillRule Fillrule() const {
    return fillrule == DEFFILL ? defaultpen().fillrule : fillrule;
  }
  
  bool evenodd() const {
    return Fillrule() == EVENODD;
  }
  
  bool inside(Int count) const {
    return evenodd() ? count % 2 : count != 0;
  }
  
  BaseLine Baseline() const {
    return baseline == DEFBASE ? defaultpen().baseline : baseline;
  }
  
  Transparency transp() const {
    return transparency == DEFTRANSP ? defaultpen().transparency : transparency;
  }
  
  string blend() const {
    return transparency == DEFTRANSP ? defaultpen().transparency.blend :
      transparency.blend;
  }
  
  double opacity() const {
    return transparency == DEFTRANSP ? defaultpen().transparency.opacity :
      transparency.opacity;
  }
  
  Int cap() const {
    return linecap == DEFCAP ? defaultpen().linecap : linecap;
  }
  
  Int join() const {
    return linejoin == DEFJOIN ? defaultpen().linejoin : linejoin;
  }
  
  double miter() const {
    return miterlimit == DEFMITER ? defaultpen().miterlimit : miterlimit;
  }
  
  overwrite_t Overwrite() const {
    return overwrite == DEFWRITE ? defaultpen().overwrite : overwrite;
  }
  
  ColorSpace colorspace() const {
    return color == DEFCOLOR ? defaultpen().color : color;
  }
  
  bool invisible() const {return colorspace() == INVISIBLE;}
  
  bool grayscale() const {return colorspace() == GRAYSCALE;}
  
  bool rgb() const {return colorspace() == RGB;}
  
  bool cmyk() const {return colorspace() == CMYK;}
  
  double gray() const {return color == DEFCOLOR ? defaultpen().grey : grey;}
  
  double red() const {return color == DEFCOLOR ? defaultpen().r : r;}
  
  double green() const {return color == DEFCOLOR ? defaultpen().g : g;}
  
  double blue() const {return color == DEFCOLOR ? defaultpen().b : b;}
  
  double cyan() const {return red();}
  
  double magenta() const {return green();}
  
  double yellow() const {return blue();}
  
  double black() const {return gray();}
  
  double rgbsaturation() const {return max(max(r,g),b);}
  
  double cmyksaturation() const {return max(rgbsaturation(),black());}
  
  void greytorgb() {
    r=g=b=grey; grey=0.0;
    color=RGB;
  }
  
  void rgbtogrey() {
    grey=0.299*r+0.587*g+0.114*b; // Standard YUV luminosity coefficients
    r=g=b=0.0;
    color=GRAYSCALE;
  }
  
  void greytocmyk() {
    grey=1.0-grey;
    r=g=b=0.0;
    color=CMYK;
  }
  
  void rgbtocmyk() {
    double sat=rgbsaturation();
    grey=1.0-sat;
    if(sat) {
      double scale=1.0/sat;
      r=1.0-r*scale;
      g=1.0-g*scale;
      b=1.0-b*scale;
    }
    color=CMYK;
  }

  void cmyktorgb() {
    double sat=1.0-grey;
    r=(1.0-r)*sat;
    g=(1.0-g)*sat;
    b=(1.0-b)*sat;
    grey=0.0;
    color=RGB;
  }

  void cmyktogrey() {
    cmyktorgb();
    rgbtogrey();
  }
  
  void togrey() {
    if(rgb()) rgbtogrey();
    else if(cmyk()) cmyktogrey();
  }
  
  void torgb() {
    if(cmyk()) cmyktorgb();
    else if(grayscale()) greytorgb();
  }
  
  void tocmyk() {
    if(rgb()) rgbtocmyk();
    else if(grayscale()) greytocmyk();
  }
  
  void settransparency(const pen& p) {
    transparency=p.transparency;
  }
                                                               
  void convert() {
    if(settings::gray || settings::bw) {
      if(rgb()) rgbtogrey();
      else if(cmyk()) cmyktogrey();
      if(settings::bw) {grey=(grey == 1.0) ? 1.0 : 0.0;}
    } 
    else if(settings::rgb && cmyk()) cmyktorgb();
    else if(settings::cmyk && rgb()) rgbtocmyk();
  }   
  
  // Try to upgrade to the specified colorspace.
  bool promote(ColorSpace c) {
    if(color == c) return true;
    
    switch(color) {
      case PATTERN:
      case INVISIBLE:
        break;
      case DEFCOLOR:
      {
        return true;
        break;
      }
      break;
      case GRAYSCALE:
      {
        if(c == RGB) {greytorgb(); return true;}
        else if(c == CMYK) {greytocmyk(); return true;}
        break;
      }
      case RGB:
      {
        if(c == CMYK) {rgbtocmyk(); return true;}
        break;
      }
      case CMYK:
      {
        break;
      }
    }
    return false;
  }
  
  friend pen operator * (double x, const pen& q) {
    pen p=q;
    if(x < 0.0) x = 0.0;
    switch(p.color) {
      case PATTERN:
      case INVISIBLE:
      case DEFCOLOR:
        break;
      case GRAYSCALE:
      {
        p.grey *= x;
        p.greyrange();
        break;
      }
      case RGB:
      {
        p.r *= x;
        p.g *= x;
        p.b *= x;
        p.rgbrange();
        break;
      }
      case CMYK:
      {
        p.r *= x;
        p.g *= x;
        p.b *= x;
        p.grey *= x;
        p.cmykrange();
        break;
      }
    }
    return p;
  }
  
  friend pen operator + (const pen& p, const pen& q) {
    pen P=p;
    pen Q=q;
    
    if(P.color == PATTERN && P.pattern.empty()) P.color=DEFCOLOR;
    ColorSpace colorspace=(ColorSpace) max((Int) P.color,(Int) Q.color);
    
    if(!(p.transparency == DEFTRANSP && q.transparency == DEFTRANSP))
      P.transparency.opacity=max(p.opacity(),q.opacity());
    
    switch(colorspace) {
      case PATTERN:
      case INVISIBLE:
      case DEFCOLOR:
        break;
      case GRAYSCALE:
      {
        P.grey += Q.grey;
        P.greyrange();
        break;
      }
      
      case RGB:
      {
        if(P.color == GRAYSCALE) P.greytorgb();
        else if(Q.color == GRAYSCALE) Q.greytorgb();
        
        P.r += Q.r;
        P.g += Q.g;
        P.b += Q.b;
        P.rgbrange();
        break;
      }
      
      case CMYK:
      {
        if(P.color == GRAYSCALE) P.greytocmyk();
        else if(Q.color == GRAYSCALE) Q.greytocmyk();
        
        if(P.color == RGB) P.rgbtocmyk();
        else if(Q.color == RGB) Q.rgbtocmyk();
        
        P.r += Q.r;
        P.g += Q.g;
        P.b += Q.b;
        P.grey += Q.grey;
        P.cmykrange();
        break;
      }
    }
    
    return pen(q.line == DEFLINE ? p.line : q.line,
               q.linewidth == DEFWIDTH ? p.linewidth : q.linewidth,
               q.P.empty() ? p.P : q.P,
               q.font.empty() ? p.font : q.font,
               q.fontsize == 0.0 ? p.fontsize : q.fontsize,
               q.lineskip == 0.0 ? p.lineskip : q.lineskip,
               colorspace,P.r,P.g,P.b,P.grey,
               q.pattern == DEFPAT ? p.pattern : q.pattern,
               q.fillrule == DEFFILL ? p.fillrule : q.fillrule,
               q.baseline == DEFBASE ? p.baseline : q.baseline,
               q.transparency == DEFTRANSP ? p.transparency : q.transparency,
               q.linecap == DEFCAP ? p.linecap : q.linecap,
               q.linejoin == DEFJOIN ? p.linejoin : q.linejoin,
               q.miterlimit == DEFMITER ? p.miterlimit : q.miterlimit,
               q.overwrite == DEFWRITE ? p.overwrite : q.overwrite,
               q.t.isNull() ? p.t : q.t);
  }

  friend pen interpolate(const pen& p, const pen& q, double t) {
    pen P=p;
    pen Q=q;
  
    if(P.color == PATTERN && P.pattern.empty()) P.color=DEFCOLOR;
    ColorSpace colorspace=(ColorSpace) max((Int) P.color,(Int) Q.color);
  
    switch(colorspace) {
      case PATTERN:
      case INVISIBLE:
      case DEFCOLOR:
      case GRAYSCALE:
        break;
      case RGB:
      {
        if(P.color == GRAYSCALE) P.greytorgb();
        else if(Q.color == GRAYSCALE) Q.greytorgb();
        break;
      }
      
      case CMYK:
      {
        if(P.color == GRAYSCALE) P.greytocmyk();
        else if(Q.color == GRAYSCALE) Q.greytocmyk();
        
        if(P.color == RGB) P.rgbtocmyk();
        else if(Q.color == RGB) Q.rgbtocmyk();
        break;
      }
    }
    
    return (1-t)*P+t*Q;
  }

  friend bool operator == (const pen& p, const pen& q) {
    return  p.linetype() == q.linetype() 
      && p.width() == q.width() 
      && p.Path() == q.Path()
      && p.Font() == q.Font()
      && p.Lineskip() == q.Lineskip()
      && p.size() == q.size()
      && p.colorspace() == q.colorspace()
      && (!(p.grayscale() || p.cmyk()) || p.gray() == q.gray())
      && (!(p.rgb() || p.cmyk()) || (p.red() == q.red() &&
                                     p.green() == q.green() &&
                                     p.blue() == q.blue()))
      && p.pattern == q.pattern
      && p.Fillrule() == q.Fillrule()
      && p.Baseline() == q.Baseline()
      && p.transp() == q.transp()
      && p.cap() == q.cap()
      && p.join() == q.join()
      && p.miter() == q.miter()
      && p.Overwrite() == q.Overwrite()
      && p.t == q.t;
  }
  
  friend bool operator != (const pen& p, const pen& q) {
    return !(p == q);
  }
  
  friend ostream& operator << (ostream& out, const pen& p) {
    out << "(";
    if(p.line == DEFLINE)
      out << p.line.pattern;
    else
      out << "[" << p.line.pattern << "]";
    if(p.line.offset)
      out << p.line.offset;
    if(!p.line.scale)
      out << " bp";
    if(!p.line.adjust)
      out << " fixed";
    if(p.linewidth != DEFWIDTH)
      out << ", linewidth=" << p.linewidth;
    if(!p.P.empty())
      out << ", path=" << p.P;
    if(p.linecap != DEFCAP)
      out << ", linecap=" << Cap[p.linecap];
    if(p.linejoin != DEFJOIN)
      out << ", linejoin=" << Join[p.linejoin];
    if(p.miterlimit != DEFMITER)
      out << ", miterlimit=" << p.miterlimit;
    if(!p.font.empty())
      out << ", font=\"" << p.font << "\"";
    if(p.fontsize)
      out << ", fontsize=" << p.fontsize;
    if(p.lineskip)
      out << ", lineskip=" << p.lineskip;
    if(p.color == INVISIBLE)
      out << ", invisible";
    else if(p.color == GRAYSCALE)
      out << ", gray=" << p.grey;
    else if(p.color == RGB)
      out << ", red=" << p.red() << ", green=" << p.green() 
          << ", blue=" << p.blue();
    else if(p.color == CMYK)
      out << ", cyan=" << p.cyan() << ", magenta=" << p.magenta() 
          << ", yellow=" << p.yellow() << ", black=" << p.black();
    if(p.pattern != DEFPAT)
      out << ", pattern=" << "\"" << p.pattern << "\"";
    if(p.fillrule != DEFFILL)
      out << ", fillrule=" << FillRuleTag[p.fillrule];
    if(p.baseline != DEFBASE)
      out << ", baseline=" << BaseLineTag[p.baseline];
    if(!(p.transparency == DEFTRANSP)) {
      out << ", opacity=" << p.transparency.opacity;
      out << ", blend=" << p.transparency.blend;
    }
    if(p.overwrite != DEFWRITE)
      out << ", overwrite=" << OverwriteTag[p.overwrite];
    if(!p.t.isNull())
      out << ", transform=" << p.t;
    out << ")";
    
    return out;
  }

  const transform getTransform() const {
    return t.isNull() ? defaultpen().t : t;
  }
  
  // The bounds of the circle or ellipse the pen describes.
  bbox bounds() const
  {
    double maxx, maxy;
    pair shift;

    if(!P.empty()) return P.bounds();
    
    transform t=getTransform();
    
    if(t.isIdentity()) {
      maxx = 1;
      maxy = 1;
      shift = pair(0,0);
    } else {
      double xx = t.getxx(), xy = t.getxy(),
        yx = t.getyx(), yy = t.getyy();

      // These are the maximum x and y values that a linear transform can map
      // a point in the unit circle.  This can be proven by the Lagrange
      // Multiplier theorem or by properties of the dot product.
      maxx = length(pair(xx,xy));
      maxy = length(pair(yx,yy));

      shift = t*pair(0,0);
    }

    bbox b;
    pair z=0.5*width()*pair(maxx,maxy);
    b += z + shift;
    b += -z + shift;

    return b;
  }

  friend pen transformed(const transform& t, const pen& p) {
    pen ret = p;
    if(!p.P.empty()) ret.P = p.P.transformed(t);
    ret.t = p.t.isNull() ? t : t*p.t;
    return ret;
  }

};
  
pen transformed(const transform& t, const pen &p);
}

#endif
