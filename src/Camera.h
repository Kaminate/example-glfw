#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include "Component.h"

class GameObject;

class Camera : public Component
{
public:
  enum Type {TARGETED_AT_GAMEOBJECT, TARGETED_AT_POSITION, DIRECTIONAL, UNINITIALIZED};
  // todo: enum Projection {ORTHOGRAPHIC, PERSPECTIVE};
  Camera(const glm::vec3 & worldSpaceUp);
  void SetType(Type type);

  void SetTarget(GameObject * target);
  void SetTarget(const glm::vec3 & targetPos);
  void SetViewDirection(const glm::vec3 & viewDir);

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjMatrix() const;
  
private:
  
  float mNear;
  float mFar;
  float mFieldOfViewYDeg;
  glm::vec3 mWorldSpaceUp;

  // dependent on type
  Type mType;
  GameObject * mTarget;
  glm::vec3 mViewDirection;
  glm::vec3 mTargetPosition;

};

#endif
