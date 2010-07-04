/***** Autogenerated from runsystem.in; changes will be overwritten *****/

#line 1 "runtimebase.in"
/*****
 * runtimebase.in
 * Andy Hammerlindl  2009/07/28
 *
 * Common declarations needed for all code-generating .in files.
 *
 *****/


#line 1 "runsystem.in"
/*****
 * runsystem.in
 *
 * Runtime functions for system operations.
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

double *copyTripleArray2Components(array *a, bool square=true, size_t dim2=0,
                                   GCPlacement placement=NoGC);
}

function *realRealFunction();

#define CURRENTPEN processData().currentpen

#line 12 "runsystem.in"
#include "process.h"
#include "stack.h"
#include "locate.h"

using namespace camp;
using namespace settings;
using vm::bpinfo;
using vm::bplist;
using vm::getPos;
using vm::Default;
using vm::nullfunc;
using vm::item;
using absyntax::runnable;

typedef callable callableBp;

namespace run {
extern string emptystring;
}

function *voidFunction()
{
  return new function(primVoid());
}

function *breakpointFunction()
{
  return new function(primString(),primString(),primInt(),primInt(),
                      primCode());
}

void clear(string file, Int line, bool warn=false) 
{
  bpinfo bp(file,line);
  for(mem::list<bpinfo>::iterator p=bplist.begin(); p != bplist.end(); ++p) {
    if(*p == bp) {
      cout << "cleared breakpoint at " << file << ": " << line << endl;
      bplist.remove(bp);
      return;
    }
  }
  if(warn)
    cout << "No such breakpoint at "  << file << ": " << line << endl;
}

namespace run {
void breakpoint(stack *Stack, runnable *r)
{
  callable *atBreakpointFunction=processData().atBreakpointFunction;
  if(atBreakpointFunction &&
     !nullfunc::instance()->compare(atBreakpointFunction)) {
    position curPos=getPos();
    Stack->push<string>(curPos.filename());
    Stack->push<Int>((Int) curPos.Line());
    Stack->push<Int>((Int) curPos.Column());
    Stack->push(r ? r : vm::Default);
    atBreakpointFunction->call(Stack); // returns a string
  } else Stack->push<string>("");
}
}

string convertname(string name, const string& format)
{
  if(name.empty())
    return buildname(outname(),format,"");
  else
    name=outpath(name);
  return format.empty() ? name : format+":"+name;
}

namespace run {
void purge(Int divisor=0)
{
#ifdef USEGC
  if(divisor > 0) GC_set_free_space_divisor((GC_word) divisor);
  GC_gcollect();
#endif
}

void updateFunction(stack *Stack)
{
  callable *atUpdateFunction=processData().atUpdateFunction;
  if(atUpdateFunction && !nullfunc::instance()->compare(atUpdateFunction))
    atUpdateFunction->call(Stack);
}

void exitFunction(stack *Stack)
{
  callable *atExitFunction=processData().atExitFunction;
  if(atExitFunction && !nullfunc::instance()->compare(atExitFunction))
    atExitFunction->call(Stack);
}
}

// Autogenerated routines:


#ifndef NOSYM
#include "runsystem.symbols.h"

#endif
namespace run {
#line 108 "runsystem.in"
// string outname();
void gen_runsystem0(stack *Stack)
{
#line 109 "runsystem.in"
  {Stack->push<string>(outname()); return;}
}

#line 113 "runsystem.in"
// void atupdate(callable *f);
void gen_runsystem1(stack *Stack)
{
  callable * f=vm::pop<callable *>(Stack);
#line 114 "runsystem.in"
  processData().atUpdateFunction=f;
}

#line 118 "runsystem.in"
// callable* atupdate();
void gen_runsystem2(stack *Stack)
{
#line 119 "runsystem.in"
  {Stack->push<callable*>(processData().atUpdateFunction); return;}
}

#line 123 "runsystem.in"
// void atexit(callable *f);
void gen_runsystem3(stack *Stack)
{
  callable * f=vm::pop<callable *>(Stack);
#line 124 "runsystem.in"
  processData().atExitFunction=f;
}

#line 128 "runsystem.in"
// callable* atexit();
void gen_runsystem4(stack *Stack)
{
#line 129 "runsystem.in"
  {Stack->push<callable*>(processData().atExitFunction); return;}
}

#line 133 "runsystem.in"
// void atbreakpoint(callableBp *f);
void gen_runsystem5(stack *Stack)
{
  callableBp * f=vm::pop<callableBp *>(Stack);
#line 134 "runsystem.in"
  processData().atBreakpointFunction=f;
}

#line 138 "runsystem.in"
// void breakpoint(runnable *s=NULL);
void gen_runsystem6(stack *Stack)
{
  runnable * s=vm::pop<runnable *>(Stack,NULL);
#line 139 "runsystem.in"
  breakpoint(Stack,s);
}

#line 143 "runsystem.in"
// string locatefile(string file);
void gen_runsystem7(stack *Stack)
{
  string file=vm::pop<string>(Stack);
#line 144 "runsystem.in"
  {Stack->push<string>(locateFile(file)); return;}
}

#line 148 "runsystem.in"
// void stop(string file, Int line, runnable *s=NULL);
void gen_runsystem8(stack *Stack)
{
  runnable * s=vm::pop<runnable *>(Stack,NULL);
  Int line=vm::pop<Int>(Stack);
  string file=vm::pop<string>(Stack);
#line 149 "runsystem.in"
  file=locateFile(file);
  clear(file,line);
  cout << "setting breakpoint at " << file << ": " << line << endl;
  bplist.push_back(bpinfo(file,line,s));
}

#line 156 "runsystem.in"
// void breakpoints();
void gen_runsystem9(stack *)
{
#line 157 "runsystem.in"
  for(mem::list<bpinfo>::iterator p=bplist.begin(); p != bplist.end(); ++p)
    cout << p->f.name() << ": " << p->f.line() << endl;
}

#line 162 "runsystem.in"
// void clear(string file, Int line);
void gen_runsystem10(stack *Stack)
{
  Int line=vm::pop<Int>(Stack);
  string file=vm::pop<string>(Stack);
#line 163 "runsystem.in"
  file=locateFile(file);
  clear(file,line,true);
}

#line 168 "runsystem.in"
// void clear();
void gen_runsystem11(stack *)
{
#line 169 "runsystem.in"
  bplist.clear();
}

#line 173 "runsystem.in"
// void warn(string s);
void gen_runsystem12(stack *Stack)
{
  string s=vm::pop<string>(Stack);
#line 174 "runsystem.in"
  Warn(s);
}

#line 178 "runsystem.in"
// void nowarn(string s);
void gen_runsystem13(stack *Stack)
{
  string s=vm::pop<string>(Stack);
#line 179 "runsystem.in"
  noWarn(s);
}

#line 183 "runsystem.in"
// void warning(string s, string t, bool position=false);
void gen_runsystem14(stack *Stack)
{
  bool position=vm::pop<bool>(Stack,false);
  string t=vm::pop<string>(Stack);
  string s=vm::pop<string>(Stack);
#line 184 "runsystem.in"
  if(settings::warn(s)) {
    em.warning(position ? getPos() : nullPos,s);
    em << t;
  }
}

// Strip directory from string
#line 192 "runsystem.in"
// string stripdirectory(string *s);
void gen_runsystem15(stack *Stack)
{
  string * s=vm::pop<string *>(Stack);
#line 193 "runsystem.in"
  {Stack->push<string>(stripDir(*s)); return;}
}

// Strip directory from string
#line 198 "runsystem.in"
// string stripfile(string *s);
void gen_runsystem16(stack *Stack)
{
  string * s=vm::pop<string *>(Stack);
#line 199 "runsystem.in"
  {Stack->push<string>(stripFile(*s)); return;}
}

// Strip file extension from string
#line 204 "runsystem.in"
// string stripextension(string *s);
void gen_runsystem17(stack *Stack)
{
  string * s=vm::pop<string *>(Stack);
#line 205 "runsystem.in"
  {Stack->push<string>(stripExt(*s)); return;}
}

// Call ImageMagick convert.
#line 210 "runsystem.in"
// Int convert(string args=emptystring, string file=emptystring,            string format=emptystring);
void gen_runsystem18(stack *Stack)
{
  string format=vm::pop<string>(Stack,emptystring);
  string file=vm::pop<string>(Stack,emptystring);
  string args=vm::pop<string>(Stack,emptystring);
#line 212 "runsystem.in"
  string name=convertname(file,format);
  mem::vector<string> cmd;
  cmd.push_back(getSetting<string>("convert"));
  push_split(cmd,args);
  cmd.push_back(name);
  bool quiet=verbose <= 1;

  char *oldPath=NULL;
  string dir=stripFile(outname());
  if(!dir.empty()) {
    oldPath=getPath();
    setPath(dir.c_str());
  }
  Int ret=System(cmd,quiet ? 1 : 0,true,"convert",
                 "your ImageMagick convert utility");
  if(oldPath != NULL)
    setPath(oldPath);
  
  if(ret == 0 && verbose > 0)
    cout << "Wrote " << ((file.empty()) ? name : file) << endl;
  
  {Stack->push<Int>(ret); return;}
}

// Call ImageMagick animate.
#line 238 "runsystem.in"
// Int animate(string args=emptystring, string file=emptystring,            string format=emptystring);
void gen_runsystem19(stack *Stack)
{
  string format=vm::pop<string>(Stack,emptystring);
  string file=vm::pop<string>(Stack,emptystring);
  string args=vm::pop<string>(Stack,emptystring);
#line 240 "runsystem.in"
#ifndef __CYGWIN__
  string name=convertname(file,format);
  if(view()) {
    mem::vector<string> cmd;
    cmd.push_back(getSetting<string>("animate"));
    push_split(cmd,args);
    cmd.push_back(name);
    {Stack->push<Int>(System(cmd,0,false,"animate","your animated GIF viewer")); return;}
  }
#endif  
  {Stack->push<Int>(0); return;}
}

#line 254 "runsystem.in"
// void purge(Int divisor=0);
void gen_runsystem20(stack *Stack)
{
  Int divisor=vm::pop<Int>(Stack,0);
#line 255 "runsystem.in"
  purge(divisor);
}

} // namespace run

namespace trans {

void gen_runsystem_venv(venv &ve)
{
#line 108 "runsystem.in"
  addFunc(ve, run::gen_runsystem0, primString() , SYM(outname));
#line 113 "runsystem.in"
  addFunc(ve, run::gen_runsystem1, primVoid(), SYM(atupdate), formal(voidFunction(), SYM(f), false, false));
#line 118 "runsystem.in"
  addFunc(ve, run::gen_runsystem2, voidFunction(), SYM(atupdate));
#line 123 "runsystem.in"
  addFunc(ve, run::gen_runsystem3, primVoid(), SYM(atexit), formal(voidFunction(), SYM(f), false, false));
#line 128 "runsystem.in"
  addFunc(ve, run::gen_runsystem4, voidFunction(), SYM(atexit));
#line 133 "runsystem.in"
  addFunc(ve, run::gen_runsystem5, primVoid(), SYM(atbreakpoint), formal(breakpointFunction(), SYM(f), false, false));
#line 138 "runsystem.in"
  addFunc(ve, run::gen_runsystem6, primVoid(), SYM(breakpoint), formal(primCode(), SYM(s), true, false));
#line 143 "runsystem.in"
  addFunc(ve, run::gen_runsystem7, primString() , SYM(locatefile), formal(primString() , SYM(file), false, false));
#line 148 "runsystem.in"
  addFunc(ve, run::gen_runsystem8, primVoid(), SYM(stop), formal(primString() , SYM(file), false, false), formal(primInt(), SYM(line), false, false), formal(primCode(), SYM(s), true, false));
#line 156 "runsystem.in"
  addFunc(ve, run::gen_runsystem9, primVoid(), SYM(breakpoints));
#line 162 "runsystem.in"
  addFunc(ve, run::gen_runsystem10, primVoid(), SYM(clear), formal(primString() , SYM(file), false, false), formal(primInt(), SYM(line), false, false));
#line 168 "runsystem.in"
  addFunc(ve, run::gen_runsystem11, primVoid(), SYM(clear));
#line 173 "runsystem.in"
  addFunc(ve, run::gen_runsystem12, primVoid(), SYM(warn), formal(primString() , SYM(s), false, false));
#line 178 "runsystem.in"
  addFunc(ve, run::gen_runsystem13, primVoid(), SYM(nowarn), formal(primString() , SYM(s), false, false));
#line 183 "runsystem.in"
  addFunc(ve, run::gen_runsystem14, primVoid(), SYM(warning), formal(primString() , SYM(s), false, false), formal(primString() , SYM(t), false, false), formal(primBoolean(), SYM(position), true, false));
#line 191 "runsystem.in"
  addFunc(ve, run::gen_runsystem15, primString() , SYM(stripdirectory), formal(primString(), SYM(s), false, false));
#line 197 "runsystem.in"
  addFunc(ve, run::gen_runsystem16, primString() , SYM(stripfile), formal(primString(), SYM(s), false, false));
#line 203 "runsystem.in"
  addFunc(ve, run::gen_runsystem17, primString() , SYM(stripextension), formal(primString(), SYM(s), false, false));
#line 209 "runsystem.in"
  addFunc(ve, run::gen_runsystem18, primInt(), SYM(convert), formal(primString() , SYM(args), true, false), formal(primString() , SYM(file), true, false), formal(primString() , SYM(format), true, false));
#line 237 "runsystem.in"
  addFunc(ve, run::gen_runsystem19, primInt(), SYM(animate), formal(primString() , SYM(args), true, false), formal(primString() , SYM(file), true, false), formal(primString() , SYM(format), true, false));
#line 254 "runsystem.in"
  addFunc(ve, run::gen_runsystem20, primVoid(), SYM(purge), formal(primInt(), SYM(divisor), true, false));
}

} // namespace trans
