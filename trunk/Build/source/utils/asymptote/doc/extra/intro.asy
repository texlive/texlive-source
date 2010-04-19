// Introduction to Asymptote

orientation=Landscape;

settings.tex="pdflatex";

import slide;
import three;
import animate;

usersetting();

viewportsize=pagewidth-2pagemargin;

// To generate bibliographic references:
// asy -k goysr
// bibtex goysr_ 
bibliographystyle("alpha");

itempen=fontsize(22pt);

titlepage("Asymptote: The Vector Graphics Language",
          "Andy Hammerlindl and John Bowman",
          "University of Toronto and University of Alberta","August 16, 2007",
          "http://asymptote.sf.net");

title("History");
item("\TeX\ and METAFONT (Knuth, 1979)");
item("MetaPost (Hobby, 1989): 2D Bezier Control Point Selection");
item("Asymptote (Hammerlindl, Bowman, Prince, 2004): 2D \& 3D Graphics");

title("Statistics (as of April, 2007)");
item("Runs on Windows, Mac OS X, Linux, etc.");
item("1800 downloads a month from {\tt asymptote.sourceforge.net}.");
item("33\ 000 lines of C++ code.");
item("18\ 000 lines of Asymptote code.");

title("Vector Graphics");
item("Raster graphics assign colors to a grid of pixels.");
figure("pixel.pdf");
item("Vector graphics are graphics which still maintain their look when
    inspected at arbitrarily small scales.");
asyfigure(asywrite("
picture pic;

path zoombox(real h) {
  return box((-h,-h/2),(min(10,h),min(10,h)/2));
}

frame zoom(real h, real next=0) {
  frame f;
  draw(f, (0,-100){W}..{E}(0,0), Arrow);
  clip(f, zoombox(h));
  if(next > 0)
    draw(f, zoombox(next));

  return scale(100/h)*f;
}

add(zoom(100), (0,0));
add(zoom(10), (200,0));
add(zoom(1), (400,0));
"));

title("Cartesian Coordinates");
asyfilecode("diagonal");
item("units are {\tt PostScript} {\it big points\/} (1 {\tt bp} =
1/72 {\tt inch})");
item("{\tt --} means join the points with a linear segment to create
a {\it path}");

item("cyclic path:");

asyfilecode("square");

title("Scaling to a Given Size");

item("{\tt PostScript} units are often inconvenient.");

item("Instead, scale user coordinates to a specified final size:");

code("
size(101,101);
draw((0,0)--(1,0)--(1,1)--(0,1)--cycle);
");
asyfigure("square");

item("One can also specify the size in {\tt cm}:");

asycode("
size(3cm,3cm);
draw(unitsquare);
");

title("Labels");

item("Adding and aligning \LaTeX\ labels is easy:");

asyfilecode("labelsquare","height=6cm");


title("2D Bezier Splines");

item("Using {\tt ..} instead of {\tt --} specifies a {\it Bezier cubic
spline}:");

code("
draw(z0 .. controls c0 and c1 .. z1,blue);
");
asyfigure("beziercurve","height=7cm");

equation("(1-t)^3 z_0+3t(1-t)^2 c_0+3t^2(1-t) c_1+t^3 z_1, \qquad t\in [0,1].");

title("Smooth Paths");

item("Asymptote can choose control points for you, using the algorithms of
Hobby and Knuth \cite{Hobby86,Knuth86b}:");

string bean="
pair[] z={(0,0), (0,1), (2,1), (2,0), (1,0)};
";

asycode(preamble="size(130,0);",bean+"
draw(z[0]..z[1]..z[2]..z[3]..z[4]..cycle,
     grey+linewidth(5));
dot(z,linewidth(7));
");

item("First, linear equations involving the curvature are solved to find the
    direction through each knot.  Then, control points along those directions
    are chosen:");

asyfigure(asywrite(preamble="size(130,0);",bean+"
path p=z[0]..z[1]..z[2]..z[3]..z[4]..cycle;

dot(z);
draw(p,lightgrey+linewidth(5));
dot(z);

picture output;
save();
for (int i=0; i<length(p); ++i) {
  pair z=point(p,i), dir=dir(p,i);
  draw((z-0.3dir)--(z+0.3dir), Arrow);
}
add(output, currentpicture.fit(), (-0.5inch, 0), W);
restore();

save();
guide g;
for (int i=0; i<length(p); ++i) {
  dot(precontrol(p,i));
  dot(postcontrol(p,i));
  g=g--precontrol(p,i)--point(p,i)--postcontrol(p,i);
}
draw(g--cycle,dashed);
add(output, currentpicture.fit(), (+0.5inch, 0), E);
restore();

shipout(output);
"));

title("Filling");
item("Use {\tt fill} to fill the inside of a path:");
asycode(preamble="size(0,200);","
path star;
for (int i=0; i<5; ++i)
  star=star--dir(90+144i);
star=star--cycle;
fill(shift(-1,0)*star,orange+zerowinding);
draw(shift(-1,0)*star,linewidth(3));
fill(shift(1,0)*star,blue+evenodd);
draw(shift(1,0)*star,linewidth(3));
");

title("Filling");
item("Use a list of paths to fill a region with holes:");
asycode(preamble="size(0,300);","
path[] p={scale(2)*unitcircle, reverse(unitcircle)};
fill(p,green+zerowinding);
");

title("Clipping");
item("Pictures can be clipped to lie inside a path:");
asycode(preamble="
size(0,200);
guide star;
for (int i=0; i<5; ++i)
  star=star--dir(90+144i);
star=star--cycle;","
fill(star,orange+zerowinding);
clip(scale(0.7)*unitcircle);
draw(scale(0.7)*unitcircle);
");
item("All of Asymptote's graphical capabilities are based on four primitive
    commands: {\tt draw}, {\tt fill}, {\tt clip}, and {\tt label}.");

title("Affine Transforms");

item("Affine transformations: shifts, rotations, reflections, and scalings.");
code("
transform t=rotate(90);
write(t*(1,0));  // Writes (0,1).
");

item("Pairs, paths, pens, strings, and whole pictures can be transformed.");
code("
fill(P,blue);
fill(shift(2,0)*reflect((0,0),(0,1))*P, red);
fill(shift(4,0)*rotate(30)*P, yellow);
fill(shift(6,0)*yscale(0.7)*xscale(2)*P, green);
");
asyfigure(asywrite("
size(500,0);
real bw=0.15;
real sw=0.2;
real r=0.15;

path outside=(0,0)--(0,1)--
    (bw+sw,1)..(bw+sw+r+bw,1-(r+bw))..(bw+sw,1-2(r+bw))--
    (bw,1-2(r+bw))--(bw,0)--cycle;
path inside=(bw,1-bw-2r)--(bw,1-bw)--
    (bw+sw,1-bw)..(bw+sw+r,1-bw-r)..(bw+sw,1-bw-2r)--cycle;
//fill(new path[] {outside, reverse(inside)},yellow);

path[] P={outside, reverse(inside)};

fill(P,blue);
fill(shift(2,0)*reflect((0,0),(0,1))*P, red);
fill(shift(4,0)*rotate(30)*P, yellow);
fill(shift(6,0)*yscale(0.7)*xscale(2)*P, green);
"));

title("C++/Java-like Programming Syntax");

code("// Declaration: Declare x to be real:
real x;

// Assignment: Assign x the value 1.
x=1.0;

// Conditional: Test if x equals 1 or not.
if(x == 1.0) {
  write(\"x equals 1.0\");
} else {
  write(\"x is not equal to 1.0\");
}

// Loop: iterate 10 times
for(int i=0; i < 10; ++i) {
  write(i);
}");

title("Helpful Math Notation");

item("Integer division returns a {\tt real}.  Use {\tt quotient} for an integer
    result:");
code("3/4==0.75         quotient(3,4)==0");

item("Caret for real and integer exponentiation:");
code("2^3    2.7^3    2.7^3.2");

item("Many expressions can be implicitly scaled by a numeric constant:");
code("2pi    10cm    2x^2    3sin(x)    2(a+b)");

item("Pairs are complex numbers:");
code("(0,1)*(0,1)==(-1,0)");

title("Function Calls");

item("Functions can take default arguments in any position.  Arguments are
    matched to the first possible location:");
string unitsize="unitsize(0.65cm);";
string preamble="void drawEllipse(real xsize=1, real ysize=xsize, pen p=blue) {
  draw(xscale(xsize)*yscale(ysize)*unitcircle, p);
}
";

asycode(preamble=unitsize,preamble+"
drawEllipse(2);
drawEllipse(red);
");

item("Arguments can be given by name:");
asycode(preamble=unitsize+preamble,"
drawEllipse(xsize=2, ysize=1);
drawEllipse(ysize=2, xsize=3, green);
");

title("Rest Arguments");
item("Rest arguments allow one to write a function that takes an arbitrary
    number of arguments:");
code("
int sum(... int[] nums) {
  int total=0; 
  for (int i=0; i < nums.length; ++i)
    total += nums[i];
  return total;
}

sum(1,2,3,4);                       // returns 10
sum();                              // returns 0
sum(1,2,3 ... new int[] {4,5,6});   // returns 21

int subtract(int start ... int[] subs) {
  return start - sum(... subs);
}
");

title("Higher-Order Functions");

item("Functions are first-class values.  They can be passed to other
    functions:");
code("real f(real x) {
    return x*sin(10x);
}
draw(graph(f,-3,3,300),red);");
asyfigure(asywrite("
import graph;
size(300,0);
real f(real x) {
    return x*sin(10x);
}
draw(graph(f,-3,3,300),red);
"));

title("Higher-Order Functions");
item("Functions can return functions:");
equation("f_n(x)=n\sin\left(\frac{x}{n}\right).");
skip();
string preamble="
import graph;
size(300,0);
";
string graphfunc2="
typedef real func(real);
func f(int n) {
  real fn(real x) {
    return n*sin(x/n);
  }
  return fn;
}

func f1=f(1);
real y=f1(pi);

for (int i=1; i<=5; ++i)
  draw(graph(f(i),-10,10),red);
";
code(graphfunc2);
string name=asywrite(graphfunc2,preamble=preamble);
asy(nativeformat(),name+".asy");
label(graphic(name+"."+nativeformat()),(0.5,0),
      Fill(figureborder,figuremattpen));

title("Anonymous Functions");

item("Create new functions with {\tt new}:");
code("
path p=graph(new real (real x) { return x*sin(10x); },-3,3,red);

func f(int n) {
  return new real (real x) { return n*sin(x/n); };
}");

item("Function definitions are just syntactic sugar for assigning function
objects to variables.");
code("
real square(real x) {
  return x^2;
}
");

remark("is equivalent to");
code("
real square(real x);
square=new real (real x) {
  return x^2;
};
");

title("Structures");

item("As in other languages, structures group together data.");
code("
struct Person {
  string firstname, lastname;
  int age;
}
Person bob=new Person;
bob.firstname=\"Bob\";
bob.lastname=\"Chesterton\";
bob.age=24;
");

item("Any code in the structure body will be executed every time a new structure
    is allocated...");
code("
struct Person {
  write(\"Making a person.\");
  string firstname, lastname;
  int age=18;
}
Person eve=new Person;   // Writes \"Making a person.\"
write(eve.age);          // Writes 18.
");

title("Object-Oriented Programming");
item("Functions are defined for each instance of a structure.");
code("
struct Quadratic {
  real a,b,c;
  real discriminant() {
    return b^2-4*a*c;
  }
  real eval(real x) {
    return a*x^2 + b*x + c;
  }
}
");

item("This allows us to construct ``methods'' which are just normal functions
    declared in the environment of a particular object:");
code("
Quadratic poly=new Quadratic;
poly.a=-1; poly.b=1; poly.c=2;

real f(real x)=poly.eval;
real y=f(2);
draw(graph(poly.eval, -5, 5));
");

title("Specialization");

item("Can create specialized objects just by redefining methods:");
code("
struct Shape {
    void draw();
    real area();
}

Shape rectangle(real w, real h) {
  Shape s=new Shape;
  s.draw = new void () {
                   fill((0,0)--(w,0)--(w,h)--(0,h)--cycle); };
  s.area = new real () { return w*h; };
  return s;
}

Shape circle(real radius) {
  Shape s=new Shape;
  s.draw = new void () { fill(scale(radius)*unitcircle); };
  s.area = new real () { return pi*radius^2; }
  return s;
}
");

title("Overloading");
item("Consider the code:");
code("
int x1=2;
int x2() {
  return 7;
}
int x3(int y) {
  return 2y;
}

write(x1+x2());  // Writes 9.
write(x3(x1)+x2());  // Writes 11.
");

title("Overloading");
item("{\tt x1}, {\tt x2}, and {\tt x3} are never used in the same context, so
    they can all be renamed {\tt x} without ambiguity:");
code("
int x=2;
int x() {
  return 7;
}
int x(int y) {
  return 2y;
}

write(x+x());  // Writes 9.
write(x(x)+x());  // Writes 11.
");

item("Function definitions are just variable definitions, but variables are
    distinguished by their signatures to allow overloading.");

title("Operators");
item("Operators are just syntactic sugar for functions, and can be addressed or
    defined as functions with the {\tt operator} keyword.");
code("
int add(int x, int y)=operator +;
write(add(2,3));  // Writes 5.

// Don't try this at home.
int operator +(int x, int y) {
  return add(2x,y);
}
write(2+3);  // Writes 7.
");
item("This allows operators to be defined for new types.");

title("Operators");
item("Operators for constructing paths are also functions:");
code("a.. controls b and c .. d--e");
remark("is equivalent to");
code(
     "operator --(operator ..(a, operator controls(b,c), d), e)");
item("This allowed us to redefine all of the path operators for 3D paths.");
asyfigure("helix","height=10cm");

title("Packages");

item("Function and structure definitions can be grouped into packages:");
code("
// powers.asy
real square(real x) { return x^2; }
real cube(real x) { return x^3; }
");
remark("and imported:");
code("
import powers;
real eight=cube(2.0);
draw(graph(powers.square, -1, 1));
");
  
title("Packages");

item("There are packages for Feynman diagrams,");
asyfigure("eetomumu","height=6cm");
remark("data structures,");
asyfigure(asywrite("
import binarytree;

binarytree bt=binarytree(1,2,4,nil,5,nil,nil,0,nil,nil,3,6,nil,nil,7);
draw(bt);
"),"height=6cm");
newslide();
remark("algebraic knot theory:");
asyfigure("knots");
equations("\Phi\Phi(x_1,x_2,x_3,x_4,x_5)
    =   &\rho_{4b}(x_1+x_4,x_2,x_3,x_5) + \rho_{4b}(x_1,x_2,x_3,x_4) \\
      + &\rho_{4a}(x_1,x_2+x_3,x_4,x_5) - \rho_{4b}(x_1,x_2,x_3,x_4+x_5) \\
      - &\rho_{4a}(x_1+x_2,x_3,x_4,x_5) - \rho_{4a}(x_1,x_2,x_4,x_5).");

title("Textbook Graph");
asy(nativeformat(),"exp");
filecode("exp.asy");
label(graphic("exp."+nativeformat(),"height=10cm"),(0.5,0),
      Fill(figureborder,figuremattpen));

title("Scientific Graph");
asyfilecode("lineargraph","height=13cm",newslide=true);

title("Data Graph");
asyfilecode("datagraph","height=13cm",newslide=true);

title("Imported Data Graph");
asyfilecode("filegraph","height=15cm",newslide=true);

title("Logarithmic Graph");
asyfilecode("loggraph","height=15cm",newslide=true);

title("Secondary Axis");
asyfigure("secondaryaxis","height=15cm");

title("Images");
asyfigure("imagecontour","height=17cm");

title("Multiple Graphs");
asyfigure("diatom","height=17cm");

title("Hobby's 2D Direction Algorithm");
item("A tridiagonal system of linear equations is solved to determine any unspecified directions $\theta_k$ and $\phi_k$ through each knot $z_k$:");

equation("\frac{\theta_{k-1}-2\phi_k}{\ell_k}=
\frac{\phi_{k+1}-2\theta_k}{\ell_{k+1}}.");

asyfigure("Hobbydir","height=9cm");

item("The resulting shape may be adjusted by modifying optional {\it tension\/} parameters and {\it curl\/} boundary conditions.");

//involving the curvature 

title("Hobby's 2D Control Point Algorithm");
item("Having prescribed outgoing and incoming path directions $e^{i\theta}$
at node~$z_0$ and $e^{i\phi}$ at node $z_1$ relative to the
vector $z_1-z_0$, the control points are determined as:");  

equations("u&=&z_0+e^{i\theta}(z_1-z_0)f(\theta,-\phi),\nonumber\\
v&=&z_1-e^{i\phi}(z_1-z_0)f(-\phi,\theta),");

remark("where the relative distance function $f(\theta,\phi)$ is given by Hobby [1986].");

asyfigure("Hobbycontrol","height=9cm");

title("Bezier Curves in 3D");

item("Apply an affine transformation");

equation("x'_i=A_{ij} x_j+C_i");

remark("to a Bezier curve:");

equation("x(t)=\sum_{k=0}^3 B_k(t) P_k, \qquad t\in [0,1].");

item("The resulting curve is also a Bezier curve:");

equations("x'_i(t)&=&\sum_{k=0}^3 B_k(t) A_{ij}(P_k)_j+C_i\nonumber\\
&=&\sum_{k=0}^3 B_k(t) P'_k,");

remark("where $P'_k$ is the transformed $k^{\rm th}$ control point, noting
$\displaystyle\sum_{k=0}^3 B_k(t)=1.$");

title("3D Generalization of Hobby's algorithm");

item("Must reduce to 2D algorithm in planar case.");

item("Determine directions by applying Hobby's algorithm in the plane containing $z_{k-1}$, $z_k$, $z_{k+1}$.");

// Reformulate Hobby's equations in terms of the angle $\psi_k=$
item("The only ambiguity that can arise is the overall sign of the angles, which relates to viewing each 2D plane from opposing normal directions."); 

item("A reference vector based on the mean unit normal of successive segments can be used to resolve such ambiguities.");

title("3D Control Point Algorithm");

item("Hobby's control point algorithm can be generalized to 3D by expressing it  in terms of the absolute directions $\omega_0$ and $\omega_1$:");

equation("u=z_0+\omega_0\left|z_1-z_0\right|f(\theta,-\phi),");
equation("v=z_1-\omega_1\left|z_1-z_0\right|f(-\phi,\theta),");

asyfigure("Hobbycontrol");

remark("interpreting $\theta$ and $\phi$ as the angle between the corresponding path direction vector and $z_1-z_0$.");

item("In this case there is an unambiguous reference vector for determining the relative sign of the angles $\phi$ and $\theta$.");

viewportmargin=(0,0.5cm);
defaultpen(1.0);
title("Interactive 3D Saddle");
item("A unit circle in the $X$--$Y$ plane may be filled and drawn with:
(1,0,0)..(0,1,0)..(-1,0,0)..(0,-1,0)..cycle");
asyinclude("unitcircle3",8cm);
remark("and then distorted into a saddle:\\ (1,0,0)..(0,1,1)..(-1,0,0)..(0,-1,1)..cycle");
asyinclude("saddle",8cm);

viewportmargin=(0,2cm);
title("Smooth 3D surfaces");
asyinclude("GaussianSurface",15cm);
defaultpen(0.5);

title("Slide Presentations");
item("Asymptote has a package for preparing slides.");
item("It even supports embedded high-resolution PDF movies.");

code('
title("Slide Presentations");
item("Asymptote has a package for preparing slides.");
item("It even supports embedded high-resolution PDF movies.");
');
remark("\quad\ldots");

import graph;

pen p=linewidth(1);
pen dotpen=linewidth(5);

pair wheelpoint(real t) {return (t+cos(t),-sin(t));}

guide wheel(guide g=nullpath, real a, real b, int n)
{
  real width=(b-a)/n;
  for(int i=0; i <= n; ++i) {
    real t=a+width*i;
    g=g--wheelpoint(t);
  }
  return g;
}

real t1=0; 
real t2=t1+2*pi;

picture base;
draw(base,circle((0,0),1),p);
draw(base,wheel(t1,t2,100),p+linetype("0 2"));
yequals(base,Label("$y=-1$",1.0),-1,extend=true,p+linetype("4 4"));
xaxis(base,Label("$x$",align=3SW),0,p);
yaxis(base,"$y$",0,1.3,p);
pair z1=wheelpoint(t1);
pair z2=wheelpoint(t2);
dot(base,z1,dotpen);
dot(base,z2,dotpen);

animation a;

int n=25;
real dt=(t2-t1)/n;
for(int i=0; i <= n; ++i) {
  picture pic;
  size(pic,24cm);
  real t=t1+dt*i;
  add(pic,base);
  draw(pic,circle((t,0),1),p+red);
  dot(pic,wheelpoint(t),dotpen);
  a.add(pic);
}

display(a.pdf(delay=150,"controls"));

title("Automatic Sizing");
item("Figures can be specified in user coordinates, then
    automatically scaled to the desired final size.");
asyfigure(asywrite("
import graph;

size(0,100);

frame cardsize(real w=0, real h=0, bool keepAspect=Aspect) {
  picture pic;
  pic.size(w,h,keepAspect);

  real f(real t) {return 1+cos(t);}

  guide g=polargraph(f,0,2pi,operator ..)--cycle;
  filldraw(pic,g,pink);

  xaxis(pic,\"$x$\");
  yaxis(pic,\"$y$\");

  dot(pic,\"$(a,0)$\",(1,0),N);
  dot(pic,\"$(2a,0)$\",(2,0),N+E);

  frame f=pic.fit();
  label(f,\"{\tt size(\"+string(w)+\",\"+string(h)+\");}\",point(f,S),align=S);
  return f;
}

add(cardsize(0,50), (0,0));
add(cardsize(0,100), (230,0));
add(cardsize(0,200), (540,0));
"));

title("Deferred Drawing");
item("We can't draw a graphical object until we know the scaling
    factors for the user coordinates.");
item("Instead, store a function that when given the scaling information, draws
    the scaled object.");
code("
void draw(picture pic=currentpicture, path g, pen p=currentpen) {
  pic.add(new void(frame f, transform t) {
      draw(f,t*g,p);
    });
  pic.addPoint(min(g),min(p));
  pic.addPoint(max(g),max(p));
}
");

title("Coordinates");
item("Store bounding box information as the sum of user and true-size
    coordinates:");
asyfigure(asywrite("
size(0,150);

path q=(0,0){dir(70)}..{dir(70)}(100,50);
pen p=rotate(30)*yscale(0.7)*(lightblue+linewidth(20));
draw(q,p);
draw((90,10),p);

currentpicture.add(new void(frame f, transform t) {
    draw(f,box(min(t*q)+min(p),max(t*q)+max(p)), dashed);
    });

draw(box(min(q),max(q)));

frame f;
draw(f,box(min(p),max(p)));

add(f,min(q));
add(f,max(q));

draw(q);
"));

code("pic.addPoint(min(g),min(p));
pic.addPoint(max(g),max(p));");
item("Filling ignores the pen width:");
code("pic.addPoint(min(g),(0,0));
pic.addPoint(max(g),(0,0));");
item("Communicate with \LaTeX\ to determine label sizes:");

asyfigure(asywrite("
size(0,100);

pen p=fontsize(30pt);
frame f;
label(f, \"$E=mc^2$\", p);
draw(f, box(min(f),max(f)));
shipout(f);
"));

title("Sizing");

item("When scaling the final figure to a given size $S$, we first need to
    determine a scaling factor $a>0$ and a shift $b$ so that all of the
    coordinates when transformed will lie in the interval $[0,S]$.  That is, if
    $u$ and $t$ are the user and truesize components:");
equation("0\le au+t+b \le S.");

item("We are maximizing the variable $a$ subject to a number of inequalities.
    This is a linear programming problem that can be solved by the simplex
    method.");

title("Sizing");
item("Every addition of a coordinate $(t,u)$ adds two restrictions");
equation("au+t+b\ge 0,");
equation("au+t+b\le S,");
remark("and each drawing component adds two coordinates.");
item("A figure could easily produce thousands of restrictions, making the
    simplex method impractical.");

item("Most of these restrictions are redundant, however.  For instance, with
    concentric circles, only the largest circle needs to be accounted for.");
asyfigure(asywrite("
import palette;
size(160,0);
pen[] p=Rainbow(NColors=11);
for (int i=1; i<10; ++i) {
  draw(scale(i)*unitcircle, p[i]+linewidth(2));
}
"));

title("Redundant Restrictions");
item("In general, if $u\le u'$ and $t\le t'$ then");
equation("au+t+b\le au'+t'+b");
remark("for all choices of $a>0$ and $b$, so");
equation("0\le au+t+b\le au'+t'+b\le S.");
item("This defines a partial ordering on coordinates.  When sizing a picture,
    the program first computes which coordinates are maximal (or minimal) and
    only sends effective constraints to the simplex algorithm.");
item("In practice, the linear programming problem will have less than a dozen
    restraints.");
item("All picture sizing is implemented in Asymptote code.");

title("Infinite Lines");
item("Deferred drawing allows us to draw infinite lines.");
code("drawline(P, Q);");

asyfigure("elliptic","height=12cm");

title("A Final Example: Quilting");
asyfigure(asywrite("
import math;

int n=8, skip=3;

pair r(int k) { return unityroot(n,k); }

pen col=blue, col2=purple;

guide square=box((1,1),(-1,-1));

guide step(int mult)
{
  guide g;
  for (int k=0; k<n; ++k)
    g=g--r(mult*k);
  g=g--cycle;
  return g;
}

guide oct=step(1), star=step(skip);

guide wedge(pair z, pair v, real r, real a)
{
  pair w=expi(a/2.0);
  v=unit(v)*r;
  return shift(z)*((0,0)--v*w--v*conj(w)--cycle);
}

filldraw(square, col);
filldraw(oct, yellow);

// The interior angle of the points of the star.
real intang=pi*(1-((real)2skip)/((real)n));

for (int k=0; k<n; ++k) {
  pair z=midpoint(r(k)--r(k+1));
  guide g=wedge(z,-z,1,intang);
  filldraw(g,col2);
}

fill(star,yellow);
filldraw(star,evenodd+col);

size(5inch,0);
"));

bibliography("refs");

viewportsize=viewportmargin=0;
title("\mbox{Asymptote: 2D \& 3D Vector Graphics Language}");
asyinclude("../examples/logo3");
skip();
center("\tt http://asymptote.sf.net");
center("(freely available under the GNU public license)");
