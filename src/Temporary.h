#ifndef _TEMPORARY_H_
#define _TEMPORARY_H_

#include <memory>

#include "gl/glew.h"
#include "gl/glfw.h"
#include "System.h"
#include "Effect.h"
#include "Texture.h"
#include "DebugDraw.h"

#include <AntTweakBar.h>

class GameObject;
class Shader;
class Model;
class Mesh;


class Glow : public Effect
{
  Shader * mBlurVShader;
  Shader * mBlurHShader;
  Shader * mBlendShader;

  GLuint mGlowFrameBuffer[2];
  Texture * mTexture[2];
public:
  virtual void Init();
  virtual void Draw();
};

class Temporary : public System
{
  TwBar * mTreeBar;
  void CreateCubeAtPosition(float x, float y, float z);
  void CreateDude();
  void CreateLanternGuy();
  void CreateFBXAnimTri();
  void CreateDAEAnimTri();
  void CreateOBJAnimTri();
  void CreateCamera();
  Glow * mGlow;
  Shader * mPosTexMvpBonesShader;
  Shader * mPosMvpBonesShader;
  Shader * mPosTexMvpShader;
  Shader * mPosMvpShader;

  Model * mTriFBX;
  Model * mTriOBJ;
  Model * mTriDAE;
  std::unique_ptr<DebugDrawLayer> mDebugLayer;

public:
  Temporary();
  virtual void Init();
  virtual void Update(float dt);
  virtual void Cleanup();
  void CreatePosTexMvpBonesShader();
  void CreatePosMvpBonesShader();
  void CreatePosTexMvpShader();
  void CreatePosMvpShader();

};

#endif