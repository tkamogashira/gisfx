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
#include "XGL.h"
#include <string>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mutex.h>
#include <libnkf.h>

#include "libarc/arc.h"

extern struct URL_module URL_module_file;
extern struct URL_module URL_module_dir;
extern struct URL_module URL_module_http;
extern struct URL_module URL_module_ftp;
extern struct URL_module URL_module_news;
extern struct URL_module URL_module_newsgroup;

#define LINESIZE 1024

int main(int argc, char *argv[])
{ 
  if (argc <= 1) {
    printf ("Usage: %s [urlname]\n", argv[0]);
    return 0;
  }

  url_add_modules(&URL_module_file,
		  &URL_module_dir,
		  &URL_module_http,
		  &URL_module_ftp,
		  &URL_module_news,
		  &URL_module_newsgroup,
		  NULL);
  
  char * urlname = argv[1];
  URL url = url_arc_open(urlname);
  
  char line[LINESIZE];
  while(1)
    {
      char * ret = url_gets(url, line, LINESIZE);
      if(ret != line)
	break;
      printf("%s", line);
    }
  
  url_close(url);

  return 0;
}
