#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <map>

#include <assimp/material.h>
#include <GL/glew.h>

#include "Math.h"

#include "ShaderVariableTypes.h"

// class Shader;

/*
  - defines the appearance of a mesh
  - references and binds uniforms to a shader
*/

class Model;
class Geometry;
class Texture;

class Material
{
  //Shader * mShader;
  std::map<ShaderUniform, std::string> mTextureNames; // currently only hold textures
public:
  Material();
  virtual ~Material();
  //void SetShader(Shader * shader);
  void Load(aiMaterial * material);
  //Shader * GetShader();

  glm::mat4x4 GetMatrix(ShaderUniform type, Model * model);
  // fills out the matrix count
  void GetMatrixArray(ShaderUniform type, Model * model, Geometry * geometry,std::vector<glm::mat4x4> & data);
  Texture * GetTexture(ShaderUniform type);
  float GetFloat(ShaderUniform type);

};

#endif
