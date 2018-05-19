#ifndef VIEWER_H
#define VIEWER_H

#include <SDL.h>
#include <GL/glut.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

// default window size
#define D_WIDTH 1024
#define D_HEIGHT 768
#define D_BPP 32

// gis internal size
//#define DEFAULT_LB 20
//#define DEFAULT_LT 50
//#define DEFAULT_LL 120
//#define DEFAULT_LR 160

//([seconds])
#define DEFAULT_CENTER_LONGITUDE 503500
#define DEFAULT_CENTER_LATITUDE 128500
#define DEFAULT_WIDTH 250
#define DEFAULT_RATIO 1280/1024
  
#define DIFF_RATIO 1.005
#define DIFF_PERCENT .5
#define DIFF_ANGLE .5

#define DEFAULT_VIEW_HEIGHT 100
#define DEFAULT_VIEW_ANGLE 120

// viewer modes
// 1. Parallel projection (glOrtho)
// 2. Perspective projection (gluPerspective)
// 3. Mesh only
enum ViewerMode{PARALLEL, PERSPECTIVE, MESHONLY};

class Viewer
{
 private:
  int windowWidth, windowHeight, windowBpp;

  int mouseDown, mouseSelected;
  int selectionStartX, selectionStartY, selectionCurrentX, selectionCurrentY;
  int currentMouseX, currentMouseY;
  
  int keyDown, keyCode;
  
  ViewerMode viewerMode;
  
  double centerLongitude, centerLatitude;
  double viewerWidth;
  double viewerRatio;
  double angleX, angleY, angleZ;
  
  double viewHeight, viewAngle;

  void scaleDown(int i);
  void scaleUp(int i);

 public:
  ViewerMode getViewerMode();
  void setViewerMode(ViewerMode mode);
  void toggleViewerMode();
  
  int sdl_init_video(int width, int height);

  int getWindowWidth();
  int getWindowHeight();
  int getWindowBpp();
  
  void mousePressed(int button, int x, int y);
  void mouseReleased(int button, int x, int y);
  void mouseMotion(int x, int y, int diffx, int diffy);
  int isMouseDown();

  int isMouseSelected();
  int mouseX();
  int mouseY();
  int mouseSelectionStartX();
  int mouseSelectionStartY();
  int mouseSelectionCurrentX();
  int mouseSelectionCurrentY();
  
  void keyPressed(int key);
  void keyReleased(int key);
  int isKeyDown();
  int getKey();
  
  // 押された時一回だけ実行
  int runOnceKeyFunction();
  // 押している間中一定間隔で実行(by thread)
  void pressedKeyFunction();

  double getCenterLongitude();
  void setCenterLongitude(double longitude);
  double getCenterLatitude();
  void setCenterLatitude(double latitude);
  
  double getLeftLongitude();
  double getRightLongitude();
  double getTopLatitude();
  double getBottomLatitude();

  double getViewerWidth();
  double getViewerHeight();
  double getViewerRatio();
  double getAngleX();
  double getAngleY();
  double getAngleZ();

  double getViewHeight();
  double getViewAngle();

  Viewer();
  ~Viewer();
};

#endif
