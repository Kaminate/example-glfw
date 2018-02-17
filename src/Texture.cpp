#include "Texture.h"

#include "debug.h"

#include <iostream>
#include <SOIL.h>

// Texture bank
TextureBank::~TextureBank()
{
  for (auto it = mTextures.begin(); it != mTextures.end(); ++it)
  {
    TexturePair pair = *it;
    Texture * myTexture = pair.second;
    delete myTexture;
  }
}

Texture * TextureBank::AddTexture(const char * folder, const char * filename)
{
  auto it = mTextures.find(filename);
  bool alreadyLoaded = it != mTextures.end();
  //ErrorIf(it != mTextures.end(), "The TextureBank already has this texture");
  Texture * theTexture = 0;
  if (alreadyLoaded)
  {
    theTexture = mTextures[filename];
  }
  else
  {
    theTexture = new Texture(filename, folder);
    mTextures[filename] = theTexture;
  }
  
  return theTexture;
}

Texture * TextureBank::CreateTexture(
  const char * textureName, 
  int width, 
  int height )
{
  auto it = mTextures.find(textureName);
  bool alreadyLoaded = it != mTextures.end();
  ErrorIf(it != mTextures.end(), "A texture of this name already exists!");
  Texture * theTexture = 0;
  //if (alreadyLoaded)
  //{
  //  const TexturePair & pair = *it;
  //  theTexture = pair.second;
  //}
  //else
  //{
    theTexture = new Texture(textureName, width, height);
    mTextures[textureName] = theTexture;
  ///}

  return theTexture;
}

Texture * TextureBank::GetTexture(const char * name)
{
  auto it = mTextures.find(name);
  ErrorIf(it == mTextures.end(), "This Texture is not in the TextureBank");
  const TexturePair & pair = *it;
  return pair.second;
}



//Texture 
Texture::Texture(const char * name, const char * filepath)
  : mName(name) 
{
  // todo: load file
  std::string entirePath(filepath);
  entirePath += name;

  mTexture = SOIL_load_OGL_texture(
    entirePath.c_str(), 
    SOIL_LOAD_AUTO,
    SOIL_CREATE_NEW_ID,
    SOIL_FLAG_MIPMAPS 
    | SOIL_FLAG_INVERT_Y 
    | SOIL_FLAG_NTSC_SAFE_RGB 
    | SOIL_FLAG_COMPRESS_TO_DXT);
  glBindTexture(GL_TEXTURE_2D, mTexture);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &mWidth);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &mHeight);
  //std::cout << "Texture size: ("<< mWidth << ", " << mHeight << ")"<< std::endl;
  
  ErrorIf(mTexture == 0, 
    "Soil failed to load an image: '%s'\n", SOIL_last_result());
}

Texture::Texture( const char * name, int width, int height ) 
  : mName(name), mWidth(width), mHeight(height)
{ 
  glGenTextures(1, &mTexture);

  glBindTexture(GL_TEXTURE_2D, mTexture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
    width, 
    height, 
    0, GL_RGBA, GL_UNSIGNED_BYTE, 
    0);

  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
  glDeleteTextures(1, &mTexture);
}

GLuint Texture::GetGLuint() const
{
  return mTexture;
}

int Texture::GetWidth() const
{
  return mWidth;
}

int Texture::GetHeight() const
{
  return mHeight;
}
