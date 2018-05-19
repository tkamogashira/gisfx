#include "XGL.h"

int XGL::getGLenum(char * str, GLenum * glenum)
{
  static GLenum glenumList[] = {
    GL_POINTS, GL_LINES,
    GL_LINE_STRIP, GL_LINE_LOOP,
    GL_TRIANGLES, GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN, GL_QUADS,
    GL_QUAD_STRIP, GL_POLYGON,
  };
  
  static char glstrList[][32] = {
    "GL_POINTS", "GL_LINES",
    "GL_LINE_STRIP", "GL_LINE_LOOP",
    "GL_TRIANGLES", "GL_TRIANGLE_STRIP",
    "GL_TRIANGLE_FAN", "GL_QUADS",
    "GL_QUAD_STRIP", "GL_POLYGON",
  };
  
  for(int i = 0;i < sizeof(glenumList)/sizeof(GLenum);i ++)
    {
      if(strcmp(str, glstrList[i]) == 0)
	{
	  *glenum = glenumList[i];
	  return 0;
	}
    }
  return -1;
}

xmlChar * XGL::getFirstContent(xmlNode * node)
{
  xmlNode * child;
  for(child = node->children;node != NULL;child = child->next)
    {
      if(child->type == XML_TEXT_NODE)
	{
	  return child->content;
	}
    }
  return NULL;
}

xmlChar * XGL::searchAttribute(xmlChar * attrName, xmlNode * node)
{
  xmlAttr * attr;
  for(attr = node->properties;attr != NULL;attr = attr->next)
    {
      // attr->name = the name of the property
      // attr->children->name = the value of the property
      if(strcmp((char *)attr->name, (char *)attrName) == 0)
	{
	  return attr->children->content;
	}
    }
  return NULL;
}

void XGL::Color3(xmlNodePtr node)
{
  double red, green, blue;
  sscanf((char *)getFirstContent(node), "%lf %lf %lf", &red, &green, &blue);
  glColor3d(red, green, blue);
};

void XGL::Color4(xmlNodePtr node)
{
  double red, green, blue, alpha;
  sscanf((char *)getFirstContent(node), "%lf %lf %lf %lf", &red, &green, &blue, &alpha);
  glColor4d(red, green, blue, alpha);
};

void XGL::Vertex2(xmlNodePtr node)
{
  double x, y;
  sscanf((char *)getFirstContent(node), "%lf %lf", &x, &y);
  glVertex2d(x, y);
};

void XGL::Vertex3(xmlNodePtr node)
{
  double x = 1, y = 1, z = 0;
  sscanf((char *)getFirstContent(node), "%lf %lf %lf", &x, &y, &z);
  glVertex3d(x, y, z);
}

void XGL::Vertex4(xmlNodePtr node)
{
  double x = 1, y = 1, z = 0, w = 1;
  sscanf((char *)getFirstContent(node), "%lf %lf %lf %lf", &x, &y, &z, &w);
  glVertex4d(x, y, z, w);
}

void XGL::Viewport(xmlNodePtr node)
{
  int x = 0, y = 0, width, height;
  sscanf((char *)getFirstContent(node), "%d %d %d %d", &x, &y, &width, &height);
  glViewport(x, y, width, height);
}

void XGL::Ortho(xmlNodePtr node)
{
  double left, right, bottom, top, near_val, far_val;
  sscanf((char *)getFirstContent(node), "%lf %lf %lf %lf %lf %lf",
	 &left, &right, &bottom, &top, &near_val, &far_val);  
  glOrtho(left, right, bottom, top, near_val, far_val);
}

void XGL::LookAt(xmlNodePtr node)
{
  double eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ;
  sscanf((char *)getFirstContent(node), "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
	 &eyeX, &eyeY, &eyeZ, &centerX, &centerY, &centerZ, &upX, &upY, &upZ);
  gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
}

void XGL::Perspective(xmlNodePtr node)
{
  double fovy, aspect, zNear, zFar;
  sscanf((char *)getFirstContent(node), "%lf %lf %lf %lf", &fovy, &aspect, &zNear, &zFar);  
  gluPerspective(fovy, aspect, zNear, zFar);
}

void XGL::LoadIdentity(xmlNodePtr node)
{
  glLoadIdentity();
}

void XGL::Vertices(xmlNodePtr node)
{
  GLenum glenum = GL_POINTS;
  char * glString = (char *)searchAttribute((xmlChar *)MODE_ATTR_STRING, node);
  getGLenum(glString, &glenum);
  glBegin(glenum);
  xmlNodePtr child;
  for(child = node->children;child != NULL;child = child->next)
    {
      execNode(child);
    }
  glEnd();
}

// そのうち<boost/function.hpp>でも使うか...
int XGL::execNode(xmlNodePtr node)
{
  static nodeFunction function[] = {
    &XGL::LoadIdentity, &XGL::Color3, &XGL::Color4,
    &XGL::Vertex2, &XGL::Vertex3, &XGL::Vertex4, &XGL::Vertices,
    &XGL::Viewport, &XGL::Ortho, &XGL::LookAt, &XGL::Perspective,
  };
  
  static char functionString[][32] = {
    "LoadIdentity", "Color3", "Color4",
    "Vertex2", "Vertex3", "Vertex4", "Vertices",
    "Viewport", "Ortho", "LookAt", "Perspective",
  };
  
  for(int i = 0;i < sizeof(function)/sizeof(nodeFunction);i ++)
    {
      if(strcmp((char *)node->name, functionString[i]) == 0)
	{
	  (this->*(function[i]))(node);
	  return 0;
	}
    }
  return -1;  
}
