#include "ProcessingTree.h"

#include "Transform.h"
#include "Math.h"
#include "Engine.h"
#include "Graphics.h"
#include "DebugDraw.h"

#include <stack>
#include <vector>
#include <iomanip>


const unsigned ProcessingTree::sMaxIterations = 9;
unsigned ProcessingTree::privateIterationCount = 0;
ProcessingTree::Mat4Stack ProcessingTree::sMatrixStack;

void ProcessingTree::DrawStackLine(const glm::vec3 & p0, const glm::vec3 & p1) const
{
  const Transform * myTransform = GetSibling<Transform>();
  ErrorIf(!myTransform, "ProcessingTree must have a transform sibling");

  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();
  DebugDrawer * drawer = graphicsSystem.GetDebugDrawer();
  if (!drawer) 
    return;

  glm::mat4 transformMatrix = myTransform->ConstructMatrix(); // 4x4
  
  glm::vec4 p0vec4 = glm::vec4(p0,1);
  glm::vec4 p1vec4 = glm::vec4(p1,1);
  
  drawer->DrawLine
    ( (transformMatrix * sMatrixStack.top() * p0vec4).xyz()
    , (transformMatrix * sMatrixStack.top() * p1vec4).xyz()
    , glm::vec4(1,0,0,1));
}

void ProcessingTree::Render() const
{
  // draw
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();
  DebugDrawer * drawer = graphicsSystem.GetDebugDrawer();
  if (!drawer) 
    return;

  glm::vec3 limb(0, 0, mTrunkHeight);

  glm::mat4 branchToRoot = glm::translate(glm::mat4(), limb);

  sMatrixStack.push(branchToRoot);

  DrawStackLine(glm::vec3(), -limb);

  privateIterationCount = 0;
  Branch(mTrunkHeight);

  sMatrixStack.pop();
}

void ProcessingTree::Update( float dt )
{
  Clamp(mBranchAttenuationFactor, 0,1);
  
  privateSwayTime += dt;

  float freq = 1.0f / mSwayPeriod; // loops per second
  float b = freq * TWO_PI;
  float percent = sin(b * privateSwayTime) * 0.5f + 0.5f;
  
  privateCurrBranchAngleDeg 
    = percent * (mSwayMaxAngleDeg - mSwayMinAngleDeg) + mSwayMinAngleDeg;
}

void ProcessingTree::Branch(float lastLimbHeight) const
{
  float currLimbHeight = lastLimbHeight * mBranchAttenuationFactor;

  bool maxIterationsReached = ++privateIterationCount > sMaxIterations;
  bool minLimbHeightReached = currLimbHeight < mMinimumBranchHeight;
  if (maxIterationsReached || minLimbHeightReached)
  {
    --privateIterationCount;
    return;
  }
  
  glm::vec3 limb(0, 0, currLimbHeight);

  glm::mat4 trans = glm::translate(glm::mat4(), limb);
  glm::mat4 rot;

  rot = glm::rotate(glm::mat4(), privateCurrBranchAngleDeg, glm::vec3(0,1,0) );
  sMatrixStack.push(sMatrixStack.top() * rot * trans);
  DrawStackLine(glm::vec3(), -limb);
  Branch(currLimbHeight);
  sMatrixStack.pop();

  rot = glm::rotate(glm::mat4(), -privateCurrBranchAngleDeg, glm::vec3(0,1,0) );
  sMatrixStack.push(sMatrixStack.top() * rot * trans);
  DrawStackLine(glm::vec3(), -limb);
  Branch(currLimbHeight);
  sMatrixStack.pop();
  --privateIterationCount;
}

void ProcessingTree::ValidateValues()
{
  ErrorIf(mTrunkHeight < 0.1f, 
    "Trunk height must be at least 0.1f");
  ErrorIf(mBranchAttenuationFactor <= 0 || mBranchAttenuationFactor >= 1, 
    "Branch Attenuation Factor must be bounded [0,1]");
  ErrorIf(mMinimumBranchHeight <= 0, 
    "Minimum branch height must be greater than 0");
}

ProcessingTree::ProcessingTree
  ( bool randomInitialAngle 
  , float trunkHeight /*= 0.4f */
  , float branchAttenuationFactor /*= 0.6f */
  , float swayPeriod /*= 2.0f */
  , float swayMinAngleRad /*= 25.0f * DEG_TO_RAD */
  , float swayMaxAngleRad /*= 45.0f * DEG_TO_RAD*/
  , float minimumBranchHeight /*= 0.1f*/)
  : mTrunkHeight(trunkHeight)
  , mBranchAttenuationFactor(branchAttenuationFactor)
  , mSwayPeriod(swayPeriod)
  , mSwayMinAngleDeg(swayMinAngleRad)
  , mSwayMaxAngleDeg(swayMaxAngleRad)
  , privateSwayTime(0)
  , mMinimumBranchHeight(minimumBranchHeight)
{
  if (randomInitialAngle)
    privateSwayTime += RandomFloat() * swayPeriod;
  
  ValidateValues();
}
