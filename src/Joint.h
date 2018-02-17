#ifndef _JOINT_H_
#define _JOINT_H_

#include <string>
#include <vector>
#include <map>

#include "Math.h"

#include <assimp/mesh.h>
#include <assimp/scene.h>

/*
  Joints have something to do with the skeleton
*/
class Joint 
{
  // maps from const char * boneName to unsigned boneIndex
  // the boneIndex indexes into the mOffsetMatrix and mFinalTransform
public: // todo: make this not public...
  typedef std::map<std::string, unsigned> BoneMap;
  BoneMap mBoneMap;

  //std::vector<std::string> mBoneNames;

  // aka BoneOffset in ogldev tutorial
  std::vector<aiMatrix4x4> mOffsetMatrix; // localSpace to bindspace
  std::vector<aiMatrix4x4> mFinalTransform;// local to bind to animated localspace

public:
  Joint(aiMesh * mesh);

};

#endif
