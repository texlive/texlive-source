/*****
 * inst.h
 * Andy Hammerlindl 2002/06/27
 * 
 * Descibes the items and instructions that are used by the virtual machine.
 *****/

#ifndef INST_H
#define INST_H

#include <iterator>
#include <iostream>

#include "errormsg.h"
#include "item.h"
#include "vm.h"

namespace vm {

// Forward declarations
struct inst; class stack; class program;
 
// A function "lambda," that is, the code that runs a function.
// It also need the closure of the enclosing module or function to run.
struct lambda : public gc {
  // The instructions to follow.
  program *code;

  // The number of parameters of the function.  This does not include the
  // closure of the enclosing module or function.
  size_t params;

  // The total number of items that will be stored in the closure of this
  // function.  Includes the higher closure, the parameters, and the local
  // variables.
  // NOTE: In order to help garbage collection, this could be modified to
  // have one array store escaping items, and another to store non-
  // escaping items.
  size_t vars;
};

// The code run is just a string of instructions.  The ops are actual commands
// to be run, but constants, labels, and other objects can be in the code.
struct inst : public gc {
  enum opcode {
    pop, intpush, constpush,
    varpush, varsave, fieldpush, fieldsave,
    builtin, jmp, cjmp, njmp, popcall,
    pushclosure, makefunc, ret,
    alloc, pushframe, popframe
  };
  opcode op;
  position pos;
  item ref;
};
template<typename T>
inline T get(const inst& it)
{ return get<T>(it.ref); }

} // namespace vm

#endif
  
