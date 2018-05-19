#include <GL/glut.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "TextureImage.h"
#include "TextureSDL_ttf.h"
#include "GISFile.h"
#include "SalFile.h"
#include "Mesh50M.h"
#include <string>
#include <string.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mutex.h>
#include <libxml/tree.h>

#include "libarc/arc.h"

extern struct URL_module URL_module_file;
extern struct URL_module URL_module_dir;
extern struct URL_module URL_module_http;
extern struct URL_module URL_module_ftp;
extern struct URL_module URL_module_news;
extern struct URL_module URL_module_newsgroup;

#define PI 3.141592654
#define TIMER 10
#define STACK 20

#define MAX(x,y) (x>y?x:y)
#define MIN(x,y) (x<y?x:y)

double W = 200.0;
double H = 200.0;

static double x_ = 0, y_ = 0, z_ = 0;

const SDL_VideoInfo* sdlVideoInfo = NULL;
SDL_Surface *screen;
int resizeDisplay = 0;

#define D_WIDTH 640
#define D_HEIGHT 480
// mouse selection
static int defaultWidth = D_WIDTH;
static int defaultHeight = D_HEIGHT;
static int defaultBpp = 32;
int newWidth;
int newHeight;
int currentWidth;
int currentHeight;

int keyboardDOWN = 0;
int keyboardKEY = 0;

int mouseDOWN = 0;
int selected = 0;
int origx = 0, origy = 0, curx = 0, cury = 0;

// gis internal size
//#define DEFAULT_LB 20
//#define DEFAULT_LT 50
//#define DEFAULT_LL 120
//#define DEFAULT_LR 160

// new viewer <2 modes>
// 1. Parallel projection (glOrtho)
// 2. Perspective projection (gluPerspective)
enum ViewerMode{PARALLEL, PERSPECTIVE};
ViewerMode viewerMode = PARALLEL;

//([seconds])
#define DEFAULT_CENTER_LONGITUDE 503500
#define DEFAULT_CENTER_LATITUDE 128500
#define DEFAULT_WIDTH 250
#define DEFAULT_RATIO 1

#define DIFF_RATIO 1.005
#define DIFF_PERCENT 0.5
#define DIFF_ANGLE 1

double center_longitude = DEFAULT_CENTER_LONGITUDE;
double center_latitude = DEFAULT_CENTER_LATITUDE;
double viewer_width = DEFAULT_WIDTH;
double viewer_ratio = DEFAULT_RATIO;

double angle_x = 0;
double angle_y = 0;
double angle_z = 0;

TextureImage *texture;
TextureSDL_ttf * sdl_ttf, *sdl_ttf2;
GISFile *gisFile, *gisFile2, *gisFile3;
SalFile *salFile, *salFile2, *salFile3, *salFile4,
  *salFile5, *salFile6;

SalFile ** salList;

Mesh50M me1;

int salListSize = 0;

int displayList1=0, displayList2=0, displayList3=0,
  displayList4=0, displayList5=0;
int global_thread_data = 0;
int global_thread_data2 = 0;
int quitDisplay = 0;

void reset()
{
  displayList1 = 0;
  displayList2 = 0;
  displayList3 = 0;
  displayList4 = 0;
  displayList5 = 0;
}

void stroke_output(char *format, ...)
{
  va_list args;
  char buffer[200], *p;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  for (p = buffer; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
}

void resize(int w, int h)
{
  currentWidth = w;
  currentHeight = h;
  glViewport(0, 0, w, h);
  return;
}

void disMESH(SalFile * sal)
{
  double hyoukou;
  for(xmlNode * node = sal->doc->children->children;node->next != NULL;node = node->next)
    {
      if(strcmp((char *)node->name, "MH") == 0)
	{
	  for(xmlNode * tf = node->children;tf != NULL;tf = tf->next)
	    {
	      if(strcmp((char *)tf->name, "HK") == 0)
		{
		  // hyoukou
		  sscanf((char *)tf->children->content, "%lf", &hyoukou);
		}
	      if(strcmp((char *)tf->name, "PT") == 0)
		{
		  int line;
		  double x, y;
		  sscanf((char *)tf->children->content, "%06d", &line);
		  y = ((double)sal->mhslp_[line-1].latitude) / 10000.0;
		  x = ((double)sal->mhslp_[line-1].longitude) / 10000.0;

		  me1.add_point(hyoukou, x, y);
		}
	    }
	}
    }
  
}

void disSAL(SalFile * sal)
{
  glLineWidth(3);
  glBegin(GL_LINE_LOOP);
  glColor3d(0, 0, 1);
  glVertex2i(sal->point_start_x/10000,
	     sal->point_start_y/10000);

  glColor3d(1, 1, 1);
  glVertex2i(sal->point_start_x/10000,
	     sal->point_start_y/10000 + sal->point_range_y/10000);

  glColor3d(1, 1, 1);
  glVertex2i(sal->point_start_x/10000 + sal->point_range_x/10000,
	     sal->point_start_y/10000 + sal->point_range_y/10000);
  
  glColor3d(1, 1, 1);
  glVertex2i(sal->point_start_x/10000 + sal->point_range_x/10000,
	     sal->point_start_y/10000);
  glEnd();
  glLineWidth(1);
  
  for(xmlNode * node = sal->doc->children->children;node->next != NULL;node = node->next)
    {
      if(strcmp((char *)node->name, "DK") == 0)
	{
	  for(xmlNode * tf = node->children->next;tf != NULL;tf = tf->next)
	    {
	      if(strcmp((char *)tf->name, "CV") == 0)
		{
		  // XXXXXX,XXXXXX...
		  char * cur = (char *)tf->children->content;
		  int times = (strlen((char *)tf->children->content) + 1)/7;
		  int line;
		  double x, y;
		  glBegin(GL_LINE_STRIP);
		  for(int i = 0;i < times;i ++)
			{
			  sscanf(cur, "%06d", &line);
			  cur += 7;
			  
			  y = ((double)sal->slp_[line-1].latitude) / 10000.0;
			  x = ((double)sal->slp_[line-1].longitude) / 10000.0;
			  
			  glColor3d(0.7, 0.7, 1.0);
			  glVertex3d(x, y, 0);
			}
		  glEnd();  
		}
	    }
	}
    }
  
  for(xmlNode * node = sal->doc->children->children;node->next != NULL;node = node->next)
    {
      if(strcmp((char *)node->name, "TK") == 0)
	{
	  for(xmlNode * tf = node->children->next;tf != NULL;tf = tf->next)
	    {
	      if(strcmp((char *)tf->name, "CV") == 0)
		{
		  // XXXXXX,XXXXXX...
		  char * cur = (char *)tf->children->content;
		  int times = (strlen((char *)tf->children->content) + 1)/7;
		  int line;
		  double x, y;
		  glBegin(GL_LINE_STRIP);
		  for(int i = 0;i < times;i ++)
		    {
		      sscanf(cur, "%06d", &line);
		      cur += 7;
		      
		      y = ((double)sal->slp_[line-1].latitude) / 10000.0;
		      x = ((double)sal->slp_[line-1].longitude) / 10000.0;
		      
		      //fprintf(stderr, "%f, %f\n", x, y);
		      glColor3d(1.0, 1.0, .3);
		      glVertex3d(x, y, 0);
		    }
		  glEnd();  
		}
	    }
	}
    }
  
}

void splash(char * str)
{
  FontTextureInfo *font;

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_LINE_SMOOTH);  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, currentWidth, currentHeight, 0, -2.0, 2.0);
  SDL_Color color_ = {255, 255, 255, 0};
  glEnable(GL_TEXTURE_2D);
  glColor3d(1, 1, 1);
  if((font = sdl_ttf->getFontTexture(str, color_)) == NULL)
    exit(-1);
  glBindTexture(GL_TEXTURE_2D, font->textureID);
  glBegin(GL_POLYGON);
  glTexCoord2f(0, 0);
  glVertex3f(0, 0, 1);
  glTexCoord2f(1, 0);
  glVertex3f(0+font->width, 0, 1);
  glTexCoord2f(1, 1);
  glVertex3f(0+font->width, 0+font->height, 1);
  glTexCoord2f(0, 1);
  glVertex3f(0, 0+font->height, 1);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  SDL_GL_SwapBuffers();
}

void display(void)
{
  TextureInfo * tex;
  FontTextureInfo *font, *font2;

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_LINE_SMOOTH);  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  // Models

  glPushMatrix();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if(viewerMode == PARALLEL)
    {
      glOrtho(-viewer_width, viewer_width,
	      -viewer_width*viewer_ratio, viewer_width*viewer_ratio,
	      -4, 4);
    }
  else // viewerMode == PERSPECTIVE
    {
      gluPerspective(120.0, viewer_ratio, 0.0, 10.0);
    }
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if(viewerMode == PARALLEL)
    {
      glRotated(angle_x,0.01,0,0);
      glRotated(angle_y,0,0.01,0);
      glRotated(angle_z,0,0,0.01);
      glTranslated(-center_longitude, -center_latitude, 0);
    }
  else // viewerMode == PERSPECTIVE
    {
      glRotated(angle_x,1,0,0);
      glRotated(angle_y,0,1,0);
      glRotated(angle_z,0,0,1);
      glTranslated(0, 0, -100);
      glTranslated(-center_longitude, -center_latitude, 0);
    }
  
  /*  
  if(displayList1 != 1)
    {
      glNewList(1, GL_COMPILE);
      for(std::vector<ARCPolygon>::iterator text = gisFile->vARC.begin();
	  text != gisFile->vARC.end();
	  text ++)
	{
	  int i;
	  glBegin(GL_LINE_STRIP);
	  for(i = 0;i < text->vVertexX.size();i ++)
	    {
	      glColor3d(0.5, 0.5, 0.5);
	      glVertex2d(text->vVertexX.at(i), text->vVertexY.at(i));
	    }
	  glEnd();  
	}
      glEndList();
      displayList1 = 1;
    }
  else
    {
      glCallList(1);
    }

  if(displayList2 != 1)
    {
      glNewList(2, GL_COMPILE);
      for(std::vector<ARCPolygon>::iterator text = gisFile2->vARC.begin();
	  text != gisFile2->vARC.end();
	  text ++)
	{
	  int i;
	  glBegin(GL_LINE_STRIP);
	  for(i = 0;i < text->vVertexX.size();i ++)
	    {
	      glColor3d(0.3, 0.5, 1.0);
	      glVertex2d(text->vVertexX.at(i), text->vVertexY.at(i));
	    }
	  glEnd();  
	}
      glEndList();
      displayList2 = 1;
    }
  else
    {
      glCallList(2);
    }
  
  if(displayList3 != 1)
    {
      glNewList(3, GL_COMPILE);
      for(std::vector<ARCPolygon>::iterator text = gisFile3->vARC.begin();
	  text != gisFile3->vARC.end();
	  text ++)
	{
	  int i;
	  glBegin(GL_LINE_STRIP);
	  for(i = 0;i < text->vVertexX.size();i ++)
	    {
	      glColor3d(1.0, 0.5, 0.0);
	      glVertex2d(text->vVertexX.at(i), text->vVertexY.at(i));
	    }
	  glEnd();  
	}
      glEndList();
      displayList3 = 1;
    }
  else
    {
      glCallList(3);
    }
  */
  
  if(displayList4 != 1)
    {
      // /SAL/DK/CV{}
      glNewList(4, GL_COMPILE);
      for(int i = 0;i < salListSize-1;i ++)
	{
	  fprintf(stderr, "sal[%d]", i);
	  glLineWidth(1);
	  disSAL(salList[i]);
	  //glLineWidth(1);
	  disMESH(salList[i]);
	}

      
      #define D_ 1
      // ++
      std::map<std::pair<double,double>, Mesh50M_E>::iterator it = me1.meshes.begin();
      while(it != me1.meshes.end())
	{
	  double ** dvv = it->second.height;
	  double dvx = it->second.base_longitude;
	  double dvy = it->second.base_latitude;
	  
	  for(int ax = 0;ax < size_longitude;ax += D_)
	    for(int ay = 0;ay < size_latitude;ay += D_)
	      {
		  glBegin(GL_LINE_LOOP);
		  glColor3d(1, 1, 1);
		  glVertex3d(dvx+ax*mesh_longitude,     dvy+ay*mesh_latitude,     dvv[ax][ay]/1);
		  glVertex3d(dvx+(ax+D_)*mesh_longitude, dvy+ay*mesh_latitude,     dvv[ax+D_][ay]/1);
		  glVertex3d(dvx+(ax+D_)*mesh_longitude, dvy+(ay+D_)*mesh_latitude, dvv[ax+D_][ay+D_]/1);
		  glVertex3d(dvx+ax*mesh_longitude,     dvy+(ay+D_)*mesh_latitude, dvv[ax][ay+D_]/1);
		  glEnd();
	      }
	  ++ it;
	}
	
      //++

      glEndList();
      displayList4 = 1;
    }
  else
    {
      glCallList(4);
    }
    
  glPopMatrix();
  
  // ==== UI start ====
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, currentWidth, currentHeight, 0, -2.0, 2.0);
  
  /* print mouse selection(START) */
  if(mouseDOWN == 1||selected == 1)
    {
      glBegin(GL_QUADS);
      glColor4d(.2, .7, .9, 0.6);
      glVertex3d(curx, cury, .5);
      glVertex3d(curx, origy, .5);
      glVertex3d(origx, origy, .5);
      glVertex3d(origx, cury, .5);
      glEnd();
    }
  /* (END) */

  
  /* print font */

  char fff[1024];
  sprintf(fff,"%d,%d", curx, cury);

  SDL_Color color_ = {255, 255, 255, 0};
  glEnable(GL_TEXTURE_2D);
  glColor3d(1, 1, 1);
  if((font = sdl_ttf->getFontTexture(fff, color_)) == NULL)
    exit(-1);
  glBindTexture(GL_TEXTURE_2D, font->textureID);
  glBegin(GL_POLYGON);
  glTexCoord2f(0, 0);
  glVertex3f(0, 0, 1);
  glTexCoord2f(1, 0);
  glVertex3f(0+font->width, 0, 1);
  glTexCoord2f(1, 1);
  glVertex3f(0+font->width, 0+font->height, 1);
  glTexCoord2f(0, 1);
  glVertex3f(0, 0+font->height, 1);
  glEnd();
  glDisable(GL_TEXTURE_2D);
    
  char PERS[] = "Æ©»ë", PARA[] = "Ê¿¹Ô";
  if(viewerMode == PERSPECTIVE)
    sprintf(fff,"<%s>", PERS);
  else
    sprintf(fff,"<%s>", PARA);

  SDL_Color color2_ = {0, 205, 155, 0};
  glEnable(GL_TEXTURE_2D);
  glColor3d(1, 1, 1);
  if((font2 = sdl_ttf2->getFontTextureEUCJP(fff, color2_)) == NULL)
    {
      exit(-1);
    }
  glBindTexture(GL_TEXTURE_2D, font2->textureID);
  glBegin(GL_POLYGON);
  glTexCoord2f(0, 0);
  glVertex3f(0, 60, 1);
  glTexCoord2f(1, 0);
  glVertex3f(0+font2->width/2, 60, 1);
  glTexCoord2f(1, 1);
  glVertex3f(0+font2->width/2, font2->height/2+60, 1);
  glTexCoord2f(0, 1);
  glVertex3f(0, font2->height/2+60, 1);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  
  /*
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, currentWidth, currentHeight, 0, -2.0, 2.0);
    glColor4d(1, 1, 1, 0);
    glEnable(GL_TEXTURE_2D);
    if((tex = texture->getTexture("toka_after.png")) == NULL)
    exit(-1);
    glBindTexture(GL_TEXTURE_2D, tex->textureID);
    glBegin(GL_POLYGON);
    glTexCoord2f(0, 1);
    glVertex3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(0, tex->height, 1);
    glTexCoord2f(1, 0);
    glVertex3f(tex->width, tex->height, 1);
    glTexCoord2f(1, 1);
    glVertex3f(tex->width, 0, 1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
  */

  glPopMatrix();
  // ==== UI end ====

  SDL_GL_SwapBuffers();
}

void mouse(int type, int key, int x, int y)
{
  switch(key)
    {
      // drag selection
    case 1:
      if(type == SDL_MOUSEBUTTONDOWN)
	{
	  mouseDOWN = 1;
	  origx = x; origy = y;
	}
      if(type == SDL_MOUSEBUTTONUP)
	{
	  mouseDOWN = 0;
	  if(curx != origx||cury != origy)
	    {
	      selected = 1;
	    }
	  else
	    {
	      selected = 0;
	    }
	}
      break;
    }
  curx = x; cury = y;

  return;
}

void mousemotion(int x, int y)
{
  if(mouseDOWN == 1)
    {
      curx = x; cury = y;
    }
  return;
}

void keyboard(int type, int key)
{
  switch (key) {
  case SDLK_f :
    angle_x -= DIFF_ANGLE;
    break;
  case SDLK_v :
    angle_x += DIFF_ANGLE;
    break;
  case SDLK_d :
    angle_y -= DIFF_ANGLE;
    break;
  case SDLK_c :
    angle_y += DIFF_ANGLE;
    break;
  case SDLK_z :
    angle_z -= DIFF_ANGLE;
    break;
  case SDLK_x :
    angle_z += DIFF_ANGLE;
    break;
    
  case SDLK_LEFT :
    center_longitude -= viewer_width*DIFF_PERCENT/100;
    break;
  case SDLK_RIGHT :
    center_longitude += viewer_width*DIFF_PERCENT/100;
    break;
  case SDLK_DOWN :
    center_latitude -= viewer_width*viewer_ratio*DIFF_PERCENT/100;
    break;
  case SDLK_UP :
    center_latitude += viewer_width*viewer_ratio*DIFF_PERCENT/100;
    break;
  case SDLK_PAGEDOWN :
    viewer_width *= DIFF_RATIO;
    break;
  case SDLK_PAGEUP :
    viewer_width /= DIFF_RATIO;
    break;
    
  default:
    break;
  }
  return;
}

int sdl_event()
{
  SDL_Event event;
  SDL_WaitEvent(&event);
  switch(event.type)
    {
    case SDL_KEYDOWN:
      keyboardDOWN = 1;
      fprintf(stderr, "[%s](%d)\n", SDL_GetKeyName(event.key.keysym.sym), event.type);
      keyboardKEY = event.key.keysym.sym;
      
      switch(event.key.keysym.sym)
	{
	case SDLK_SPACE :
	  if (viewerMode == PERSPECTIVE)
	    {
	      fprintf(stderr, "-->PARALLEL\n");
	      viewerMode = PARALLEL;
	    }
	  else // viewerMode == PARALLEL
	    {
	      fprintf(stderr, "-->PERSPECTIVE\n");
	      viewerMode = PERSPECTIVE;
	    }
	  break;
	  
	case SDLK_q :
	  //center_longitude = DEFAULT_CENTER_LONGITUDE;
	  //center_latitude = DEFAULT_CENTER_LATITUDE;
	  viewer_width = DEFAULT_WIDTH;
	  viewer_ratio = DEFAULT_RATIO;
	  angle_x = 0;
	  angle_y = 0;
	  angle_z = 0;
	  break;
	  
	case SDLK_ESCAPE :
	  return -1;
	  break;
	default:
	  break;
	}
      break;
      
    case SDL_KEYUP:
      keyboardDOWN  = 0;
      fprintf(stderr, "[%s](%d)\n", SDL_GetKeyName(event.key.keysym.sym), event.type);
      //keyboard(event.type, event.key.keysym.sym);
      break;
      
    case SDL_MOUSEMOTION:
      /* fprintf(stderr, "Mouse moved by %d,%d to (%d,%d)\n", 
	 event.motion.xrel, event.motion.yrel,
	 event.motion.x, event.motion.y); */
      mousemotion(event.motion.x, event.motion.y);
      break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      /* fprintf(stderr, "Mouse button %d () at (%d,%d)\n",
	 event.button.button, event.type, event.button.x, event.button.y); */
      mouse(event.type, event.button.button, event.button.x, event.button.y);
      break;
      
    case SDL_VIDEORESIZE:
      fprintf(stderr, "Resize (%d,%d)\n",
	      event.resize.w, event.resize.h);
      newWidth = event.resize.w;
      newHeight = event.resize.h;
      SDL_SetVideoMode(newWidth, newHeight,
		       defaultBpp, SDL_OPENGL|SDL_RESIZABLE);
      resizeDisplay = 1;
      break;
      
    case SDL_QUIT:
      break;
    }
  return 0;
}

const double co = 0.1;
int thread_func(void *data)
{
  while(global_thread_data != -1)
    {
      SDL_Delay(10);
      if(keyboardDOWN == 1)
	{
	  keyboard(0, keyboardKEY);
	}
    }
  return 0;
}

int thread_func2(void *data)
{
  while(global_thread_data2 != -1)
    {
      if(sdl_event() != 0)
	break;
    }
  quitDisplay = 1;
  return 0;
}

int main(int argc, char *argv[])
{  
  url_add_modules(&URL_module_file,
                  &URL_module_dir,
                  &URL_module_http,
                  &URL_module_ftp,
                  &URL_module_news,
                  &URL_module_newsgroup,
                  NULL);

  int status;

  putenv("SDL_VIDEO_WINDOW_POS");
  putenv("SDL_VIDEO_CENTERED=1");

  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      fprintf( stderr, "SDL_Init(SDL_INIT_VIDEO): %s\n", SDL_GetError());
      SDL_Quit();
      exit(2);
    }
  
  sdlVideoInfo = SDL_GetVideoInfo();
  defaultBpp = sdlVideoInfo->vfmt->BitsPerPixel;
  screen = SDL_SetVideoMode(defaultWidth, defaultHeight,
			    defaultBpp, SDL_OPENGL|SDL_RESIZABLE);
  if(!screen)
    {
      fprintf(stderr, "SDL_SetVideoMode(): %s\n", SDL_GetError());
      SDL_Quit();
      exit(2);
    }
  
  SDL_WM_SetCaption("GISfx", "gisfx");
  resize(defaultWidth, defaultHeight);

  texture = new TextureImage();
  sdl_ttf = new TextureSDL_ttf("/usr/X11R6/lib/X11/fonts/TTF/VeraBd.ttf", 50);
  sdl_ttf2 = new TextureSDL_ttf("/usr/X11R6/lib/X11/fonts/truetype/kochi-gothic.ttf", 120);
  //gisFile = new GISFile(argv[1]);
  //gisFile2 = new GISFile(argv[2]);
  //gisFile3 = new GISFile(argv[3]);
  /*
    salFile = new SalFile(argv[1]);
    salFile2 = new SalFile(argv[2]);
    salFile3 = new SalFile(argv[4]);
    salFile4 = new SalFile(argv[3]);
    salFile5 = new SalFile(argv[5]);
    salFile6 = new SalFile(argv[6]);
  */
  
  splash("Loading...");
  
  salList = new SalFile*[argc];
  salListSize = argc;
  for (int i = 1; i < argc;i ++)
    {
      fprintf(stderr, "sal[%d]: %s\n", i, argv[i]);
      salList[i-1] = new SalFile(argv[i]);
      center_longitude = salList[0]->point_start_x/10000 + salList[0]->point_range_x/10000/2;
      center_latitude = salList[0]->point_start_y/10000 + salList[0]->point_range_y/10000/2;
      fprintf(stderr, "center(auto):%f,%f.\n", center_longitude, center_latitude);
    }
  
  SDL_Thread *thread;
  
  thread = SDL_CreateThread(thread_func, NULL);
  if(thread == NULL)
    {
      fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
      exit(2);
    }
    
  SDL_Thread *thread2;
  
  thread = SDL_CreateThread(thread_func2, NULL);
  if(thread2 == NULL)
    {
      fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
      exit(2);
    }
  
  while(1)
    {
      //fprintf(stderr,".");
      display();
      if(quitDisplay == 1)
	{
	  //SDL_WaitThread(thread2, &status);
	  break;
	}
      if(resizeDisplay == 1)
	{
	  resize(newWidth, newHeight);
	  resizeDisplay = 0;
	}
    }
  
  global_thread_data = -1;
  //SDL_WaitThread(thread, &status);
  SDL_Quit();
  return 0;
}
