#include <GL/glut.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "GISFile.h"
#include "SalFile.h"
#include <string>
#include <libnkf.h>
#include "XGL.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "libarc/arc.h"
#include "libarc/url.h"

static int defaultWidth = 640;
static int defaultHeight = 480;
int currentWidth;
int currentHeight;
XGL xgl;

char *docname;
xmlDocPtr doc;
xmlNodePtr cur;

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_LINE_SMOOTH);  
  glEnable(GL_DEPTH_TEST);

  xmlNodePtr child;
  for(child = cur->children;child != NULL;child = child->next)
    {
      xgl.execNode(child);
    }
  
  glutSwapBuffers();
}

void resize(int w, int h)
{
  currentWidth = w;
  currentHeight = h;

  glViewport(0, 0, w, h);
  glLoadIdentity();
  gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
  glTranslated(0, -1, -25);
}

void keyboard(unsigned char key, int x, int y)
{
  printf("Key: %c at (%d,%d)\n", key, x, y);
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
    xmlFreeDoc(doc);
    exit(0);
  default:
    break;
  }
}

int main(int argc, char *argv[])
{
  if (argc <= 1) {
    printf ("Usage: %s [xml filename]\n", argv[0]);
    return 0;
  }
  docname = argv[1];
  
  /* parse xml */
  fprintf (stderr, "Parsing %s ...", docname);
  doc = xmlParseFile(docname);
  if(doc == NULL)
    {
      fprintf(stderr, "not parsed successfully. \n");
      return -1;
    }
  fprintf(stderr, "done.\n");
  cur = xmlDocGetRootElement (doc);
  if(cur == NULL)
    {
      fprintf (stderr, "empty document.\n");
      xmlFreeDoc(doc);
      return -1;
    }
  fprintf (stderr, "<%s>.\n", (char *)cur->name);

  glutInitWindowPosition(100, 100);
  glutInitWindowSize(defaultWidth, defaultHeight);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  
  glutMainLoop();
  return 0;
}
