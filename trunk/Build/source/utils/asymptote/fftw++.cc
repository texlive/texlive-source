#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBFFTW3
#include "fftw++.h"

std::ifstream fftw::ifWisdom;
std::ofstream fftw::ofWisdom;
bool fftw::Wise=false;

// User settings:
unsigned int fftw::effort=FFTW_PATIENT;
const char *fftw::WisdomName=".wisdom";

#endif
