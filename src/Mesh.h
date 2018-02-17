#ifndef _MESH_H_
#define _MESH_H_

#include "Geometry.h"
#include "Material.h"
#include "Joint.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <memory>

class Mesh;
class g;
class Shader;

class MeshBank
{
  typedef std::pair<std::string, Mesh*> MeshPair;
  std::map<std::string, Mesh*> mMeshes;

public:
  ~MeshBank();
  Mesh * AddMesh(const char * folder, const char * filename); 
  Mesh * GetMesh(const char * filename);
};

/*
  Mesh = material + geometry
    It is a group of polygons that use 1 material
*/

struct MeshEntry;

class Mesh
{
  // The importer deletes the scene on destruction
  Assimp::Importer mImporter;
  const aiScene * mScene; 

  Shader * mShader;

  aiMatrix4x4 mRootToLocal; // rootNodeSpace->localSpace
  
  std::vector<MeshEntry> mMeshEntries;
  std::vector<std::unique_ptr<Material> > mMaterials;
  
  // loops linearly through every channel, returns the one with the same name
  const aiNodeAnim * FindNodeAnim(const aiAnimation* anim, const char * name);

  // called in bonetransform. Is prolly recursive
  void ReadNodeHeirarchy(
    float AnimationTime, 
    const aiNode* pNode, 
    const aiMatrix4x4 & ParentTransform);

  unsigned FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
  unsigned FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
  unsigned FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
  void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
  void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
  void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

public:
  Mesh();
  void Load(const char * filepath);

  bool IsAnimated() const;

  // fills the joint's mFinalTransform vector
  void GetSketalTransforms(const aiScene * scene, float elapsedTime);

  void Render(Model * model); // we need model->GetSibling<Transform>()
  void RenderGeometryWithoutMaterialData(Shader * s);

  const aiScene * GetScene();
  // const aiMatrix4x4 & GetRootToLocal(); // rootNodeSpace->localSpace
  void SetShader(Shader * shader);
  Shader * GetShader();


};

struct MeshEntry
{
  MeshEntry(aiMesh * mesh);
  
  unsigned mMaterialIndex;
  std::shared_ptr<Geometry> mGeometry;
};

#endif
