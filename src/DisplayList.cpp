#include "DisplayList.h"

int DisplayList::newList(char * listName)
{
  std::map<std::string,gl_List>::iterator i =
    displayLists.find(std::string(listName));
  if(i != displayLists.end())
    {
      return -2;
    }
  GLuint l = glGenLists(1);
  if(l != 0)
    {
      gl_List gList;
      gList.dList = l;
      gList.compiled = 0;
      displayLists.insert(std::pair<std::string,gl_List>(std::string(listName),gList));
      glNewList(l, GL_COMPILE);
    }
  return -1;
}

int DisplayList::endList(char * listName)
{
  std::map<std::string,gl_List>::iterator i =
    displayLists.find(std::string(listName));
  if(i == displayLists.end())
    {
      return -1;
    }
  i->second.compiled = 1;
  glEndList();
  return 0;
}

int DisplayList::callList(char * listName)
{
  std::map<std::string,gl_List>::iterator i =
    displayLists.find(std::string(listName));
  if(i == displayLists.end())
    {
      return -1;
    }
  if(i->second.compiled != 1)
    {
      return -1;
    }
  glCallList(i->second.dList);
  return 0;
}

int DisplayList::deleteList(char * listName)
{
  std::map<std::string,gl_List>::iterator i =
    displayLists.find(std::string(listName));
  if(i == displayLists.end())
    {
      return -1;
    }
  displayLists.erase(i);
  return 0;
}
