import graph;
size(300,0);

real f(real x) {return (x != 0.0) ? x * sin(1.0 / x) : 0.0;}
pair F(real x) {return (x,f(x));}

xaxis("$x$",red);
yaxis(red);
draw(graph(f,-1.2/pi,1.2/pi,1000));
label("$x\sin\frac{1}{x}$",F(1.1/pi),NW);

picture pic;
size(pic,50,IgnoreAspect);
xaxis(pic,red);
yaxis(pic,red);
draw(pic,graph(pic,f,-0.1/pi,0.1/pi,1000));
attach(bbox(pic,blue),point(N+0.85W),10SE);

