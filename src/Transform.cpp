#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
Transform::Transform()
  : mPos(0,0,0)
  , mScale(1,1,1)
  , mRot(0,0,0)
{
}

Transform::Transform( const glm::vec3 & pos )
  : mPos(pos)
  , mScale(1,1,1)
  , mRot(0,0,0)
{

}

glm::mat4 Transform::ConstructMatrix() const
{
  glm::mat4 scale(
    mScale.x,0,0,0,
    0,mScale.y,0,0,
    0,0,mScale.z,0,
    0,0,0,1);
  
  glm::mat4 translate = 
    glm::translate(glm::mat4(1.0f), mPos);

  static float angle = 0;
  angle += 0.25f;
  glm::mat4 rotate = glm::mat4(1.0f);//glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0,1,0));

  return translate * rotate * scale;
}
