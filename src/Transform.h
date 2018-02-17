#ifndef TRANSFORMH
#define TRANSFORMH

#include "Component.h"

#include <glm/glm.hpp>

class Transform : public Component
{
public:
  Transform();
  Transform(const glm::vec3 & pos);

  // unit: arbitrary units.
  // Physics has a units->meter conversino
  // Graphics works on arbitrary units?

  glm::vec3 mPos;   // x, y, z
  glm::vec3 mScale; // x, y, z
  glm::vec3 mRot;   // x, y, z, in radians
  
  glm::mat4 ConstructMatrix() const;
};

#endif
