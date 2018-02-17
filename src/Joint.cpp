#include "Joint.h"
#include "Mesh.h"
#include "debug.h"
#include <assimp/anim.h>


// BoneInfo::BoneInfo( const aiMatrix4x4 & offsetMatrix )
//   : mOffsetMatrix(offsetMatrix)
// {
// }

// BoneInfo::BoneInfo() 
//   : mOffsetMatrix() // for std::vector
// {} 

Joint::Joint(aiMesh * mesh)
{
  for (unsigned boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
  {
    aiBone * currBone = mesh->mBones[boneIndex];
    
    // add the bone to the map
    mBoneMap[currBone->mName.C_Str()] = boneIndex;


    //mBoneMatrixInfo.push_back(BoneInfo(currBone->mOffsetMatrix));
    mOffsetMatrix.push_back(currBone->mOffsetMatrix);
    mFinalTransform.push_back(aiMatrix4x4());
    
  }
}
