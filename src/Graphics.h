#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
/*
http://www.horde3d.org/docs/manual.html
mModel = mMesh + mJoint

mMesh = mMaterial + mGeometry 
  group of polygons with 1 material
mJoint = something with skeleton

mMaterial = define appearance of a mMesh.
  Refernces and binds uniforms to a shader

mGeometry = loaded from binary files 
  store the polygonal data used by mModels and mMeshes. 
  Index  buffers 
  Vertex buffers (positions, normals, tangent, texcoord, joint weights)
  It can also include additional information about the skeleton of a model 
*/

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>

#include "System.h"
#include "RenderNode.h"

#include <glm/glm.hpp>
#define GLFW_CDECL 
#include <AntTweakBar.h>

#include "RunningAverage.h"

class RenderContext;
class RenderVisitor;
class Camera;
class DebugDrawer;

// banks
class ShaderBank;
class MeshBank;
class TextureBank;
class Model;
class Effect;
class ProcessingTree;

class Scene
{
private:
  std::set<Model*> mModels;
  std::set<ProcessingTree*> mProcessingTrees;
public:
  void Update(float dt);
  void Draw() const;
  void AddModel(Model * model);
  void AddProcessingTree(ProcessingTree * tree);
  void RemoveModel(Model * model);
};

class Graphics : public System
{
public:
  Graphics();

  virtual void Init();
  virtual void Update(float dt);
  virtual void Cleanup();

  int GetWidth() const; 
  int GetHeight() const;
  float GetAspectRatio() const; // w/h

  bool IsWireframe() const;
  void DrawWireframe(bool drawWireframe);

  void SetActiveCamera(Camera * camera);
  Camera * GetActiveCamera();

  void SetActiveScene(Scene * scene);
  Scene * GetActiveScene();

  const glm::mat4 & GetWorldToCam() const;
  const glm::mat4 & GetCamToProj() const;

  void AddEffect(Effect * effect);
  void RemoveEffect(Effect * effect);
  bool IsEffectActive(Effect * effect);

  DebugDrawer * GetDebugDrawer();

  // banks
  ShaderBank * GetShaderBank();
  MeshBank * GetMeshBank();
  TextureBank * GetTextureBank();

private:
  int mWidth, mHeight;
  bool mDrawWireframe;

  Camera * mActiveCamera;
  Scene * mActiveScene;

  glm::mat4 mWorldToCam;
  glm::mat4 mCamToProj;

  std::vector<Effect*> mEffects;

  std::unique_ptr< DebugDrawer> mDebugDrawer;

  // banks
  std::unique_ptr<ShaderBank> mShaderBank;  
  std::unique_ptr<MeshBank> mMeshBank;
  std::unique_ptr<TextureBank> mTextureBank;

  // tweak bar
  TwBar * mStats;
  float mFoo;
  float mFPS;
  RunningAverage mFrameTimes;

  static void ResizeCallback(int width, int height);
};

#endif
