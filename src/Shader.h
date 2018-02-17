#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>

// http://www.sfml-dev.org/documentation/2.0/classsf_1_1Shader.php
// They do shader parameter type detection via function overloading
#include "Math.h"
#include "ShaderVariableTypes.h"



class Geometry;
class Material;
class GameObject;
class Shader;
class Texture;
class Model;
class Mesh;

class ShaderBank
{
  typedef std::pair<std::string, Shader *> ShaderPair;
  std::map<std::string, Shader *> mShaders;  // todo: who owns the shaders?
  
public:
  ~ShaderBank();
  Shader * CreateShader(
    const char * name,
    const char * folder,
    const char * vsName, 
    const char * fsName);
  Shader * GetShader(const char * name);
};

class Shader
{
  const char * mName; // todo: std::string
  GLuint mProgram;

  typedef std::pair<VertAttrib,    GLuint> AttributePair;
  typedef std::pair<ShaderUniform, GLuint> UniformPair;

  std::map<VertAttrib,    GLuint> mAttributes;     // will all be unique
  std::map<ShaderUniform, GLuint> mUniforms;       // will all be unique
  
  std::map<ShaderUniform, ShaderDataType> mTypes;  // rtti of each uniform

  std::vector<ShaderUniform> mTextureIndicies;     // the index into the array
  void SetTextureIndex(ShaderUniform textureType); // add to array
  unsigned GetTextureIndex(ShaderUniform textureType) const; 
  
  friend class ShaderBank;
  Shader(); 
  void Load(const char * vsFilepath, const char * fsFilepath);
  void Load(const char * folder, const char * vsName, const char * fsName);
public:
  Shader(const char * name);
  ~Shader();
  
  void SetActive();

  // stores the address of a shader variable into our map
  void BindAttribute(VertAttrib type, const char * name);
  void BindUniform(ShaderUniform type, ShaderDataType data, const char * name);
  
  // returns the gluint of the variable with the name
  GLuint GetAttribute(VertAttrib type) const;
  GLuint GetUniform(ShaderUniform type) const;

  // sending data automatically through normal render loop
  void SendAttributes(Geometry * geom) const;
  void SendUniforms(Material * mat, Model * model, Geometry * geometry) const;

  // if you want to send uniforms directly (without materials)
  void SendData(ShaderUniform uniformType, float data) const;
  void SendData(ShaderUniform uniformType, const glm::mat4x4 & data) const;
  void SendData(ShaderUniform uniformType, const glm::mat4x4 * data, unsigned matrixCount) const;
  //void SendData(ShaderUniform uniformType, const aiMatrix4x4 * data, unsigned matrixCount) const;
  void SendData(ShaderUniform uniformType, Texture * texture) const;
};

#endif
