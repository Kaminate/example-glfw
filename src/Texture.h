#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include <map>
#include <GL/glew.h>
#include <gl/glfw.h>

class Texture;

class TextureBank
{
  typedef std::pair<std::string, Texture *> TexturePair;
  std::map<std::string, Texture *> mTextures;
  
public:
  ~TextureBank();
  // todo: replace occurances of const char * with std::string
  
  Texture * AddTexture(const char * folder, const char * filename); 
  Texture * CreateTexture(const char * textureName, int width, int height);
  Texture * GetTexture(const char * filename);
};

class Texture
{
  const char * mName;
  int mWidth, mHeight;
  GLuint mTexture;
  Texture(const char * name, const char * filepath); friend class TextureBank;
  Texture(const char * name, int width, int height); // create an empty texture
  ~Texture();
public:
  
  GLuint GetGLuint() const;
  int GetWidth() const;
  int GetHeight() const;
};

#endif
