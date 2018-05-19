#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <string>
#include <map>
#include <GL/glut.h>

typedef struct{
  GLuint dList;
  int compiled;
} gl_List;

class DisplayList
{
 private:
  std::map<std::string,gl_List> displayLists;
 public:
  int newList(char * listName);
  int endList(char * listName);
  int callList(char * listName);
  int deleteList(char * listName);
};

#endif
