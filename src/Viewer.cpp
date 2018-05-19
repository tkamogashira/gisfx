#include "Viewer.h"

ViewerMode Viewer::getViewerMode()
{
  return viewerMode;
}

void Viewer::setViewerMode(ViewerMode mode)
{
  viewerMode = mode;
}

void Viewer::toggleViewerMode()
{
  if(viewerMode == PARALLEL)
    {
      viewerMode = PERSPECTIVE;
    }
  else if(viewerMode == PERSPECTIVE)
    {
      viewerMode = MESHONLY;
    }
  else if(viewerMode == MESHONLY)
    {
      viewerMode = PARALLEL;
    }
}

int Viewer::sdl_init_video(int width, int height)
{
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      fprintf(stderr, "SDL_Init(SDL_INIT_VIDEO): %s\n", SDL_GetError());
      return -1;
    }
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if(width != 0)
    windowWidth = width;
  if(height != 0)
    windowHeight = height;
  
  int bpp = SDL_VideoModeOK(windowWidth, windowHeight,
			windowBpp, SDL_OPENGL|SDL_RESIZABLE);
  if(!bpp)
    {
      fprintf(stderr, "SDL_VideoModeOK(): not available\n");
      return -1;
    }

  // SDL_RESIZABLE: thread safeでresizeするには、
  // XInitThreads()をmain()の始めにおく。(Xlib: unexpected async reply)
  // SDL_Events系はnot thread safeなので、とりあえずあきらめる。
  SDL_Surface *screen = SDL_SetVideoMode(windowWidth, windowHeight,
			    windowBpp, SDL_OPENGLBLIT);
  
  if(!screen)
    {
      fprintf(stderr, "SDL_SetVideoMode(): %s\n", SDL_GetError());
      return -1;
    }
  glViewport(0, 0, windowWidth, windowHeight);
  return 0;
}

int Viewer::getWindowWidth()
{
  return windowWidth;
}

int Viewer::getWindowHeight()
{
  return windowHeight;
}

int Viewer::getWindowBpp()
{
  return windowBpp;
}

void Viewer::mousePressed(int button, int x, int y)
{
  mouseDown = button;
  currentMouseX = x;
  currentMouseY = y;
  switch(button)
    {
    case 3:
      mouseSelected = 1;
      selectionCurrentX = selectionStartX = x;
      selectionCurrentY = selectionStartY = y;
      break;
    case 4:
      scaleUp(20);
      break;
    case 5:
      scaleDown(20);
      break;
    default:
      break;
    }
}

void Viewer::mouseReleased(int button, int x, int y)
{
  mouseDown = 0;
  currentMouseX = x;
  currentMouseY = y;
  switch(button)
    {
    case 3:
      if(selectionStartX != x||selectionStartY != y)
	{
	  mouseSelected = 1;
	  selectionCurrentX = x;
	  selectionCurrentY = y;	  
	}
      // 選択しはじめた時と同じ所でrelease = 選択していない
      else
	{
	  mouseSelected = 0;
	}
      break;
    default:
      break;
    }
}

void Viewer::mouseMotion(int x, int y, int diffx, int diffy)
{
  currentMouseX = x;
  currentMouseY = y;
  switch(mouseDown)
    {
    case 3:
      selectionCurrentX = x;
      selectionCurrentY = y;
      break;
    case 2:
      angleX -= DIFF_ANGLE / 2 * diffy;
      angleZ += DIFF_ANGLE / 2 * diffx;
      break;
    case 1:
      centerLongitude -=
	cos(2*M_PI*angleZ/360)*2*((double)diffx/(double)windowWidth*(double)viewerWidth) -
	sin(2*M_PI*angleZ/360)*2*((double)diffy/(double)windowHeight*(double)viewerWidth*(double)viewerRatio);
      centerLatitude +=
	cos(2*M_PI*angleZ/360)*2*((double)diffy/(double)windowHeight*(double)viewerWidth*(double)viewerRatio) +
	sin(2*M_PI*angleZ/360)*2*((double)diffx/(double)windowWidth*(double)viewerWidth);
      break;
    default:
      break;
    }
}

int Viewer::isMouseDown()
{
  return mouseDown;
}

int Viewer::isMouseSelected()
{
  return mouseSelected;
}

int Viewer::mouseX()
{
  return currentMouseX;
}

int Viewer::mouseY()
{
  return currentMouseY;
}

int Viewer::mouseSelectionStartX()
{
  return selectionStartX;
}

int Viewer::mouseSelectionStartY()
{
  return selectionStartY;
}

int Viewer::mouseSelectionCurrentX()
{
  return selectionCurrentX;
}

int Viewer::mouseSelectionCurrentY()
{
  return selectionCurrentY;
}

void Viewer::keyPressed(int key)
{
  keyDown = 1;
  keyCode = key;
}

void Viewer::keyReleased(int key)
{
  keyDown = 0;
  keyCode = key;
}

int Viewer::isKeyDown()
{
  return keyDown;
}

int Viewer::getKey()
{
  return keyCode;
}

double Viewer::getCenterLongitude()
{
  return centerLongitude;
}

void Viewer::setCenterLongitude(double longitude)
{
  centerLongitude = longitude;
}

double Viewer::getCenterLatitude()
{
  return centerLatitude;
}

void Viewer::setCenterLatitude(double latitude)
{
  centerLatitude = latitude;
}

double Viewer::getViewerWidth()
{
  return viewerWidth;
}

double Viewer::getViewerHeight()
{
  return viewerWidth*viewerRatio;
}

double Viewer::getViewerRatio()
{
  return viewerRatio;
}

double Viewer::getLeftLongitude()
{
  return centerLongitude - viewerWidth;
}

double Viewer::getRightLongitude()
{
  return centerLongitude + viewerWidth;
}
 
double Viewer::getTopLatitude()
{
  return centerLatitude + viewerWidth*viewerRatio;
}

double Viewer::getBottomLatitude()
{
  return centerLatitude - viewerWidth*viewerRatio;
}

double Viewer::getAngleX()
{
  return angleX;
}

double Viewer::getAngleY()
{
  return angleY;
}

double Viewer::getAngleZ()
{
  return angleZ;
}

double Viewer::getViewHeight()
{
  return viewHeight;
}

double Viewer::getViewAngle()
{
  return viewAngle;
}

Viewer::~Viewer()
{
  ;
}

Viewer::Viewer()
{  
  windowWidth = D_WIDTH;
  windowHeight = D_HEIGHT;
  windowBpp = D_BPP;
  
  mouseDown = 0;
  mouseSelected = 0;
  
  keyDown = 0;
  viewerMode = PARALLEL;
  
  centerLongitude = DEFAULT_CENTER_LONGITUDE;
  centerLatitude = DEFAULT_CENTER_LATITUDE;
  viewerWidth = DEFAULT_WIDTH;
  viewerRatio = DEFAULT_RATIO;
  
  angleX = 0;
  angleY = 0;
  angleZ = 0;

  viewHeight = DEFAULT_VIEW_HEIGHT;
  viewAngle = DEFAULT_VIEW_ANGLE;
}

int Viewer::runOnceKeyFunction()
{
  switch(keyCode)
    {
    case SDLK_SPACE :
      toggleViewerMode();
      break;
      
    case SDLK_q :
      viewerWidth = DEFAULT_WIDTH;
      viewerRatio = DEFAULT_RATIO;
      angleX = 0;
      angleY = 0;
      angleZ = 0;
      viewHeight = DEFAULT_VIEW_HEIGHT;
      viewAngle = DEFAULT_VIEW_ANGLE;
      break;
      
    case SDLK_ESCAPE :
      return -1;
      break;
    default:
      break;
    }
  return 0;
}

void Viewer::pressedKeyFunction()
{
  switch (keyCode) {
  case SDLK_f :
    angleX -= DIFF_ANGLE;
    break;
  case SDLK_v :
    angleX += DIFF_ANGLE;
    break;
  case SDLK_d :
    angleY -= DIFF_ANGLE;
    break;
  case SDLK_c :
    angleY += DIFF_ANGLE;
    break;
  case SDLK_z :
    angleZ -= DIFF_ANGLE;
    break;
  case SDLK_x :
    angleZ += DIFF_ANGLE;
    break;

  case SDLK_o :
    if(viewAngle-DIFF_ANGLE > 0)
      viewAngle -= DIFF_ANGLE;
    break;
  case SDLK_p :
    if(viewAngle+DIFF_ANGLE < 180)
      viewAngle += DIFF_ANGLE;
    break;
    
  case SDLK_LEFT :
    centerLongitude -= cos(2*M_PI*angleZ/360)*viewerWidth*DIFF_PERCENT/100;
    centerLatitude += sin(2*M_PI*angleZ/360)*viewerWidth*viewerRatio*DIFF_PERCENT/100;
    break;
  case SDLK_RIGHT :
    centerLongitude += cos(2*M_PI*angleZ/360)*viewerWidth*DIFF_PERCENT/100;
    centerLatitude -= sin(2*M_PI*angleZ/360)*viewerWidth*viewerRatio*DIFF_PERCENT/100;
    break;
  case SDLK_UP :
    centerLatitude += cos(2*M_PI*angleZ/360)*viewerWidth*viewerRatio*DIFF_PERCENT/100;
    centerLongitude += sin(2*M_PI*angleZ/360)*viewerWidth*DIFF_PERCENT/100;
    break;
  case SDLK_DOWN :
    centerLatitude -= cos(2*M_PI*angleZ/360)*viewerWidth*viewerRatio*DIFF_PERCENT/100;
    centerLongitude -= sin(2*M_PI*angleZ/360)*viewerWidth*DIFF_PERCENT/100;
    break;
    
  case SDLK_PAGEDOWN :
    scaleDown(1);
    break;
  case SDLK_PAGEUP :
    scaleUp(1);
    break;
    
  default:
    break;
  }
  return;
}

void Viewer::scaleDown(int i)
{
  for(int j = 0;j < i;j ++)
    {
      viewerWidth *= DIFF_RATIO;
      viewHeight += viewHeight*DIFF_PERCENT/100;
    }
}

void Viewer::scaleUp(int i)
{
  for(int j = 0;j < i;j ++)
    {
      viewerWidth /= DIFF_RATIO;
      viewHeight -= viewHeight*DIFF_PERCENT/100;
    }
}
