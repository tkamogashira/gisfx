#ifndef XGL_H
#define XGL_H

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <GL/glut.h>
#include <libxml/tree.h>
#include "Data.h"

#define VERTEX_NODE_STRING "Vertex"

#define MODE_ATTR_STRING "mode"

class XGL
{
 private:
  xmlChar * getFirstContent(xmlNode * node);
  xmlChar * searchAttribute(xmlChar * attrName, xmlNode * node);

  int getGLenum(char * str, GLenum * glenum);

  void LoadIdentity(xmlNodePtr node);
  void Color3(xmlNodePtr node);
  void Color4(xmlNodePtr node);
  void Vertex2(xmlNodePtr node);
  void Vertex3(xmlNodePtr node);
  void Vertex4(xmlNodePtr node);
  void Vertices(xmlNodePtr node);
  void Viewport(xmlNodePtr node);
  void Ortho(xmlNodePtr node);
  void LookAt(xmlNodePtr node);
  void Perspective(xmlNodePtr node);

 public:
  int execNode(xmlNode * node);
};

typedef void (XGL::*nodeFunction)(xmlNodePtr) ;

#endif

