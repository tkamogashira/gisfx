#ifndef TEXTURESDL_TTF_H
#define TEXTURESDL_TTF_H

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <libnkf.h>

#include <iostream>
#include <GL/glut.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include <map>
#include <string>

typedef struct{
  std::string str;
  GLuint textureID;
  int width;
  int height;
  SDL_Color color;
} FontTextureInfo;

class TextureSDL_ttf
{
 private:
  std::map<std::string, FontTextureInfo> Textures;
  TTF_Font * font;
  void openFont(char * file, int ptsize);
  int loadFontSurface(char * str, SDL_Color color);
 public:
  FontTextureInfo * getFontTextureSJIS(char * sjis_str, SDL_Color color);
  FontTextureInfo * getFontTextureEUCJP(char * eucjp_str, SDL_Color color);
  FontTextureInfo * getFontTexture(char * utf8_str, SDL_Color color);
  //freeTexture(char * str);
  //freeAllTexture();
  TextureSDL_ttf(char * file, int ptsize);
  TextureSDL_ttf();
};

#endif
