settings.outformat = "png";;
settings.render = 16;
size(8cm);

import smoothcontour3;

// Erdos lemniscate of order n:
real erdos(pair z, int n) { return abs(z^n-1)^2 - 1; }

real h = 0.12;

real lemn3(real x, real y) { return erdos((x,y), 3); }

real f(real x, real y, real z) {
  return lemn3(x,y)^2 + (16*abs((x,y))^4 + 1) * (z^2 - h^2);
}

draw(implicitsurface(f,a=(-3,-3,-3),b=(3,3,3),overlapedges=true),
     surfacepen=material(diffusepen=gray(0.5),emissivepen=gray(0.4),
                         specularpen=gray(0.1)));
