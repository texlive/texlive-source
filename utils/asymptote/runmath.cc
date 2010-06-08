/***** Autogenerated from runmath.in; changes will be overwritten *****/

#line 1 "runtimebase.in"
/*****
 * runtimebase.in
 * Andy Hammerlindl  2009/07/28
 *
 * Common declarations needed for all code-generating .in files.
 *
 *****/


#line 1 "runmath.in"
/*****
 * runmath.in
 *
 * Runtime functions for math operations.
 *
 *****/

#line 1 "runtimebase.in"
#include "stack.h"
#include "types.h"
#include "builtin.h"
#include "entry.h"
#include "errormsg.h"
#include "array.h"
#include "triple.h"
#include "callable.h"
#include "opsymbols.h"

using vm::stack;
using vm::error;
using vm::array;
using vm::read;
using vm::callable;
using types::formal;
using types::function;
using camp::triple;

#define PRIMITIVE(name,Name,asyName) using types::prim##Name;
#include <primitives.h>
#undef PRIMITIVE

typedef double real;

void unused(void *);

namespace run {
array *copyArray(array *a);
array *copyArray2(array *a);
array *copyArray3(array *a);

inline size_t checkdimension(const array *a, size_t dim)
{
  size_t size=checkArray(a);
  if(dim && size != dim) {
    ostringstream buf;
    buf << "array of length " << dim << " expected";
    error(buf);
  }
  return size;
}

template<class T>
void copyArrayC(T* &dest, const array *a, size_t dim=0,
                GCPlacement placement=NoGC)
{
  size_t size=checkdimension(a,dim);
  dest=(placement == NoGC) ? new T[size] : new(placement) T[size];
  for(size_t i=0; i < size; i++) 
    dest[i]=read<T>(a,i);
}

template<class T>
void copyArray2C(T* &dest, const array *a, bool square=true, size_t dim2=0,
                 GCPlacement placement=NoGC)
{
  size_t n=checkArray(a);
  size_t m=(square || n == 0) ? n : checkArray(read<array*>(a,0));
  if(n > 0 && dim2 && m != dim2) {
    ostringstream buf;
    buf << "second matrix dimension must be " << dim2;
    error(buf);
  }
  
  dest=(placement == NoGC) ? new T[n*m] : new(placement) T[n*m];
  for(size_t i=0; i < n; i++) {
    array *ai=read<array*>(a,i);
    size_t aisize=checkArray(ai);
    if(aisize == m) {
      T *desti=dest+i*m;
      for(size_t j=0; j < m; j++) 
        desti[j]=read<T>(ai,j);
    } else
      error(square ? "matrix must be square" : "matrix must be rectangular");
  }
}

double *copyTripleArray2Components(array *a, bool square=true, size_t dim2=0,
                                   GCPlacement placement=NoGC);
}

function *realRealFunction();

#define CURRENTPEN processData().currentpen

#line 12 "runmath.in"
#include "mathop.h"
#include "path.h"

using namespace camp;

typedef array realarray;
typedef array pairarray;

using types::realArray;
using types::pairArray;

using run::integeroverflow;
using vm::frame;

const char *invalidargument="invalid argument";

// Return the factorial of a non-negative integer using a lookup table.
Int factorial(Int n)
{
  static Int *table;
  static Int size=0;
  if(size == 0) {
    Int f=1;
    size=2;
    while(f <= Int_MAX/size)
      f *= (size++);
    table=new Int[size];
    table[0]=f=1;
    for(Int i=1; i < size; ++i) {
      f *= i;
      table[i]=f;
    }
  }
  if(n >= size) integeroverflow(0);
  return table[n];
}

static inline Int Round(double x) 
{
  return Int(x+((x >= 0) ? 0.5 : -0.5));
}

inline Int sgn(double x) 
{
  return (x > 0.0 ? 1 : (x < 0.0 ? -1 : 0));
}

static bool initializeRandom=true;

void Srand(Int seed)
{ 
  initializeRandom=false;
  const int n=256;
  static char state[n];
  initstate(intcast(seed),state,n);
}  

// Autogenerated routines:



#include "runmath.symbols.h"

namespace run {
#line 72 "runmath.in"
// real ^(real x, Int y);
void gen_runmath0(stack *Stack)
{
  Int y=vm::pop<Int>(Stack);
  real x=vm::pop<real>(Stack);
#line 73 "runmath.in"
  {Stack->push<real>(pow(x,y)); return;}
}

#line 77 "runmath.in"
// pair ^(pair z, Int y);
void gen_runmath1(stack *Stack)
{
  Int y=vm::pop<Int>(Stack);
  pair z=vm::pop<pair>(Stack);
#line 78 "runmath.in"
  {Stack->push<pair>(pow(z,y)); return;}
}

#line 82 "runmath.in"
// Int quotient(Int x, Int y);
void gen_runmath2(stack *Stack)
{
  Int y=vm::pop<Int>(Stack);
  Int x=vm::pop<Int>(Stack);
#line 83 "runmath.in" 
  if(y == 0) dividebyzero();
  if(y == -1) {Stack->push<Int>(Negate(x)); return;}
// Implementation-independent definition of integer division: round down
  {Stack->push<Int>((x-portableMod(x,y))/y); return;}
}

#line 90 "runmath.in"
// Int abs(Int x);
void gen_runmath3(stack *Stack)
{
  Int x=vm::pop<Int>(Stack);
#line 91 "runmath.in" 
  {Stack->push<Int>(Abs(x)); return;}
}

#line 95 "runmath.in"
// Int sgn(real x);
void gen_runmath4(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 96 "runmath.in" 
  {Stack->push<Int>(sgn(x)); return;}
}

#line 100 "runmath.in"
// Int rand();
void gen_runmath5(stack *Stack)
{
#line 101 "runmath.in" 
  if(initializeRandom)
    Srand(1);
  {Stack->push<Int>(random()); return;}
}

#line 107 "runmath.in"
// void srand(Int seed);
void gen_runmath6(stack *Stack)
{
  Int seed=vm::pop<Int>(Stack);
#line 108 "runmath.in" 
  Srand(seed);
}

// a random number uniformly distributed in the interval [0,1]
#line 113 "runmath.in"
// real unitrand();
void gen_runmath7(stack *Stack)
{
#line 114 "runmath.in"                         
  {Stack->push<real>(((real) random())/RAND_MAX); return;}
}

#line 118 "runmath.in"
// Int ceil(real x);
void gen_runmath8(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 119 "runmath.in" 
  {Stack->push<Int>(Intcast(ceil(x))); return;}
}

#line 123 "runmath.in"
// Int floor(real x);
void gen_runmath9(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 124 "runmath.in" 
  {Stack->push<Int>(Intcast(floor(x))); return;}
}

#line 128 "runmath.in"
// Int round(real x);
void gen_runmath10(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 129 "runmath.in" 
  if(validInt(x)) {Stack->push<Int>(Round(x)); return;}
  integeroverflow(0);
}

#line 134 "runmath.in"
// Int Ceil(real x);
void gen_runmath11(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 135 "runmath.in" 
  {Stack->push<Int>(Ceil(x)); return;}
}

#line 139 "runmath.in"
// Int Floor(real x);
void gen_runmath12(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 140 "runmath.in" 
  {Stack->push<Int>(Floor(x)); return;}
}

#line 144 "runmath.in"
// Int Round(real x);
void gen_runmath13(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 145 "runmath.in" 
  {Stack->push<Int>(Round(Intcap(x))); return;}
}

#line 149 "runmath.in"
// real fmod(real x, real y);
void gen_runmath14(stack *Stack)
{
  real y=vm::pop<real>(Stack);
  real x=vm::pop<real>(Stack);
#line 150 "runmath.in"
  if (y == 0.0) dividebyzero();
  {Stack->push<real>(fmod(x,y)); return;}
}

#line 155 "runmath.in"
// real atan2(real y, real x);
void gen_runmath15(stack *Stack)
{
  real x=vm::pop<real>(Stack);
  real y=vm::pop<real>(Stack);
#line 156 "runmath.in" 
  {Stack->push<real>(atan2(y,x)); return;}
}

#line 160 "runmath.in"
// real hypot(real x, real y);
void gen_runmath16(stack *Stack)
{
  real y=vm::pop<real>(Stack);
  real x=vm::pop<real>(Stack);
#line 161 "runmath.in" 
  {Stack->push<real>(hypot(x,y)); return;}
}

#line 165 "runmath.in"
// real remainder(real x, real y);
void gen_runmath17(stack *Stack)
{
  real y=vm::pop<real>(Stack);
  real x=vm::pop<real>(Stack);
#line 166 "runmath.in" 
  {Stack->push<real>(remainder(x,y)); return;}
}

#line 170 "runmath.in"
// real Jn(Int n, real x);
void gen_runmath18(stack *Stack)
{
  real x=vm::pop<real>(Stack);
  Int n=vm::pop<Int>(Stack);
#line 171 "runmath.in"
  {Stack->push<real>(jn(n,x)); return;}
}

#line 175 "runmath.in"
// real Yn(Int n, real x);
void gen_runmath19(stack *Stack)
{
  real x=vm::pop<real>(Stack);
  Int n=vm::pop<Int>(Stack);
#line 176 "runmath.in"
  {Stack->push<real>(yn(n,x)); return;}
}

#line 180 "runmath.in"
// real erf(real x);
void gen_runmath20(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 181 "runmath.in"
  {Stack->push<real>(erf(x)); return;}
}

#line 185 "runmath.in"
// real erfc(real x);
void gen_runmath21(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 186 "runmath.in"
  {Stack->push<real>(erfc(x)); return;}
}

#line 190 "runmath.in"
// Int factorial(Int n);
void gen_runmath22(stack *Stack)
{
  Int n=vm::pop<Int>(Stack);
#line 191 "runmath.in"
  if(n < 0) error(invalidargument);
  {Stack->push<Int>(factorial(n)); return;}
}

#line 195 "runmath.in"
// Int choose(Int n, Int k);
void gen_runmath23(stack *Stack)
{
  Int k=vm::pop<Int>(Stack);
  Int n=vm::pop<Int>(Stack);
#line 196 "runmath.in"
  if(n < 0 || k < 0 || k > n) error(invalidargument);
  Int f=1;
  Int r=n-k;
  for(Int i=n; i > r; --i) {
    if(f > Int_MAX/i) integeroverflow(0);
    f=(f*i)/(n-i+1);
  }
  {Stack->push<Int>(f); return;}
}

#line 206 "runmath.in"
// real gamma(real x);
void gen_runmath24(stack *Stack)
{
  real x=vm::pop<real>(Stack);
#line 207 "runmath.in"
#ifdef HAVE_TGAMMA
  {Stack->push<real>(tgamma(x)); return;}
#else
  real lg = lgamma(x);
  {Stack->push<real>(signgam*exp(lg)); return;}
#endif
}

#line 216 "runmath.in"
// realarray* quadraticroots(real a, real b, real c);
void gen_runmath25(stack *Stack)
{
  real c=vm::pop<real>(Stack);
  real b=vm::pop<real>(Stack);
  real a=vm::pop<real>(Stack);
#line 217 "runmath.in"
  quadraticroots q(a,b,c);
  array *roots=new array(q.roots);
  if(q.roots >= 1) (*roots)[0]=q.t1;
  if(q.roots == 2) (*roots)[1]=q.t2;
  {Stack->push<realarray*>(roots); return;}
}

#line 225 "runmath.in"
// pairarray* quadraticroots(explicit pair a, explicit pair b, explicit pair c);
void gen_runmath26(stack *Stack)
{
  pair c=vm::pop<pair>(Stack);
  pair b=vm::pop<pair>(Stack);
  pair a=vm::pop<pair>(Stack);
#line 226 "runmath.in"
  Quadraticroots q(a,b,c);
  array *roots=new array(q.roots);
  if(q.roots >= 1) (*roots)[0]=q.z1;
  if(q.roots == 2) (*roots)[1]=q.z2;
  {Stack->push<pairarray*>(roots); return;}
}

#line 234 "runmath.in"
// realarray* cubicroots(real a, real b, real c, real d);
void gen_runmath27(stack *Stack)
{
  real d=vm::pop<real>(Stack);
  real c=vm::pop<real>(Stack);
  real b=vm::pop<real>(Stack);
  real a=vm::pop<real>(Stack);
#line 235 "runmath.in"
  cubicroots q(a,b,c,d);
  array *roots=new array(q.roots);
  if(q.roots >= 1) (*roots)[0]=q.t1;
  if(q.roots >= 2) (*roots)[1]=q.t2;
  if(q.roots == 3) (*roots)[2]=q.t3;
  {Stack->push<realarray*>(roots); return;}
}


// Logical operations
#line 246 "runmath.in"
// bool !(bool b);
void gen_runmath28(stack *Stack)
{
  bool b=vm::pop<bool>(Stack);
#line 247 "runmath.in"
  {Stack->push<bool>(!b); return;}
}

#line 252 "runmath.in"
void boolMemEq(stack *Stack)
{
  frame * b=vm::pop<frame *>(Stack);
  frame * a=vm::pop<frame *>(Stack);
#line 253 "runmath.in"
  {Stack->push<bool>(a == b); return;}
}

#line 257 "runmath.in"
void boolMemNeq(stack *Stack)
{
  frame * b=vm::pop<frame *>(Stack);
  frame * a=vm::pop<frame *>(Stack);
#line 258 "runmath.in"
  {Stack->push<bool>(a != b); return;}
}

#line 262 "runmath.in"
void boolFuncEq(stack *Stack)
{
  callable * b=vm::pop<callable *>(Stack);
  callable * a=vm::pop<callable *>(Stack);
#line 263 "runmath.in"
  {Stack->push<bool>(a->compare(b)); return;}
}

#line 267 "runmath.in"
void boolFuncNeq(stack *Stack)
{
  callable * b=vm::pop<callable *>(Stack);
  callable * a=vm::pop<callable *>(Stack);
#line 268 "runmath.in"
  {Stack->push<bool>(!(a->compare(b))); return;}
}


// Bit operations
#line 274 "runmath.in"
// Int AND(Int a, Int b);
void gen_runmath33(stack *Stack)
{
  Int b=vm::pop<Int>(Stack);
  Int a=vm::pop<Int>(Stack);
#line 275 "runmath.in"
  {Stack->push<Int>(a & b); return;}
}

#line 280 "runmath.in"
// Int OR(Int a, Int b);
void gen_runmath34(stack *Stack)
{
  Int b=vm::pop<Int>(Stack);
  Int a=vm::pop<Int>(Stack);
#line 281 "runmath.in"
  {Stack->push<Int>(a | b); return;}
}

#line 285 "runmath.in"
// Int XOR(Int a, Int b);
void gen_runmath35(stack *Stack)
{
  Int b=vm::pop<Int>(Stack);
  Int a=vm::pop<Int>(Stack);
#line 286 "runmath.in"
  {Stack->push<Int>(a ^ b); return;}
}

#line 290 "runmath.in"
// Int NOT(Int a);
void gen_runmath36(stack *Stack)
{
  Int a=vm::pop<Int>(Stack);
#line 291 "runmath.in"
  {Stack->push<Int>(~a); return;}
}

#line 295 "runmath.in"
// Int CLZ(Int a);
void gen_runmath37(stack *Stack)
{
  Int a=vm::pop<Int>(Stack);
#line 296 "runmath.in"
  if((unsignedInt) a > 0xFFFFFFFF) {Stack->push<Int>(-1); return;}
#ifdef __GNUC__
  {Stack->push<Int>(__builtin_clz(a)); return;}
#else
// find the log base 2 of a 32-bit integer
  static const int MultiplyDeBruijnBitPosition[32] = {
    0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
    8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
  };

  a |= a >> 1; // first round down to one less than a power of 2 
  a |= a >> 2;
  a |= a >> 4;
  a |= a >> 8;
  a |= a >> 16;

  {Stack->push<Int>(31-MultiplyDeBruijnBitPosition[(unsignedInt)(a * 0x07C4ACDDU) >> 27]); return;}
#endif
}

#line 317 "runmath.in"
// Int CTZ(Int a);
void gen_runmath38(stack *Stack)
{
  Int a=vm::pop<Int>(Stack);
#line 318 "runmath.in"
  if((unsignedInt) a > 0xFFFFFFFF) {Stack->push<Int>(-1); return;}
#ifdef __GNUC__
  {Stack->push<Int>(__builtin_ctz(a)); return;}
#else
  // find the number of trailing zeros in a 32-bit number
  static const int MultiplyDeBruijnBitPosition[32] = {
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
  };
  {Stack->push<Int>(MultiplyDeBruijnBitPosition[((unsignedInt)((a & -a) * 0x077CB531U))
                                     >> 27]); return;}
#endif
}

} // namespace run

namespace trans {

void gen_runmath_venv(venv &ve)
{
#line 72 "runmath.in"
  addFunc(ve, run::gen_runmath0, primReal(), SYM_CARET, formal(primReal(), SYM(x), false, false), formal(primInt(), SYM(y), false, false));
#line 77 "runmath.in"
  addFunc(ve, run::gen_runmath1, primPair(), SYM_CARET, formal(primPair(), SYM(z), false, false), formal(primInt(), SYM(y), false, false));
#line 82 "runmath.in"
  addFunc(ve, run::gen_runmath2, primInt(), SYM(quotient), formal(primInt(), SYM(x), false, false), formal(primInt(), SYM(y), false, false));
#line 90 "runmath.in"
  addFunc(ve, run::gen_runmath3, primInt(), SYM(abs), formal(primInt(), SYM(x), false, false));
#line 95 "runmath.in"
  addFunc(ve, run::gen_runmath4, primInt(), SYM(sgn), formal(primReal(), SYM(x), false, false));
#line 100 "runmath.in"
  addFunc(ve, run::gen_runmath5, primInt(), SYM(rand));
#line 107 "runmath.in"
  addFunc(ve, run::gen_runmath6, primVoid(), SYM(srand), formal(primInt(), SYM(seed), false, false));
#line 112 "runmath.in"
  addFunc(ve, run::gen_runmath7, primReal(), SYM(unitrand));
#line 118 "runmath.in"
  addFunc(ve, run::gen_runmath8, primInt(), SYM(ceil), formal(primReal(), SYM(x), false, false));
#line 123 "runmath.in"
  addFunc(ve, run::gen_runmath9, primInt(), SYM(floor), formal(primReal(), SYM(x), false, false));
#line 128 "runmath.in"
  addFunc(ve, run::gen_runmath10, primInt(), SYM(round), formal(primReal(), SYM(x), false, false));
#line 134 "runmath.in"
  addFunc(ve, run::gen_runmath11, primInt(), SYM(Ceil), formal(primReal(), SYM(x), false, false));
#line 139 "runmath.in"
  addFunc(ve, run::gen_runmath12, primInt(), SYM(Floor), formal(primReal(), SYM(x), false, false));
#line 144 "runmath.in"
  addFunc(ve, run::gen_runmath13, primInt(), SYM(Round), formal(primReal(), SYM(x), false, false));
#line 149 "runmath.in"
  addFunc(ve, run::gen_runmath14, primReal(), SYM(fmod), formal(primReal(), SYM(x), false, false), formal(primReal(), SYM(y), false, false));
#line 155 "runmath.in"
  addFunc(ve, run::gen_runmath15, primReal(), SYM(atan2), formal(primReal(), SYM(y), false, false), formal(primReal(), SYM(x), false, false));
#line 160 "runmath.in"
  addFunc(ve, run::gen_runmath16, primReal(), SYM(hypot), formal(primReal(), SYM(x), false, false), formal(primReal(), SYM(y), false, false));
#line 165 "runmath.in"
  addFunc(ve, run::gen_runmath17, primReal(), SYM(remainder), formal(primReal(), SYM(x), false, false), formal(primReal(), SYM(y), false, false));
#line 170 "runmath.in"
  addFunc(ve, run::gen_runmath18, primReal(), SYM(Jn), formal(primInt(), SYM(n), false, false), formal(primReal(), SYM(x), false, false));
#line 175 "runmath.in"
  addFunc(ve, run::gen_runmath19, primReal(), SYM(Yn), formal(primInt(), SYM(n), false, false), formal(primReal(), SYM(x), false, false));
#line 180 "runmath.in"
  addFunc(ve, run::gen_runmath20, primReal(), SYM(erf), formal(primReal(), SYM(x), false, false));
#line 185 "runmath.in"
  addFunc(ve, run::gen_runmath21, primReal(), SYM(erfc), formal(primReal(), SYM(x), false, false));
#line 190 "runmath.in"
  addFunc(ve, run::gen_runmath22, primInt(), SYM(factorial), formal(primInt(), SYM(n), false, false));
#line 195 "runmath.in"
  addFunc(ve, run::gen_runmath23, primInt(), SYM(choose), formal(primInt(), SYM(n), false, false), formal(primInt(), SYM(k), false, false));
#line 206 "runmath.in"
  addFunc(ve, run::gen_runmath24, primReal(), SYM(gamma), formal(primReal(), SYM(x), false, false));
#line 216 "runmath.in"
  addFunc(ve, run::gen_runmath25, realArray(), SYM(quadraticroots), formal(primReal(), SYM(a), false, false), formal(primReal(), SYM(b), false, false), formal(primReal(), SYM(c), false, false));
#line 225 "runmath.in"
  addFunc(ve, run::gen_runmath26, pairArray(), SYM(quadraticroots), formal(primPair(), SYM(a), false, true), formal(primPair(), SYM(b), false, true), formal(primPair(), SYM(c), false, true));
#line 234 "runmath.in"
  addFunc(ve, run::gen_runmath27, realArray(), SYM(cubicroots), formal(primReal(), SYM(a), false, false), formal(primReal(), SYM(b), false, false), formal(primReal(), SYM(c), false, false), formal(primReal(), SYM(d), false, false));
#line 244 "runmath.in"
  addFunc(ve, run::gen_runmath28, primBoolean(), SYM_LOGNOT, formal(primBoolean(), SYM(b), false, false));
#line 252 "runmath.in"
  REGISTER_BLTIN(run::boolMemEq,"boolMemEq");
#line 257 "runmath.in"
  REGISTER_BLTIN(run::boolMemNeq,"boolMemNeq");
#line 262 "runmath.in"
  REGISTER_BLTIN(run::boolFuncEq,"boolFuncEq");
#line 267 "runmath.in"
  REGISTER_BLTIN(run::boolFuncNeq,"boolFuncNeq");
#line 272 "runmath.in"
  addFunc(ve, run::gen_runmath33, primInt(), SYM(AND), formal(primInt(), SYM(a), false, false), formal(primInt(), SYM(b), false, false));
#line 280 "runmath.in"
  addFunc(ve, run::gen_runmath34, primInt(), SYM(OR), formal(primInt(), SYM(a), false, false), formal(primInt(), SYM(b), false, false));
#line 285 "runmath.in"
  addFunc(ve, run::gen_runmath35, primInt(), SYM(XOR), formal(primInt(), SYM(a), false, false), formal(primInt(), SYM(b), false, false));
#line 290 "runmath.in"
  addFunc(ve, run::gen_runmath36, primInt(), SYM(NOT), formal(primInt(), SYM(a), false, false));
#line 295 "runmath.in"
  addFunc(ve, run::gen_runmath37, primInt(), SYM(CLZ), formal(primInt(), SYM(a), false, false));
#line 317 "runmath.in"
  addFunc(ve, run::gen_runmath38, primInt(), SYM(CTZ), formal(primInt(), SYM(a), false, false));
}

} // namespace trans
