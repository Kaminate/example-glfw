#ifndef _MODEL_H_
#define _MODEL_H_

#include "Component.h"
#include "Mesh.h"
#include "Joint.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;

class Model : public Component
{ 
  bool mVisible;
  Mesh * mMesh;
  // todo: skeleton
  
  glm::mat4 mModelToWorld; // constructed 

  void ConstructWorldMatrix(); // called during graphics update

public:

  // will return false if we're still waiting for our
  // mesh or material to load, in which case we won't render
  //bool IsInitialized() const;
  //void AttachChild(Model * child, Vector4 attachPoint);

  Model();
  ~Model();

  void SetMesh(Mesh * mesh); // constness?
  
  Mesh * GetMesh();
  
  void Draw();
  
  void Update(float dt); // called once per loop, updates world matrix and skeleton

  const glm::mat4 & GetModelToWorld() const;
  bool GetVisible() const;
  void SetVisible(bool visible);
};

#endif
