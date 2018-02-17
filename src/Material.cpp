#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "Transform.h"
#include "Model.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Shader.h"
#include "Texture.h"
#include "Graphics.h"

#include <iostream>

// for Material::Load
bool operator == (const aiString & lhs, const char * rhs)
{
  const char * l = lhs.C_Str();
  bool equal = strcmp(l, rhs) == 0; // strcmp isnt working?
  return equal;
}

Material::Material() 
  // : mShader(0)
{

}

Material::~Material()
{
  
}

//void Material::SetShader( Shader * shader )
//{
//  mShader = shader;
//}

// AI_MATKEY_COLOR_DIFFUSE is #defined as "$clr.diffuse",0,0
// this function separates out the string portion
// this is a hacky piece of shit. don't use this.
const char * GetCStringFromAssimpMacro(const char * str, int null1, int null2)
{
  null1; 
  null2;
  return str;
}

// http://assimp.sourceforge.net/lib_html/materials.html

// loads the textures into the texture bank
void Material::Load( aiMaterial * material )
{
  if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
  {
    aiString textureName;
    material->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);

    // load the texture int the texture bank
    Graphics & graphicsSystem = Engine::GetSystem<Graphics>();
    TextureBank * myTextureBank = graphicsSystem.GetTextureBank();
    std::string textureFilepath("resources/assets/");
    myTextureBank->AddTexture(textureFilepath.c_str(), textureName.C_Str());

    // add the name of the texture to our map of shader uniforms
    mTextureNames[DIFFUSE_TEXTURE] = textureName.C_Str();
  }
  
  // for (unsigned i = 0; i < material->mNumProperties; ++i)
  // {
  //   aiMaterialProperty * matProp = material->mProperties[i];
  //   std::cout << matProp->mKey.C_Str() << std::endl;
  //   
  //   if (matProp->mKey == GetCStringFromAssimpMacro(AI_MATKEY_COLOR_DIFFUSE))
  //   {
  // 
  //     aiString * poop = reinterpret_cast<aiString*>(matProp->mData); // NOPE
  //     std:: cout << poop->C_Str() << std::endl;                      // NOPE
  // 
  //     aiPropertyTypeInfo aitypeinfo =  matProp->mType; // why is this float?
  // 
  //     mTextureNames[DIFFUSE_TEXTURE] = poop->C_Str(); // todo: for realsies
  //   }
  //   
  //   AI_MATKEY_SHININESS_STRENGTH;
  //   AI_MATKEY_COLOR_DIFFUSE;
  //   _AI_MATKEY_TEXTURE_BASE;
  // }
  /*
  material->mProperties;
  material->Get();
  material->GetTexture()

  aiTexture name;
  material->Get(AI_MATKEY_TEXTURE_AMBIENT, name);

  int texIndex = 0;
  aiString texPath;   //contains filename of texture
  
  if(AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
  {
    glBindTexture(GL_TEXTURE_2D, texId);
  }
  

  aiColor3D color (0.f,0.f,0.f);
  material->Get(AI_MATKEY_COLOR_DIFFUSE,color);

  for (unsigned i = 0; i < material->mNumProperties; ++i)
  {
    aiMaterialProperty * property = material->mProperties[i];
    std::cout << std::endl;
    std::cout << property->mData       << std::endl;
    std::cout << property->mDataLength << std::endl;
    std::cout << property->mIndex      << std::endl;
    std::cout << property->mKey.C_Str()<< std::endl;
    std::cout << property->mSemantic   << std::endl;
    std::cout << property->mType       << std::endl;
  }
  */
}

//Shader * Material::GetShader()
//{
//  ErrorIf(!mShader, "This material is not associated with a shader!");
//  return mShader;
//}
    
glm::mat4x4 Material::GetMatrix( ShaderUniform type, Model * model )
{
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();

  switch(type)
  {
  case MVP_MATRIX:
      return graphicsSystem.GetCamToProj()
        * graphicsSystem.GetWorldToCam()
        * model->GetModelToWorld();
  case BONE_MATRIXES:
    // todo:
    __debugbreak();
    //model->
  default:
      ErrorIf(true, "Getting a matrix of unknown use (shaderuniform)");
  }
  return glm::mat4x4(); // get rid of a warning (will never be hit)
}

Texture * Material::GetTexture( ShaderUniform type )
{
  TextureBank * texBank = Engine::GetSystem<Graphics>().GetTextureBank();
  return texBank->GetTexture(mTextureNames[type].c_str());
}

float Material::GetFloat( ShaderUniform type )
{
  ErrorIf(true, "todo");
  return 0;
}

void Material::GetMatrixArray( ShaderUniform type, Model * model, Geometry * geometry, std::vector<glm::mat4x4> & data)
{
  switch(type)
  {
  case BONE_MATRIXES:
    {
      Joint * myjoint = geometry->GetJoint();
      std::vector<aiMatrix4x4> & matrixTransforms = myjoint->mFinalTransform;
      data.reserve(matrixTransforms.size());
      for (auto it = matrixTransforms.begin(); it != matrixTransforms.end(); ++it)
      {
        aiMatrix4x4 & transform = *it;
        data.push_back(CopyaiMat(transform));
      }
      break;
    }

  default:
    ErrorIf(true, "todo: handle different matrix arrays");
  }
}



// more thoughts:

/*
http://www.horde3d.org/docs/manual.html

Material
  Materials define the visual appearance of a mesh or 
  renderable object in general. 

  A material references a shader and binds the uniforms to it. 

  The uniforms can either be texture maps, 
  floating point vectors that pass arbitrary application-specific data 
  to the shader.
*/

/*
  idea from: 
  http://doc.minko.io/wiki/Texture_Basics
  
  var material : BasicMaterial = new BasicMaterial();

  loader.complete.add(function(loader : TextureLoader, texture : TextureResource) : void
  {
    material.diffuseMap = texture;
  });

*/