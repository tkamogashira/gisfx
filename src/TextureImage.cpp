#include "TextureImage.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int TextureImage::loadImage(char *filename)
{
  SDL_Surface *Image = NULL;
  SDL_Surface *tmpImage;
  GLuint texID = 0;
  TextureInfo texture;

  if(Image = IMG_Load(filename))
    {
      tmpImage = SDL_CreateRGBSurface(SDL_SWSURFACE, 0, 0, 32, 
				      0x000000ff, 
				      0x0000ff00, 
				      0x00ff0000, 
				      0xff000000);
      Image = SDL_ConvertSurface(Image, tmpImage->format, SDL_SWSURFACE);
      SDL_FreeSurface(tmpImage);
      
      glGenTextures(1, &texID);
      glBindTexture(GL_TEXTURE_2D, texID);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      /* glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Image->w, Image->h, 0,
	 GL_RGBA, GL_UNSIGNED_BYTE, Image->pixels); */
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, Image->w, Image->h,
			GL_RGBA, GL_UNSIGNED_BYTE, Image->pixels);

      SDL_FreeSurface(Image);
      
      texture.filename = filename;
      texture.textureID = texID;
      texture.width = Image->w;
      texture.height = Image->h;
      Textures.insert(std::pair<std::string, TextureInfo>(filename, texture));
      return 0;
    }
  return -1;
}

TextureInfo * TextureImage::getTexture(char *filename)
{
  std::map<std::string, TextureInfo>::iterator i = Textures.find(filename);
  if(i != Textures.end())
    {
      return &(i->second);
    }
  if(loadImage(filename) == 0)
    {
      return getTexture(filename);
    }
  return NULL;
}
