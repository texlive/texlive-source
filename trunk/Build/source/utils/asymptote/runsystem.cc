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
#line 107 "runsystem.in"
// string outname();
void gen_runsystem0(stack *Stack)
{
#line 108 "runsystem.in"
  {Stack->push<string>(outname()); return;}
}

#line 112 "runsystem.in"
// void atupdate(callable *f);
void gen_runsystem1(stack *Stack)
{
  callable * f=vm::pop<callable *>(Stack);
#line 113 "runsystem.in"
  processData().atUpdateFunction=f;
}

#line 117 "runsystem.in"
// callable* atupdate();
void gen_runsystem2(stack *Stack)
{
#line 118 "runsystem.in"
  {Stack->push<callable*>(processData().atUpdateFunction); return;}
}

#line 122 "runsystem.in"
// void atexit(callable *f);
void gen_runsystem3(stack *Stack)
{
  callable * f=vm::pop<callable *>(Stack);
#line 123 "runsystem.in"
  processData().atExitFunction=f;
}

#line 127 "runsystem.in"
// callable* atexit();
void gen_runsystem4(stack *Stack)
{
#line 128 "runsystem.in"
  {Stack->push<callable*>(processData().atExitFunction); return;}
}

#line 132 "runsystem.in"
// void atbreakpoint(callableBp *f);
void gen_runsystem5(stack *Stack)
{
  callableBp * f=vm::pop<callableBp *>(Stack);
#line 133 "runsystem.in"
  processData().atBreakpointFunction=f;
}

#line 137 "runsystem.in"
// void breakpoint(runnable *s=NULL);
void gen_runsystem6(stack *Stack)
{
  runnable * s=vm::pop<runnable *>(Stack,NULL);
#line 138 "runsystem.in"
  breakpoint(Stack,s);
}

#line 142 "runsystem.in"
// string locatefile(string file);
void gen_runsystem7(stack *Stack)
{
  string file=vm::pop<string>(Stack);
#line 143 "runsystem.in"
  {Stack->push<string>(locateFile(file)); return;}
}

#line 147 "runsystem.in"
// void stop(string file, Int line, runnable *s=NULL);
void gen_runsystem8(stack *Stack)
{
  runnable * s=vm::pop<runnable *>(Stack,NULL);
  Int line=vm::pop<Int>(Stack);
  string file=vm::pop<string>(Stack);
#line 148 "runsystem.in"
  file=locateFile(file);
  clear(file,line);
  cout << "setting breakpoint at " << file << ": " << line << endl;
  bplist.push_back(bpinfo(file,line,s));
}

#line 155 "runsystem.in"
// void breakpoints();
void gen_runsystem9(stack *)
{
#line 156 "runsystem.in"
  for(mem::list<bpinfo>::iterator p=bplist.begin(); p != bplist.end(); ++p)
    cout << p->f.name() << ": " << p->f.line() << endl;
}

#line 161 "runsystem.in"
// void clear(string file, Int line);
void gen_runsystem10(stack *Stack)
{
  Int line=vm::pop<Int>(Stack);
  string file=vm::pop<string>(Stack);
#line 162 "runsystem.in"
  file=locateFile(file);
  clear(file,line,true);
}

#line 167 "runsystem.in"
// void clear();
void gen_runsystem11(stack *)
{
#line 168 "runsystem.in"
  bplist.clear();
}

#line 172 "runsystem.in"
// void warn(string s);
void gen_runsystem12(stack *Stack)
{
  string s=vm::pop<string>(Stack);
#line 173 "runsystem.in"
  Warn(s);
}

#line 177 "runsystem.in"
// void nowarn(string s);
void gen_runsystem13(stack *Stack)
{
  string s=vm::pop<string>(Stack);
#line 178 "runsystem.in"
  noWarn(s);
}

#line 182 "runsystem.in"
// void warning(string s, string t, bool position=false);
void gen_runsystem14(stack *Stack)
{
  bool position=vm::pop<bool>(Stack,false);
  string t=vm::pop<string>(Stack);
  string s=vm::pop<string>(Stack);
#line 183 "runsystem.in"
  if(settings::warn(s)) {
    em.warning(position ? getPos() : nullPos,s);
    em << t;
  }
}

// Strip directory from string
#line 191 "runsystem.in"
// string stripdirectory(string *s);
void gen_runsystem15(stack *Stack)
{
  string * s=vm::pop<string *>(Stack);
#line 192 "runsystem.in"
  {Stack->push<string>(stripDir(*s)); return;}
}

// Strip directory from string
#line 197 "runsystem.in"
// string stripfile(string *s);
void gen_runsystem16(stack *Stack)
{
  string * s=vm::pop<string *>(Stack);
#line 198 "runsystem.in"
  {Stack->push<string>(stripFile(*s)); return;}
}

// Strip file extension from string
#line 203 "runsystem.in"
// string stripextension(string *s);
void gen_runsystem17(stack *Stack)
{
  string * s=vm::pop<string *>(Stack);
#line 204 "runsystem.in"
  {Stack->push<string>(stripExt(*s)); return;}
}

// Call ImageMagick convert.
#line 209 "runsystem.in"
// Int convert(string args=emptystring, string file=emptystring,            string format=emptystring);
void gen_runsystem18(stack *Stack)
{
  string format=vm::pop<string>(Stack,emptystring);
  string file=vm::pop<string>(Stack,emptystring);
  string args=vm::pop<string>(Stack,emptystring);
#line 211 "runsystem.in"
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
    cout << "Wrote " << (file.empty() ? name : file) << endl;
  
  {Stack->push<Int>(ret); return;}
}

// Call ImageMagick animate.
#line 237 "runsystem.in"
// Int animate(string args=emptystring, string file=emptystring,            string format=emptystring);
void gen_runsystem19(stack *Stack)
{
  string format=vm::pop<string>(Stack,emptystring);
  string file=vm::pop<string>(Stack,emptystring);
  string args=vm::pop<string>(Stack,emptystring);
#line 239 "runsystem.in"
#ifndef __MSDOS__
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

#line 253 "runsystem.in"
// void purge(Int divisor=0);
void gen_runsystem20(stack *Stack)
{
  Int divisor=vm::pop<Int>(Stack,0);
#line 254 "runsystem.in"
  purge(divisor);
}

} // namespace run

namespace trans {

void gen_runsystem_venv(venv &ve)
{
#line 107 "runsystem.in"
  addFunc(ve, run::gen_runsystem0, primString() , SYM(outname));
#line 112 "runsystem.in"
  addFunc(ve, run::gen_runsystem1, primVoid(), SYM(atupdate), formal(voidFunction(), SYM(f), false, false));
#line 117 "runsystem.in"
  addFunc(ve, run::gen_runsystem2, voidFunction(), SYM(atupdate));
#line 122 "runsystem.in"
  addFunc(ve, run::gen_runsystem3, primVoid(), SYM(atexit), formal(voidFunction(), SYM(f), false, false));
#line 127 "runsystem.in"
  addFunc(ve, run::gen_runsystem4, voidFunction(), SYM(atexit));
#line 132 "runsystem.in"
  addFunc(ve, run::gen_runsystem5, primVoid(), SYM(atbreakpoint), formal(breakpointFunction(), SYM(f), false, false));
#line 137 "runsystem.in"
  addFunc(ve, run::gen_runsystem6, primVoid(), SYM(breakpoint), formal(primCode(), SYM(s), true, false));
#line 142 "runsystem.in"
  addFunc(ve, run::gen_runsystem7, primString() , SYM(locatefile), formal(primString() , SYM(file), false, false));
#line 147 "runsystem.in"
  addFunc(ve, run::gen_runsystem8, primVoid(), SYM(stop), formal(primString() , SYM(file), false, false), formal(primInt(), SYM(line), false, false), formal(primCode(), SYM(s), true, false));
#line 155 "runsystem.in"
  addFunc(ve, run::gen_runsystem9, primVoid(), SYM(breakpoints));
#line 161 "runsystem.in"
  addFunc(ve, run::gen_runsystem10, primVoid(), SYM(clear), formal(primString() , SYM(file), false, false), formal(primInt(), SYM(line), false, false));
#line 167 "runsystem.in"
  addFunc(ve, run::gen_runsystem11, primVoid(), SYM(clear));
#line 172 "runsystem.in"
  addFunc(ve, run::gen_runsystem12, primVoid(), SYM(warn), formal(primString() , SYM(s), false, false));
#line 177 "runsystem.in"
  addFunc(ve, run::gen_runsystem13, primVoid(), SYM(nowarn), formal(primString() , SYM(s), false, false));
#line 182 "runsystem.in"
  addFunc(ve, run::gen_runsystem14, primVoid(), SYM(warning), formal(primString() , SYM(s), false, false), formal(primString() , SYM(t), false, false), formal(primBoolean(), SYM(position), true, false));
#line 190 "runsystem.in"
  addFunc(ve, run::gen_runsystem15, primString() , SYM(stripdirectory), formal(primString(), SYM(s), false, false));
#line 196 "runsystem.in"
  addFunc(ve, run::gen_runsystem16, primString() , SYM(stripfile), formal(primString(), SYM(s), false, false));
#line 202 "runsystem.in"
  addFunc(ve, run::gen_runsystem17, primString() , SYM(stripextension), formal(primString(), SYM(s), false, false));
#line 208 "runsystem.in"
  addFunc(ve, run::gen_runsystem18, primInt(), SYM(convert), formal(primString() , SYM(args), true, false), formal(primString() , SYM(file), true, false), formal(primString() , SYM(format), true, false));
#line 236 "runsystem.in"
  addFunc(ve, run::gen_runsystem19, primInt(), SYM(animate), formal(primString() , SYM(args), true, false), formal(primString() , SYM(file), true, false), formal(primString() , SYM(format), true, false));
#line 253 "runsystem.in"
  addFunc(ve, run::gen_runsystem20, primVoid(), SYM(purge), formal(primInt(), SYM(divisor), true, false));
}

} // namespace trans
