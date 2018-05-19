#include <X11/Xlib.h>
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
#include "Viewer.h"
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

TextureImage *texture;
TextureSDL_ttf * sdl_ttf, *sdl_ttf2;
GISFile *gisFile, *gisFile2, *gisFile3;
SalFile *salFile, *salFile2, *salFile3, *salFile4,
  *salFile5, *salFile6;

SalFile ** salList;

Mesh50M me1;

Viewer viewer;

SDL_mutex * global_screen_lock;

int salListSize = 0;

int displayList1=0, displayList2=0, displayList3=0, displayList4=0, displayList5=0;
int global_thread_data1 = 0, global_thread_data2 = 0;

void reset()
{
  displayList1 = 0;
  displayList2 = 0;
  displayList3 = 0;
  displayList4 = 0;
  displayList5 = 0;
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
  glOrtho(0, viewer.getWindowWidth(), viewer.getWindowHeight(), 0, -2.0, 2.0);
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
  //TextureInfo * tex;
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
  if(viewer.getViewerMode() == PARALLEL)
    {
      glOrtho(-viewer.getViewerWidth(),
	      viewer.getViewerWidth(),
	      -viewer.getViewerWidth()*viewer.getViewerRatio(),
	      viewer.getViewerWidth()*viewer.getViewerRatio(),
	      -4, 4);
    }
  else // viewerMode == PERSPECTIVE
    {
      gluPerspective(120.0, viewer.getViewerRatio(), 0.0, 10.0);
    }
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if(viewer.getViewerMode() == PARALLEL)
    {
      glRotated(viewer.getAngleX(),0.01,0,0);
      glRotated(viewer.getAngleY(),0,0.01,0);
      glRotated(viewer.getAngleZ(),0,0,0.01);
      glTranslated(-viewer.getCenterLongitude(), -viewer.getCenterLatitude(), 0);
    }
  else // viewerMode == PERSPECTIVE
    {
      glRotated(viewer.getAngleX(),1,0,0);
      glRotated(viewer.getAngleY(),0,1,0);
      glRotated(viewer.getAngleZ(),0,0,1);
      glTranslated(0, 0, -100);
      glTranslated(-viewer.getCenterLongitude(), -viewer.getCenterLatitude(), 0);
    }
  
  // __

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
  glOrtho(0, viewer.getWindowWidth(), viewer.getWindowHeight(), 0, -2.0, 2.0);
  
  /* print mouse selection(START) */
  if(viewer.isMouseSelected() == 1)
    {
      glBegin(GL_QUADS);
      glColor4d(.2, .7, .9, 0.6);
      glVertex3d(viewer.mouseSelectionStartX(),   viewer.mouseSelectionStartY(),   .5);
      glVertex3d(viewer.mouseSelectionStartX(),   viewer.mouseSelectionCurrentY(), .5);
      glVertex3d(viewer.mouseSelectionCurrentX(), viewer.mouseSelectionCurrentY(), .5);
      glVertex3d(viewer.mouseSelectionCurrentX(), viewer.mouseSelectionStartY(),   .5);
      glEnd();
    }
  /* (END) */

  
  /* print font */

  char fff[1024];
  sprintf(fff,"%d,%d",viewer.mouseSelectionCurrentX(), viewer.mouseSelectionCurrentY());

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
  if(viewer.getViewerMode() == PERSPECTIVE)
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

int sdl_event()
{
  SDL_Event event;
  SDL_WaitEvent(&event);
  switch(event.type)
    {
    case SDL_KEYDOWN:
      viewer.keyPressed(event.key.keysym.sym);
      if(viewer.runOnceKeyFunction() != 0)
	return -1;
      break; 
    case SDL_KEYUP:
      viewer.keyReleased(event.key.keysym.sym);
      break;      
    case SDL_MOUSEMOTION:
      viewer.mouseMotion(event.motion.x, event.motion.y);
      break;
    case SDL_MOUSEBUTTONDOWN:
      viewer.mousePressed(event.button.button, event.button.x, event.button.y);
      break;
    case SDL_MOUSEBUTTONUP:
      viewer.mouseReleased(event.button.button, event.button.x, event.button.y);
      break;
    case SDL_VIDEORESIZE:
      viewer.registerResize(event.resize.w, event.resize.h, 0);
      break;
    case SDL_QUIT:
      return -1;
      break;
    }
  return 0;
}

int thread_keyboard(void *data)
{
  while(global_thread_data1 == 0)
    {
      SDL_Delay(10);
      if(viewer.isKeyDown() == 1)
	{
	  viewer.pressedKeyFunction();
	}
    }
  return 0;
}

int thread_sdl_event(void *data)
{
  while(global_thread_data2 == 0)
    {
      if(sdl_event() != 0)
	break;
    }
  global_thread_data2 = 1;
  global_thread_data1 = 1;
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
  
  if(viewer.sdl_init_video() != 0)
    {
      SDL_Quit();
      exit(2);
    }
  
  if(viewer.resize(D_WIDTH, D_HEIGHT, SDL_GetVideoInfo()->vfmt->BitsPerPixel) != 0)
    {
      SDL_Quit();
      exit(2);
    }
  SDL_WM_SetCaption("GISfx", "gisfx");

  texture = new TextureImage();
  sdl_ttf = new TextureSDL_ttf("/usr/X11R6/lib/X11/fonts/TTF/VeraBd.ttf", 50);
  sdl_ttf2 = new TextureSDL_ttf("/usr/X11R6/lib/X11/fonts/truetype/kochi-gothic.ttf", 120);
  //gisFile = new GISFile(argv[1]);
  //gisFile2 = new GISFile(argv[2]);
  //gisFile3 = new GISFile(argv[3]);
  
  splash("Loading...");
  
  salList = new SalFile*[argc];
  salListSize = argc;
  for (int i = 1; i < argc;i ++)
    {
      fprintf(stderr, "sal[%d]: %s\n", i, argv[i]);
      salList[i-1] = new SalFile(argv[i]);
      viewer.setCenterLongitude(salList[0]->point_start_x/10000 + salList[0]->point_range_x/10000/2);
      viewer.setCenterLatitude (salList[0]->point_start_y/10000 + salList[0]->point_range_y/10000/2);
      fprintf(stderr, "center(auto):%f,%f.\n", viewer.getCenterLongitude(), viewer.getCenterLatitude());
    }
  
  SDL_Thread *thread = SDL_CreateThread(thread_keyboard, NULL);
  if(thread == NULL)
    {
      fprintf(stderr, "Unable to create thread_keyboard: %s\n", SDL_GetError());
      exit(2);
    }
    
  SDL_Thread *thread2 = SDL_CreateThread(thread_sdl_event, NULL);
  if(thread2 == NULL)
    {
      fprintf(stderr, "Unable to create thread_sdl_event: %s\n", SDL_GetError());
      exit(2);
    }
  
  while(1)
    {
      if(viewer.isResizeRegistered() != 0)
	{
	  viewer.resize(0,0,0);
	}
      //display();
      if(global_thread_data2 != 0)
	{
	  SDL_WaitThread(thread2, &status);
	  break;
	}
    }
  
  SDL_WaitThread(thread, &status);

  SDL_Quit();
  return 0;
}
