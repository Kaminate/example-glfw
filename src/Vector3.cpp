#include "DebugDraw.h"

#include "debug.h"
#include "Engine.h"
#include "Graphics.h"
#include "GraphicsErrors.h"
#include "Shader.h"

/////////////////
// DebugDrawer //
/////////////////

DebugDrawer::DebugDrawer() : mShader(0), mBufferPositions(-1)
{

}

void DebugDrawer::Init()
{
  mPositions.reserve(BUFFER_SIZE);
  //mColors.reserve(BUFFER_SIZE);
  glGenBuffers(1, &mBufferPositions); // it will crash if glew is uninitialized
  //glGenBuffers(1, &mBufferColors); // it will crash if glew is uninitialized
  Clear();

  Graphics & system = Engine::GetSystem<Graphics>();
  ShaderBank * bank = system.GetShaderBank();
  ErrorIf(!bank, "Shader bank is null...");
  mShader = bank->CreateShader
    ( "Debug draw shader"
    , "resources/shaders"
    , "pos_mvp.v.txt"
    , "red.f.txt");
  mShader->BindUniform(MVP_MATRIX, MATRIX, "mvp");
  mShader->BindAttribute(POSITION, "coord3d");

  CheckOpenGLErrors();
}

void DebugDrawer::Render()
{
  if (mPositions.size() > BUFFER_SIZE)
    std::cout << "Debug drawing too many lines" << std::endl;

  ErrorIf(!mShader, "You  need to call init before using debugdrawer");
  mShader->SetActive();

  CheckOpenGLErrors();

  // mvp
  Graphics & system = Engine::GetSystem<Graphics>();
  glm::mat4x4 mvp = system.GetCamToProj() * system.GetWorldToCam();
  mShader->SendData(MVP_MATRIX, mvp);

  CheckOpenGLErrors();

  // update our buffer with our new data
  glBindBuffer(GL_ARRAY_BUFFER, mBufferPositions);

  CheckOpenGLErrors();

  // todo: make sure float is 4 bytes...
  float * data = (float*) glMapBufferRange(
    GL_ARRAY_BUFFER,0,BUFFER_SIZE*3*4, GL_MAP_WRITE_BIT);
  CheckglMapBufferRangeErrors();
  
  // copy the data
  memcpy(data, &mPositions[0], BUFFER_SIZE*3*4);

  glUnmapBuffer(GL_ARRAY_BUFFER); // unmap the buffer?
  CheckOpenGLErrors();

  // tell the shader to use this attribute when rendering (?)
  glEnableVertexAttribArray(mShader->GetAttribute(POSITION));

  // move data stored in GL_ARRAY_BUFFER to the shader 
  // at the first argument's attribute
  glVertexAttribPointer(
    mShader->GetAttribute(POSITION), // index of the generic vertex atribute to be modified
    3, // 1, 2, 3, or 4
    GL_FLOAT, // data type of the component
    GL_FALSE, // normalized
    0, // byte offset between the starts of 2 attributes
    0); // offset of first component of attribute in the array currently bound

  CheckOpenGLErrors();

  // Render
  glDrawArrays(GL_LINES, 0, mPositions.size());

  CheckOpenGLErrors();

}

void DebugDrawer::Update()
{
  if (!mPositions.empty())
  {
    Render();
  }
  
  Clear(); // clears arrays and respecifies buffers
}

void DebugDrawer::Clear()
{
  // Buffer re-specification
  // http://www.opengl.org/wiki/Buffer_Object_Streaming
 
  // orphan the old buffers with a new buffer of the same size.
  // Bad way: Buffers have to wait to draw, then replace data
  // Faster way: Buffers will still draw from the orphaned buffer, 
  //   and now we're have a new buffer we're using under the same handle
  CheckOpenGLErrors();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferPositions);
  
  CheckOpenGLErrors();

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    BUFFER_SIZE *  4 * 4, // assumed float is 4 bytes, 4 floats (r,g,b,a), 11kb
    0, // data
    GL_STREAM_DRAW);//GL_DYNAMIC_DRAW);

  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferColors);
  //glBufferData(
  //  GL_ELEMENT_ARRAY_BUFFER,
  //  BUFFER_SIZE *  4 * 4, // assumed float is 4 bytes, 4 floats (r,g,b,a), 11kb
  //  0, // data
  //  GL_STREAM_DRAW);//GL_DYNAMIC_DRAW);

  CheckOpenGLErrors();

  mPositions.clear();
  //mColors.clear();
}

void DebugDrawer::AddLayer(DebugDrawLayer * layer)
{
  mLayers.push_back(layer);
}

void DebugDrawer::RemoveLayer(DebugDrawLayer * layer)
{
  std::remove(mLayers.begin(), mLayers.end(), layer);
}

void DebugDrawer::DrawLine(
  const glm::vec3 & p0, 
  const glm::vec3 & p1, 
  const glm::vec4 & color)
{
  mPositions.push_back(p0);
  mPositions.push_back(p1);
  //mColors.push_back(color);
  //mColors.push_back(color);
}

// helper function
void SetVal (glm::vec3 & vec, Plane p, float val)
{
  switch (p)
  {
    case X:
      vec.x = val;
      break;
    case Y:
      vec.y = val;
      break;
    case Z:
      vec.z = val;
      break;
  }
}

// helper function
void GetPoint(
  Plane sinPlane, 
  Plane cosPlane, 
  float angle, 
  float radius,
  glm::vec3 & vec)
{
  SetVal(vec, sinPlane, radius * sin(angle));
  SetVal(vec, cosPlane, radius * cos(angle));
}

void DebugDrawer::DrawCircle(
  Plane sinPlane, 
  Plane cosPlane, 
  const glm::vec3 & center, 
  float radius, 
  const glm::vec4 & color,
  unsigned numSides/* = 8*/)
{
  ErrorIf(sinPlane == cosPlane, "They gotta be different");
  ErrorIf(numSides < 3, "Circles must have a smoothness of at least 3");
  
  float angleRad = 2.0f * 3.14159f / numSides;
  float currAngle = 0;
  glm::vec3 lastPoint;

  GetPoint(sinPlane, cosPlane, currAngle, radius, lastPoint);

  while (numSides)
  {
    currAngle += angleRad;
    glm::vec3 currPoint;
    GetPoint(sinPlane, cosPlane, currAngle, radius, currPoint);

    DrawLine(lastPoint, currPoint, color);

    lastPoint = currPoint;

    --numSides;
  }
}

void DebugDrawer::DrawSphere(
  const glm::vec3 & center, 
  float radius, 
  const glm::vec4 & color,
  unsigned numSides /*= 8*/)
{
  DrawCircle(X, Y, center, radius, color, numSides);
  DrawCircle(X, Z, center, radius, color, numSides);
  DrawCircle(Y, Z, center, radius, color, numSides);
}

DebugDrawer::~DebugDrawer()
{
  while (!mLayers.empty())
  {
    DebugDrawLayer * layer = mLayers[0];
    layer->UnhookFromDrawer();
  }
}

void DebugDrawer::DrawArrow( 
  const glm::vec3 & pos, 
  const glm::vec3 & dir_and_magnitude, 
  const glm::vec4 & color )
{
  if (glm::length(dir_and_magnitude) == 0) return;

  glm::vec3 head = pos + dir_and_magnitude;
  DrawLine(pos, head, color);

  glm::vec3 xAxis = -dir_and_magnitude;
  // get an arbitrary perpendicular vector
  glm::vec3 yAxis = glm::cross(xAxis, glm::vec3(1,0,0));
  if (glm::length(yAxis) == 0)
    yAxis = glm::cross(xAxis, glm::vec3(0,1,0));
  glm::vec3 zAxis = glm::cross(xAxis, yAxis);

  xAxis = glm::normalize(xAxis);
  yAxis = glm::normalize(yAxis);
  zAxis = glm::normalize(zAxis);

  glm::mat3x3 rotatedToStandard(xAxis, yAxis, zAxis);
  glm::mat3x3 standardToRotated = glm::transpose(rotatedToStandard);

  const unsigned numHooks = 3;
  glm::vec3 firstPoint;
  glm::vec3 lastPoint;
  for (unsigned i = 0; i < numHooks; ++i)
  {
    float angle = 3.14159f * 2.0f * i / numHooks;
    float rad30 = ToRadians(30);
    glm::vec3 hook(cos(rad30), 0, sin(rad30));

    glm::mat3 rotX = RotationMatrixX(angle);
    hook = rotX * hook;
    hook = rotatedToStandard * hook;
    hook *= glm::length(dir_and_magnitude);
    hook /= 5.0f;

    glm::vec3 currPoint = head + hook;

    if (i == 0)
      firstPoint = currPoint;
    else
      DrawLine(lastPoint, currPoint, color);

    lastPoint = currPoint;

    DrawLine(head, currPoint, color);
  }
  
  DrawLine(firstPoint, lastPoint, color);
}

////////////////////
// DebugDrawLayer //
////////////////////

DebugDrawLayer::DebugDrawLayer(const std::string & name)
  : mName(name)
  , mDrawer(0)
  , mVisible(true)
{

}

DebugDrawer * DebugDrawLayer::RequestDebugDrawer()
{
  if (!mVisible) 
    return 0;
  else 
    return mDrawer;
}

void DebugDrawLayer::Init()
{
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();
  mDrawer = graphicsSystem.GetDebugDrawer();
  mDrawer->AddLayer(this);
}

void DebugDrawLayer::UnhookFromDrawer()
{
  if (mDrawer)
  {
    mDrawer->RemoveLayer(this);
    mDrawer = 0;
  }
} 

bool DebugDrawLayer::GetVisible() const
{
  return mVisible;
}

void DebugDrawLayer::SetVisible(bool v)
{
  mVisible = v;
}

DebugDrawLayer::~DebugDrawLayer()
{
  UnhookFromDrawer();
}
