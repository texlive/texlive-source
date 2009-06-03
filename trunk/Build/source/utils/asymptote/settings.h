/*****
 * settings.h
 * Andy Hammerlindl 2004/05/10
 *
 * Declares a list of global variables that act as settings in the system.
 *****/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>

#include "common.h"
#include "pair.h"
#include "item.h"

namespace types {
class record;
}

namespace camp {
void glrenderWrapper();
}

namespace gl {
extern bool glthread;
extern bool initialize;

#ifdef HAVE_LIBPTHREAD
extern pthread_cond_t initSignal;
extern pthread_mutex_t initLock;
extern pthread_cond_t readySignal;
extern pthread_mutex_t readyLock;
extern pthread_cond_t quitSignal;
extern pthread_mutex_t quitLock;
extern pthread_t mainthread;
void wait(pthread_cond_t& signal, pthread_mutex_t& lock);
void endwait(pthread_cond_t& signal, pthread_mutex_t& lock);
#endif
}

namespace settings {
extern const char PROGRAM[];
extern const char VERSION[];
extern const char BUGREPORT[];

extern char *argv0;

extern string systemDir;
extern string docdir;
extern const string dirsep;
  
extern bool safe;
  
bool globalwrite();

enum origin {CENTER,BOTTOM,TOP,ZERO};
  
extern const string suffix;
extern const string guisuffix;
  
extern string historyname;
  
void SetPageDimensions();

types::record *getSettingsModule();

vm::item& Setting(string name);
  
template <typename T>
inline T getSetting(string name)
{
  return vm::get<T>(Setting(name));
}

extern Int verbose;
extern bool gray;
extern bool bw;
extern bool rgb;
extern bool cmyk;
  
bool view();
bool trap();
string outname();

void setOptions(int argc, char *argv[]);

// Access the arguments once options have been parsed.
int numArgs();
char *getArg(int n);
 
Int getScroll();
  
bool pdf(const string& texengine);
bool latex(const string& texengine);
bool context(const string& texengine);
  
string nativeformat();
string defaultformat();
  
const char *beginput(const string& texengine);
const char *endput(const string& texengine);
const char *beginlabel(const string& texengine);
const char *endlabel(const string& texengine);
const char *rawpostscript(const string& texengine);
const char *beginpicture(const string& texengine);
const char *endpicture(const string& texengine);
const char *beginspecial(const string& texengine);
const char *endspecial();
const char *texunits(const string& texengine);
  
extern bool fataltex[];
const char **texabort(const string& texengine);
  
string texcommand();
string texprogram();
  
const double inches=72;
const double cm=inches/2.54;
}

extern const char *SVN_REVISION;

#endif
