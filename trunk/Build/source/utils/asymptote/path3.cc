/*****
 * path3.cc
 * John Bowman
 *
 * Compute information for a three-dimensional path.
 *****/

#include <cfloat>

#include "path3.h"
#include "util.h"
#include "camperror.h"
#include "mathop.h"

namespace camp {

using run::operator *;
using vm::array;

path3 nullpath3;
  
void checkEmpty3(Int n) {
  if(n == 0)
    reportError("nullpath3 has no points");
}

double bound(triple z0, triple c0, triple c1, triple z1,
             double (*m)(double, double),
             double (*f)(const triple&, double*), double *t,
             double b, int depth)
{
  b=m(b,m(f(z0,t),f(z1,t)));
  if(m(-1.0,1.0)*(b-m(f(c0,t),f(c1,t))) >= -sqrtFuzz || depth == 0)
    return b;
  --depth;

  triple m0=0.5*(z0+c0);
  triple m1=0.5*(c0+c1);
  triple m2=0.5*(c1+z1);
  triple m3=0.5*(m0+m1);
  triple m4=0.5*(m1+m2);
  triple m5=0.5*(m3+m4);

  // Check both Bezier subpaths.
  b=bound(z0,m0,m3,m5,m,f,t,b,depth);
  return bound(m5,m4,m2,z1,m,f,t,b,depth);
}

triple path3::point(double t) const
{
  checkEmpty3(n);
    
  Int i = Floor(t);
  Int iplus;
  t = fmod(t,1);
  if (t < 0) t += 1;

  if (cycles) {
    i = imod(i,n);
    iplus = imod(i+1,n);
  }
  else if (i < 0)
    return nodes[0].point;
  else if (i >= n-1)
    return nodes[n-1].point;
  else
    iplus = i+1;

  double one_t = 1.0-t;

  triple a = nodes[i].point,
    b = nodes[i].post,
    c = nodes[iplus].pre,
    d = nodes[iplus].point,
    ab   = one_t*a   + t*b,
    bc   = one_t*b   + t*c,
    cd   = one_t*c   + t*d,
    abc  = one_t*ab  + t*bc,
    bcd  = one_t*bc  + t*cd,
    abcd = one_t*abc + t*bcd;

  return abcd;
}

triple path3::precontrol(double t) const
{
  checkEmpty3(n);
                     
  Int i = Floor(t);
  Int iplus;
  t = fmod(t,1);
  if (t < 0) t += 1;

  if (cycles) {
    i = imod(i,n);
    iplus = imod(i+1,n);
  }
  else if (i < 0)
    return nodes[0].pre;
  else if (i >= n-1)
    return nodes[n-1].pre;
  else
    iplus = i+1;

  double one_t = 1.0-t;

  triple a = nodes[i].point,
    b = nodes[i].post,
    c = nodes[iplus].pre,
    ab   = one_t*a   + t*b,
    bc   = one_t*b   + t*c,
    abc  = one_t*ab  + t*bc;

  return (abc == a) ? nodes[i].pre : abc;
}
        
 
triple path3::postcontrol(double t) const
{
  checkEmpty3(n);
  
  Int i = Floor(t);
  Int iplus;
  t = fmod(t,1);
  if (t < 0) t += 1;

  if (cycles) {
    i = imod(i,n);
    iplus = imod(i+1,n);
  }
  else if (i < 0)
    return nodes[0].post;
  else if (i >= n-1)
    return nodes[n-1].post;
  else
    iplus = i+1;

  double one_t = 1.0-t;
  
  triple b = nodes[i].post,
    c = nodes[iplus].pre,
    d = nodes[iplus].point,
    bc   = one_t*b   + t*c,
    cd   = one_t*c   + t*d,
    bcd  = one_t*bc  + t*cd;

  return (bcd == d) ? nodes[iplus].post : bcd;
}

path3 path3::reverse() const
{
  mem::vector<solvedKnot3> nodes(n);
  Int len=length();
  for (Int i = 0, j = len; i < n; i++, j--) {
    nodes[i].pre = postcontrol(j);
    nodes[i].point = point(j);
    nodes[i].post = precontrol(j);
    nodes[i].straight = straight(j-1);
  }
  return path3(nodes, n, cycles);
}

path3 path3::subpath(Int a, Int b) const
{
  if(empty()) return path3();

  if (a > b) {
    const path3 &rp = reverse();
    Int len=length();
    path3 result = rp.subpath(len-a, len-b);
    return result;
  }

  if (!cycles) {
    if (a < 0)
      a = 0;
    if (b > n-1)
      b = n-1;
  }

  Int sn = b-a+1;
  mem::vector<solvedKnot3> nodes(sn);

  for (Int i = 0, j = a; j <= b; i++, j++) {
    nodes[i].pre = precontrol(j);
    nodes[i].point = point(j);
    nodes[i].post = postcontrol(j);
    nodes[i].straight = straight(j);
  }
  nodes[0].pre = nodes[0].point;
  nodes[sn-1].post = nodes[sn-1].point;

  return path3(nodes, sn);
}

inline triple split(double t, const triple& x, const triple& y) {
  return x+(y-x)*t;
}

inline void splitCubic(solvedKnot3 sn[], double t, const solvedKnot3& left_,
                       const solvedKnot3& right_)
{
  solvedKnot3 &left=(sn[0]=left_), &mid=sn[1], &right=(sn[2]=right_);
  if(left.straight) {
    mid.point=split(t,left.point,right.point);
    triple deltaL=third*(mid.point-left.point);
    left.post=left.point+deltaL;
    mid.pre=mid.point-deltaL;
    triple deltaR=third*(right.point-mid.point);
    mid.post=mid.point+deltaR;
    right.pre=right.point-deltaR;
    mid.straight=true;
  } else {
    triple x=split(t,left.post,right.pre); // m1
    left.post=split(t,left.point,left.post); // m0
    right.pre=split(t,right.pre,right.point); // m2
    mid.pre=split(t,left.post,x); // m3
    mid.post=split(t,x,right.pre); // m4 
    mid.point=split(t,mid.pre,mid.post); // m5
  }
}

path3 path3::subpath(double a, double b) const
{
  if(empty()) return path3();
  
  if (a > b) {
    const path3 &rp = reverse();
    Int len=length();
    return rp.subpath(len-a, len-b);
  }

  solvedKnot3 aL, aR, bL, bR;
  if (!cycles) {
    if (a < 0) {
      a = 0;
      if (b < 0)
        b = 0;
    }   
    if (b > n-1) {
      b = n-1;
      if (a > n-1)
        a = n-1;
    }
    aL = nodes[(Int)floor(a)];
    aR = nodes[(Int)ceil(a)];
    bL = nodes[(Int)floor(b)];
    bR = nodes[(Int)ceil(b)];
  } else {
    if(run::validInt(a) && run::validInt(b)) {
      aL = nodes[imod((Int) floor(a),n)];
      aR = nodes[imod((Int) ceil(a),n)];
      bL = nodes[imod((Int) floor(b),n)];
      bR = nodes[imod((Int) ceil(b),n)];
    } else reportError("invalid path3 index");
  }

  if (a == b) return path3(point(a));

  solvedKnot3 sn[3];
  path3 p = subpath(Ceil(a), Floor(b));
  if (a > floor(a)) {
    if (b < ceil(a)) {
      splitCubic(sn,a-floor(a),aL,aR);
      splitCubic(sn,(b-a)/(ceil(b)-a),sn[1],sn[2]);
      return path3(sn[0],sn[1]);
    }
    splitCubic(sn,a-floor(a),aL,aR);
    p=concat(path3(sn[1],sn[2]),p);
  }
  if (ceil(b) > b) {
    splitCubic(sn,b-floor(b),bL,bR);
    p=concat(p,path3(sn[0],sn[1]));
  }
  return p;
}

// Special case of subpath for paths of length 1 used by intersect.
void path3::halve(path3 &first, path3 &second) const
{
  solvedKnot3 sn[3];
  splitCubic(sn,0.5,nodes[0],nodes[1]);
  first=path3(sn[0],sn[1]);
  second=path3(sn[1],sn[2]);
}
  
// Calculate the coefficients of a Bezier derivative divided by 3.
static inline void derivative(triple& a, triple& b, triple& c,
                              const triple& z0, const triple& c0,
                              const triple& c1, const triple& z1)
{
  a=z1-z0+3.0*(c0-c1);
  b=2.0*(z0+c1)-4.0*c0;
  c=c0-z0;
}

bbox3 path3::bounds() const
{
  if(!box.empty) return box;
  
  if (empty()) {
    // No bounds
    return bbox3();
  }
  
  Int len=length();
  box.add(point(len));

  for (Int i = 0; i < len; i++) {
    addpoint(box,i);
    if(straight(i)) continue;
    
    triple a,b,c;
    derivative(a,b,c,point(i),postcontrol(i),precontrol(i+1),point(i+1));
    
    // Check x coordinate
    quadraticroots x(a.getx(),b.getx(),c.getx());
    if(x.distinct != quadraticroots::NONE && goodroot(x.t1))
      addpoint(box,i+x.t1);
    if(x.distinct == quadraticroots::TWO && goodroot(x.t2))
      addpoint(box,i+x.t2);
    
    // Check y coordinate
    quadraticroots y(a.gety(),b.gety(),c.gety());
    if(y.distinct != quadraticroots::NONE && goodroot(y.t1))
      addpoint(box,i+y.t1);
    if(y.distinct == quadraticroots::TWO && goodroot(y.t2))
      addpoint(box,i+y.t2);
    
    // Check z coordinate
    quadraticroots z(a.getz(),b.getz(),c.getz());
    if(z.distinct != quadraticroots::NONE && goodroot(z.t1))
      addpoint(box,i+z.t1);
    if(z.distinct == quadraticroots::TWO && goodroot(z.t2))
      addpoint(box,i+z.t2);
  }
  return box;
}

pair path3::bounds(double (*m)(double, double), 
                   double (*x)(const triple&, double*),
                   double (*y)(const triple&, double*), double *t) const
{
  checkEmpty3(n);
  
  triple v=point((Int) 0);
  pair B=pair(x(v,t),y(v,t));
  
  Int n=length();
  for(Int i=0; i <= n; ++i) {
    if(straight(i)) {
      triple v=point(i);
      B=pair(m(B.getx(),x(v,t)),m(B.gety(),y(v,t)));
    } else {
      triple z0=point(i);
      triple c0=postcontrol(i);
      triple c1=precontrol(i+1);
      triple z1=point(i+1);
      B=pair(bound(z0,c0,c1,z1,m,x,t,B.getx()),
             bound(z0,c0,c1,z1,m,y,t,B.gety()));
    }
  }
  return B;
}

// {{{ Arclength Calculations

static triple a,b,c;

static double ds(double t)
{
  double dx=quadratic(a.getx(),b.getx(),c.getx(),t);
  double dy=quadratic(a.gety(),b.gety(),c.gety(),t);
  double dz=quadratic(a.getz(),b.getz(),c.getz(),t);
  return sqrt(dx*dx+dy*dy+dz*dz);
}

// Calculates arclength of a cubic using adaptive simpson integration.
double path3::cubiclength(Int i, double goal) const
{
  const triple& z0=point(i);
  const triple& z1=point(i+1);
  double L;
  if(straight(i)) {
    L=(z1-z0).length();
    return (goal < 0 || goal >= L) ? L : -goal/L;
  }
  const triple& c0=postcontrol(i);
  const triple& c1=precontrol(i+1);
  
  double integral;
  derivative(a,b,c,z0,c0,c1,z1);
  
  if(!simpson(integral,ds,0.0,1.0,DBL_EPSILON,1.0))
    reportError("nesting capacity exceeded in computing arclength");
  L=3.0*integral;
  if(goal < 0 || goal >= L) return L;
  
  double t=goal/L;
  goal *= third;
  static double dxmin=sqrt(DBL_EPSILON);
  if(!unsimpson(goal,ds,0.0,t,100.0*DBL_EPSILON,integral,1.0,dxmin))
    reportError("nesting capacity exceeded in computing arctime");
  return -t;
}

double path3::arclength() const
{
  if (cached_length != -1) return cached_length;

  double L=0.0;
  for (Int i = 0; i < n-1; i++) {
    L += cubiclength(i);
  }
  if(cycles) L += cubiclength(n-1);
  cached_length = L;
  return cached_length;
}

double path3::arctime(double goal) const
{
  if (cycles) {
    if (goal == 0 || cached_length == 0) return 0;
    if (goal < 0)  {
      const path3 &rp = this->reverse();
      double result = -rp.arctime(-goal);
      return result;
    }
    if (cached_length > 0 && goal >= cached_length) {
      Int loops = (Int)(goal / cached_length);
      goal -= loops*cached_length;
      return loops*n+arctime(goal);
    }      
  } else {
    if (goal <= 0)
      return 0;
    if (cached_length > 0 && goal >= cached_length)
      return n-1;
  }
    
  double l,L=0;
  for (Int i = 0; i < n-1; i++) {
    l = cubiclength(i,goal);
    if (l < 0)
      return (-l+i);
    else {
      L += l;
      goal -= l;
      if (goal <= 0)
        return i+1;
    }
  }
  if (cycles) {
    l = cubiclength(n-1,goal);
    if (l < 0)
      return -l+n-1;
    if (cached_length > 0 && cached_length != L+l) {
      reportError("arclength != length.\n"
                  "path3::arclength(double) must have broken semantics.\n"
                  "Please report this error.");
    }
    cached_length = L += l;
    goal -= l;
    return arctime(goal)+n;
  }
  else {
    cached_length = L;
    return length();
  }
}

// }}}

// {{{ Path3 Intersection Calculations

// Return all intersection times of path3 g with the triple v.
void intersections(std::vector<double>& T, const path3& g, const triple& v,
                   double fuzz)
{
  double fuzz2=fuzz*fuzz;
  Int n=g.length();
  bool cycles=g.cyclic();
  for(Int i=0; i < n; ++i) {
    // Check all directions to circumvent degeneracy.
    std::vector<double> r;
    roots(r,g.point(i).getx(),g.postcontrol(i).getx(),
          g.precontrol(i+1).getx(),g.point(i+1).getx(),v.getx());
    roots(r,g.point(i).gety(),g.postcontrol(i).gety(),
          g.precontrol(i+1).gety(),g.point(i+1).gety(),v.gety());
    roots(r,g.point(i).getz(),g.postcontrol(i).getz(),
          g.precontrol(i+1).getz(),g.point(i+1).getz(),v.getz());
    
    size_t m=r.size();
    for(size_t j=0 ; j < m; ++j) {
      double t=r[j];
      if(t >= -Fuzz && t <= 1.0+Fuzz) {
        double s=i+t;
        if((g.point(s)-v).abs2() <= fuzz2) {
          if(cycles && s >= n-Fuzz) s=0;
          T.push_back(s);
        }
      }
    }
  }
}

// An optimized implementation of intersections(g,p--q);
// if there are an infinite number of intersection points, the returned list is
// only guaranteed to include the endpoint times of the intersection.
void intersections(std::vector<double>& S, std::vector<double>& T,
                   const path3& g, const triple& p, double fuzz)
{
  std::vector<double> S1;
  intersections(S1,g,p,fuzz);
  size_t n=S1.size();
  for(size_t i=0; i < n; ++i) {
    S.push_back(S1[i]);
    T.push_back(0);
  }
}

void add(std::vector<double>& S, std::vector<double>& T, double s, double t,
         const path3& p, const path3& q, double fuzz2)
{
  triple P=p.point(s);
  for(size_t i=0; i < S.size(); ++i)
    if((p.point(S[i])-P).abs2() <= fuzz2) return;
  S.push_back(s);
  T.push_back(t);
}
  
void add(double& s, double& t, std::vector<double>& S, std::vector<double>& T,
         std::vector<double>& S1, std::vector<double>& T1,
         double pscale, double qscale, double poffset, double qoffset,
         const path3& p, const path3& q, double fuzz, bool single)
{
  if(single) {
    s=s*pscale+poffset;
    t=t*qscale+qoffset;
  } else {
    double fuzz2=4.0*fuzz*fuzz;
    size_t n=S1.size();
    for(size_t i=0; i < n; ++i)
      add(S,T,pscale*S1[i]+poffset,qscale*T1[i]+qoffset,p,q,fuzz2);
  }
}

void add(double& s, double& t, std::vector<double>& S, std::vector<double>& T,
         std::vector<double>& S1, std::vector<double>& T1,
         const path3& p, const path3& q, double fuzz, bool single)
{
  size_t n=S1.size();
  if(single) {
    if(n > 0) {
      s=S1[0];
      t=T1[0];
    }
  } else {
    double fuzz2=4.0*fuzz*fuzz;
    for(size_t i=0; i < n; ++i)
      add(S,T,S1[i],T1[i],p,q,fuzz2);
  }
}

bool intersections(double &s, double &t, std::vector<double>& S,
                   std::vector<double>& T, path3& p, path3& q,
                   double fuzz, bool single, bool exact, unsigned depth)
{
  if(errorstream::interrupt) throw interrupted();
  
  Int lp=p.length();
  if(lp == 0 && exact) {
    std::vector<double> T1,S1;
    intersections(T1,S1,q,p.point(lp),fuzz);
    add(s,t,S,T,S1,T1,p,q,fuzz,single);
    return S1.size() > 0;
  }
  
  Int lq=q.length();
  if(lq == 0 && exact) {
    std::vector<double> S1,T1;
    intersections(S1,T1,p,q.point(lq),fuzz);
    add(s,t,S,T,S1,T1,p,q,fuzz,single);
    return S1.size() > 0;
  }
  
  triple maxp=p.max();
  triple minp=p.min();
  triple maxq=q.max();
  triple minq=q.min();
  
  if(maxp.getx()+fuzz >= minq.getx() &&
     maxp.gety()+fuzz >= minq.gety() && 
     maxp.getz()+fuzz >= minq.getz() && 
     maxq.getx()+fuzz >= minp.getx() &&
     maxq.gety()+fuzz >= minp.gety() &&
     maxq.getz()+fuzz >= minp.getz()) {
    // Overlapping bounding boxes

    --depth;
    if((maxp-minp).length()+(maxq-minq).length() <= fuzz || depth == 0) {
      if(single) {
        s=0;
        t=0;
      } else {
        S.push_back(0.0);
        T.push_back(0.0);
      }
      return true;
    }
    
    path3 p1,p2;
    double pscale,poffset;
    
    if(lp <= 1) {
      if(lp == 1) p.halve(p1,p2);
      if(lp == 0 || p1 == p || p2 == p) {
        std::vector<double> T1,S1;
        intersections(T1,S1,q,p.point((Int) 0),fuzz);
        add(s,t,S,T,S1,T1,p,q,fuzz,single);
        return S1.size() > 0;
      }
      pscale=poffset=0.5;
    } else {
      Int tp=lp/2;
      p1=p.subpath(0,tp);
      p2=p.subpath(tp,lp);
      poffset=tp;
      pscale=1.0;
    }
      
    path3 q1,q2;
    double qscale,qoffset;
    
    if(lq <= 1) {
      if(lq == 1) q.halve(q1,q2);
      if(lq == 0 || q1 == q || q2 == q) {
        std::vector<double> S1,T1;
        intersections(S1,T1,p,q.point((Int) 0),fuzz);
        add(s,t,S,T,S1,T1,p,q,fuzz,single);
        return S1.size() > 0;
      }
      qscale=qoffset=0.5;
    } else {
      Int tq=lq/2;
      q1=q.subpath(0,tq);
      q2=q.subpath(tq,lq);
      qoffset=tq;
      qscale=1.0;
    }
      
    bool Short=lp == 1 && lq == 1;
    
    static size_t maxcount=9;
    size_t count=0;
    
    std::vector<double> S1,T1;
    if(intersections(s,t,S1,T1,p1,q1,fuzz,single,exact,depth)) {
      add(s,t,S,T,S1,T1,pscale,qscale,0.0,0.0,p,q,fuzz,single);
      if(single || depth <= mindepth)
        return true;
      count += S1.size();
      if(Short && count > maxcount) return true;
    }
    
    S1.clear();
    T1.clear();
    if(intersections(s,t,S1,T1,p1,q2,fuzz,single,exact,depth)) {
      add(s,t,S,T,S1,T1,pscale,qscale,0.0,qoffset,p,q,fuzz,single);
      if(single || depth <= mindepth)
        return true;
      count += S1.size();
      if(Short && count > maxcount) return true;
    }
    
    S1.clear();
    T1.clear();
    if(intersections(s,t,S1,T1,p2,q1,fuzz,single,exact,depth)) {
      add(s,t,S,T,S1,T1,pscale,qscale,poffset,0.0,p,q,fuzz,single);
      if(single || depth <= mindepth)
        return true;
      count += S1.size();
      if(Short && count > maxcount) return true;
    }
    
    S1.clear();
    T1.clear();
    if(intersections(s,t,S1,T1,p2,q2,fuzz,single,exact,depth)) {
      add(s,t,S,T,S1,T1,pscale,qscale,poffset,qoffset,p,q,fuzz,single);
      if(single || depth <= mindepth)
        return true;
      count += S1.size();
      if(Short && count > maxcount) return true;
    }
    
    return S.size() > 0;
  }
  return false;
}

// }}}

path3 concat(const path3& p1, const path3& p2)
{
  Int n1 = p1.length(), n2 = p2.length();

  if (n1 == -1) return p2;
  if (n2 == -1) return p1;
  triple a=p1.point(n1);
  triple b=p2.point((Int) 0);

  mem::vector<solvedKnot3> nodes(n1+n2+1);

  Int i = 0;
  nodes[0].pre = p1.point((Int) 0);
  for (Int j = 0; j < n1; j++) {
    nodes[i].point = p1.point(j);
    nodes[i].straight = p1.straight(j);
    nodes[i].post = p1.postcontrol(j);
    nodes[i+1].pre = p1.precontrol(j+1);
    i++;
  }
  for (Int j = 0; j < n2; j++) {
    nodes[i].point = p2.point(j);
    nodes[i].straight = p2.straight(j);
    nodes[i].post = p2.postcontrol(j);
    nodes[i+1].pre = p2.precontrol(j+1);
    i++;
  }
  nodes[i].point = nodes[i].post = p2.point(n2);

  return path3(nodes, i+1);
}

path3 transformed(const array& t, const path3& p)
{
  Int n = p.size();
  mem::vector<solvedKnot3> nodes(n);

  for (Int i = 0; i < n; ++i) {
    nodes[i].pre = t * p.precontrol(i);
    nodes[i].point = t * p.point(i);
    nodes[i].post = t * p.postcontrol(i);
    nodes[i].straight = p.straight(i);
  }

  return path3(nodes, n, p.cyclic());
}

double xproject(const triple& v, double *t)
{
  double x=v.getx();
  double y=v.gety();
  double z=v.getz();
  double f=t[12]*x+t[13]*y+t[14]*z+t[15];
  if(f == 0.0) run::dividebyzero();
  return (t[0]*x+t[1]*y+t[2]*z+t[3])/f;
}

double yproject(const triple& v, double *t)
{
  double x=v.getx();
  double y=v.gety();
  double z=v.getz();
  double f=t[12]*x+t[13]*y+t[14]*z+t[15];
  if(f == 0.0) run::dividebyzero();
  return (t[4]*x+t[5]*y+t[6]*z+t[7])/f;
}

double xratio(const triple& v, double *)
{
  double z=v.getz();
  return v.getx()/z;
}

double yratio(const triple& v, double *)
{
  double z=v.getz();
  return v.gety()/z;
}

struct Split {
  double m0,m1,m2,m3,m4,m5;
  Split(double z0, double c0, double c1, double z1) {
    m0=0.5*(z0+c0);
    m1=0.5*(c0+c1);
    m2=0.5*(c1+z1);
    m3=0.5*(m0+m1);
    m4=0.5*(m1+m2);
    m5=0.5*(m3+m4);
  }
};
  
double cornerbound(double *p, double (*m)(double, double)) 
{
  double b=m(p[0],p[3]);
  b=m(b,p[12]);
  return m(b,p[15]);
}

double controlbound(double *p, double (*m)(double, double)) 
{
  double b=m(p[1],p[2]);
  b=m(b,p[4]);
  b=m(b,p[5]);
  b=m(b,p[6]);
  b=m(b,p[7]);
  b=m(b,p[8]);
  b=m(b,p[9]);
  b=m(b,p[10]);
  b=m(b,p[11]);
  b=m(b,p[13]);
  return m(b,p[14]);
}

double cornerbound(triple *p, double (*m)(double, double),
                   double (*f)(const triple&, double*), double *t) 
{
  double b=m(f(p[0],t),f(p[3],t));
  b=m(b,f(p[12],t));
  return m(b,f(p[15],t));
}

double controlbound(triple *p, double (*m)(double, double),
                    double (*f)(const triple&, double*), double *t) 
{
  double b=m(f(p[1],t),f(p[2],t));
  b=m(b,f(p[4],t));
  b=m(b,f(p[5],t));
  b=m(b,f(p[6],t));
  b=m(b,f(p[7],t));
  b=m(b,f(p[8],t));
  b=m(b,f(p[9],t));
  b=m(b,f(p[10],t));
  b=m(b,f(p[11],t));
  b=m(b,f(p[13],t));
  return m(b,f(p[14],t));
}

double bound(double *p, double (*m)(double, double), double b, int depth)
{
  b=m(b,cornerbound(p,m));
  if(m(-1.0,1.0)*(b-controlbound(p,m)) >= -sqrtFuzz || depth == 0)
    return b;
  --depth;

  Split c0(p[0],p[1],p[2],p[3]);
  Split c1(p[4],p[5],p[6],p[7]);
  Split c2(p[8],p[9],p[10],p[11]);
  Split c3(p[12],p[13],p[14],p[15]);

  Split c4(p[12],p[8],p[4],p[0]);
  Split c5(c3.m0,c2.m0,c1.m0,c0.m0);
  Split c6(c3.m3,c2.m3,c1.m3,c0.m3);
  Split c7(c3.m5,c2.m5,c1.m5,c0.m5);
  Split c8(c3.m4,c2.m4,c1.m4,c0.m4);
  Split c9(c3.m5,c2.m5,c1.m5,c0.m5);
  Split c10(p[15],p[11],p[7],p[3]);

  // Check all 4 Bezier subpatches.
  double s0[]={c4.m5,c5.m5,c6.m5,c7.m5,c4.m3,c5.m3,c6.m3,c7.m3,
               c4.m0,c5.m0,c6.m0,c7.m0,p[12],c3.m0,c3.m3,c3.m5};
  b=bound(s0,m,b,depth);
  double s1[]={p[0],c0.m0,c0.m3,c0.m5,c4.m2,c5.m2,c6.m2,c7.m2,
               c4.m4,c5.m4,c6.m4,c7.m4,c4.m5,c5.m5,c6.m5,c7.m5};
  b=bound(s1,m,b,depth);
  double s2[]={c0.m5,c0.m4,c0.m2,p[3],c7.m2,c8.m2,c9.m2,c10.m2,
               c7.m4,c8.m4,c9.m4,c10.m4,c7.m5,c8.m5,c9.m5,c10.m5};
  b=bound(s2,m,b,depth);
  double s3[]={c7.m5,c8.m5,c9.m5,c10.m5,c7.m3,c8.m3,c9.m3,c10.m3,
               c7.m0,c8.m0,c9.m0,c10.m0,c3.m5,c3.m4,c3.m2,p[15]};
  return bound(s3,m,b,depth);
}
  
double bound(triple *p, double (*m)(double, double),
             double (*f)(const triple&, double*), double *t,
             double b, int depth)
{
  b=m(b,cornerbound(p,m,f,t));
  if(m(-1.0,1.0)*(b-controlbound(p,m,f,t)) >= -sqrtFuzz || depth == 0)
    return b;
  --depth;

  Split3 c0(p[0],p[1],p[2],p[3]);
  Split3 c1(p[4],p[5],p[6],p[7]);
  Split3 c2(p[8],p[9],p[10],p[11]);
  Split3 c3(p[12],p[13],p[14],p[15]);

  Split3 c4(p[12],p[8],p[4],p[0]);
  Split3 c5(c3.m0,c2.m0,c1.m0,c0.m0);
  Split3 c6(c3.m3,c2.m3,c1.m3,c0.m3);
  Split3 c7(c3.m5,c2.m5,c1.m5,c0.m5);
  Split3 c8(c3.m4,c2.m4,c1.m4,c0.m4);
  Split3 c9(c3.m5,c2.m5,c1.m5,c0.m5);
  Split3 c10(p[15],p[11],p[7],p[3]);

  // Check all 4 Bezier subpatches.
  
  triple s0[]={c4.m5,c5.m5,c6.m5,c7.m5,c4.m3,c5.m3,c6.m3,c7.m3,
               c4.m0,c5.m0,c6.m0,c7.m0,p[12],c3.m0,c3.m3,c3.m5};
  b=bound(s0,m,f,t,b,depth);
  triple s1[]={p[0],c0.m0,c0.m3,c0.m5,c4.m2,c5.m2,c6.m2,c7.m2,
               c4.m4,c5.m4,c6.m4,c7.m4,c4.m5,c5.m5,c6.m5,c7.m5};
  b=bound(s1,m,f,t,b,depth);
  triple s2[]={c0.m5,c0.m4,c0.m2,p[3],c7.m2,c8.m2,c9.m2,c10.m2,
               c7.m4,c8.m4,c9.m4,c10.m4,c7.m5,c8.m5,c9.m5,c10.m5};
  b=bound(s2,m,f,t,b,depth);
  triple s3[]={c7.m5,c8.m5,c9.m5,c10.m5,c7.m3,c8.m3,c9.m3,c10.m3,
               c7.m0,c8.m0,c9.m0,c10.m0,c3.m5,c3.m4,c3.m2,p[15]};
  return bound(s3,m,f,t,b,depth);
}

} //namespace camp
  
