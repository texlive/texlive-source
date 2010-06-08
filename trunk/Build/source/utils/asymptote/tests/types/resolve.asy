import TestLib;
StartTest("resolve");
struct A {} struct B {} struct C {}

int f(B, real) { return 1; }
int f(C, int) { return 2; }
B operator cast(A) { return new B; }

assert(f(new A, 3) == 1);
C operator cast(A) { return new C; }
assert(f(new A, 3) == 2);

int givex(int x, int y) { return x; }
assert(givex(2002,3) == 2002);
assert(givex(2002,2002) == 2002);
assert(givex(-2005,2005) == -2005);
assert(givex(x=-77,205) == -77);
assert(givex(-77,y=205) == -77);
assert(givex(-77,x=205) == 205);
assert(givex(x=-77,y=205) == -77);
assert(givex(y=-77,x=205) == 205);

int g(real x, real y) { return 7; }
int g(int x, real y) { return 8; }

assert(g(4, 4) == 8);
assert(g(4, 4.4) == 8);
assert(g(4.4, 4) == 7);
assert(g(4.4, 4.4) == 7);

assert(g(x=4, y=4) == 8);
assert(g(x=4, y=4.4) == 8);
assert(g(x=4.4, y=4) == 7);
assert(g(x=4.4, y=4.4) == 7);

assert(g(x=4, 4) == 8);
assert(g(x=4, 4.4) == 8);
assert(g(x=4.4, 4) == 7);
assert(g(x=4.4, 4.4) == 7);

assert(g(4, y=4) == 8);
assert(g(4, y=4.4) == 8);
assert(g(4.4, y=4) == 7);
assert(g(4.4, y=4.4) == 7);

assert(g(y=4, x=4) == 8);
assert(g(y=4, x=4.4) == 7);
assert(g(y=4.4, x=4) == 8);
assert(g(y=4.4, x=4.4) == 7);

assert(g(4, x=4) == 8);
assert(g(4, x=4.4) == 7);
assert(g(4.4, x=4) == 8);
assert(g(4.4, x=4.4) == 7);

assert(g(y=4, 4) == 8);
assert(g(y=4, 4.4) == 7);
assert(g(y=4.4, 4) == 8);
assert(g(y=4.4, 4.4) == 7);

EndTest();
