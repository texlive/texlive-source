import solids;

size(0,100);

revolution r=cylinder(O,1,1.5,Y+Z);
draw(surface(r),green);
draw(r,blue);
