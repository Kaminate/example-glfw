#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include <GL/glew.h>
#include <GL/glfw.h> // GLuint
#include <assimp/mesh.h>
#include <assimp/anim.h>
#include <assimp/scene.h>
#include <map>
#include <memory>
#include <vector>
#include "ShaderVariableTypes.h"
#include <glm/glm.hpp>
#include "Math.h"

#include "Joint.h"

// TLDR: The geometry holds all of an object's vertex attribute information


class Geometry
{
public:
  
  Geometry(aiMesh * mesh);
  
  GLuint GetAttribute(VertAttrib attrib);
  GLuint GetIndexBuffer();

  Joint * GetJoint();
private:

  // pos, normal, color, uv, boneweight, boneindex, etc
  std::map<VertAttrib, GLuint> mBufferObjects; 

  GLuint mIndexBuffer;
  std::unique_ptr<Joint> mJoint;

  // helper functinos
  void Load(aiMesh * mesh);

  void LoadVertexAttribute(
    int count, 
    VertAttrib attribute, 
    int attributeSize, 
    void * data);

  void LoadIndicies(int faceCount, aiFace * faces);

  // creates the bonemap and boneinfo vector
  // loads the boneweight vertex attribute
  // loads the boneindex vertex attribute
  void LoadVertexBoneData(aiMesh * mesh);
};

#endif
