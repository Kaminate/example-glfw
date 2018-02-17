#ifndef __PROCESSING_TREE_H_
#define __PROCESSING_TREE_H_

#include <stack>
#include <vector>

#include "Component.h"
#include "Math.h"

// Based off: http://processing.org/examples/tree.html
class ProcessingTree : public Component
{
public: 
  // public so it can be ant tweaked
  float mTrunkHeight;
  float mBranchAttenuationFactor;
  float mSwayPeriod;  
  float mSwayMinAngleDeg;
  float mSwayMaxAngleDeg;
  float mMinimumBranchHeight;
  ProcessingTree
    ( bool randomInitialAngle = false
    , float trunkHeight = 1.3f
    , float branchAttenuationFactor = 0.6f
    , float swayPeriod = 2.0f
    , float swayMinAngleDeg = 20.0f
    , float swayMaxAngleDeg = 35.0f
    , float minimumBranchHeight = 0.1f);
  void Render() const; // draw lines
  void Update(float dt); // compute angle

private:
  float privateSwayTime;
  float privateCurrBranchAngleDeg;
  
  
  const static unsigned sMaxIterations;
  typedef std::stack< glm::mat4 , std::vector<glm::mat4> > Mat4Stack;
  static unsigned privateIterationCount;
  static Mat4Stack sMatrixStack;
  void DrawStackLine(const glm::vec3 & p0, const glm::vec3 & p1) const;
  void Branch(float lastLimbHeight) const;
  void ValidateValues();
};


#endif
