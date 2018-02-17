#include "Shader.h"
#include "shader_utils.h"
#include "debug.h"

#include "Geometry.h"
#include "Material.h"
#include "Texture.h"
#include "Model.h"
#include "Mesh.h"
#include <algorithm>

// see http://www.lighthouse3d.com/very-simple-libs/vsshader-lib-very-simple-shader-library/vsshader-lib-source/

// shader bank
ShaderBank::~ShaderBank()
{
  for (auto it = mShaders.begin(); it != mShaders.end(); ++it)
  {
    ShaderPair pair = *it;
    Shader * shader = pair.second;
    delete shader;
  }
}

Shader * ShaderBank::CreateShader(
  const char * name,
  const char * folder,
  const char * vsName, 
  const char * fsName)
{
  auto it = mShaders.find(name);
  ErrorIf(it != mShaders.end(), "This shader is already in the shaderbank");

  Shader * myShader = new Shader(name);
  myShader->Load(folder, vsName, fsName);

  mShaders[name] = myShader;
  return myShader;
}
Shader * ShaderBank::GetShader(const char * name)
{
  auto it = mShaders.find(name);
  ErrorIf(it == mShaders.end(), "This shader is not in the shaderbank");
  const ShaderPair & pair = *it;
  return pair.second;
}

// shader
Shader::Shader(const char * name) : mProgram(-1), mName(name)
{
}

Shader::~Shader()
{
  if (mProgram == -1)
    return; // nothing to unload

  glDeleteProgram(mProgram);
}

void Shader::Load(const char * vsFilepath, const char * fsFilepath)
{
  // program
  GLuint vs = create_shader(vsFilepath, GL_VERTEX_SHADER);
  GLuint fs = create_shader(fsFilepath, GL_FRAGMENT_SHADER);
  ErrorIf(!vs || !fs, "Failed to create shaders");
  mProgram = glCreateProgram();
  glAttachShader(mProgram, vs);
  glAttachShader(mProgram, fs);
  glLinkProgram(mProgram);
  GLint link_ok = GL_FALSE;
  glGetProgramiv(mProgram, GL_LINK_STATUS, &link_ok);
  ErrorIfGL(!link_ok, mProgram, "Error linking program");
}

void Shader::Load( const char * folder, const char * vsName, const char * fsName )
{
  std::string vsFullPath(folder);
  vsFullPath += "/";
  vsFullPath += vsName;
  std::string fsFullPath(folder);
  fsFullPath += "/";
  fsFullPath += fsName;
  Load(vsFullPath.c_str(), fsFullPath.c_str());
}

void Shader::BindAttribute(VertAttrib type,  const char * name )
{
  auto it = mAttributes.find(type);
  // error note: perhaps you meant uniform? check your spelling
  ErrorIf(it != mAttributes.end(), "the shader already has this attribute");

  GLuint attribute = glGetAttribLocation(mProgram, name);
  ErrorIf(attribute == -1, "Couldn't bind attribute %s", name);
  mAttributes[type] = attribute;
}

void Shader::BindUniform(
  ShaderUniform uniformType, 
  ShaderDataType dataType, 
  const char * name)
{
  auto it = mUniforms.find(uniformType);
  // error note: perhaps you meant uniform? check your spelling
  ErrorIf(it != mUniforms.end(), "the shader already has this uniform");

  GLuint uniform = glGetUniformLocation(mProgram, name);
  ErrorIf(uniform == -1, "Couldn't bind uniform %s", name);
  mUniforms[uniformType] = uniform;
  mTypes[uniformType] = dataType;

  // indicies of multiple data types
  switch (dataType)
  {
  case MATRIX:
    break;
  case TEXTURE:
    SetTextureIndex(uniformType);
    break;
  }
}

GLuint Shader::GetAttribute( VertAttrib type) const
{
  auto it = mAttributes.find(type);

  // error note: perhaps you meant uniform? check your spelling
  ErrorIf(it == mAttributes.end(), "the shader does not have this attribute");
  const std::pair<VertAttrib, GLuint> & foo = *it;
  return foo.second;
}

GLuint Shader::GetUniform( ShaderUniform type ) const
{
  auto it = mUniforms.find(type);
  
  // error note: perhaps you meant attribute? check your spelling
  ErrorIf(it == mUniforms.end(), "the shader does not have this uniform");
  const std::pair<ShaderUniform, GLuint> & foo = *it;
  return foo.second;
}

void Shader::SendAttributes( Geometry * geom ) const
{
  for (auto it = mAttributes.begin(); it != mAttributes.end(); ++it)
  {
    const AttributePair & pair = *it;
    VertAttrib type = pair.first;
    GLuint attribute = pair.second;
    GLuint buffer = geom->GetAttribute(type);

    // select current buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    // open shader var
    glEnableVertexAttribArray(attribute);

    //send to gpu at the shader's shader var
    int stride = 0; // dist between the starts of 2 attribs (0 means auto)
    int componentsPerAttribute = GetComponentsPerAttribute(type);
    
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribPointer.xml
    glVertexAttribPointer(
      attribute, // index of the generic vertex atribute to be modified
      componentsPerAttribute, // 1, 2, 3, or 4
      GL_FLOAT, // data type of the component
      GL_FALSE, // normalized
      stride, // byte offset between the starts of 2 attributes
      0); // offset of first component of attribute in the array currently bound
          // to GL_ARRAY_BUFFER
    
  }
}

void Shader::SendUniforms( Material * mat, Model * model, Geometry * geometry) const
{
  for (auto it = mUniforms.begin(); it != mUniforms.end(); ++it)
  {
    const UniformPair & pair = *it;
    ShaderUniform uniformtype = pair.first;
    ShaderDataType datatype = mTypes.at(uniformtype);
    
    GLuint uniformLocation = pair.second;
    
    switch (datatype)
    {
    case FLOAT: 
      SendData(uniformtype, mat->GetFloat(uniformtype));
      break;
    case MATRIX:
      SendData(uniformtype, mat->GetMatrix(uniformtype, model));
      break;
    case MATRIX_ARRAY:
      {
      std::vector<glm::mat4x4> matrixes;
      mat->GetMatrixArray(uniformtype, model, geometry, matrixes);
      if (!matrixes.empty())
        SendData(uniformtype, &matrixes[0], matrixes.size());
      break;
      }
    case TEXTURE:
      SendData(uniformtype, mat->GetTexture(uniformtype));
      break;
    default:
      ErrorIf(true, "Shader trying to send a datatype it can't handle");
      break;
    }
  }
}

// We shouldn't need to do error checking, it should already have been
// done for us in Shader::BindUniform
void Shader::SetTextureIndex(ShaderUniform textureType) // add to array
{
  mTextureIndicies.push_back(textureType);

  ErrorIf(mTextureIndicies.size() > GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    "Exceeded number of textures that can be sent to the shader");
}

// index in array
unsigned Shader::GetTextureIndex(ShaderUniform textureType) const 
{
  // linear search is fine
  for (unsigned index = 0; index < mTextureIndicies.size(); ++index)
  {
    ShaderUniform currTexture = mTextureIndicies[index];
    if (currTexture == textureType)
      return index;
  }

  ErrorIf(true, "Trying to get the index of a texture that has never had "
    "its texture index set");
  return 1337; // quell warning
}

void Shader::SetActive()
{
  ErrorIf(mProgram == -1, "This shader doesn't have a program");
  glUseProgram(mProgram);
}

// if you want to send uniforms directly (without materials)
void Shader::SendData( ShaderUniform uniformType, float data ) const
{
  glUniform1f(GetUniform(uniformType), data);
}

void Shader::SendData( ShaderUniform uniformType, Texture * texture ) const
{
  glActiveTexture(GL_TEXTURE0 + GetTextureIndex(uniformType));
  glBindTexture(GL_TEXTURE_2D, texture->GetGLuint());
}

void Shader::SendData( ShaderUniform uniformType, const glm::mat4x4 & data ) const
{
  glUniformMatrix4fv(GetUniform(uniformType), 1, GL_FALSE, glm::value_ptr(data));
}

void Shader::SendData( ShaderUniform uniformType, const glm::mat4x4 * data, unsigned matrixCount ) const
{
  // will this work? (float*) part
  ErrorIf(matrixCount > 28, "Your shader probably doesn't have 28 matrixes. "
    "Or if it does, that's probably too many");
  glUniformMatrix4fv(GetUniform(uniformType), matrixCount, GL_FALSE, (float*) data);
}

// void Shader::SendData( ShaderUniform uniformType, const aiMatrix4x4 * data, unsigned matrixCount ) const
// {
//   // true transposes from col major (aimatrix) to row major (glm)
//   glUniformMatrix4fv(GetUniform(uniformType), matrixCount, GL_TRUE, data);
// }
