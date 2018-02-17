#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <AntTweakBar.h>

#include "Graphics.h"
#include "debug.h"
#include "Engine.h"
#include "Camera.h"
#include "Shader.h"  // ShaderBank
#include "Mesh.h"    // MeshBank
#include "Texture.h" // TextureBank
#include "Model.h"
#include "Effect.h"
#include "GraphicsErrors.h"
#include "n8time.h"
#include "DebugDraw.h"

#include "ProcessingTree.h"

#define TWEAK

// scene

void Scene::AddProcessingTree( ProcessingTree * tree )
{
  auto it = mProcessingTrees.find(tree);
  bool inSet = it != mProcessingTrees.end();
  ErrorIf(inSet, "Already in scene");
  mProcessingTrees.insert(tree);
}

void Scene::AddModel( Model * model )
{
  auto it = mModels.find(model);
  bool modelInSet = it != mModels.end();
  ErrorIf(modelInSet, "Model already in scene");
  mModels.insert(model);
}

void Scene::RemoveModel( Model * model )
{
  auto it = mModels.find(model);
  bool modelInSet = it != mModels.end();
  ErrorIf(!modelInSet, "Model not in scene");
  mModels.erase(it);
}

void Scene::Draw() const
{
  // render obejcts
  for (auto it = mModels.begin(); it != mModels.end(); ++it)
  {
    Model * myModel = *it;
    myModel->Draw();
  }
  for (auto it = mProcessingTrees.begin(); it != mProcessingTrees.end(); ++it)
  {
    ProcessingTree * currTree = *it;
    currTree->Render();
  }
}

void Scene::Update( float dt )
{
  for (auto it = mModels.begin(); it != mModels.end(); ++it)
  {
    Model * myModel = *it;
    myModel->Update(dt);
  }
  for (auto it = mProcessingTrees.begin(); it != mProcessingTrees.end(); ++it)
  {
    ProcessingTree * currTree = *it;
    currTree->Update(dt);
  }
  
}

// graphics

void mymousecallback(int x, int y)
{

}

Graphics::Graphics()
  : mActiveCamera(0)
  , mActiveScene(0)
  , mWidth(640)
  , mHeight(480)
  , mShaderBank(new ShaderBank())
  , mMeshBank(new MeshBank())
  , mTextureBank(new TextureBank())
  , mDrawWireframe(false)
  , mFrameTimes(60)
  , mDebugDrawer(new DebugDrawer())
{
}

void Graphics::Init()
{
  // Create a windowed mode window and its OpenGL context
  ErrorIf(!glfwOpenWindow(mWidth, mHeight, 8, 8, 8, 0, 24, 0, GLFW_WINDOW), 
    "Failed to open window");
  int major, minor, revision;
  glfwGetVersion(&major, &minor, &revision);
  //std::cout 
  //  << "GLFW version" << std::endl
  //  << "major   : " << major << std::endl
  //  << "minor   : " << minor<< std::endl
  //  << "revision: " << revision << std::endl;

  glfwSetWindowTitle("Shaderain");
  glfwSetWindowPos(50,150);
  glfwSetWindowSizeCallback(Graphics::ResizeCallback);
  
  // Extension wrangler initialising
  GLenum glew_status = glewInit();
  ErrorIf(glew_status != GLEW_OK, 
    "Error: %s\n", glewGetErrorString(glew_status));
  ErrorIf(!GLEW_VERSION_2_0, "Your graphics card doesn't support OpenGl 2.0");

  mDebugDrawer->Init();

#ifdef TWEAK
  // ant tweak bar
  TwInit(TW_OPENGL, 0);
  TwWindowSize(mWidth, mHeight);

  mStats = TwNewBar("Graphics stats");
  TwDefine(" GLOBAL help='This example shows how to integrate " 
    "AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.
  //mFoo = 0;
  //TwAddVarRW(mStats, "Poop", TW_TYPE_FLOAT, &mFoo, 
  //  " label='Wireframe mode' key=w help='Toggle wireframe display mode.' ");

  TwAddVarRO(mStats, "FPS", TW_TYPE_FLOAT, &mFPS, 
    "label = 'FPS'");
#endif

  CheckOpenGLErrors();
}

void Graphics::Update( float dt )
{
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  if (mDrawWireframe) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBlendFunc(GL_ONE, GL_ZERO);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // use the backbuffer

  ErrorIf(!mActiveCamera, "No active camera set, please assign one");
  mWorldToCam = mActiveCamera->GetViewMatrix();
  mCamToProj = mActiveCamera->GetProjMatrix(); // based on aspect ratio
  glViewport(0,0, mWidth, mHeight);

  // Draw the scene
  ErrorIf(!mActiveScene, "No active scene set, please assign one");
  mActiveScene->Update(dt);
  mActiveScene->Draw();
  //CheckOpenGLErrors();

  // Debug draw
  mDebugDrawer->Update();

  // Render effects
  for (auto it = mEffects.begin(); it != mEffects.end(); ++it)
  {
    Effect * myEffect = *it;
    myEffect->Draw();
  }


#ifdef TWEAK
  static float lastUpdateTime = CurrTimeInSecs();

  float currTime = CurrTimeInSecs();
  float difference = currTime - lastUpdateTime;
  mFrameTimes.Push(difference);
  float time_per_one_frame = mFrameTimes.GetAverage();
  mFPS = 1.0f /  time_per_one_frame;
  lastUpdateTime = currTime;

  TwDraw(); // ant tweak bar
#endif

  glfwSwapBuffers();

  
  //CheckOpenGLErrors();
  
}

int Graphics::GetWidth() const
{
  return mWidth;
}

int Graphics::GetHeight() const
{
  return mHeight;
}

void Graphics::SetActiveCamera( Camera * camera )
{
  mActiveCamera = camera;
}

Camera * Graphics::GetActiveCamera()
{
  ErrorIf(!mActiveCamera, "No active camera");
  return mActiveCamera;
}

void Graphics::SetActiveScene( Scene * scene )
{
  mActiveScene = scene;
}

Scene * Graphics::GetActiveScene()
{
  ErrorIf(!mActiveScene, "No active scene set");
  return mActiveScene;
}

ShaderBank * Graphics::GetShaderBank()
{
  return mShaderBank.get();
}

MeshBank * Graphics::GetMeshBank() 
{
  return mMeshBank.get(); 
}

TextureBank * Graphics::GetTextureBank()
{
  return mTextureBank.get();
}

const glm::mat4 & Graphics::GetWorldToCam() const
{
  return mWorldToCam;
}

const glm::mat4 & Graphics::GetCamToProj() const
{
  return mCamToProj;
}

void Graphics::AddEffect( Effect * effect )
{
  ErrorIf(IsEffectActive(effect), "Adding an effect that is already applied");
  mEffects.push_back(effect);
}

void Graphics::RemoveEffect( Effect * effect )
{
  ErrorIf(!IsEffectActive(effect), "Removing an effect that isn't applied");
  mEffects.erase(std::find(mEffects.begin(), mEffects.end(), effect));
}

bool Graphics::IsEffectActive( Effect * effect )
{
  auto it = std::find(mEffects.begin(), mEffects.end(), effect);
  bool effectInList = it != mEffects.end();
  return effectInList;
}

void Graphics::ResizeCallback( int width, int height )
{
  Engine::GetSystem<Graphics>().mWidth = width;
  Engine::GetSystem<Graphics>().mHeight = height;

  // todo: stuff
  TwWindowSize(width, height);
}

bool Graphics::IsWireframe() const
{
  return mDrawWireframe;
}

void Graphics::DrawWireframe( bool drawWireframe )
{
  mDrawWireframe = drawWireframe;
}

float Graphics::GetAspectRatio() const
{
  return (float) mWidth / mHeight;
}

void Graphics::Cleanup()
{
  glfwCloseWindow();

  TwTerminate();
  // glfwTerminate(); todo?
}

DebugDrawer * Graphics::GetDebugDrawer()
{
  return mDebugDrawer.get();
}
