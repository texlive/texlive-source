/*****
 * glrender.cc
 * John Bowman and Orest Shardt
 * Render 3D Bezier paths and surfaces.
 *****/

#include <stdlib.h>
#include <fstream>
#include <cstring>
#include <sys/time.h>
#include <signal.h>

#include "common.h"

namespace gl {
bool glthread=false;
bool initialize=true;
}

#ifdef HAVE_LIBGL

// For CYGWIN
#ifndef FGAPI
#define FGAPI GLUTAPI
#endif
#ifndef FGAPIENTRY
#define FGAPIENTRY APIENTRY
#endif

#define GLUT_BUILDING_LIB

#include "picture.h"
#include "arcball.h"
#include "bbox3.h"
#include "drawimage.h"
#include "interact.h"
#include "tr.h"

#ifdef FREEGLUT
#include <GL/freeglut_ext.h>
#endif

namespace gl {
  
using camp::picture;
using camp::drawImage;
using camp::transform;
using camp::pair;
using camp::triple;
using vm::array;
using camp::bbox3;
using settings::getSetting;
using settings::Setting;

const double moveFactor=1.0;  
const double zoomFactor=1.05;
const double zoomFactorStep=0.1;
const double spinStep=60.0; // Degrees per second
const double arcballRadius=750.0;
const double resizeStep=1.2;

double Aspect;
bool View;
bool Iconify=false;
int Oldpid;
string Prefix;
const picture* Picture;
string Format;
int Width,Height;
int fullWidth,fullHeight;
int oldWidth,oldHeight;
double oWidth,oHeight;
int screenWidth,screenHeight;
int maxWidth;
int maxHeight;
int maxTileWidth;
int maxTileHeight;

double *T;

bool Xspin,Yspin,Zspin;
bool Menu;
bool Motion;
bool ignorezoom;

int Fitscreen;
int Mode;

double H;
double xmin,xmax;
double ymin,ymax;
double zmin,zmax;

double Xmin,Xmax;
double Ymin,Ymax;
double X,Y;

int minimumsize=50; // Minimum initial rendering window width and height

const double degrees=180.0/M_PI;
const double radians=1.0/degrees;

size_t Nlights;
triple *Lights; 
double *Diffuse;
double *Ambient;
double *Specular;
bool ViewportLighting;
bool queueExport=false;
bool antialias;

int x0,y0;
int mod;

double lastangle;

double Zoom;
double lastzoom;

GLfloat Rotate[16];
Arcball arcball;
  
GLUnurbs *nurb;

int window;
  
void *glrenderWrapper(void *a);

#ifdef HAVE_LIBPTHREAD
pthread_t mainthread;

pthread_cond_t initSignal=PTHREAD_COND_INITIALIZER;
pthread_mutex_t initLock=PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t quitSignal=PTHREAD_COND_INITIALIZER;
pthread_mutex_t quitLock=PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t readySignal=PTHREAD_COND_INITIALIZER;
pthread_mutex_t readyLock=PTHREAD_MUTEX_INITIALIZER;
#endif

inline void lock() 
{
#ifdef HAVE_LIBPTHREAD
  if(glthread)
    pthread_mutex_lock(&readyLock);
#endif  
}

inline void unlock() 
{
#ifdef HAVE_LIBPTHREAD
  if(glthread)
    pthread_mutex_unlock(&readyLock);
#endif  

}

template<class T>
inline T min(T a, T b)
{
  return (a < b) ? a : b;
}

template<class T>
inline T max(T a, T b)
{
  return (a > b) ? a : b;
}

void lighting()
{
  for(size_t i=0; i < Nlights; ++i) {
    GLenum index=GL_LIGHT0+i;
    triple Lighti=Lights[i];
    GLfloat position[]={Lighti.getx(),Lighti.gety(),Lighti.getz(),0.0};
    glLightfv(index,GL_POSITION,position);
  }
}

void initlighting() 
{
  glEnable(GL_LIGHTING);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,getSetting<bool>("twosided"));
    
  for(size_t i=0; i < Nlights; ++i) {
    GLenum index=GL_LIGHT0+i;
    glEnable(index);
    
    size_t i4=4*i;
    
    GLfloat diffuse[]={Diffuse[i4],Diffuse[i4+1],Diffuse[i4+2],Diffuse[i4+3]};
    glLightfv(index,GL_DIFFUSE,diffuse);
    
    GLfloat ambient[]={Ambient[i4],Ambient[i4+1],Ambient[i4+2],Ambient[i4+3]};
    glLightfv(index,GL_AMBIENT,ambient);
    
    GLfloat specular[]={Specular[i4],Specular[i4+1],Specular[i4+2],
                        Specular[i4+3]};
    glLightfv(index,GL_SPECULAR,specular);
  }
  
  static size_t lastNlights=0;
  for(size_t i=Nlights; i < lastNlights; ++i) {
    GLenum index=GL_LIGHT0+i;
    glDisable(index);
  }
  lastNlights=Nlights;
  
  if(ViewportLighting)
    lighting();
}

void setDimensions(int Width, int Height, double X, double Y)
{
  double Aspect=((double) Width)/Height;
  double X0=X*(xmax-xmin)/(lastzoom*Width);
  double Y0=Y*(ymax-ymin)/(lastzoom*Height);
  if(H == 0.0) {
    double xsize=Xmax-Xmin;
    double ysize=Ymax-Ymin;
    if(xsize < ysize*Aspect) {
      double r=0.5*ysize*Zoom*Aspect;
      xmin=-r-X0;
      xmax=r-X0;
      ymin=Ymin*Zoom-Y0;
      ymax=Ymax*Zoom-Y0;
    } else {
      double r=0.5*xsize*Zoom/Aspect;
      xmin=Xmin*Zoom-X0;
      xmax=Xmax*Zoom-X0;
      ymin=-r-Y0;
      ymax=r-Y0;
    }
  } else {
    double r=H*Zoom;
    xmin=-r*Aspect-X0;
    xmax=r*Aspect-X0;
    ymin=-r-Y0;
    ymax=r-Y0;
  }
}

void setProjection()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  setDimensions(Width,Height,X,Y);
  if(H == 0.0)
    glOrtho(xmin,xmax,ymin,ymax,-zmax,-zmin);
  else
    glFrustum(xmin,xmax,ymin,ymax,-zmax,-zmin);
  glMatrixMode(GL_MODELVIEW);
  arcball.set_params(vec2(0.5*Width,0.5*Height),arcballRadius/Zoom);
}

bool capsize(int& width, int& height) 
{
  bool resize=false;
  if(width > maxWidth) {
    width=maxWidth;
    resize=true;
  }
  if(height > maxHeight) {
    height=maxHeight;
    resize=true;
  }
  return resize;
}

void reshape0(int width, int height)
{
  X=X/Width*width;
  Y=Y/Height*height;
  
  Width=width;
  Height=height;
  
  setProjection();
  glViewport(0,0,Width,Height);
}
  
void update() 
{
  lastzoom=Zoom;
  glLoadIdentity();
  double cz=0.5*(zmin+zmax);
  glTranslatef(0,0,cz);
  glMultMatrixf(Rotate);
  glTranslatef(0,0,-cz);
  setProjection();
  glutPostRedisplay();
}

void drawscene(double Width, double Height)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(!ViewportLighting) 
    lighting();
    
  triple m(xmin,ymin,zmin);
  triple M(xmax,ymax,zmax);
  double perspective=H == 0.0 ? 0.0 : 1.0/zmax;
  
  double size2=hypot(Width,Height);
  
  glEnable(GL_BLEND);
  // Render opaque objects
  Picture->render(nurb,size2,m,M,perspective,false);
  
  // Enable transparency
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  
  // Render transparent objects
  Picture->render(nurb,size2,m,M,perspective,true);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
}

// Return x divided by y rounded up to the nearest integer.
int Quotient(int x, int y) 
{
  return (x+y-1)/y;
}

void Export()
{
  glReadBuffer(GL_BACK_LEFT);
  glPixelStorei(GL_PACK_ALIGNMENT,1);
  glFinish();
  try {
    size_t ndata=3*fullWidth*fullHeight;
    unsigned char *data=new unsigned char[ndata];
    if(data) {
      TRcontext *tr=trNew();
      int width=Quotient(fullWidth,Quotient(fullWidth,min(maxTileWidth,Width)));
      int height=Quotient(fullHeight,Quotient(fullHeight,
                                              min(maxTileHeight,Height)));
      if(settings::verbose > 1) 
        cout << "Exporting " << Prefix << " as " << fullWidth << "x" 
             << fullHeight << " image" << " using tiles of size "
             << width << "x" << height << endl;

      trTileSize(tr,width,height,0);
      trImageSize(tr,fullWidth,fullHeight);
      trImageBuffer(tr,GL_RGB,GL_UNSIGNED_BYTE,data);

      setDimensions(fullWidth,fullHeight,X/Width*fullWidth,Y/Width*fullWidth);
      if(H == 0.0)
        trOrtho(tr,xmin,xmax,ymin,ymax,-zmax,-zmin);
      else
        trFrustum(tr,xmin,xmax,ymin,ymax,-zmax,-zmin);
   
      size_t count=0;
      do {
        trBeginTile(tr);
        drawscene(fullWidth,fullHeight);
        ++count;
      } while (trEndTile(tr));
      if(settings::verbose > 1)
        cout << count << " tile" << (count > 1 ? "s" : "") << " drawn" << endl;
      trDelete(tr);

      picture pic;
      double w=oWidth;
      double h=oHeight;
      double Aspect=((double) fullWidth)/fullHeight;
      if(w > h*Aspect) w=(int) (h*Aspect+0.5);
      else h=(int) (w/Aspect+0.5);
      // Render an antialiased image.
      drawImage *Image=new drawImage(data,fullWidth,fullHeight,
                                     transform(0.0,0.0,w,0.0,0.0,h),antialias);
      pic.append(Image);
      pic.shipout(NULL,Prefix,Format,0.0,false,View);
      delete Image;
      delete[] data;
    } 
  } catch(handled_error) {
  } catch(std::bad_alloc&) {
    outOfMemory();
  }
  setProjection();
}
  
void LockedExport()
{
  if(interact::interactive)
    lock();
  Export();
  if(interact::interactive)
    unlock();
}

void windowposition(int& x, int& y, int width=Width, int height=Height)
{
  pair z=getSetting<pair>("position");
  x=(int) z.getx();
  y=(int) z.gety();
  if(x < 0) {
    x += screenWidth-width;
    if(x < 0) x=0;
  }
  if(y < 0) {
    y += screenHeight-height;
    if(y < 0) y=0;
  }
}

void setsize(int w, int h, int minsize=0)
{
  int x,y;
  
  if(minsize) {
    if(w < minsize) {
      h=(int) (h*(double) minsize/w+0.5);
      w=minsize;
    }
  
    if(h < minsize) {
      w=(int) (w*(double) minsize/h+0.5);
      h=minsize;
    }
  }
  
  capsize(w,h);
  windowposition(x,y,w,h);
  glutPositionWindow(x,y);
  glutReshapeWindow(w,h);
  reshape0(w,h);
  glutPostRedisplay();
}

void fullscreen() 
{
  Width=screenWidth;
  Height=screenHeight;
#ifdef __CYGWIN__
  glutFullScreen();
#else
  glutPositionWindow(0,0);
  glutReshapeWindow(Width,Height);
  reshape0(Width,Height);
  glutPostRedisplay();
#endif    
}

void fitscreen() 
{
  switch(Fitscreen) {
    case 0: // Original size
    {
      setsize(oldWidth,oldHeight,minimumsize);
      break;
    }
    case 1: // Fit to screen in one dimension
    {       
      oldWidth=Width;
      oldHeight=Height;
      int w=screenWidth;
      int h=screenHeight;
      if(w >= h*Aspect) w=(int) (h*Aspect+0.5);
      else h=(int) (w/Aspect+0.5);
      setsize(w,h,minimumsize);
      break;
    }
    case 2: // Full screen
    {
      fullscreen();
      break;
    }
  }
}

void togglefitscreen() 
{
  ++Fitscreen;
  if(Fitscreen > 2) Fitscreen=0;
  fitscreen();
}

void updateHandler(int)
{
  initlighting();
  update();
  if(interact::interactive) {
    glutShowWindow();
    glutShowWindow(); // Call twice to work around apparent freeglut bug.
  } else if(glthread) fitscreen();
}

void autoExport()
{
  if(!Iconify)
    glutShowWindow();
  Export();
  if(!Iconify)
    glutHideWindow();
}

void exportHandler(int)
{
#ifdef HAVE_LIBPTHREAD
  if(glthread)
    wait(readySignal,readyLock);
#endif
  initlighting();
  autoExport();
}

void reshape(int width, int height)
{
  if(glthread) {
    static bool initialize=true;
    if(initialize) {
      initialize=false;
      signal(SIGUSR1,updateHandler);
      signal(SIGUSR2,exportHandler);
      unlock();
    }
  }
  
  if(capsize(width,height))
    glutReshapeWindow(width,height);
 
  reshape0(width,height);
}
  
#ifdef HAVE_LIBPTHREAD
void endwait(pthread_cond_t& signal, pthread_mutex_t& lock)
{
  pthread_cond_signal(&signal);
}
void wait(pthread_cond_t& signal, pthread_mutex_t& lock)
{
  pthread_mutex_lock(&lock);
  pthread_cond_signal(&signal);
  pthread_cond_wait(&signal,&lock);
  endwait(signal,lock);
  pthread_mutex_unlock(&lock);
}

#endif

void quit() 
{
  if(glthread) {
    if(interact::interactive) {
      glutHideWindow();
    } else {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glutSwapBuffers();
    }
#ifdef HAVE_LIBPTHREAD
    if(!interact::interactive) {
      pthread_mutex_lock(&quitLock);
      pthread_cond_signal(&quitSignal);
      pthread_mutex_unlock(&quitLock);
    }
#endif
  } else {
    glutDestroyWindow(window);
    exit(0);
  }
}

void display()
{
  if(interact::interactive)
    lock();
  drawscene(Width,Height);
  glutSwapBuffers();
  if(queueExport) {
    Export();
    queueExport=false;
  }
  if(interact::interactive)
    unlock();
  if(!glthread) {
    if(Oldpid != 0 && waitpid(Oldpid,NULL,WNOHANG) != Oldpid) {
      kill(Oldpid,SIGHUP);
      Oldpid=0;
    }
  }
}

void move(int x, int y)
{
  if(x > 0 && y > 0) {
    X += (x-x0)*Zoom;
    Y += (y0-y)*Zoom;
    x0=x; y0=y;
    update();
  }
}
  
void capzoom() 
{
  static double maxzoom=sqrt(DBL_MAX);
  static double minzoom=1/maxzoom;
  if(Zoom <= minzoom) Zoom=minzoom;
  if(Zoom >= maxzoom) Zoom=maxzoom;
  
}

void disableMenu() 
{
  glutDetachMenu(GLUT_RIGHT_BUTTON);
  Menu=false;
}

void zoom(int x, int y)
{
  if(ignorezoom) {ignorezoom=false; y0=y; return;}
  if(x > 0 && y > 0) {
    if(Menu) {
      disableMenu();
      y0=y;
      return;
    }
    Motion=true;
    static const double limit=log(0.1*DBL_MAX)/log(zoomFactor);
    lastzoom=Zoom;
    double s=zoomFactorStep*(y-y0);
    if(fabs(s) < limit) {
      Zoom *= pow(zoomFactor,s);
      capzoom();
      y0=y;
      setProjection();
      glutPostRedisplay();
    }
  }
}
  
void mousewheel(int wheel, int direction, int x, int y) 
{
  lastzoom=Zoom;
  if(direction > 0)
    Zoom /= zoomFactor;
  else
    Zoom *= zoomFactor;
  
  capzoom();
  setProjection();
  glutPostRedisplay();
}

void rotate(int x, int y)
{
  if(x > 0 && y > 0) {
    if(Menu) {
      disableMenu();
      arcball.mouse_down(x,Height-y);
      return;
    }
    Motion=true;
    arcball.mouse_motion(x,Height-y,0,
                         mod == GLUT_ACTIVE_SHIFT, // X rotation only
                         mod == GLUT_ACTIVE_CTRL);  // Y rotation only

    for(int i=0; i < 4; ++i) {
      const vec4& roti=arcball.rot[i];
      int i4=4*i;
      for(int j=0; j < 4; ++j)
        Rotate[i4+j]=roti[j];
    }
    
    update();
  }
}
  
double Degrees(int x, int y) 
{
  return atan2(0.5*Height-y-Y,x-0.5*Width-X)*degrees;
}

void updateArcball() 
{
  for(int i=0; i < 4; ++i) {
    int i4=4*i;
    vec4& roti=arcball.rot[i];
    for(int j=0; j < 4; ++j)
      roti[j]=Rotate[i4+j];
  }
  update();
}
  
void rotateX(double step) 
{
  glLoadIdentity();
  glRotatef(step,1,0,0);
  glMultMatrixf(Rotate);
  glGetFloatv(GL_MODELVIEW_MATRIX,Rotate);
  updateArcball();
}

void rotateY(double step) 
{
  glLoadIdentity();
  glRotatef(step,0,1,0);
  glMultMatrixf(Rotate);
  glGetFloatv(GL_MODELVIEW_MATRIX,Rotate);
  updateArcball();
}

void rotateZ(double step) 
{
  glLoadIdentity();
  glRotatef(step,0,0,1);
  glMultMatrixf(Rotate);
  glGetFloatv(GL_MODELVIEW_MATRIX,Rotate);
  updateArcball();
}

void rotateZ(int x, int y)
{
  if(x > 0 && y > 0) {
    if(Menu) {
      disableMenu();
      x=x0; y=y0;
      return;
    }
    Motion=true;
    double angle=Degrees(x,y);
    rotateZ(angle-lastangle);
    lastangle=angle;
  }
}

#ifndef GLUT_WHEEL_UP
#define GLUT_WHEEL_UP 3
#endif

#ifndef GLUT_WHEEL_DOWN
#define GLUT_WHEEL_DOWN 4
#endif

// Mouse bindings.
// LEFT: rotate
// SHIFT LEFT: zoom
// CTRL LEFT: shift
// MIDDLE: menu
// RIGHT: zoom
// SHIFT RIGHT: rotateX
// CTRL RIGHT: rotateY
// ALT RIGHT: rotateZ
void mouse(int button, int state, int x, int y)
{
  if(button == GLUT_WHEEL_UP) {
    mousewheel(0,1,x,y);
    return;
  } 
  if(button == GLUT_WHEEL_DOWN) {
    mousewheel(0,-1,x,y);
    return;
  }     
  
  mod=glutGetModifiers();
  
  if(button == GLUT_RIGHT_BUTTON) {
    if(state == GLUT_UP && !Motion) {
      glutAttachMenu(GLUT_RIGHT_BUTTON);
      Menu=true;
      return;
    }
  }
  
  if(Menu) disableMenu();
  else Motion=false;
  
  if(state == GLUT_DOWN) {
    if(button == GLUT_LEFT_BUTTON && mod == GLUT_ACTIVE_CTRL) {
      x0=x; y0=y;
      glutMotionFunc(move);
      return;
    } 
    if((button == GLUT_LEFT_BUTTON && mod == GLUT_ACTIVE_SHIFT) ||
       (button == GLUT_RIGHT_BUTTON && mod == 0)) {
      y0=y;
      glutMotionFunc(zoom);
      return;
    }
    if(button == GLUT_RIGHT_BUTTON && mod == GLUT_ACTIVE_ALT) {
      lastangle=Degrees(x,y);
      glutMotionFunc(rotateZ);
      return;
    }
    arcball.mouse_down(x,Height-y);
    glutMotionFunc(rotate);
  } else
    arcball.mouse_up();
}

timeval lasttime;

double spinstep() 
{
  timeval tv;
  gettimeofday(&tv,NULL);
  double step=spinStep*(tv.tv_sec-lasttime.tv_sec+
                        ((double) tv.tv_usec-lasttime.tv_usec)/1000000.0);
  lasttime=tv;
  return step;
}

void xspin()
{
  rotateX(spinstep());
}

void yspin()
{
  rotateY(spinstep());
}

void zspin()
{
  rotateZ(spinstep());
}

void initTimer() 
{
  gettimeofday(&lasttime,NULL);
}

void expand() 
{
  setsize((int) (Width*resizeStep+0.5),(int) (Height*resizeStep+0.5));
}

void shrink() 
{
  setsize(max((int) (Width/resizeStep+0.5),1),
          max((int) (Height/resizeStep+0.5),1));
}

void idleFunc(void (*f)())
{
  initTimer();
  glutIdleFunc(f);
}

void mode() 
{
  switch(Mode) {
    case 0:
      for(size_t i=0; i < Nlights; ++i) 
        glEnable(GL_LIGHT0+i);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      gluNurbsProperty(nurb,GLU_DISPLAY_MODE,GLU_FILL);
      ++Mode;
      break;
    case 1:
      for(size_t i=0; i < Nlights; ++i) 
        glDisable(GL_LIGHT0+i);
      gluNurbsProperty(nurb,GLU_DISPLAY_MODE,GLU_OUTLINE_POLYGON);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      ++Mode;
      break;
    case 2:
      gluNurbsProperty(nurb,GLU_DISPLAY_MODE,GLU_OUTLINE_PATCH);
      Mode=0;
      break;
  }
  glutPostRedisplay();
}

void idle() 
{
  glutIdleFunc(NULL);
  Xspin=Yspin=Zspin=false;
}

void spinx() 
{
  if(Xspin)
    idle();
  else {
    idleFunc(xspin);
    Xspin=true;
    Yspin=Zspin=false;
  }
}

void spiny()
{
  if(Yspin)
    idle();
  else {
    idleFunc(yspin);
    Yspin=true;
    Xspin=Zspin=false;
  }
}

void spinz()
{
  if(Zspin)
    idle();
  else {
    idleFunc(zspin);
    Zspin=true;
    Xspin=Yspin=false;
  }
}

void home() 
{
  idle();
  X=Y=0.0;
  arcball.init();
  glLoadIdentity();
  glGetFloatv(GL_MODELVIEW_MATRIX,Rotate);
  lastzoom=Zoom=1.0;
}

void write(const char *text, const double *v)
{
  cout << text << "=(" << v[0] << "," << v[1] << "," << v[2] << ")";
}

void camera()
{
  double cz=0.5*(zmin+zmax);
  camp::Triple vCamera,vTarget,vUp;
  
  for(int i=0; i < 3; ++i) {
    double sumCamera=0.0, sumTarget=0.0, sumUp=0.0;
    for(int j=0; j < 4; ++j) {
      sumCamera += T[4*i+j]*(Rotate[4*j+3]-cz*Rotate[4*j+2]);
      sumUp += T[4*i+j]*(Rotate[4*j+3]+Rotate[4*j+1]);
      sumTarget += T[4*i+j]*Rotate[4*j+3];
    }
    vCamera[i]=sumCamera;
    vUp[i]=sumUp-sumTarget;
    vTarget[i]=sumTarget;
  }
  
  triple Camera=triple(vCamera);
  triple Up=triple(vUp);
  triple Target=triple(vTarget);
  
  cout << "camera=" << Camera << "," << endl
       << "up=" << Up << "," << endl
       << "target=" << Target << endl << endl;
}

void keyboard(unsigned char key, int x, int y)
{
  switch(key) {
    case 'h':
      home();
      update();
      break;
    case 'f':
      togglefitscreen();
      break;
    case 'x':
      spinx();
      break;
    case 'y':
      spiny();
      break;
    case 'z':
      spinz();
      break;
    case 's':
      idle();
      break;
    case 'm':
      mode();
      break;
    case 'e':
      LockedExport();
      break;
    case 'c':
      camera();
      break;
    case '+':
    case '=':
      expand();
      break;
    case '-':
    case '_':
      shrink();
      break;
    case 17: // Ctrl-q
    case 'q':
      if(!Format.empty()) LockedExport();
      quit();
      break;
  }
}
 
enum Menu {HOME,FITSCREEN,XSPIN,YSPIN,ZSPIN,STOP,MODE,EXPORT,CAMERA,QUIT};

void menu(int choice)
{
  disableMenu();
  ignorezoom=true;
  Motion=true;
  switch (choice) {
    case HOME: // Home
      home();
      update();
      break;
    case FITSCREEN:
      togglefitscreen();
      break;
    case XSPIN:
      spinx();
      break;
    case YSPIN:
      spiny();
      break;
    case ZSPIN:
      spinz();
      break;
    case STOP:
      idle();
      break;
    case MODE:
      mode();
      break;
    case EXPORT:
      queueExport=true;
      break;
    case CAMERA:
      camera();
      break;
    case QUIT:
      quit();
      break;
  }
}

void setosize()
{
  oldWidth=(int) ceil(oWidth);
  oldHeight=(int) ceil(oHeight);
}

void init() 
{
  string options=string(settings::argv0)+" ";
  if(Iconify)
    options += "-iconic ";
  options += getSetting<string>("glOptions");
  char **argv=args(options.c_str(),true);
  int argc=0;
  while(argv[argc] != NULL)
    ++argc;
  
  glutInit(&argc,argv);
  
  screenWidth=glutGet(GLUT_SCREEN_WIDTH);
  screenHeight=glutGet(GLUT_SCREEN_HEIGHT);
}

// angle=0 means orthographic.
void glrender(const string& prefix, const picture *pic, const string& format,
              double width, double height,
              double angle, const triple& m, const triple& M, double *t,
              size_t nlights, triple *lights, double *diffuse,
              double *ambient, double *specular, bool Viewportlighting,
              bool view, int oldpid)
{
#ifndef __CYGWIN__    
  Iconify=getSetting<bool>("iconify");
#endif      
  
  width=max(width,1.0);
  height=max(height,1.0);
  
  static bool initializedView=false;
  
  if(view || initializedView) lock();
    
  Prefix=prefix;
  Picture=pic;
  Format=format;
  T=t;
  Nlights=min(nlights,(size_t) GL_MAX_LIGHTS);
  Lights=lights;
  Diffuse=diffuse;
  Ambient=ambient;
  Specular=specular;
  ViewportLighting=Viewportlighting;
  View=view;
  Oldpid=oldpid;
  
  static bool initialized=false;

  if(!initialized) {
    init();
    Fitscreen=1;
  }
  
  Xmin=m.getx();
  Xmax=M.getx();
  Ymin=m.gety();
  Ymax=M.gety();
  zmin=m.getz();
  zmax=M.getz();
  H=angle != 0.0 ? -tan(0.5*angle*radians)*zmax : 0.0;
   
  Menu=false;
  Motion=true;
  ignorezoom=false;
  Mode=0;
  
  if(!initialized || !interact::interactive) {
    antialias=getSetting<Int>("antialias") > 1;
    double expand=getSetting<double>("render");
    if(expand < 0)
      expand *= (Format.empty() || Format == "eps" || Format == "pdf") 
        ? -2.0 : -1.0;
    if(antialias) expand *= 2.0;
  
    // Force a hard viewport limit to work around direct rendering bugs.
    // Alternatively, one can use -glOptions=-indirect (with a performance
    // penalty).
    pair maxViewport=getSetting<pair>("maxviewport");
    maxWidth=(int) ceil(maxViewport.getx());
    maxHeight=(int) ceil(maxViewport.gety());
    if(maxWidth <= 0) maxWidth=max(maxHeight,2);
    if(maxHeight <= 0) maxHeight=max(maxWidth,2);
    if(screenWidth <= 0) screenWidth=maxWidth;
    if(screenHeight <= 0) screenHeight=maxHeight;
  
    oWidth=width;
    oHeight=height;
    Aspect=width/height;
  
    fullWidth=(int) ceil(expand*width);
    fullHeight=(int) ceil(expand*height);
  
    Width=min(fullWidth,screenWidth);
    Height=min(fullHeight,screenHeight);
  
    if(Width > Height*Aspect) 
      Width=min((int) (ceil(Height*Aspect)),screenWidth);
    else 
      Height=min((int) (ceil(Width/Aspect)),screenHeight);
  
    Aspect=((double) Width)/Height;
  
    pair maxtile=getSetting<pair>("maxtile");
    maxTileWidth=(int) maxtile.getx();
    maxTileHeight=(int) maxtile.gety();
    if(maxTileWidth <= 0) maxTileWidth=screenWidth;
    if(maxTileHeight <= 0) maxTileHeight=screenHeight;
  
    setosize();
  
    if(View && settings::verbose > 1) 
      cout << "Rendering " << prefix << " as " << Width << "x" << Height
           << " image" << endl;
  }
  
#ifdef HAVE_LIBPTHREAD
  if(initializedView && glthread) {
    if(View)
      pthread_kill(mainthread,SIGUSR1);
    else
      pthread_kill(mainthread,SIGUSR2);
    unlock();
    return;
  }
#endif    
  
  unsigned int displaymode=GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH;
  
  bool havewindow=initialized && glthread;
  
  if(View) {
    int x,y;
    if(havewindow)
      glutDestroyWindow(window);
    
    windowposition(x,y);
    glutInitWindowPosition(x,y);
    glutInitWindowSize(1,1);
    Int multisample=getSetting<Int>("multisample");
    if(multisample <= 1) multisample=0;
    if(multisample)
      displaymode |= GLUT_MULTISAMPLE;
    glutInitDisplayMode(displaymode);

    ostringstream buf;
    int samples;
#ifdef FREEGLUT
#ifdef GLUT_INIT_MAJOR_VERSION
    while(true) {
      if(multisample > 0)
        glutSetOption(GLUT_MULTISAMPLE,multisample);
#endif      
#endif      
      string title=string(settings::PROGRAM)+": "+prefix+
        " [Double click right button for menu]";
      window=glutCreateWindow(title.c_str());
      GLint samplebuf[1];
      glGetIntegerv(GL_SAMPLES,samplebuf);
      samples=samplebuf[0];
#ifdef FREEGLUT
#ifdef GLUT_INIT_MAJOR_VERSION
      if(samples < multisample) {
        --multisample;
        if(multisample > 1) {
          glutDestroyWindow(window);
          continue;
        }
      }
      break;
    }
#endif      
#endif      
    if(samples > 1) {
      if(settings::verbose > 1 && samples > 1)
        cout << "Multisampling enabled with sample width " << samples << endl;
    }
    glutShowWindow();
  } else if(!havewindow) {
    glutInitWindowSize(maxTileWidth,maxTileHeight);
    glutInitDisplayMode(displaymode);
    window=glutCreateWindow("");
    glutHideWindow();
  }
  
  initialized=true;
  
  glClearColor(1.0,1.0,1.0,1.0);
   
  glMatrixMode(GL_MODELVIEW);
    
  home();
  
  if(View) {
    if(!getSetting<bool>("fitscreen"))
      Fitscreen=0;
    fitscreen();
    setosize();
  }
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MAP1_VERTEX_3);
  glEnable(GL_MAP2_VERTEX_3);
  glEnable(GL_MAP2_COLOR_4);
  
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  
  nurb=gluNewNurbsRenderer();
  if(nurb == NULL) 
    outOfMemory();
  gluNurbsProperty(nurb,GLU_SAMPLING_METHOD,GLU_PARAMETRIC_ERROR);
  gluNurbsProperty(nurb,GLU_SAMPLING_TOLERANCE,0.5);
  gluNurbsProperty(nurb,GLU_PARAMETRIC_TOLERANCE,1.0);
  gluNurbsProperty(nurb,GLU_CULLING,GLU_TRUE);
  
  // The callback tesselation algorithm avoids artifacts at degenerate control
  // points.
  gluNurbsProperty(nurb,GLU_NURBS_MODE,GLU_NURBS_TESSELLATOR);
  gluNurbsCallback(nurb,GLU_NURBS_BEGIN,(_GLUfuncptr) glBegin);
  gluNurbsCallback(nurb,GLU_NURBS_VERTEX,(_GLUfuncptr) glVertex3fv);
  gluNurbsCallback(nurb,GLU_NURBS_END,(_GLUfuncptr) glEnd);
  gluNurbsCallback(nurb,GLU_NURBS_COLOR,(_GLUfuncptr) glColor4fv);
  mode();
  
  initlighting();
  
  if(View) {
    initializedView=true;
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
  
    glutCreateMenu(menu);
    glutAddMenuEntry("(h) Home",HOME);
    glutAddMenuEntry("(f) Fitscreen",FITSCREEN);
    glutAddMenuEntry("(x) X spin",XSPIN);
    glutAddMenuEntry("(y) Y spin",YSPIN);
    glutAddMenuEntry("(z) Z spin",ZSPIN);
    glutAddMenuEntry("(s) Stop",STOP);
    glutAddMenuEntry("(m) Mode",MODE);
    glutAddMenuEntry("(e) Export",EXPORT);
    glutAddMenuEntry("(c) Camera",CAMERA);
    glutAddMenuEntry("(q) Quit" ,QUIT);
  
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

    glutMainLoop();
  } else {
    if(glthread) {
      exportHandler(0);
    } else {
      autoExport();
      quit();
    }
  }
}

} // namespace gl

#endif
