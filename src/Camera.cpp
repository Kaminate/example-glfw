#include "Camera.h"
#include "debug.h"
#include "Transform.h"
#include "Graphics.h"
#include "Engine.h"

#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Math.h"
#include <iostream>

Camera::Camera(const glm::vec3 & worldSpaceUp)
  : mTarget(0) 
  , mFieldOfViewYDeg(80)
  , mNear(0.1f)
  , mFar(1000.0f)
  , mWorldSpaceUp(worldSpaceUp)
  , mType(UNINITIALIZED)
{
}

glm::mat4 Camera::GetViewMatrix() const
{ 
  const Transform * cameraTransform = GetSibling<Transform>();
  ErrorIf(!cameraTransform, "No transforms");

  glm::vec3 targetPos;
  switch(mType)
  {
    case DIRECTIONAL:
    {
      targetPos = cameraTransform->mPos + mViewDirection;
      break;
    }
    case TARGETED_AT_GAMEOBJECT:
    {
      ErrorIf(!mTarget, "Camera doesnt have a target");
      Transform * targetTransform = mTarget->GetComponent<Transform>();
      targetPos = targetTransform->mPos;
      break;
    }
    case TARGETED_AT_POSITION:
    {
      targetPos = mTargetPosition;
    }
  }

  glm::mat4 view = glm::lookAt(cameraTransform->mPos, targetPos, mWorldSpaceUp);
  std::cout << "view: " << std::endl << view << std::endl;
  return view;
}

glm::mat4 Camera::GetProjMatrix() const
{
  glm::mat4 perspective = glm::perspective(
    mFieldOfViewYDeg, 
    Engine::GetSystem<Graphics>().GetAspectRatio(), 
    mNear, 
    mFar);

  std::cout << "perspective: " << std::endl << perspective << std::endl;

  return perspective;
}

void Camera::SetType( Type type )
{
  mType = type;
}

void Camera::SetTarget(GameObject * target)
{
  ErrorIf(mType != TARGETED_AT_GAMEOBJECT, "This camera isn't object-targeted");
  mTarget = target;
}

void Camera::SetTarget( const glm::vec3 & targetPos )
{
  ErrorIf(mType != TARGETED_AT_POSITION, "This camera isn't pos-targeted");
  mTargetPosition = targetPos;
}

void Camera::SetViewDirection( const glm::vec3 & viewDir )
{
  ErrorIf(mType != DIRECTIONAL, "This camera isn't directional type");
  mViewDirection = viewDir;
}

