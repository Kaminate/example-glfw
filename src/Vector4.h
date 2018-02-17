#ifndef __DEBUG_DRAW_H_ 
#define __DEBUG_DRAW_H_ 

#include <GL/glew.h>
#include <GL/glfw.h> // GLuint
#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "Math.h"

class DebugDrawer;
class DebugDrawLayer;
class Shader;

enum Plane { X, Y, Z};
// Owned by the graphics system
// actually draws things each frame
class DebugDrawer
{
  Shader * mShader;
  GLuint mBufferPositions;
  //GLuint mBufferColors;

  // updated each frame
  std::vector< glm::vec3 > mPositions;
  //std::vector< glm::vec4 > mColors;
  std::vector< DebugDrawLayer *> mLayers; // layers aren't owned by us
  const static int BUFFER_SIZE = 4096; // 4096 points, 2048 lines
  void Clear();
  void Render();

public:
  DebugDrawer();
  void Init();
  void Update();
  
  void AddLayer(DebugDrawLayer * layer);
  void RemoveLayer(DebugDrawLayer * layer);
  void DrawArrow(
    const glm::vec3 & pos, 
    const glm::vec3 & dir_and_magnitude, 
    const glm::vec4 & color);
  void DrawLine(
    const glm::vec3 & begin, 
    const glm::vec3 & end, 
    const glm::vec4 & color);
  void DrawSphere(
    const glm::vec3 & center, 
    float radius, 
    const glm::vec4 & color,
    unsigned numSides = 12);
  void DrawCircle(Plane sinPlane, 
    Plane cosPlane, 
    const glm::vec3 & center, 
    float radius, 
    const glm::vec4 & color,
    unsigned numSides = 12
    );
  ~DebugDrawer();
};

class DebugDrawLayer
{
  DebugDrawer * mDrawer;
  std::string mName;

  bool mVisible;

public:
  DebugDrawLayer(const std::string & name);
  ~DebugDrawLayer();
  DebugDrawer * RequestDebugDrawer();
  void Init();
  void UnhookFromDrawer();
  bool GetVisible() const;
  void SetVisible(bool v);
};

#endif
