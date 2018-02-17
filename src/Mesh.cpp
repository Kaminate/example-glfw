#include "Mesh.h"
#include "debug.h"

#include <GL/glfw.h>
#include <iostream>
#include <string>

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp> // c++ interface

#include "Engine.h" // temp
#include "Shader.h"

#include "GraphicsErrors.h"

// Mesh bank
MeshBank::~MeshBank()
{
  for (auto it = mMeshes.begin(); it != mMeshes.end(); ++it)
  {
    MeshPair pair = *it;
    Mesh * Mesh = pair.second;
    delete Mesh; // todo smart pointers
  }
}

Mesh * MeshBank::AddMesh(const char * folder, const char * filename)
{
  auto it = mMeshes.find(filename);
  ErrorIf(it != mMeshes.end(), "This mesh is already in the MeshBank");

  Mesh * myMesh = new Mesh();
  std::string filepath(folder);
  filepath += "/";
  filepath += filename;
  myMesh->Load(filepath.c_str());

  return mMeshes[filename] = myMesh;
}

Mesh * MeshBank::GetMesh(const char * name)
{
  auto it = mMeshes.find(name);
  ErrorIf(it == mMeshes.end(), "This Mesh is not in the MeshBank");
  const MeshPair & pair = *it;
  return pair.second;
}

void Mesh::Load( const char * filepath )
{
  // http://assimp.sourceforge.net/lib_html/usage.html

  mScene = mImporter.ReadFile(filepath, 
    aiProcess_CalcTangentSpace       | 
    aiProcess_Triangulate            |
    aiProcess_JoinIdenticalVertices  |
    aiProcess_SortByPType);
  
  // non-assigned fields in an mtl file lead to a "String too long" error.
  ErrorIf(!mScene, mImporter.GetErrorString());
  
  // load the geometry
  ErrorIf(!mScene->HasMeshes(), 
    "Maybe your file doesn't end with a newline character! \n"
    "Maybe we don't support your filetype! \n"
    "See: http://assimp.sourceforge.net/main_features_formats.html");
  
  for (unsigned i = 0; i < mScene->mNumMeshes; ++i)
  {
    aiMesh * mesh = mScene->mMeshes[i];
    // load geometry, joint
    mMeshEntries.push_back(MeshEntry(mesh));
  }

  mRootToLocal = mScene->mRootNode->mTransformation;
  mRootToLocal.Inverse();
  // globalInverseTransform represents rootBoneSpace -> localSpace
  
  // if (mScene->HasAnimations())
  // {
  //   for (unsigned i = 0; i < mScene->mNumAnimations; ++i)
  //   {
  //     //mGeometry->LoadAnimation(mScene->mAnimations[i]);
  //   }
  // }
  

  // why is this 0 ? 
  // std::cout << "Scene texture count: " << scene->mNumTextures << std::endl;
  // aiTexture * texture = scene->mTextures[0];

  // load the material
  for (unsigned i = 0; i < mScene->mNumMaterials; ++i)
  {
    aiMaterial * currMaterial = mScene->mMaterials[i];

    Material * newMaterial = new Material;
    newMaterial->Load(currMaterial);
    mMaterials.push_back(std::unique_ptr<Material>(newMaterial)); 
  }

  //aiMaterial * material = mScene->mMaterials[mesh->mMaterialIndex];
  //mMaterial->Load(material); // todo perhaps: Load(material, mTextures)

  //temp
  //float elapsedTime = Engine::Instance()->GetElapsedTime();
  //mJoint->GetSketalTransforms(elapsedTime);
}

// Material * Mesh::GetMaterial()
// {
//   //return mMaterial;
//   return 0; // temp
// }
// 
// Geometry * Mesh::GetGeometry()
// {
//   //return mGeometry;
//   return 0; //todo
// }

const aiScene * Mesh::GetScene()
{
  return mScene;
}

// const aiMatrix4x4 & Mesh::GetRootToLocal()
// {
//   return mRootToLocal;
// }

void Mesh::SetShader( Shader * shader )
{
  mShader = shader;
}

Mesh::Mesh() : mShader(0)
{

}

Shader * Mesh::GetShader()
{
  return mShader;
}

void Mesh::Render( Model * model )
{
  ErrorIf(!mShader, "Trying to draw a mesh with no shader attached!");
  mShader->SetActive(); // shader must be active to send data to
  CheckOpenGLErrors();

  // draw each meshEntry owned by this model
  for (auto it = mMeshEntries.begin(); it != mMeshEntries.end(); ++it)
  {
    MeshEntry & currMeshEntry = *it;
    
    Material * currMaterial = mMaterials[currMeshEntry.mMaterialIndex].get();
    Geometry * currGeometry = currMeshEntry.mGeometry.get();

    mShader->SendUniforms(currMaterial, model, currGeometry);
    CheckOpenGLErrors();

    mShader->SendAttributes(currGeometry);
    CheckOpenGLErrors();

    // index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currGeometry->GetIndexBuffer());
    int indexbuffersize; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexbuffersize);
    int indexbufferElementSize = sizeof(GLushort);
    int indexCount = indexbuffersize / indexbufferElementSize;

    // 0 is an offset, not a pointer!!!
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0); 
    CheckOpenGLErrors();
  }
}
void Mesh::RenderGeometryWithoutMaterialData(Shader * s)
{
  s->SetActive(); // shader must be active to send data to
  CheckOpenGLErrors();

  // draw each meshEntry owned by this model
  for (auto it = mMeshEntries.begin(); it != mMeshEntries.end(); ++it)
  {
    MeshEntry & currMeshEntry = *it;

    Geometry * currGeometry = currMeshEntry.mGeometry.get();

    s->SendAttributes(currGeometry);
    CheckOpenGLErrors();

    // index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currGeometry->GetIndexBuffer());
    int indexbuffersize; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexbuffersize);
    int indexbufferElementSize = sizeof(GLushort);
    int indexCount = indexbuffersize / indexbufferElementSize;

    // 0 is an offset, not a pointer!!!
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0); 
    CheckOpenGLErrors();
  }
}

MeshEntry::MeshEntry( aiMesh * mesh ) 
  : mMaterialIndex(mesh->mMaterialIndex) 
  , mGeometry( new Geometry(mesh))
{

}


// @brief
//   fills a vector with bone-to-local matrixes
// @param timeElapsed
//   in seconds
// @param transforms
//   final transform from bonespace->localspace for each bone
void Mesh::GetSketalTransforms( const aiScene * scene, float elapsedTime)
{

  //if (!scene->HasAnimations()) return;
  aiAnimation * myAnimation = scene->mAnimations[0];

  float TicksPerSecond 
    = (float)(myAnimation->mTicksPerSecond != 0 
    ? myAnimation->mTicksPerSecond 
    : 25.0f);
  float TimeInTicks = elapsedTime * TicksPerSecond;
  float AnimationTime = fmod(TimeInTicks, (float)myAnimation->mDuration);

  ReadNodeHeirarchy(AnimationTime, scene->mRootNode, aiMatrix4x4());

  //for (unsigned i = 0 ; i < numBones ; i++)
  //  transforms[i] = mBoneMatrixInfo[i].mFinalTransform; //local to anim.local
  //

  // testing
  // glm::mat4 transform = CopyaiMat(transforms[0]);
  // glm::vec4 glmzero(0,0,0,1);
  // std::cout << glmzero << std::endl;
  // std::cout << transform << std::endl;
  // std::cout << transform * glmzero << std::endl;

  //return transforms;
}

// loops linearly through every channel, returns the one with the same name
const aiNodeAnim * Mesh::FindNodeAnim(const aiAnimation* anim, const char * name)
{
  for (unsigned i = 0 ; i < anim->mNumChannels ; i++) 
  {
    const aiNodeAnim* pNodeAnim = anim->mChannels[i];

    if (strcmp(pNodeAnim->mNodeName.C_Str(), name) == 0)
      return pNodeAnim;
  }

  return 0;
}

void Mesh::ReadNodeHeirarchy(
  float AnimationTime, // in ticks
  const aiNode* pNode, 
  const aiMatrix4x4 & ParentTransform) 
{
  // the below two comments are probably(?) true
  // for the root node, this is identity
  // for the root node, this is NOT the rootnode's Transform
  aiMatrix4x4 nodeToParent(pNode->mTransformation); 

  const aiAnimation* pAnimation = mScene->mAnimations[0];
  const char * nodeName(pNode->mName.C_Str());
  const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);

  // TRS
  if (pNodeAnim)
  {
    //std::cout << nodeName << " is animated" << std::endl;
    // Interpolate scaling and generate scaling transformation matrix
    aiVector3D Scaling;
    CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
    aiMatrix4x4 scaleMatrix = ScaleMatrix(Scaling);


    // Interpolate rotation and generate rotation transformation matrix
    aiQuaternion RotationQ;
    CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim); 
    aiMatrix4x4 rotationMatrix(RotationQ.GetMatrix());

    // Interpolate translation and generate translation transformation matrix
    aiVector3D Translation;
    CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
    aiMatrix4x4 translationMatrix = TranslationMatrix(Translation);

    // Combine the above transformations (override nodeToparent)
    nodeToParent = translationMatrix * rotationMatrix * scaleMatrix;

  }

  // if pNode is RootNode, this matrix represents root to local
  aiMatrix4x4 boneToRoot = ParentTransform * nodeToParent;

  // C++11 range-based for loops
  for (auto meshit=mMeshEntries.begin(); meshit!=mMeshEntries.end(); ++meshit)
  {
    MeshEntry & currEntry = *meshit;
    Joint * currJoint = currEntry.mGeometry->GetJoint();
    Joint::BoneMap & currBoneMap = currJoint->mBoneMap;

    auto boneit = currBoneMap.find(nodeName);
    bool boneInMap = boneit != currBoneMap.end();

    if (boneInMap)
    {
      unsigned boneIndex = (*boneit).second;
      currJoint->mFinalTransform[boneIndex]
        = mRootToLocal                         //  ^  to local
        * boneToRoot                           // /|\ to animated parent to root
        * currJoint->mOffsetMatrix[boneIndex]; //  |  local space to bone space
    }
  }

  for (unsigned i = 0 ; i < pNode->mNumChildren ; i++) 
    ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], boneToRoot);
}

unsigned Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  ErrorIf(pNodeAnim->mNumScalingKeys == 0, "There aren't any scaling keys");

  for (unsigned i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++)
  {
    float nextKeyTime = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
    if (AnimationTime < nextKeyTime)
    {
      return i;
    }
  }

  ErrorIf(true, "Should have returned something by now.");

  return 0;
}
unsigned Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  assert(pNodeAnim->mNumRotationKeys> 0);

  for (unsigned i = 0 ; i < pNodeAnim->mNumRotationKeys- 1 ; i++) {
    if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
      return i;
    }
  }

  assert(0);

  return 0;
}
unsigned Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  assert(pNodeAnim->mNumPositionKeys > 0);

  for (unsigned i = 0 ; i < pNodeAnim->mNumPositionKeys- 1 ; i++) {
    if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
      return i;
    }
  }

  assert(0);

  return 0;
}

void Mesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  if (pNodeAnim->mNumScalingKeys == 1) 
  {
    Out = pNodeAnim->mScalingKeys[0].mValue;
    return;
  }
  // todo: find out what any of the stuff vvv does
  unsigned ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
  unsigned NextScalingIndex = ScalingIndex + 1;
  ErrorIf(NextScalingIndex >= pNodeAnim->mNumScalingKeys, 
    "There is no key for the next scaling index");
  
  aiVectorKey & currKey = pNodeAnim->mScalingKeys[ScalingIndex];
  aiVectorKey & nextKey = pNodeAnim->mScalingKeys[NextScalingIndex];

  float DeltaTime = (float)(nextKey.mTime - currKey.mTime);
  float Factor = (AnimationTime - (float)currKey.mTime) / DeltaTime;

  // perhaps your animation starts at 1 instead of 0
  ErrorIf(Factor < 0 || Factor > 1, "Invalid range for factor");
  

  const aiVector3D& Start = currKey.mValue;
  const aiVector3D& End   = nextKey.mValue;
  aiVector3D Delta = End - Start;
  Out = Start + Factor * Delta;
}
void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  // we need at least two values to interpolate...
  if (pNodeAnim->mNumRotationKeys == 1) {
    Out = pNodeAnim->mRotationKeys[0].mValue;
    return;
  }

  unsigned RotationIndex = FindRotation(AnimationTime, pNodeAnim);
  unsigned NextRotationIndex = (RotationIndex + 1);
  ErrorIf(NextRotationIndex >= pNodeAnim->mNumRotationKeys, 
    "There is no key for the next rotation index");

  aiQuatKey & currKey = pNodeAnim->mRotationKeys[RotationIndex];
  aiQuatKey & nextKey = pNodeAnim->mRotationKeys[NextRotationIndex];

  float DeltaTime = float(nextKey.mTime - currKey.mTime);
  float Factor = (AnimationTime - (float)currKey.mTime) / DeltaTime;
  ErrorIf(Factor < 0 || Factor > 1, "Invalid range for factor");
  const aiQuaternion& StartRotationQ = currKey.mValue;
  const aiQuaternion& EndRotationQ = nextKey.mValue;
  aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
  Out = Out.Normalize();
}
void Mesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  if (pNodeAnim->mNumPositionKeys == 1) {
    Out = pNodeAnim->mPositionKeys[0].mValue;
    return;
  }

  unsigned PositionIndex = FindPosition(AnimationTime, pNodeAnim);
  unsigned NextPositionIndex = PositionIndex + 1;
  ErrorIf(NextPositionIndex >= pNodeAnim->mNumPositionKeys, 
    "There is no key for the next position index" );

  aiVectorKey & currKey = pNodeAnim->mPositionKeys[PositionIndex];
  aiVectorKey & nextKey = pNodeAnim->mPositionKeys[NextPositionIndex];

  float DeltaTime = (float)(nextKey.mTime - currKey.mTime);
  float Factor = (AnimationTime - (float) currKey.mTime) / DeltaTime;
  ErrorIf(Factor < 0.0f || Factor > 1.0f, 
    "the time is not between the curr key / next key");
  const aiVector3D& Start = currKey.mValue;
  const aiVector3D& End = nextKey.mValue;
  aiVector3D Delta = End - Start;
  Out = Start + Factor * Delta;
}

bool Mesh::IsAnimated() const
{
  return mScene->HasAnimations();
}
