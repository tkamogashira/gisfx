#include <X11/Xlib.h>
#include <GL/glut.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <string.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mutex.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "libarc/arc.h"

#include "TextureImage.h"
#include "TextureSDL_ttf.h"
#include "GISFile.h"
#include "SalFile.h"
#include "Mesh50M.h"
#include "Viewer.h"
#include "SLMset.h"

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

std::vector<SalFile *> salPtrV;
std::map<std::strin,SalFile *> salPtrMap;

static unsigned int previousSalPtrVSize = 0;
static unsigned int loadedSalPtrVSize = 0;

Mesh50M me1;
Viewer viewer;
SLMset * slmset;
DisplayList displayList;

int displayList1=-1, displayList2=-1, displayList3=-1, displayList4=-1, displayList5=-1;
int global_thread_data1 = 0, global_thread_data2 = 0;

xmlXPathObjectPtr xml_GetNodeSet (xmlDocPtr doc, xmlChar *xpath)
{        
  xmlXPathContextPtr context;
  xmlXPathObjectPtr result;

  context = xmlXPathNewContext(doc);
  if (context == NULL)
    {
      fprintf(stderr, "xmlXPathNewContext failed.\n");
      return NULL;
    }
  result = xmlXPathEvalExpression(xpath, context);
  xmlXPathFreeContext(context);
  if (result == NULL)
    {
      fprintf(stderr, "xmlXPathEvalExpression failed.\n");
      return NULL;
    }
  if(xmlXPathNodeSetIsEmpty(result->nodesetval))
    {
      xmlXPathFreeObject(result);
      fprintf(stderr, "No result.\n");
      return NULL;
    }
  return result;
}

xmlNodePtr xml_GetNamedFirstChild(xmlNodePtr node, xmlChar * childName)
{
  for(xmlNodePtr tf = node->children;tf != NULL;tf = tf->next)
    {
      if(strcmp((char *)tf->name, (char *)childName) == 0)
	{
	  return tf;
	}
    }
  return NULL;
}

void disMESH(SalFile * sal)
{
  xmlChar *xpath;
  xmlNodeSetPtr nodeset;
  xmlXPathObjectPtr result;
  //==
  xpath = (xmlChar*) "//MH";
  result = xml_GetNodeSet(sal->doc, xpath);
  if(result)
    {
      nodeset = result->nodesetval;
      // nodeNr	: number of nodes in the set
      for (int i = 0; i < nodeset->nodeNr; i++)
	{
	  //<MH><HK></HK><PT></PT></MH>
	  xmlNodePtr HK = xml_GetNamedFirstChild(nodeset->nodeTab[i], (xmlChar *)"HK");
	  xmlNodePtr PT = xml_GetNamedFirstChild(nodeset->nodeTab[i], (xmlChar *)"PT");
	  if(HK != NULL && PT != NULL)
	    {
	      double hyoukou, x, y;
	      int line;
	      // xmlNodeGetContent(): xmlFree()必要かよくわからない
	      sscanf((char *)xmlNodeGetContent(HK), "%lf", &hyoukou);
	      sscanf((char *)xmlNodeGetContent(PT), "%06d", &line);
	      y = ((double)sal->mhslp_[line-1].latitude) / 10000.0;
	      x = ((double)sal->mhslp_[line-1].longitude) / 10000.0;
	      me1.add_point(hyoukou, x, y);
	    }
	}
      xmlXPathFreeObject (result);
    }
  xmlCleanupParser();
  //==
}

void disCM(SalFile * sal)
{
  glEnable(GL_TEXTURE_2D);
  
  xmlChar *xpath;
  xmlNodeSetPtr nodeset;
  xmlXPathObjectPtr result;
  //==
  xpath = (xmlChar*) "//CM";
  result = xml_GetNodeSet(sal->doc, xpath);
  if(result)
    {
      nodeset = result->nodesetval;
      // nodeNr	: number of nodes in the set
      for (int i = 0; i < nodeset->nodeNr; i++)
	{
	  //<CM><NM></NM><PT></PT></CM>
	  xmlNodePtr NM = xml_GetNamedFirstChild(nodeset->nodeTab[i], (xmlChar *)"NM");
	  xmlNodePtr PT = xml_GetNamedFirstChild(nodeset->nodeTab[i], (xmlChar *)"PT");
	  if(NM != NULL && PT != NULL)
	    {
	      double x, y;
	      int line;
	      // xmlNodeGetContent(): xmlFree()必要かよくわからない
	      sscanf((char *)xmlNodeGetContent(PT), "%06d", &line);
	      x = ((double)sal->slp_[line-1].longitude) / 10000.0;
	      y = ((double)sal->slp_[line-1].latitude) / 10000.0;
	      
	      /*
		char * sjis_str = (char *)xmlNodeGetContent(NM);
		char * euc_str = new char[strlen(sjis_str)*2];
		nkf(sjis_str, euc_str, strlen(euc_str)*2, "-S -e");
		fprintf(stderr, "[%s] at (%f, %f)\n", euc_str, x, y);
	      */

	      SDL_Color color2_ = {255, 255, 255, 0};
	      FontTextureInfo *font2;
	      if((font2 = sdl_ttf2->getFontTextureSJIS((char *)xmlNodeGetContent(NM),
						       color2_)) == NULL)
		{
		  exit(-1);
		}
	      glBindTexture(GL_TEXTURE_2D, font2->textureID);
	      glBegin(GL_POLYGON);
	      glTexCoord2d(0, 0);
	      glVertex3d(x, y, 0.3);
	      glTexCoord2d(0, 1);
	      glVertex3d(x, y - font2->height/13, 0.3);
	      glTexCoord2d(1, 1);
	      glVertex3d(x + font2->width/13, y - font2->height/13, 0.3);
	      glTexCoord2d(1, 0);
	      glVertex3d(x + font2->width/13, y, 0.3);
	      glEnd();
	    }
	}
      xmlXPathFreeObject (result);
    }
  xmlCleanupParser();
  //==
  glDisable(GL_TEXTURE_2D);
}

void disCV(SalFile * sal, char * xpath)
{
  xmlNodeSetPtr nodeset;
  xmlXPathObjectPtr result;
  //==
  result = xml_GetNodeSet(sal->doc, (xmlChar *)xpath);
  if(result)
    {
      nodeset = result->nodesetval;
      for (int i = 0; i < nodeset->nodeNr; i++)
	{
	  // xmlNodeGetContent(): xmlFree()でSegmentation Fault?
	  // It's up to the caller to free the memory with xmlFree().とは?
	  char * cur = (char *)xmlNodeGetContent(nodeset->nodeTab[i]);
	  int line, vertexNumber = (strlen(cur) + 1)/7;
	  glBegin(GL_LINE_STRIP);
	  for(int i = 0;i < vertexNumber;i ++)
	    {
	      sscanf(cur, "%06d", &line);
	      glVertex3d(((double)sal->slp_[line-1].longitude) / 10000.0,
			 ((double)sal->slp_[line-1].latitude) / 10000.0,
			 0);
	      cur += 7;	      
	    }
	  glEnd();  
	}
      xmlXPathFreeObject (result);
    }
  xmlCleanupParser();
}

void disSAL(SalFile * sal)
{
  glLineWidth(2);
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

  glColor3d(0.7, 0.7, 1.0);
  disCV(sal, "//DK/CV");
  glColor3d(1.0, 1.0, .3);
  disCV(sal, "//TK/CV");
  glColor3d(.0, .0, 1.0);
  disCV(sal, "//GK/CV");
  glColor3d(.0, 1.0, .0);
  disCV(sal, "//KK/CV"); 
  glColor3d(1.0, .0, .0);
  disCV(sal, "//SK/CV");
  glColor3d(1.0, 1.0, 1.0);
  //disCM(sal);
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
      gluPerspective(viewer.getViewAngle(), viewer.getViewerRatio(), 0.0, 10.0);
    }
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if(viewer.getViewerMode() == PARALLEL)
    {
      //glRotated(viewer.getAngleX(),0.00001,0,0);
      //glRotated(viewer.getAngleY(),0,0.00001,0);
      glRotated(viewer.getAngleZ(),0,0,1);
      glTranslated(-viewer.getCenterLongitude(), -viewer.getCenterLatitude(), 0);
    }
  else // viewerMode == PERSPECTIVE
    {
      glRotated(viewer.getAngleX(),1,0,0);
      glRotated(viewer.getAngleY(),0,1,0);
      glRotated(viewer.getAngleZ(),0,0,1);
      glTranslated(0, 0, -viewer.getViewHeight());
      glTranslated(-viewer.getCenterLongitude(), -viewer.getCenterLatitude(), 0);
    }

  // auto Display
  std::vector<SLM> slmV =
    slmset->searchSLM(viewer.getLeftLongitude(), viewer.getRightLongitude(),
		      viewer.getTopLatitude(), viewer.getBottomLatitude());
  
  for(std::vector<SLM>::iterator slmi = slmV.begin();slmi != slmV.end();slmi ++)
    {
      if(displayList.callList(slmi->city_number.c_str()) != 0)
	{
	  displayList.newList(slmi->city_number.c_str());
	  disSAL(salPtrV.at(i));
	  displayList.endList(slmi->city_number.c_str());
	}
    }

  
  // __

  if(displayList4 != 1||previousSalPtrVSize != loadedSalPtrVSize)
    {
      previousSalPtrVSize = loadedSalPtrVSize;
      if(displayList4 == 1)
	{
	  glDeleteLists(displayList4, 1);
	}
      // /SAL/DK/CV{}
      glNewList(4, GL_COMPILE);
      for(unsigned int i = 0;i < loadedSalPtrVSize;i ++)
	{
	  glLineWidth(1);
	  disSAL(salPtrV.at(i));
	  //glLineWidth(1);
	  disMESH(salPtrV.at(i));
	}
      glEndList();
      displayList4 = 1;
    }
  else
    {
      glCallList(4);
    }

  if(displayList3 != 1)
    {
      glNewList(3, GL_COMPILE);
#define D_ 1
      std::map<std::pair<double,double>, Mesh50M_E>::iterator it = me1.meshes.begin();
      while(it != me1.meshes.end())
	{
	  double ** dvv = it->second.height;
	  double dvx = it->second.base_longitude;
	  double dvy = it->second.base_latitude;
	  
	  for(int ax = 0;ax < size_longitude;ax += D_)
	    for(int ay = 0;ay < size_latitude;ay += D_)
	      {
		glBegin(GL_LINE_STRIP);
		glColor3d(1, 1, 1);
		glVertex3d(dvx+(ax+D_)*mesh_longitude, dvy+ay*mesh_latitude,     dvv[ax+D_][ay]/10);
		glVertex3d(dvx+ax*mesh_longitude,     dvy+ay*mesh_latitude,     dvv[ax][ay]/10);
		glVertex3d(dvx+ax*mesh_longitude,     dvy+(ay+D_)*mesh_latitude, dvv[ax][ay+D_]/10);
	       	//glVertex3d(dvx+(ax+D_)*mesh_longitude, dvy+(ay+D_)*mesh_latitude, dvv[ax+D_][ay+D_]/10);
		glEnd();
	      }
	  ++ it;
	}
      glEndList();
      displayList3 = 1;
    }
  else
    {
      if(viewer.getViewerMode() == PERSPECTIVE)
	{
	  glCallList(3);
	}
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
    
  char PERS[] = "透視", PARA[] = "平行";
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
  glVertex3f(0+font2->width, 60, 1);
  glTexCoord2f(1, 1);
  glVertex3f(0+font2->width, font2->height+60, 1);
  glTexCoord2f(0, 1);
  glVertex3f(0, font2->height+60, 1);
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
      viewer.mouseMotion(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
      break;
    case SDL_MOUSEBUTTONDOWN:
      viewer.mousePressed(event.button.button, event.button.x, event.button.y);
      break;
    case SDL_MOUSEBUTTONUP:
      viewer.mouseReleased(event.button.button, event.button.x, event.button.y);
      break;
    case SDL_VIDEORESIZE:
      return -1;
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
      
      std::vector<SLM> slmV =
	slmset->searchSLM(viewer.getLeftLongitude(),
			  viewer.getRightLongitude(),
			  viewer.getTopLatitude(),
			  viewer.getBottomLatitude());
      for(std::vector<SLM>::iterator i = slmV.begin();i != slmV.end();i ++)
	{
	  char url[SIZE_OF_LINE];
	  sprintf(url,
		  "http://sb.itc.u-tokyo.ac.jp/GSI/sdf.gsi.go.jp/data25k/%s/%s.lzh#%s/%s",
		  i->pref.c_str(), i->city_number.c_str(),
		  i->city_number.c_str(), i->city_number.c_str());
	  int c_i = slmset->cityNumber((char *)i->city_number.c_str());
	  if(slmset->isSLMLoaded(c_i) != 1)
	    {
	      slmset->slmLoaded(c_i);
	      salPtrV.push_back(new SalFile(url));
	    }
	}
      loadedSalPtrVSize = salPtrV.size();
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
  
  if(viewer.sdl_init_video(0, 0) != 0)
    {
      SDL_Quit();
      exit(2);
    }
  SDL_WM_SetCaption("GISfx", "gisfx");

  texture = new TextureImage();
  sdl_ttf = new TextureSDL_ttf("/usr/X11R6/lib/X11/fonts/TTF/VeraBd.ttf", 50);
  sdl_ttf2 = new TextureSDL_ttf("/usr/X11R6/lib/X11/fonts/truetype/kochi-gothic.ttf", 30);
  //gisFile = new GISFile(argv[1]);
  //gisFile2 = new GISFile(argv[2]);
  //gisFile3 = new GISFile(argv[3]);

  slmset = new SLMset("slm.dat");
  
  splash("Loading...");
  
  for (int i = 1; i < argc;i ++)
    {
      fprintf(stderr, "sal[%d]: %s\n", i, argv[i]);
      SalFile * salf = new SalFile(argv[i]);
      salPtrV.push_back(salf);
      viewer.setCenterLongitude(salf->point_start_x/10000 + salf->point_range_x/10000/2);
      viewer.setCenterLatitude (salf->point_start_y/10000 + salf->point_range_y/10000/2);
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
      display();
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
