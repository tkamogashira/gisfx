#include "TextureSDL_ttf.h"

TextureSDL_ttf::TextureSDL_ttf()
{
  TTF_Init();
}

TextureSDL_ttf::TextureSDL_ttf(char * file, int ptsize)
{
  TTF_Init();
  openFont(file, ptsize);
}

void TextureSDL_ttf::openFont(char * file, int ptsize)
{
  font = TTF_OpenFont(file, ptsize);
  if(!font)
   {
     fprintf(stderr, "!TextureSDL_ttf::openFont(%s)\n", file);
     font = NULL;
   }
}

int TextureSDL_ttf::loadFontSurface(char * str, SDL_Color color)
{
  SDL_Surface *Image = NULL;
  SDL_Surface *tmpImage;
  Uint32 rmask, gmask, bmask, amask;
  GLuint texID = 0;
  FontTextureInfo texture;

  // Solid: error when rendering string with spaces
  //if(Image = TTF_RenderUTF8_Solid(this->font, str, color))
  if(Image = TTF_RenderUTF8_Blended(this->font, str, color))
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      rmask = 0xff000000;
      gmask = 0x00ff0000;
      bmask = 0x0000ff00;
      amask = 0x000000ff;
#else
      rmask = 0x000000ff;
      gmask = 0x0000ff00;
      bmask = 0x00ff0000;
      amask = 0xff000000;
#endif
      
      tmpImage = SDL_CreateRGBSurface(SDL_SWSURFACE, Image->w, Image->h, 32,
                                   rmask, gmask, bmask, amask);
      Image = SDL_ConvertSurface(Image, tmpImage->format, SDL_SWSURFACE);
      SDL_FreeSurface(tmpImage);

      glGenTextures(1, &texID);
      glBindTexture(GL_TEXTURE_2D, texID);
      // GL_LINEAR ... for fast machine
      // GL_NEAREST ... for slow machine
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, Image->w, Image->h,
			GL_RGBA, GL_UNSIGNED_BYTE, Image->pixels);
      SDL_FreeSurface(Image);
      
      texture.str = str;
      texture.textureID = texID;
      texture.width = Image->w;
      texture.height = Image->h;
      texture.color = color;
      Textures.insert(std::pair<std::string, FontTextureInfo>(str, texture));
      return 0;
    }
  fprintf(stderr, "!TTF_RenderUTF8_Solid()\n");
  return -1;
}

FontTextureInfo * TextureSDL_ttf::getFontTextureSJIS(char * sjis_str, SDL_Color color)
{
  FontTextureInfo * ret;
  char * utf8_str = new char[strlen(sjis_str)*2];
  nkf(sjis_str, utf8_str, strlen(sjis_str)*2, "-S -w");
  ret = getFontTexture(utf8_str, color);
  delete[] utf8_str;
  return ret;
}

FontTextureInfo * TextureSDL_ttf::getFontTextureEUCJP(char * eucjp_str, SDL_Color color)
{
  FontTextureInfo * ret;
  char * utf8_str = new char[strlen(eucjp_str)*2];
  nkf(eucjp_str, utf8_str, strlen(eucjp_str)*2, "-E -w");
  ret = getFontTexture(utf8_str, color);
  delete[] utf8_str;
  return ret;
}

FontTextureInfo * TextureSDL_ttf::getFontTexture(char * utf8_str, SDL_Color color)
{
  std::map<std::string, FontTextureInfo>::iterator i = Textures.find(utf8_str);
  if(i != Textures.end())
    {
      return &(i->second);
    }
  if(loadFontSurface(utf8_str, color) == 0)
    {
      return getFontTexture(utf8_str, color);
    }
  return NULL;
}
