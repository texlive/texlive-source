/*****
 * inst.h
 * Tom Prince and John Bowman 2005/04/12
 *
 * Descibes the items that are used by the virtual machine.
 *****/

#ifndef ITEM_H
#define ITEM_H

#include "common.h"
#include <cfloat>
#include <cmath>
#include <typeinfo>
#include <cassert>

namespace vm {

class item;
class bad_item_value {};

template<typename T>
T get(const item&);

#if COMPACT
// Identify a default argument.
extern const Int DefaultValue;
  
// Identify an undefined item.
extern const Int Undefined;
#endif
  
extern const item Default;

class item : public gc {
private:
  
#if !COMPACT  
  const std::type_info *kind;
#endif
  
  union {
    Int i;
    double x;
    bool b;
    void *p;
  };

public:
#if COMPACT    
  bool empty() const
  {return i >= Undefined;}
  
  item() : i(Undefined) {}
  
  item(Int i)
    : i(i) {}
  item(int i)
    : i(i) {}
  item(double x)
    : x(x) {}
  item(bool b)
    : b(b) {}
  
  item& operator= (int a)
  { i=a; return *this; }
  item& operator= (Int a)
  { i=a; return *this; }
  item& operator= (double a)
  { x=a; return *this; }
  item& operator= (bool a)
  { b=a; return *this; }
  
  template<class T>
  item(T *p)
    : p((void *) p) {
    assert(p < (void *) Undefined);
  }
  
  template<class T>
  item(const T &P)
    : p(new(UseGC) T(P)) {
    assert(p < (void *) Undefined);
  }
  
  template<class T>
  item& operator= (T *a)
  { p=(void *) a; return *this; }
  
  template<class T>
  item& operator= (const T &it)
  { p=new(UseGC) T(it); return *this; }
#else    
  bool empty() const
  {return *kind == typeid(void);}
  
  item()
    : kind(&typeid(void)) {}
  
  item(Int i)
    : kind(&typeid(Int)), i(i) {}
  item(int i)
    : kind(&typeid(Int)), i(i) {}
  item(double x)
    : kind(&typeid(double)), x(x) {}
  item(bool b)
    : kind(&typeid(bool)), b(b) {}
  
  item& operator= (int a)
  { kind=&typeid(Int); i=a; return *this; }
  item& operator= (Int a)
  { kind=&typeid(Int); i=a; return *this; }
  item& operator= (double a)
  { kind=&typeid(double); x=a; return *this; }
  item& operator= (bool a)
  { kind=&typeid(bool); b=a; return *this; }
  
  template<class T>
  item(T *p)
    : kind(&typeid(T)), p((void *) p) {}
  
  template<class T>
  item(const T &p)
    : kind(&typeid(T)), p(new(UseGC) T(p)) {}
  
  template<class T>
  item& operator= (T *a)
  { kind=&typeid(T); p=(void *) a; return *this; }
  
  template<class T>
  item& operator= (const T &it)
  { kind=&typeid(T); p=new(UseGC) T(it); return *this; }
  
  const std::type_info &type() const
  { return *kind; }
#endif  
  
  template<typename T>
  friend inline T get(const item&);

  friend inline bool isdefault(const item&);
  
  friend ostream& operator<< (ostream& out, const item& i);

private:
  template <typename T>
  struct help;
  
  template <typename T>
  struct help<T*> {
    static T* unwrap(const item& it)
    {
#if COMPACT      
      if(!it.empty())
        return (T*) it.p;
#else        
      if(*it.kind == typeid(T))
        return (T*) it.p;
#endif        
      throw vm::bad_item_value();
    }
  };
  
  template <typename T>
  struct help {
    static T& unwrap(const item& it)
    {
#if COMPACT      
      if(!it.empty())
        return *(T*) it.p;
#else      
      if(*it.kind == typeid(T))
        return *(T*) it.p;
#endif      
      throw vm::bad_item_value();
    }
  };
};
  
class frame : public gc {
#ifdef DEBUG_FRAME
  string name;
#endif
  typedef mem::vector<item> vars_t;
  vars_t vars;
public:
#ifdef DEBUG_FRAME
  frame(string name, size_t size)
    : name(name), vars(size)
  {}

  string getName() { return name; }
#else
  frame(size_t size)
    : vars(size)
  {}
#endif

  item& operator[] (size_t n)
  { return vars[n]; }
  item operator[] (size_t n) const
  { return vars[n]; }

  size_t size()
  { return vars.size(); }
  
  // Extends vars to ensure it has a place for any variable indexed up to n.
  void extend(size_t n) {
    if(vars.size() < n)
      vars.resize(n);
  }
};

template<typename T>
inline T get(const item& it)
{
  return item::help<T>::unwrap(it);
} 

template <>
inline int get<int>(const item&)
{
  throw vm::bad_item_value();
}
  
template <>
inline Int get<Int>(const item& it)
{
#if COMPACT  
  if(!it.empty())
    return it.i;
#else
  if(*it.kind == typeid(Int))
    return it.i;
#endif  
  throw vm::bad_item_value();
}
  
template <>
inline double get<double>(const item& it)
{
#if COMPACT  
  if(!it.empty())
    return it.x;
#else
  if(*it.kind == typeid(double))
    return it.x;
#endif
  throw vm::bad_item_value();
}

template <>
inline bool get<bool>(const item& it)
{
#if COMPACT  
  if(!it.empty())
    return it.b;
#else  
  if(*it.kind == typeid(bool))
    return it.b;
#endif  
  throw vm::bad_item_value();
}

#if !COMPACT
// This serves as the object for representing a default argument.
struct default_t : public gc {};
#endif

inline bool isdefault(const item& it)
{
#if COMPACT  
  return it.i == DefaultValue;
#else  
  return *it.kind == typeid(default_t);
#endif  
} 

ostream& operator<< (ostream& out, const item& i);

} // namespace vm

#endif // ITEM_H
