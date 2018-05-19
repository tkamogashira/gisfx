#ifndef TEXTUREIMAGE_H
#define TEXTUREIMAGE_H

#include <iostream>
#include <GL/glut.h>
#include <SDL/SDL_image.h>
#include <vector>
#include <map>
#include <string>

typedef struct{
  std::string filename;
  GLuint textureID;
  int width;
  int height;
} TextureInfo;

class TextureImage
{
 private:
  std::map<std::string, TextureInfo> Textures;
 public:
  int loadImage(char * filename);
  TextureInfo * getTexture(char * filename);
};

#endif
