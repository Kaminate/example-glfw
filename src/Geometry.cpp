#include "Geometry.h"

#include <vector>

#include "debug.h"
#include <iostream>


#include "Math.h"


Geometry::Geometry( aiMesh * mesh )
  : mJoint(new Joint(mesh))
{
  Load(mesh);
}


/*
  @param size
    size of an individual attribute
    example: size of UV attribute = 2 * sizeof(float)
*/
void Geometry::LoadVertexAttribute(
  int count, 
  VertAttrib attribute, 
  int attributeSize, 
  void * data)
{
  if (count == 0) 
    return; // perhaps should error

  auto it = mBufferObjects.find(attribute);
  bool alreadyLoaded = it != mBufferObjects.end();
  ErrorIf(alreadyLoaded, "This attribute has already been loaded");

  GLuint buffer;

  glGenBuffers(1, &buffer); // it will crash here if glew is uninitialized
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    attributeSize * count,
    data,
    GL_STATIC_DRAW);

  mBufferObjects[attribute] = buffer;
}

void Geometry::LoadIndicies(int faceCount, aiFace * faces)
{
  std::vector<GLushort> indicies;

  for (int i = 0; i < faceCount; ++i)
  {
    const aiFace & face = faces[i];
    ErrorIf(face.mNumIndices != 3, "A face doesn't have exactly 3 verticies");

    indicies.push_back(face.mIndices[0]);
    indicies.push_back(face.mIndices[1]);
    indicies.push_back(face.mIndices[2]);
  }

  glGenBuffers(1, & mIndexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    indicies.size() * sizeof(GLushort), 
    &indicies[0], 
    GL_STATIC_DRAW);
}

//@brief
//  helper class in generating bone-related vertex attributes     
//  similar to http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html
class BoneData
{
  std::vector<unsigned> mBoneIds;  // each vertex has 4 of these
  std::vector<float> mBoneWeights; // each vertex has 4 of these
  std::vector<unsigned> mNumAdded; // each vertex has 1 of these
  // We store them in separate arrays so we can call glBufferData

public:
  static const int BONES_PER_VERTEX = 4;

  BoneData(unsigned numVertices)
      : mBoneIds    (numVertices * BONES_PER_VERTEX, 0)
      , mBoneWeights(numVertices * BONES_PER_VERTEX, 0)
      , mNumAdded   (numVertices, 0)
  {}
  
  void AddBoneData( unsigned vertexId, unsigned boneId, float weight)
  {
    unsigned & numAdded = mNumAdded[vertexId];

    ErrorIf(numAdded == BONES_PER_VERTEX, 
      "Cannot add anymore bones. Max is %i", BONES_PER_VERTEX);

    mBoneWeights[vertexId * BONES_PER_VERTEX + numAdded] = weight;
    mBoneIds    [vertexId * BONES_PER_VERTEX + numAdded] = boneId;

    ++numAdded;
  }

  const unsigned * GetBoneIds() const
  {
    return &mBoneIds[0];
  }
  const float * GetBoneWeights() const
  {
    return &mBoneWeights[0];
  }

};

// creates the bonemap and boneinfo vector
// loads the boneweight vertex attribute
// loads the boneindex vertex attribute
void Geometry::LoadVertexBoneData(aiMesh * mesh)
{
  BoneData myBoneData(mesh->mNumVertices);
  // for each bone
  for (unsigned boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
  {
    aiBone * currBone = mesh->mBones[boneIndex];

    unsigned numWeights = currBone->mNumWeights;

    // for each weight
    for (unsigned weightIndex = 0; weightIndex < numWeights; ++weightIndex)
    {
      aiVertexWeight & currWeight = currBone->mWeights[weightIndex];

      myBoneData.AddBoneData(
        currWeight.mVertexId, 
        boneIndex, 
        currWeight.mWeight);
    }
  }
  LoadVertexAttribute(
    mesh->mNumVertices * BoneData::BONES_PER_VERTEX, 
    BONE_ID, sizeof(unsigned),
    (void*) myBoneData.GetBoneIds());
  LoadVertexAttribute(
    mesh->mNumVertices * BoneData::BONES_PER_VERTEX, 
    BONE_WEIGHT, sizeof(float),
    (void*) myBoneData.GetBoneWeights());
}


/*
void Geometry::LoadAnimation(aiAnimation * animation)
{
  animation->mDuration;
  animation->mName;
  animation->mTicksPerSecond;

  unsigned numChannels = animation->mNumChannels;
  aiNodeAnim ** myChannels = animation->mChannels; // each affects a single node
  for (unsigned i = 0; i < animation->mNumChannels; ++i)
  {
    aiNodeAnim * myNodeAnimation = animation->mChannels[i];
    
    myNodeAnimation->mNodeName;

    myNodeAnimation->mNumPositionKeys;
    myNodeAnimation->mPositionKeys;
    aiVectorKey & myKey = myNodeAnimation->mPositionKeys[0];
    myKey.mTime;
    myKey.mValue;

    myNodeAnimation->mNumRotationKeys;
    myNodeAnimation->mRotationKeys;

    myNodeAnimation->mNumScalingKeys;
    myNodeAnimation->mScalingKeys;
    
    myNodeAnimation->mPostState;
    myNodeAnimation->mPreState;
    
  }


  //  Not supported (?)
  // unsigned numMeshChannels = animations->mNumMeshChannels; // each affects a single mesh
  // aiMeshAnim ** myMeshChannels = animations->mMeshChannels;
  // for (unsigned i = 0; i < numMeshChannels; ++i)
  // {
  //   aiMeshAnim * myMeshAnim = myMeshChannels[i];
  //   myMeshAnim->mName;
  // 
  //   for (unsigned keyIndex = 0; keyIndex < myMeshAnim->mNumKeys; ++keyIndex)
  //   {
  //     aiMeshKey * myKey = myMeshAnim->mKeys[keyIndex];
  //     myKey->mTime;
  //     myKey->mValue; // index into aiMesh::mAnimMeshes
  //   }
  // }
  // 
}
*/

void Geometry::Load( aiMesh * mesh )
{
  if (mesh->HasPositions())
    LoadVertexAttribute(mesh->mNumVertices,
    POSITION, sizeof(aiVector3D), mesh->mVertices);

  ErrorIf (!mesh->HasFaces(), "This mesh needs a face");
  LoadIndicies(mesh->mNumFaces, mesh->mFaces);

  if (mesh->HasBones())
    LoadVertexBoneData(mesh);

  // wait these are stored in  a vector3d
  ErrorIf(mesh->mNumUVComponents[0] > 2, "right now only support uv, not uvw");
  if (mesh->HasTextureCoords(0)) // todo: parameter
  {
    // lets make our texcoords have u,v, not u,v,w
    std::vector<float> texcoords;
    texcoords.reserve(2 * mesh->mNumVertices);
    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
      texcoords.push_back(mesh->mTextureCoords[0][i].x);
      texcoords.push_back(mesh->mTextureCoords[0][i].y);
    }
    LoadVertexAttribute(mesh->mNumVertices, // todo: parameter
      TEXCOORD, sizeof(float) * 2, &texcoords[0]);
  }


  //if (mesh->HasVertexColors(0)) // todo: parameter
  //  LoadVertexAttribute(mesh->mNumVertices,
  //  COLOR, mesh->GetNumColorChannels() * sizeof(float), mesh->mColors);
  //
  //if (mesh->HasTangentsAndBitangents())
  //{
  //  LoadVertexAttribute(mesh->mNumVertices,
  //    TANGENT, sizeof(aiVector3D), mesh->mTangents);
  //
  //  LoadVertexAttribute(mesh->mNumVertices,
  //    BITANGENT, sizeof(aiVector3D), mesh->mBitangents);
  //}
  
  
}


GLuint Geometry::GetIndexBuffer()
{
  return mIndexBuffer;
}

GLuint Geometry::GetAttribute( VertAttrib attrib )
{
  std::map<VertAttrib, GLuint>::iterator it = mBufferObjects.find(attrib);
  ErrorIf(it == mBufferObjects.end(), 
    "Geometry does not have this attribtue" );
  return (*it).second;
}

Joint * Geometry::GetJoint()
{
  return mJoint.get();
}

// more thoughts


/*
http://www.horde3d.org/docs/manual.html

Geometry resources are loaded from binary files and store the polygonal data 
used by models and meshes. 

The resource contains triangle indices and several streams for 
vertex attributes like positions, normals, tangents, texture coordinates and 
joint weights. 

It can also include additional information about the skeleton of a model and 
separate data streams for morph targets.
*/

// todo: GeometryManager


/*
inspiration on how to manage loading models using the
open asset import library (assimp) from:

http://ogldev.atspace.co.uk/www/tutorial22/tutorial22.html

I especially like the way they represent their mesh, so i'm copying some of
that and improving (cough) on some of it

Mesh = MeshEntry + Textures(attributes to bind to shader)
^          ^         ^
|          |         |
model   actual mesh   material
*/
