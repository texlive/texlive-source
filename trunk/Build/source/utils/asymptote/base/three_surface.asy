import bezulate;

int nslice=12;
real camerafactor=1.2;

private real Fuzz=10.0*realEpsilon;
private real nineth=1/9;

struct patch {
  triple[][] P=new triple[4][4];
  triple[] normals; // Optionally specify 4 normal vectors at the corners.
  pen[] colors;     // Optionally specify 4 corner colors.
  bool straight;    // Patch is based on a piecewise straight external path.
  bool3 planar;     // Patch is planar.

  path3 external() {
    return
      P[0][0]..controls P[0][1] and P[0][2]..
      P[0][3]..controls P[1][3] and P[2][3]..
      P[3][3]..controls P[3][2] and P[3][1]..
      P[3][0]..controls P[2][0] and P[1][0]..cycle;
  }

  triple[] internal() {
    return new triple[] {P[1][1],P[1][2],P[2][2],P[2][1]};
  }

  triple cornermean() {
    return 0.25*(P[0][0]+P[0][3]+P[3][3]+P[3][0]);
  }

  triple[] corners() {return new triple[] {P[0][0],P[0][3],P[3][3],P[3][0]};}

  real[] map(real f(triple)) {
    return new real[] {f(P[0][0]),f(P[0][3]),f(P[3][3]),f(P[3][0])};
  }

  triple[] controlpoints() {
    return new triple[] {
      P[0][0],P[0][1],P[0][2],P[0][3],
        P[1][0],P[1][1],P[1][2],P[1][3],
        P[2][0],P[2][1],P[2][2],P[2][3],
        P[3][0],P[3][1],P[3][2],P[3][3]};
  }

  triple Bu(int j, real u) {return bezier(P[0][j],P[1][j],P[2][j],P[3][j],u);}
  triple BuP(int j, real u) {return bezierP(P[0][j],P[1][j],P[2][j],P[3][j],u);}
  triple BuPP(int j, real u) {
    return bezierPP(P[0][j],P[1][j],P[2][j],P[3][j],u);
  }
  triple BuPPP(int j) {return bezierPPP(P[0][j],P[1][j],P[2][j],P[3][j]);}

  path3 uequals(real u) {
    return straight ? Bu(0,u)--Bu(3,u) :
      Bu(0,u)..controls Bu(1,u) and Bu(2,u)..Bu(3,u);
  }

  triple Bv(int i, real v) {return bezier(P[i][0],P[i][1],P[i][2],P[i][3],v);}
  triple BvP(int i, real v) {return bezierP(P[i][0],P[i][1],P[i][2],P[i][3],v);}
  triple BvPP(int i, real v) {
    return bezierPP(P[i][0],P[i][1],P[i][2],P[i][3],v);
  }
  triple BvPPP(int i) {return bezierPPP(P[i][0],P[i][1],P[i][2],P[i][3]);}

  path3 vequals(real v) {
    return straight ? Bv(0,v)--Bv(3,v) :
      Bv(0,v)..controls Bv(1,v) and Bv(2,v)..Bv(3,v);
  }

  triple point(real u, real v) {        
    return bezier(Bu(0,u),Bu(1,u),Bu(2,u),Bu(3,u),v);
  }

  // compute normal vectors for degenerate cases
  private triple normal0(real u, real v, real epsilon) {
    triple n=0.5*(cross(bezier(BvPP(0,v),BvPP(1,v),BvPP(2,v),BvPP(3,v),u),
                        bezier(BuP(0,u),BuP(1,u),BuP(2,u),BuP(3,u),v))+
                  cross(bezier(BvP(0,v),BvP(1,v),BvP(2,v),BvP(3,v),u),   
                        bezier(BuPP(0,u),BuPP(1,u),BuPP(2,u),BuPP(3,u),v)));
    return abs(n) > epsilon ? n :
      0.25*cross(bezier(BvPP(0,v),BvPP(1,v),BvPP(2,v),BvPP(3,v),u),   
                 bezier(BuPP(0,u),BuPP(1,u),BuPP(2,u),BuPP(3,u),v))+
      1/6*(cross(bezier(BvPPP(0),BvPPP(1),BvPPP(2),BvPPP(3),u),
                 bezier(BuP(0,u),BuP(1,u),BuP(2,u),BuP(3,u),v))+
           cross(bezier(BvP(0,v),BvP(1,v),BvP(2,v),BvP(3,v),u),   
                 bezier(BuPPP(0),BuPPP(1),BuPPP(2),BuPPP(3),v)))+
      1/12*(cross(bezier(BvPPP(0),BvPPP(1),BvPPP(2),BvPPP(3),u),
                  bezier(BuPP(0,u),BuPP(1,u),BuPP(2,u),BuPP(3,u),v))+
            cross(bezier(BvPP(0,v),BvPP(1,v),BvPP(2,v),BvPP(3,v),u),   
                  bezier(BuPPP(0),BuPPP(1),BuPPP(2),BuPPP(3),v)))+
      1/36*cross(bezier(BvPPP(0),BvPPP(1),BvPPP(2),BvPPP(3),u),   
                 bezier(BuPPP(0),BuPPP(1),BuPPP(2),BuPPP(3),v));
  }

  static real fuzz=1000*realEpsilon;

  triple normal(real u, real v) {
    triple n=cross(bezier(BvP(0,v),BvP(1,v),BvP(2,v),BvP(3,v),u),
                   bezier(BuP(0,u),BuP(1,u),BuP(2,u),BuP(3,u),v));
    real epsilon=fuzz*change2(P);
    return (abs(n) > epsilon) ? n : normal0(u,v,epsilon);
  }
  
  triple normal00() {
    triple n=9*cross(P[0][1]-P[0][0],P[1][0]-P[0][0]);
    real epsilon=fuzz*change2(P);
    return abs(n) > epsilon ? n : normal0(0,0,epsilon);
  }

  triple normal01() {
    triple n=9*cross(P[0][3]-P[0][2],P[1][3]-P[0][3]);
    real epsilon=fuzz*change2(P);
    return abs(n) > epsilon ? n : normal0(0,1,epsilon);
  }

  triple normal11() {
    triple n=9*cross(P[3][3]-P[3][2],P[3][3]-P[2][3]);
    real epsilon=fuzz*change2(P);
    return abs(n) > epsilon ? n : normal0(1,1,epsilon);
  }

  triple normal10() {
    triple n=9*cross(P[3][1]-P[3][0],P[3][0]-P[2][0]);
    real epsilon=fuzz*change2(P);
    return abs(n) > epsilon ? n : normal0(1,0,epsilon);
  }

  pen[] colors(material m, light light=currentlight) {
    bool nocolors=colors.length == 0;
    if(normals.length > 0)
      return new pen[] {light.color(normals[0],nocolors ? m : colors[0]),
          light.color(normals[1],nocolors ? m : colors[1]),
          light.color(normals[2],nocolors ? m : colors[2]),
          light.color(normals[3],nocolors ? m : colors[3])};
    if(planar) {
      triple normal=normal(0.5,0.5);
      return new pen[] {light.color(normal,nocolors ? m : colors[0]),
          light.color(normal,nocolors ? m : colors[1]),
          light.color(normal,nocolors ? m : colors[2]),
          light.color(normal,nocolors ? m : colors[3])};
    }
    return new pen[] {light.color(normal00(),nocolors ? m : colors[0]),
        light.color(normal01(),nocolors ? m : colors[1]),
        light.color(normal11(),nocolors ? m : colors[2]),
        light.color(normal10(),nocolors ? m : colors[3])};
  }
  
  triple bound(real m(real[], real), triple b) {
    real x=m(new real[] {P[0][0].x,P[0][1].x,P[0][2].x,P[0][3].x,
                         P[1][0].x,P[1][1].x,P[1][2].x,P[1][3].x,
                         P[2][0].x,P[2][1].x,P[2][2].x,P[2][3].x,
                         P[3][0].x,P[3][1].x,P[3][2].x,P[3][3].x},b.x);
    real y=m(new real[] {P[0][0].y,P[0][1].y,P[0][2].y,P[0][3].y,
                         P[1][0].y,P[1][1].y,P[1][2].y,P[1][3].y,
                         P[2][0].y,P[2][1].y,P[2][2].y,P[2][3].y,
                         P[3][0].y,P[3][1].y,P[3][2].y,P[3][3].y},b.y);
    real z=m(new real[] {P[0][0].z,P[0][1].z,P[0][2].z,P[0][3].z,
                         P[1][0].z,P[1][1].z,P[1][2].z,P[1][3].z,
                         P[2][0].z,P[2][1].z,P[2][2].z,P[2][3].z,
                         P[3][0].z,P[3][1].z,P[3][2].z,P[3][3].z},b.z);
    return (x,y,z);
  }

  triple min3,max3;
  bool havemin3,havemax3;

  void init() {
    havemin3=false;
    havemax3=false;
  }

  triple min(triple bound=P[0][0]) {
    if(havemin3) return minbound(min3,bound);
    havemin3=true;
    return min3=bound(minbound,bound);
  }

  triple max(triple bound=P[0][0]) {
    if(havemax3) return maxbound(max3,bound);
    havemax3=true;
    return max3=bound(maxbound,bound);
  }

  triple center() {
    return 0.5*(this.min()+this.max());
  }

  pair min(projection P, pair bound=project(this.P[0][0],P.t)) {
    return minbound(controlpoints(),P.t,bound);
  }

  pair max(projection P, pair bound=project(this.P[0][0],P.t)) {
    return maxbound(controlpoints(),P.t,bound);
  }

  void operator init(triple[][] P, triple[] normals=new triple[],
                     pen[] colors=new pen[], bool straight=false,
                     bool3 planar=default) {
    init();
    this.P=copy(P);
    if(normals.length != 0)
      this.normals=copy(normals);
    if(colors.length != 0)
      this.colors=copy(colors);
    this.planar=planar;
    this.straight=straight;
  }

  void operator init(pair[][] P, triple plane(pair)=XYplane,
                     bool straight=false) {
    triple[][] Q=new triple[4][];
    for(int i=0; i < 4; ++i) {
      pair[] Pi=P[i];
      Q[i]=sequence(new triple(int j) {return plane(Pi[j]);},4);
    }
    operator init(Q,straight);
    planar=true;
  }

  void operator init(patch s) {
    operator init(s.P,s.normals,s.colors,s.straight);
  }
  
  // A constructor for a convex cyclic path3 of length <= 4 with optional
  // arrays of 4 internal points, corner normals, and pens.
  void operator init(path3 external, triple[] internal=new triple[],
                     triple[] normals=new triple[], pen[] colors=new pen[],
                     bool3 planar=default) {
    init();

    if(internal.length == 0 && planar == default)
      this.planar=normal(external) != O;
    else this.planar=planar;

    int L=length(external);
    if(L > 4 || !cyclic(external))
      abort("cyclic path3 of length <= 4 expected");
    if(L == 1) {
      external=external--cycle--cycle--cycle;
      if(colors.length > 0) colors.append(array(3,colors[0]));
      if(normals.length > 0) normals.append(array(3,normals[0]));
    } else if(L == 2) {
      external=external--cycle--cycle;
      if(colors.length > 0) colors.append(array(2,colors[0]));
      if(normals.length > 0) normals.append(array(2,normals[0]));
    } else if(L == 3) {
      external=external--cycle;
      if(colors.length > 0) colors.push(colors[0]);
      if(normals.length > 0) normals.push(normals[0]);
    }
    if(normals.length != 0)
      this.normals=copy(normals);
    if(colors.length != 0)
      this.colors=copy(colors);

    if(internal.length == 0) {
      straight=piecewisestraight(external);
      internal=new triple[4];
      for(int j=0; j < 4; ++j)
        internal[j]=nineth*(-4*point(external,j)
                            +6*(precontrol(external,j)+postcontrol(external,j))
                            -2*(point(external,j-1)+point(external,j+1))
                            +3*(precontrol(external,j-1)+
                                postcontrol(external,j+1))
                            -point(external,j+2));
    } else straight=false;

    P=new triple[][] {
      {point(external,0),postcontrol(external,0),precontrol(external,1),
       point(external,1)},
      {precontrol(external,0),internal[0],internal[1],postcontrol(external,1)},
      {postcontrol(external,3),internal[3],internal[2],precontrol(external,2)},
      {point(external,3),precontrol(external,3),postcontrol(external,2),
       point(external,2)}
    };
  }

  // A constructor for a convex quadrilateral.
  void operator init(triple[] external, triple[] internal=new triple[],
                     triple[] normals=new triple[], pen[] colors=new pen[],
                     bool3 planar=default) {
    init();

    if(internal.length == 0 && planar == default)
      this.planar=normal(external) != O;
    else this.planar=planar;

    if(normals.length != 0)
      this.normals=copy(normals);
    if(colors.length != 0)
      this.colors=copy(colors);

    if(internal.length == 0) {
      internal=new triple[4];
      for(int j=0; j < 4; ++j)
        internal[j]=nineth*(4*external[j]+2*external[(j+1)%4]+
                            external[(j+2)%4]+2*external[(j+3)%4]);
    }

    straight=true;

    triple delta[]=new triple[4];
    for(int j=0; j < 4; ++j)
      delta[j]=(external[(j+1)% 4]-external[j])/3;

    P=new triple[][] {
      {external[0],external[0]+delta[0],external[1]-delta[0],external[1]},
      {external[0]-delta[3],internal[0],internal[1],external[1]+delta[1]},
      {external[3]+delta[3],internal[3],internal[2],external[2]-delta[1]},
      {external[3],external[3]-delta[2],external[2]+delta[2],external[2]}
    };
  }
}

patch operator * (transform3 t, patch s)
{ 
  patch S;
  for(int i=0; i < 4; ++i) { 
    triple[] si=s.P[i];
    triple[] Si=S.P[i];
    for(int j=0; j < 4; ++j)
      Si[j]=t*si[j]; 
  }
  
  transform3 t0=shiftless(t);
  for(int i=0; i < s.normals.length; ++i)
    S.normals[i]=t0*s.normals[i];

  S.colors=copy(s.colors);
  S.planar=s.planar;
  S.straight=s.straight;
  return S;
}
 
patch reverse(patch s) 
{
  patch S;
  S.P=transpose(s.P);
  if(s.normals.length > 0) 
    S.normals=
      new triple[] {s.normals[0],s.normals[3],s.normals[2],s.normals[1]};
  if(s.colors.length > 0) 
    S.colors=new pen[] {s.colors[0],s.colors[3],s.colors[2],s.colors[1]};
  S.planar=s.planar;
  S.straight=s.straight;
  return S;
}

struct surface {
  patch[] s;
  
  bool empty() {
    return s.length == 0;
  }

  void operator init(int n) {
    s=new patch[n];
  }

  void operator init(... patch[] s) {
    this.s=s;
  }

  void operator init(surface s) {
    this.s=new patch[s.s.length];
    for(int i=0; i < s.s.length; ++i)
      this.s[i]=patch(s.s[i]);
  }

  void operator init(triple[][][] P, triple[][] normals=new triple[][],
                     pen[][] colors=new pen[][], bool3 planar=default) {
    s=sequence(new patch(int i) {
        return patch(P[i],normals.length == 0 ? new triple[] : normals[i],
                     colors.length == 0 ? new pen[] : colors[i],planar);
      },P.length);
  }

  void colors(pen[][] palette) {
    for(int i=0; i < s.length; ++i) {
      pen[] palettei=palette[i];
      s[i].colors=new pen[] {palettei[0],palettei[1],palettei[2],palettei[3]};
    }
  }

  triple[][] corners() {
    triple[][] a=new triple[s.length][];
    for(int i=0; i < s.length; ++i)
      a[i]=s[i].corners();
    return a;
  }

  real[][] map(real f(triple)) {
    real[][] a=new real[s.length][];
    for(int i=0; i < s.length; ++i)
      a[i]=s[i].map(f);
    return a;
  }

  triple[] cornermean() {
    return sequence(new triple(int i) {return s[i].cornermean();},s.length);
  }

  // A constructor for a possibly nonconvex cyclic path in a given plane.
  void operator init (path p, triple plane(pair)=XYplane,
                      bool checkboundary=true) {
    if(!cyclic(p))
      abort("cyclic path expected");

    int L=length(p);

    if(L > 4) {
      for(path g : bezulate(p))
        s.append(surface(g,plane,checkboundary).s);
      return;
    }
        
    pair[][] P(path p) {
      if(L == 1)
        p=p--cycle--cycle--cycle;
      else if(L == 2)
        p=p--cycle--cycle;
      else if(L == 3)
        p=p--cycle;
 
      pair[] internal=new pair[4];
      for(int j=0; j < 4; ++j) {
        internal[j]=nineth*(-4*point(p,j)
                            +6*(precontrol(p,j)+postcontrol(p,j))
                            -2*(point(p,j-1)+point(p,j+1))
                            +3*(precontrol(p,j-1)+postcontrol(p,j+1))
                            -point(p,j+2));
      }
    
      return new pair[][] {
        {point(p,0),postcontrol(p,0),precontrol(p,1),point(p,1)},
          {precontrol(p,0),internal[0],internal[1],postcontrol(p,1)},
            {postcontrol(p,3),internal[3],internal[2],precontrol(p,2)},
              {point(p,3),precontrol(p,3),postcontrol(p,2),point(p,2)}
      };
    }

    bool straight=piecewisestraight(p);
    if(L <= 3 && straight) {
      s=new patch[] {patch(P(p),plane,straight)};
      return;
    }
    
    // Split p along the angle bisector at t.
    bool split(path p, real t) {
      pair dir=dir(p,t);
      if(dir != 0) {
        path g=subpath(p,t,t+length(p));
        int L=length(g);
        pair z=point(g,0);
        real[] T=intersections(g,z,z+I*dir);
        for(int i=0; i < T.length; ++i) {
          real cut=T[i];
          if(cut > sqrtEpsilon && cut < L-sqrtEpsilon) {
            pair w=point(g,cut);
            if(!inside(p,0.5*(z+w),zerowinding)) continue;
            pair delta=sqrtEpsilon*(w-z);
            if(intersections(g,z-delta--w+delta).length != 2) continue;
            s=surface(subpath(g,0,cut)--cycle,plane,checkboundary).s;
            s.append(surface(subpath(g,cut,L)--cycle,plane,checkboundary).s);
            return true;
          }
        }
      }
      return false;
    }

    // Ensure that all interior angles are less than 180 degrees.
    real fuzz=1e-4;
    int sign=sgn(windingnumber(p,inside(p,zerowinding)));
    for(int i=0; i < L; ++i) {
      if(sign*(conj(dir(p,i,-1))*dir(p,i,1)).y < -fuzz) {
        if(split(p,i)) return;
      }
    }

    pair[][] P=P(p);

    if(straight) {
      s=new patch[] {patch(P,plane,straight)};
      return;
    }
    
    // Check for degeneracy.
    pair[][] U=new pair[3][4];
    pair[][] V=new pair[4][3];

    for(int i=0; i < 3; ++i) {
      for(int j=0; j < 4; ++j)
        U[i][j]=P[i+1][j]-P[i][j];
    }
      
    for(int i=0; i < 4; ++i) {
      for(int j=0; j < 3; ++j)
        V[i][j]=P[i][j+1]-P[i][j];
    }
      
    int[] choose2={1,2,1};
    int[] choose3={1,3,3,1};

    real T[][]=new real[6][6];
    for(int p=0; p < 6; ++p) {
      int kstart=max(p-2,0);
      int kstop=min(p,3);
      real[] Tp=T[p];
      for(int q=0; q < 6; ++q) {
        real Tpq;
        int jstop=min(q,3);
        int jstart=max(q-2,0);
        for(int k=kstart; k <= kstop; ++k) {
          int choose3k=choose3[k];
          for(int j=jstart; j <= jstop; ++j) {
            int i=p-k;
            int l=q-j;
            Tpq += (conj(U[i][j])*V[k][l]).y*
              choose2[i]*choose3k*choose3[j]*choose2[l];
          }
        }
        Tp[q]=Tpq;
      }
    }

    bool3 aligned=default;
    bool degenerate=false;

    for(int p=0; p < 6; ++p) {
      for(int q=0; q < 6; ++q) {
        if(aligned == default) {
          if(T[p][q] < -sqrtEpsilon) aligned=true;
          if(T[p][q] > sqrtEpsilon) aligned=false;
        } else {
          if((T[p][q] < -sqrtEpsilon && aligned == false) ||
             (T[p][q] > sqrtEpsilon && aligned == true)) degenerate=true;
        }
      }
    }

    if(!degenerate) {
      if(aligned == (sign >= 0))
        s=new patch[] {patch(P,plane)};
      return;
    }

    if(checkboundary) {
      // Polynomial coefficients of (B_i'' B_j + B_i' B_j')/3.
      static real[][][] fpv0={
        {{5, -20, 30, -20, 5},
         {-3, 24, -54, 48, -15},
         {0, -6, 27, -36, 15},
         {0, 0, -3, 8, -5}},
        {{-7, 36, -66, 52, -15},
         {3, -36, 108, -120, 45},
         {0, 6, -45, 84, -45},
         {0, 0, 3, -16, 15}},
        {{2, -18, 45, -44, 15},
         {0, 12, -63, 96, -45},
         {0, 0, 18, -60, 45},
         {0, 0, 0, 8, -15}},
        {{0, 2, -9, 12, -5},
         {0, 0, 9, -24, 15},
         {0, 0, 0, 12, -15},
         {0, 0, 0, 0, 5}}
      };

      // Compute one-ninth of the derivative of the Jacobian along the boundary.
      real[][] c=array(4,array(5,0.0));
      for(int i=0; i < 4; ++i) {
        real[][] fpv0i=fpv0[i];
        for(int j=0; j < 4; ++j) {
          real[] w=fpv0i[j];
          c[0] += w*(conj(P[0][j]-P[1][j])*P[0][i]).y;   // u=0
          c[1] += w*(conj(P[i][3])*(P[j][3]-P[j][2])).y; // v=1
          c[2] += w*(conj(P[3][j]-P[2][j])*P[3][i]).y;   // u=1
          c[3] += w*(conj(P[i][0])*(P[j][1]-P[j][0])).y; // v=0
        }
      }
    
      pair BuP(int j, real u) {
        return bezierP(P[0][j],P[1][j],P[2][j],P[3][j],u);
      }
      pair BvP(int i, real v) {
        return bezierP(P[i][0],P[i][1],P[i][2],P[i][3],v);
      }
      real normal(real u, real v) {
        return (conj(bezier(BvP(0,v),BvP(1,v),BvP(2,v),BvP(3,v),u))*
                bezier(BuP(0,u),BuP(1,u),BuP(2,u),BuP(3,u),v)).y;
      }

      // Use Rolle's theorem to check for degeneracy on the boundary.
      real M=0;
      real cut;
      for(int i=0; i < 4; ++i) {
        if(!straight(p,i)) {
          real[] ci=c[i];
          pair[] R=quarticroots(ci[4],ci[3],ci[2],ci[1],ci[0]);
          for(pair r : R) {
            if(fabs(r.y) < sqrtEpsilon) {
              real t=r.x;
              if(0 <= t && t <= 1) {
                real[] U={0,t,1,t};
                real[] V={t,1,t,0};
                real[] T={t,t,1-t,1-t};
                real N=sign*normal(U[i],V[i]);
                if(N < M) {
                  M=N; cut=i+T[i];
                }
              }
            }
          }
        }
      }

      // Split at the worst boundary degeneracy.
      if(M < 0 && split(p,cut)) return;
    }
    
    // Split arbitrarily to resolve any remaining (internal) degeneracy.
    checkboundary=false;
    for(int i=0; i < L; ++i)
      if(!straight(p,i) && split(p,i+0.5)) return;

    while(true)
      for(int i=0; i < L; ++i)
        if(!straight(p,i) && split(p,i+unitrand())) return;
  }

  void operator init(explicit path[] g, triple plane(pair)=XYplane) {
    for(path p : bezulate(g))
      s.append(surface(p,plane).s);
  }

  // A general surface constructor for both planar and nonplanar 3D paths.
  void construct(path3 external, triple[] internal=new triple[],
                 triple[] normals=new triple[], pen[] colors=new pen[],
                 bool3 planar=default) {
    int L=length(external);
    if(!cyclic(external)) abort("cyclic path expected");

    if(L <= 3 && piecewisestraight(external)) {
      s.push(patch(external,internal,normals,colors,planar=true));
      return;
    }

    // Construct a surface from a possibly nonconvex planar cyclic path3.
    if(planar != false && internal.length == 0 && normals.length == 0 &&
       colors.length == 0) {
      triple n=normal(external);
      if(n != O) {
        transform3 T=align(n);
        external=transpose(T)*external;
        T *= shift(0,0,point(external,0).z);
        for(patch p : surface(path(external)).s)
          s.push(T*p);
        return;
      }
    }
    
    if(L <= 4 || internal.length > 0) {
      s.push(patch(external,internal,normals,colors,planar));
      return;
    }
      
    // Path is not planar; split into patches.
    real factor=1/L;
    pen[] p;
    triple[] n;
    bool nocolors=colors.length == 0;
    bool nonormals=normals.length == 0;
    triple center;
    for(int i=0; i < L; ++i)
      center += point(external,i);
    center *= factor;
    if(!nocolors)
      p=new pen[] {mean(colors)};
    if(!nonormals)
      n=new triple[] {factor*sum(normals)};
    // Use triangles for nonplanar surfaces.
    int step=normal(external) == O ? 1 : 2;
    int i=0;
    int end;
    while((end=i+step) < L) {
      s.push(patch(subpath(external,i,end)--center--cycle,
                   nonormals ? n : concat(normals[i:end+1],n),
                   nocolors ? p : concat(colors[i:end+1],p),planar));
      i=end;
    }
    s.push(patch(subpath(external,i,L)--center--cycle,
                 nonormals ? n : concat(normals[i:],normals[0:1],n),
                 nocolors ? p : concat(colors[i:],colors[0:1],p),planar));
  }

  void operator init(path3 external, triple[] internal=new triple[],
                     triple[] normals=new triple[], pen[] colors=new pen[],
                     bool3 planar=default) {
    s=new patch[];
    construct(external,internal,normals,colors,planar);
  }

  void operator init(explicit path3[] external,
                     triple[][] internal=new triple[][],
                     triple[][] normals=new triple[][],
                     pen[][] colors=new pen[][], bool3 planar=default) {
    s=new patch[];
    for(int i=0; i < external.length; ++i)
      construct(external[i],
                internal.length == 0 ? new triple[] : internal[i],
                normals.length == 0 ? new triple[] : normals[i],
                colors.length == 0 ? new pen[] : colors[i],planar);
  }

  void push(path3 external, triple[] internal=new triple[],
            triple[] normals=new triple[] ,pen[] colors=new pen[],
            bool3 planar=default) {
    s.push(patch(external,internal,normals,colors,planar));
  }

  // Construct the surface of rotation generated by rotating g
  // from angle1 to angle2 sampled n times about the line c--c+axis.
  // An optional surface pen color(int i, real j) may be specified
  // to override the color at vertex(i,j).
  void operator init(triple c, path3 g, triple axis, int n=nslice,
                     real angle1=0, real angle2= 360,
                     pen color(int i, real j)=null) {
    axis=unit(axis);
    real w=(angle2-angle1)/n;
    int L=length(g);
    s=new patch[L*n];
    int m=-1;
    transform3[] T=new transform3[n+1];
    transform3 t=rotate(w,c,c+axis);
    T[0]=rotate(angle1,c,c+axis);
    for(int k=1; k <= n; ++k)
      T[k]=T[k-1]*t;

    for(int i=0; i < L; ++i) {
      path3 h=subpath(g,i,i+1);
      path3 r=reverse(h);
      triple max=max(h);
      triple min=min(h);
      triple perp=perp(max-c,axis);
      real fuzz=epsilon*max(abs(max),abs(min));
      if(abs(perp) < fuzz)
        perp=perp(min-c,axis);
      perp=unit(perp);
      triple normal=cross(axis,perp);
      triple dir(real j) {return Cos(j)*normal-Sin(j)*perp;}
      real j=angle1;
      transform3 Tk=T[0];
      triple dirj=dir(j);
      for(int k=0; k < n; ++k, j += w) {
        transform3 Tp=T[k+1];
        triple dirp=dir(j+w);
        path3 G=Tk*h{dirj}..{dirp}Tp*r{-dirp}..{-dirj}cycle;
        Tk=Tp;
        dirj=dirp;
        s[++m]=color == null ? patch(G) :
          patch(G,new pen[] {color(i,j),color(i+1,j),color(i+1,j+w),
                             color(i,j+w)});
      }
    }
  }

  void push(patch s) {
    this.s.push(s);
  }

  void append(surface s) {
    this.s.append(s.s);
  }

  void operator init(... surface[] s) {
    for(surface S : s)
      this.s.append(S.s);
  }
}

surface operator * (transform3 t, surface s)
{ 
  surface S;
  S.s=new patch[s.s.length];
  for(int i=0; i < s.s.length; ++i)
    S.s[i]=t*s.s[i];
  return S;
}

private string nullsurface="null surface";

triple min(surface s)
{
  if(s.s.length == 0)
    abort(nullsurface);
  triple bound=s.s[0].min();
  for(int i=1; i < s.s.length; ++i)
    bound=s.s[i].min(bound);
  return bound;
}
  
triple max(surface s)
{
  if(s.s.length == 0)
    abort(nullsurface);
  triple bound=s.s[0].max();
  for(int i=1; i < s.s.length; ++i)
    bound=s.s[i].max(bound);
  return bound;
}

pair min(surface s, projection P)
{
  if(s.s.length == 0)
    abort(nullsurface);
  pair bound=s.s[0].min(P);
  for(int i=1; i < s.s.length; ++i)
    bound=s.s[i].min(P,bound);
  return bound;
}
  
pair max(surface s, projection P)
{
  if(s.s.length == 0)
    abort(nullsurface);
  pair bound=s.s[0].max(P);
  for(int i=1; i < s.s.length; ++i)
    bound=s.s[i].max(P,bound);
  return bound;
}

private triple[] split(triple z0, triple c0, triple c1, triple z1, real t=0.5)
{
  triple m0=interp(z0,c0,t);
  triple m1=interp(c0,c1,t);
  triple m2=interp(c1,z1,t);
  triple m3=interp(m0,m1,t);
  triple m4=interp(m1,m2,t);
  triple m5=interp(m3,m4,t);

  return new triple[] {m0,m3,m5,m4,m2};
}

// Return the control points for a subpatch of P on [u,1] x [v,1].
triple[][] subpatchbegin(triple[][] P, real u, real v)
{
  triple[] P0=P[0];
  triple[] P1=P[1];
  triple[] P2=P[2];
  triple[] P3=P[3];

  triple[] c0=split(P0[0],P0[1],P0[2],P0[3],v);
  triple[] c1=split(P1[0],P1[1],P1[2],P1[3],v);
  triple[] c2=split(P2[0],P2[1],P2[2],P2[3],v);
  triple[] c3=split(P3[0],P3[1],P3[2],P3[3],v);

  u=1.0-u;
  
  triple[] c7=split(c3[2],c2[2],c1[2],c0[2],u);
  triple[] c8=split(c3[3],c2[3],c1[3],c0[3],u);
  triple[] c9=split(c3[4],c2[4],c1[4],c0[4],u);
  triple[] c10=split(P3[3],P2[3],P1[3],P0[3],u);

  return new triple[][] {{c7[2],c8[2],c9[2],c10[2]},
      {c7[1],c8[1],c9[1],c10[1]},
        {c7[0],c8[0],c9[0],c10[0]},
          {c3[2],c3[3],c3[4],P3[3]}};
}

// Return the control points for a subpatch of P on [0,u] x [0,v].
triple[][] subpatchend(triple[][] P, real u, real v)
{
  triple[] P0=P[0];
  triple[] P1=P[1];
  triple[] P2=P[2];
  triple[] P3=P[3];

  triple[] c0=split(P0[0],P0[1],P0[2],P0[3],v);
  triple[] c1=split(P1[0],P1[1],P1[2],P1[3],v);
  triple[] c2=split(P2[0],P2[1],P2[2],P2[3],v);
  triple[] c3=split(P3[0],P3[1],P3[2],P3[3],v);

  u=1.0-u;
  
  triple[] c4=split(P3[0],P2[0],P1[0],P0[0],u);
  triple[] c5=split(c3[0],c2[0],c1[0],c0[0],u);
  triple[] c6=split(c3[1],c2[1],c1[1],c0[1],u);
  triple[] c7=split(c3[2],c2[2],c1[2],c0[2],u);

  return new triple[][] {
    {P0[0],c0[0],c0[1],c0[2]},
      {c4[4],c5[4],c6[4],c7[4]},
        {c4[3],c5[3],c6[3],c7[3]},
          {c4[2],c5[2],c6[2],c7[2]}};
}

patch subpatch(patch s, real ua, real va, real ub, real vb)
{
  assert(ua >= 0 && va >= 0 && ub <= 1 && vb <= 1 && ua < ub && va < vb);
  return patch(subpatchbegin(subpatchend(s.P,ub,vb),ua/ub,va/vb),
               s.straight,s.planar);
}

triple point(patch s, real u, real v)
{
  return s.point(u,v);
}

void draw3D(frame f, patch s, material m, light light=currentlight)
{
  if(s.colors.length > 0)
    m=mean(s.colors);
  bool lighton=light.on();
  if(!lighton && !invisible((pen) m))
    m=emissive(m);
  real granularity=m.granularity >= 0 ? m.granularity : defaultgranularity;
  draw(f,s.P,s.straight,m.p,m.opacity,m.shininess,granularity,
       s.planar ? s.normal(0.5,0.5) : O,lighton,s.colors);
}

void tensorshade(transform t=identity(), frame f, patch s,
                 material m, light light=currentlight, projection P)
{
  tensorshade(f,box(t*s.min(P),t*s.max(P)),m.diffuse(),
              s.colors(m,light),t*project(s.external(),P,1),
              t*project(s.internal(),P));
}

restricted pen[] nullpens={nullpen};
nullpens.cyclic(true);

void draw(transform t=identity(), frame f, surface s, int nu=1, int nv=1,
          material[] surfacepen, pen[] meshpen=nullpens,
          light light=currentlight, light meshlight=light,
          projection P=currentprojection)
{
  if(is3D()) {
    for(int i=0; i < s.s.length; ++i)
      draw3D(f,s.s[i],surfacepen[i],light);
    pen modifiers=thin()+squarecap;
    for(int k=0; k < s.s.length; ++k) {
      pen meshpen=meshpen[k];
      if(!invisible(meshpen)) {
        meshpen=modifiers+meshpen;
        real step=nu == 0 ? 0 : 1/nu;
        for(int i=0; i <= nu; ++i)
          draw(f,s.s[k].uequals(i*step),meshpen,meshlight);
        step=nv == 0 ? 0 : 1/nv;
        for(int j=0; j <= nv; ++j)
          draw(f,s.s[k].vequals(j*step),meshpen,meshlight);
      }
    }
  } else {
    begingroup(f);
    // Sort patches by mean distance from camera
    triple camera=P.camera;
    if(P.infinity) {
      triple m=min(s);
      triple M=max(s);
      camera=P.target+camerafactor*(abs(M-m)+abs(m-P.target))*unit(P.vector());
    }

    real[][] depth=new real[s.s.length][];
    for(int i=0; i < depth.length; ++i)
      depth[i]=new real[] {abs(camera-s.s[i].cornermean()),i};

    depth=sort(depth);

    light.T=shiftless(P.modelview());

    // Draw from farthest to nearest
    while(depth.length > 0) {
      real[] a=depth.pop();
      int i=round(a[1]);
      tensorshade(t,f,s.s[i],surfacepen[i],light,P);
      pen meshpen=meshpen[i];
      if(!invisible(meshpen))
        draw(f,t*project(s.s[i].external(),P),meshpen);
    }
    endgroup(f);
  }
}

void draw(transform t=identity(), frame f, surface s, int nu=1, int nv=1,
          material surfacepen=currentpen, pen meshpen=nullpen,
          light light=currentlight, light meshlight=light,
          projection P=currentprojection)
{
  material[] surfacepen={surfacepen};
  pen[] meshpen={meshpen};
  surfacepen.cyclic(true);
  meshpen.cyclic(true);
  draw(t,f,s,nu,nv,surfacepen,meshpen,light,meshlight,P);
}

void draw(picture pic=currentpicture, surface s, int nu=1, int nv=1,
          material[] surfacepen, pen[] meshpen=nullpens,
          light light=currentlight, light meshlight=light)
{
  if(s.empty()) return;

  pic.add(new void(frame f, transform3 t, picture pic, projection P) {
      surface S=t*s;
      if(is3D()) {
        draw(f,S,nu,nv,surfacepen,meshpen,light,meshlight);
      } else if(pic != null)
        pic.add(new void(frame f, transform T) {
            draw(T,f,S,nu,nv,surfacepen,meshpen,light,meshlight,P);
          },true);
      if(pic != null) {
        pic.addPoint(min(S,P));
        pic.addPoint(max(S,P));
      }
    },true);
  pic.addPoint(min(s));
  pic.addPoint(max(s));

  pen modifiers;
  if(is3D()) modifiers=thin()+squarecap;
  for(int k=0; k < s.s.length; ++k) {
    pen meshpen=meshpen[k];
    if(!invisible(meshpen)) {
      meshpen=modifiers+meshpen;
      real step=nu == 0 ? 0 : 1/nu;
      for(int i=0; i <= nu; ++i)
        addPath(pic,s.s[k].uequals(i*step),meshpen);
      step=nv == 0 ? 0 : 1/nv;
      for(int j=0; j <= nv; ++j)
        addPath(pic,s.s[k].vequals(j*step),meshpen);
    }
  }
}

void draw(picture pic=currentpicture, surface s, int nu=1, int nv=1,
          material surfacepen=currentpen, pen meshpen=nullpen,
          light light=currentlight, light meshlight=light)
{
  material[] surfacepen={surfacepen};
  pen[] meshpen={meshpen};
  surfacepen.cyclic(true);
  meshpen.cyclic(true);
  draw(pic,s,nu,nv,surfacepen,meshpen,light,meshlight);
}

void draw(picture pic=currentpicture, surface s, int nu=1, int nv=1,
          material[] surfacepen, pen meshpen,
          light light=currentlight, light meshlight=light)
{
  pen[] meshpen={meshpen};
  meshpen.cyclic(true);
  draw(pic,s,nu,nv,surfacepen,meshpen,light,meshlight);
}

surface extrude(path p, triple axis=Z)
{
  static patch[] allocate;
  path3 G=path3(p);
  path3 G2=shift(axis)*G;
  return surface(...sequence(new patch(int i) {
        return patch(subpath(G,i,i+1)--subpath(G2,i+1,i)--cycle);
      },length(G)));
}

surface extrude(explicit path[] p, triple axis=Z)
{
  surface s;
  for(path g:p)
    s.append(extrude(g,axis));
  return s;
}

triple rectify(triple dir) 
{
  real scale=max(abs(dir.x),abs(dir.y),abs(dir.z));
  if(scale != 0) dir *= 0.5/scale;
  dir += (0.5,0.5,0.5);
  return dir;
}

path3[] align(path3[] g, transform3 t=identity4, triple position,
              triple align, pen p=currentpen)
{
  if(determinant(t) == 0) return g;
  triple m=min(g);
  triple dir=rectify(inverse(t)*-align);
  triple a=m+realmult(dir,max(g)-m);
  return shift(position+align*labelmargin(p))*t*shift(-a)*g;
}

surface align(surface s, transform3 t=identity4, triple position,
              triple align, pen p=currentpen)
{
  if(determinant(t) == 0) return s;
  triple m=min(s);
  triple dir=rectify(inverse(t)*-align);
  triple a=m+realmult(dir,max(s)-m);
  return shift(position+align*labelmargin(p))*t*shift(-a)*s;
}

surface surface(Label L, triple position=O)
{
  surface s=surface(texpath(L));
  return L.align.is3D ? align(s,L.T3,position,L.align.dir3,L.p) :
    shift(position)*L.T3*s;
}

path[] path(Label L, pair z=0, projection P)
{
  path[] g=texpath(L);
  if(L.defaulttransform3) {
    return L.align.is3D ? align(g,z,project(L.align.dir3,P)-project(O,P),L.p) :
      shift(z)*g;
  } else {
    path3[] G=path3(g);
    return L.align.is3D ? shift(z)*project(align(G,L.T3,O,L.align.dir3,L.p),P) :
      shift(z)*project(L.T3*G,P);
  }
}

void label(frame f, Label L, triple position, align align=NoAlign,
           pen p=currentpen, light light=nolight,
           projection P=currentprojection)
{
  Label L=L.copy();
  L.align(align);
  L.p(p);
  if(L.defaulttransform3)
    L.T3=transform3(P);
  if(is3D()) {
    for(patch S : surface(L,position).s)
      draw3D(f,S,L.p,light);
  } else {
    if(L.filltype == NoFill)
      fill(f,path(L,project(position,P.t),P),
           light.color(L.T3*Z,L.p,shiftless(P.modelview())));
    else {
      frame d;
      fill(d,path(L,project(position,P.t),P),
           light.color(L.T3*Z,L.p,shiftless(P.modelview())));
      add(f,d,L.filltype);
    }
  }
}

void label(picture pic=currentpicture, Label L, triple position,
           align align=NoAlign, pen p=currentpen, light light=nolight)
{
  Label L=L.copy();
  L.align(align);
  L.p(p);
  L.position(0);
  path[] g=texpath(L);
  if(g.length == 0 || (g.length == 1 && size(g[0]) == 0)) return;
  pic.add(new void(frame f, transform3 t, picture pic, projection P) {
      triple v=t*position;
      if(L.defaulttransform3)
        L.T3=transform3(P);
      if(is3D())
        for(patch S : surface(L,v).s)
          draw3D(f,S,L.p,light);
      if(pic != null) {
        if(L.filltype == NoFill)
          fill(project(v,P.t),pic,path(L,P),
               light.color(L.T3*Z,L.p,shiftless(P.modelview())));
        else {
          picture d;
          fill(project(v,P.t),d,path(L,P),
               light.color(L.T3*Z,L.p,shiftless(P.modelview())));
          add(pic,d,L.filltype);
        }
      }
    },!L.defaulttransform3);

  if(L.defaulttransform3)
    L.T3=transform3(currentprojection);
  path3[] G=path3(g);
  G=L.align.is3D ? align(G,L.T3,O,L.align.dir3,L.p) : L.T3*G;
  pic.addBox(position,position,min(G),max(G));
}

void label(picture pic=currentpicture, Label L, path3 g, align align=NoAlign,
           pen p=currentpen)
{
  Label L=Label(L,align,p);
  bool relative=L.position.relative;
  real position=L.position.position.x;
  pair Align=L.align.dir;
  bool alignrelative=L.align.relative;
  if(L.defaultposition) {relative=true; position=0.5;}
  if(relative) position=reltime(g,position);
  if(L.align.default) {
    alignrelative=true;
    Align=position <= 0 ? S : position >= length(g) ? N : E;
  }
  label(pic,L,point(g,position),
        alignrelative && determinant(currentprojection.t) != 0 ?
        -Align*project(dir(g,position),currentprojection.t)*I : L.align);
}

surface extrude(Label L, triple axis=Z)
{
  Label L=L.copy();
  path[] g=texpath(L);
  surface S=extrude(g,axis);
  surface s=surface(g);
  S.append(s);
  S.append(shift(axis)*s);
  return S;
}

restricted surface nullsurface;

private real a=4/3*(sqrt(2)-1);
private transform3 t=rotate(90,O,Z);
private transform3 t2=t*t;
private transform3 t3=t2*t;
private transform3 i=xscale3(-1)*zscale3(-1);

restricted patch octant1=patch(X{Z}..{-X}Z..Z{Y}..{-Z}Y{X}..{-Y}cycle,
                               new triple[] {(1,a,a),(a,a^2,1),(a^2,a,1),
                                             (a,1,a)});

restricted surface unithemisphere=surface(octant1,t*octant1,t2*octant1,
                                          t3*octant1);
restricted surface unitsphere=surface(octant1,t*octant1,t2*octant1,t3*octant1,
                                      i*octant1,i*t*octant1,i*t2*octant1,
                                      i*t3*octant1);

restricted patch unitfrustum(real t1, real t2)
{
  real s1=interp(t1,t2,1/3);
  real s2=interp(t1,t2,2/3);
  return patch(interp(Z,X,t2)--interp(Z,X,t1){Y}..{-X}interp(Z,Y,t1)--
               interp(Z,Y,t2){X}..{-Y}cycle,
               new triple[] {(s2,s2*a,1-s2),(s1,s1*a,1-s1),(s1*a,s1,1-s1),
                                          (s2*a,s2,1-s2)});
}

// Return a unitcone constructed from n frusta (the final one being degenerate)
surface unitcone(int n=6)
{
  surface unitcone;
  unitcone.s=new patch[4*n];
  real r=1/3;
  for(int i=0; i < n; ++i) {
    patch s=unitfrustum(i < n-1 ? r^(i+1) : 0,r^i);
    unitcone.s[i]=s;
    unitcone.s[n+i]=t*s;
    unitcone.s[2n+i]=t2*s;
    unitcone.s[3n+i]=t3*s;
  }
  return unitcone;
}

restricted surface unitcone=unitcone();
restricted surface unitsolidcone=surface(patch(unitcircle3)...unitcone.s);

private patch unitcylinder1=patch(X--X+Z{Y}..{-X}Y+Z--Y{X}..{-Y}cycle);

restricted surface unitcylinder=surface(unitcylinder1,t*unitcylinder1,
                                        t2*unitcylinder1,t3*unitcylinder1);

private patch unitplane=patch(new triple[] {O,X,X+Y,Y});
restricted surface unitcube=surface(reverse(unitplane),
                                    rotate(90,O,X)*unitplane,
                                    rotate(-90,O,Y)*unitplane,
                                    shift(Z)*unitplane,
                                    rotate(90,X,X+Y)*unitplane,
                                    rotate(-90,Y,X+Y)*unitplane);
restricted surface unitplane=surface(unitplane);
restricted surface unitdisk=surface(unitcircle3);

void dot(frame f, triple v, material p=currentpen,
         light light=nolight, projection P=currentprojection)
{
  pen q=(pen) p;
  if(is3D()) {
    material m=material(p,p.granularity >= 0 ? p.granularity : dotgranularity);
    for(patch s : unitsphere.s)
      draw3D(f,shift(v)*scale3(0.5*linewidth(dotsize(q)+q))*s,m,light);
  } else dot(f,project(v,P.t),q);
}

void dot(frame f, path3 g, material p=currentpen,
         projection P=currentprojection)
{
  for(int i=0; i <= length(g); ++i) dot(f,point(g,i),p,P);
}

void dot(frame f, path3[] g, material p=currentpen,
         projection P=currentprojection)
{
  for(int i=0; i < g.length; ++i) dot(f,g[i],p,P);
}

void dot(picture pic=currentpicture, triple v, material p=currentpen,
         light light=nolight)
{
  pen q=(pen) p;
  real size=0.5*linewidth(dotsize(q)+q);
  pic.add(new void(frame f, transform3 t, picture pic, projection P) {
      if(is3D()) {
        material m=material(p,p.granularity >= 0 ? p.granularity :
                            dotgranularity);
        for(patch s : unitsphere.s)
          draw3D(f,shift(t*v)*scale3(size)*s,m,light);
      }
      if(pic != null)
        dot(pic,project(t*v,P.t),q);
    },true);
  triple R=size*(1,1,1);
  pic.addBox(v,v,-R,R);
}

void dot(picture pic=currentpicture, triple[] v, material p=currentpen)
{
  for(int i=0; i < v.length; ++i) dot(pic,v[i],p);
}

void dot(picture pic=currentpicture, explicit path3 g, material p=currentpen)
{
  for(int i=0; i <= length(g); ++i) dot(pic,point(g,i),p);
}

void dot(picture pic=currentpicture, path3[] g, material p=currentpen)
{
  for(int i=0; i < g.length; ++i) dot(pic,g[i],p);
}

void dot(picture pic=currentpicture, Label L, triple v, align align=NoAlign,
         string format=defaultformat, material p=currentpen)
{
  Label L=L.copy();
  if(L.s == "") {
    if(format == "") format=defaultformat;
    L.s="("+format(format,v.x)+","+format(format,v.y)+","+
      format(format,v.z)+")";
  }
  L.align(align,E);
  L.p((pen) p);
  dot(pic,v,p);
  label(pic,L,v);
}
