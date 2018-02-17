#include <iostream>

#include "Temporary.h"

#include "GameObject.h"
#include "Engine.h"
#include "Transform.h"
#include "Model.h"
#include "Geometry.h"
#include "Shader.h"
#include "debug.h"
#include "Camera.h"
#include "Graphics.h"
#include "Input.h"
#include "Texture.h"
#include "Math.h"
#include "ProcessingTree.h"

#include "GraphicsErrors.h"

using namespace std;

Temporary::Temporary() : mGlow(new Glow()), mDebugLayer(new DebugDrawLayer("test draw layer"))
{

}

void Temporary::CreatePosTexMvpBonesShader()
{
  ShaderBank * bank = Engine::Instance().GetSystem<Graphics>().GetShaderBank();
  mPosTexMvpBonesShader = bank->CreateShader(
    "We're boned", 
    "resources/shaders",
    "pos_tex_mvp_bones.v.txt", 
    "pos_tex_mvp.f.txt");
  mPosTexMvpBonesShader->BindUniform(DIFFUSE_TEXTURE, TEXTURE, "mytexture");
  mPosTexMvpBonesShader->BindUniform(BONE_MATRIXES, MATRIX_ARRAY, "boneMatrixes");
  mPosTexMvpBonesShader->BindUniform(MVP_MATRIX, MATRIX, "mvp");
  mPosTexMvpBonesShader->BindAttribute(POSITION, "coord3d");
  mPosTexMvpBonesShader->BindAttribute(TEXCOORD, "texcoord");
  mPosTexMvpBonesShader->BindAttribute(BONE_ID, "boneIds");
  mPosTexMvpBonesShader->BindAttribute(BONE_WEIGHT, "boneWeights");
}

void Temporary::CreatePosMvpBonesShader()
{
  ShaderBank * bank = Engine::Instance().GetSystem<Graphics>().GetShaderBank();
  mPosMvpBonesShader = bank->CreateShader(
    "Shadah For the animated tri", 
    "resources/shaders",
    "pos_mvp_bones.v.txt", 
    "red.f.txt");
  mPosMvpBonesShader->BindUniform(BONE_MATRIXES, MATRIX_ARRAY, "boneMatrixes");
  mPosMvpBonesShader->BindUniform(MVP_MATRIX, MATRIX, "mvp");
  mPosMvpBonesShader->BindAttribute(POSITION, "coord3d");
  mPosMvpBonesShader->BindAttribute(BONE_ID, "boneIds");
  mPosMvpBonesShader->BindAttribute(BONE_WEIGHT, "boneWeights");
}

void Temporary::CreatePosTexMvpShader()
{
  ShaderBank * bank = Engine::Instance().GetSystem<Graphics>().GetShaderBank();
  mPosTexMvpShader = bank->CreateShader(
    "Shader Joe's", 
    "resources/shaders",
    "pos_tex_mvp.v.txt", 
    "pos_tex_mvp.f.txt");
  mPosTexMvpShader->BindUniform(DIFFUSE_TEXTURE, TEXTURE, "mytexture");
  mPosTexMvpShader->BindUniform(MVP_MATRIX, MATRIX, "mvp");
  mPosTexMvpShader->BindAttribute(POSITION, "coord3d");
  mPosTexMvpShader->BindAttribute(TEXCOORD, "texcoord");
}

void Temporary::CreatePosMvpShader()
{
  ShaderBank * bank = Engine::GetSystem<Graphics>().GetShaderBank();
  mPosMvpShader = bank->CreateShader( // todo: map of string to smart pointers
    "Pos Mvp shader", 
    "resources/shaders",
    "pos_mvp.v.txt", 
    "pos_mvp.f.txt");
  mPosMvpShader->BindUniform(MVP_MATRIX, MATRIX, "mvp");
  mPosMvpShader->BindAttribute(POSITION, "coord3d");
}

void Temporary::CreateCubeAtPosition(float x, float y, float z)
{
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();
  MeshBank * meshBank = graphicsSystem.GetMeshBank();
  Mesh * myMesh = meshBank->AddMesh("resources/assets", "cube.obj");
  
  myMesh->SetShader(mPosTexMvpShader);

  GameObject * cube = Engine::Instance().CreateGameObject("random cube");
  cube->AddComponent(new Transform(glm::vec3(x,y,z))); 

  Model * model = new Model;
  model->SetMesh(myMesh);
  
  cube->AddComponent(model);
  ProcessingTree * tree = new ProcessingTree();
  tree->mTrunkHeight = 2;
  
  cube->AddComponent(tree);
  Scene * activeScene = graphicsSystem.GetActiveScene();

  mTreeBar = TwNewBar("Tree");
  TwAddVarRW(mTreeBar, "", TW_TYPE_FLOAT, &tree->mBranchAttenuationFactor, 
    "label='Attenuation Factor' step = 0.05 min = 0 max = 1");
  TwAddVarRW(mTreeBar, "", TW_TYPE_FLOAT, &tree->mSwayMinAngleDeg, 
    "label='Min Angle Deg' step = 1");
  TwAddVarRW(mTreeBar, "", TW_TYPE_FLOAT, &tree->mSwayMaxAngleDeg, 
    "label='Max Angle Deg' step = 1");
  TwAddVarRW(mTreeBar, "", TW_TYPE_FLOAT, &tree->mSwayPeriod, 
    "label='Period' step = 0.2 min = 0");
  TwAddVarRW(mTreeBar, "", TW_TYPE_FLOAT, &tree->mTrunkHeight, 
    "label='Trunk Height' step = 0.1");
  TwAddVarRW(mTreeBar, "", TW_TYPE_FLOAT, &tree->mMinimumBranchHeight, 
    "label='Minimum Branch Height' step = 0.05 min = 0.05 ");
  
  activeScene->AddModel(model);
  activeScene->AddProcessingTree(tree);
}

void Temporary::CreateDude()
{
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();
  MeshBank * meshBank = graphicsSystem.GetMeshBank();
  Mesh * myMesh = meshBank->AddMesh("resources/assets", "blaze.obj");
  myMesh->SetShader(mPosTexMvpShader);
  
  GameObject * obj= Engine::Instance().CreateGameObject("Mr. Blaze");
  obj->AddComponent(new Transform(glm::vec3(0,0,0)));
  Model * model = new Model;
  model->SetMesh(Engine::GetSystem<Graphics>().GetMeshBank()->GetMesh("blaze.obj"));
  obj->AddComponent(model);
  Engine::GetSystem<Graphics>().GetActiveScene()->AddModel(model);
}

void Temporary::CreateFBXAnimTri()
{
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();
  MeshBank * myMeshBank = graphicsSystem.GetMeshBank();

  GameObject * obj= Engine::Instance().CreateGameObject("Animated Tri");
  Transform * myTransform = new Transform(glm::vec3(0,0,0));
  float scale = 1;
  myTransform->mScale = glm::vec3(scale,scale,scale);
  obj->AddComponent(myTransform);
  Model * model = new Model;

  Mesh * myMesh = myMeshBank->AddMesh("resources/assets", "right.fbx");
  
  myMesh->SetShader(mPosMvpBonesShader);
  model->SetMesh(myMesh);
  obj->AddComponent(model);
  graphicsSystem.GetActiveScene()->AddModel(model);

  model->SetVisible(true);
  
  mTriFBX = model;
}

void Temporary::CreateDAEAnimTri()
{
  MeshBank * myMeshBank = Engine::GetSystem<Graphics>().GetMeshBank();

  GameObject * obj= Engine::Instance().CreateGameObject("DAE Animated Tri");
  obj->AddComponent(new Transform(glm::vec3(0,0,0)));
  Model * model = new Model;

  Mesh * myMesh = myMeshBank->AddMesh("resources/assets", "wrong.dae");
  
  myMesh->SetShader(mPosMvpShader);
  model->SetMesh(myMesh);
  obj->AddComponent(model);
  Engine::GetSystem<Graphics>().GetActiveScene()->AddModel(model);

  mTriDAE = model;
  model->SetVisible(false);
}

void Temporary::CreateOBJAnimTri()
{
  MeshBank * myMeshBank = Engine::GetSystem<Graphics>().GetMeshBank();

  GameObject * obj= Engine::Instance().CreateGameObject("DAE Animated Tri");
  obj->AddComponent(new Transform(glm::vec3(0,0,0)));
  Model * model = new Model;

  Mesh * myMesh = myMeshBank->AddMesh("resources/assets", "wrong.obj");
  //myMesh->GetMaterial()->SetShader(mPosMvpShader);
  myMesh->SetShader(mPosMvpShader);
  model->SetMesh(myMesh);
  obj->AddComponent(model);
  Engine::GetSystem<Graphics>().GetActiveScene()->AddModel(model);

  mTriOBJ = model;
  model->SetVisible(false);
}

void Temporary::CreateCamera()
{
  // camera component
  glm::vec3 worldSpaceUp(0,0,1); // !
  Camera * myCamera = new Camera(worldSpaceUp);
  myCamera->SetType(Camera::DIRECTIONAL);
  myCamera->SetViewDirection(glm::vec3(0,1,0));
  
  Engine::GetSystem<Graphics>().SetActiveCamera(myCamera);

  // camera gameobejct
  GameObject * obj = Engine::Instance().CreateGameObject("My Camera");
  obj->AddComponent(new Transform(glm::vec3(0,-5,0)));
  obj->AddComponent(myCamera);
}

void Temporary::Init()
{
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();

  Engine::GetSystem<Graphics>().SetActiveScene(new Scene()); // possible memory leak
  CreatePosMvpShader();
  CreatePosTexMvpShader();
  CreatePosMvpBonesShader();
  CreatePosTexMvpBonesShader();
  
  //CreateDude();
  //CreateCubeAtPosition(0 - 0.2f,0,-1.4f);
  //CreateLanternGuy();

  //CreateFBXAnimTri();
  //CreateDAEAnimTri();
  //CreateOBJAnimTri();
  CreateCamera();
  mDebugLayer->Init();
  mGlow->Init();
  Engine::GetSystem<Graphics>().AddEffect(mGlow); // todo: uncomment

  
}

void Temporary::Update( float dt )
{

  Input & inputSystem = Engine::GetSystem<Input>();
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();

  DebugDrawer * drawer = mDebugLayer->RequestDebugDrawer();
  if (drawer)
  {
    drawer->DrawSphere(glm::vec3(0,0,0), 1, glm::vec4(1,0,0,1));
    
    static float angle = 0;
    static float otherAngle = 0;
    float speedAngle = 0.5f;
    float speedOtherAngle = speedAngle * 15;
    glm::vec3 dir(-1,0,0);
    
    angle -= ToRadians(speedAngle);
    otherAngle -= ToRadians(speedOtherAngle);
    
    glm::mat3 rotY = RotationMatrixY(angle);
    glm::mat3 rotOtehrY = RotationMatrixY(otherAngle);
    drawer->DrawArrow(glm::vec3(0,0,0), rotY * 3.0f * dir / 4.0f, glm::vec4(1,0,0,1)); 
    drawer->DrawArrow(glm::vec3(0,0,0), rotOtehrY * dir, glm::vec4(1,0,0,1)); 
  }



  Camera * activeCamera = graphicsSystem.GetActiveCamera();
  Transform * cameraTransform = activeCamera->GetSibling<Transform>();
  glm::vec3 & camPos = cameraTransform->mPos;
  float cameraspeed = 1;
  if (inputSystem.IsKeyDown(GLFW_KEY_LSHIFT))
    cameraspeed = 10;

  // Translate camera
  if (inputSystem.IsKeyDown('W'))           camPos.z += cameraspeed * dt;
  if (inputSystem.IsKeyDown('S'))           camPos.z -= cameraspeed * dt;
  if (inputSystem.IsKeyDown('A'))           camPos.x -= cameraspeed * dt;
  if (inputSystem.IsKeyDown('D'))           camPos.x += cameraspeed * dt;
  if (inputSystem.IsKeyDown(GLFW_KEY_UP))   camPos.y += cameraspeed * dt;
  if (inputSystem.IsKeyDown(GLFW_KEY_DOWN)) camPos.y -= cameraspeed * dt;

  // Rotate camera
  float deg_per_sec = 180;
  float deg_per_dt = deg_per_sec * dt;
  float speed = 0;
  if (inputSystem.IsKeyDown(GLFW_KEY_RIGHT))speed += deg_per_dt;
  if (inputSystem.IsKeyDown(GLFW_KEY_LEFT))speed -= deg_per_dt;
  glm::mat4 rot = glm::rotate(glm::mat4(1.0f), speed, glm::vec3(0,0,1));
  glm::vec4 newPos = rot * glm::vec4(camPos,1.0f);
  camPos.x = newPos.x;
  camPos.y = newPos.y;
  camPos.z = newPos.z;

  // Move the dude
  GameObject * dude = Engine::Instance().FindFirstObjectNamed("Mr. Blaze");
  if (dude)
  {
    Transform * dudeTransform = dude->GetComponent<Transform>();
    glm::vec3 & dudePos= dudeTransform->mPos;
    if (inputSystem.IsKeyDown(GLFW_KEY_SPACE)) dudePos.z += dt;
    if (inputSystem.IsKeyDown(GLFW_KEY_BACKSPACE)) dudePos = glm::vec3(0);
    if (inputSystem.IsKeyDown('B')) dudePos.z -= dt;
  }

  // effect
  static int effectCooldown;
  if (--effectCooldown < 0 && inputSystem.IsKeyDown(GLFW_KEY_CAPS_LOCK)) 
  {
    graphicsSystem.IsEffectActive(mGlow) 
      ? graphicsSystem.RemoveEffect(mGlow)
      : graphicsSystem.AddEffect(mGlow);
    effectCooldown = 10;
  }

  //// tri visibility
  //static int visibleCooldown;
  //if (--visibleCooldown < 0)
  //{
  //  bool b123pressed = true;
  //  if (Engine::GetSystem<Input>().IsKeyDown('1'))
  //    mTriOBJ->SetVisible(!mTriOBJ->GetVisible());
  //  else if (Engine::GetSystem<Input>().IsKeyDown('2'))
  //    mTriDAE->SetVisible(!mTriDAE->GetVisible());
  //  else if (Engine::GetSystem<Input>().IsKeyDown('3'))
  //    mTriFBX->SetVisible(!mTriFBX->GetVisible());
  //  else b123pressed = false;
  //  if (b123pressed)
  //  {
  //    std::cout << std::endl
  //      << "OBJ tri visible: " << mTriOBJ->GetVisible() << std::endl
  //      << "DAE tri visible: " << mTriDAE->GetVisible() << std::endl
  //      << "FBX tri visible: " << mTriFBX->GetVisible() << std::endl;
  //    visibleCooldown = 10;
  //  }
  //}  
}

void Temporary::Cleanup()
{

}

void Temporary::CreateLanternGuy()
{
  Graphics & graphicsSystem = Engine::GetSystem<Graphics>();
  MeshBank * myMeshBank = graphicsSystem.GetMeshBank();
  Mesh * myMesh = myMeshBank->AddMesh("resources/assets", "boblampclean.md5mesh");
  myMesh->SetShader(mPosTexMvpBonesShader);
  GameObject * obj= Engine::Instance().CreateGameObject("Lantern Guy");
  Model * model = new Model;
  model->SetMesh(myMesh);
  obj->AddComponent(model);
  Transform * myTransform = new Transform(glm::vec3(0,0,0));
  float scale = .03f;
  myTransform->mPos = glm::vec3(0.2f, -1, -1.8f);
  myTransform->mScale = glm::vec3(scale,scale,scale);
  obj->AddComponent(myTransform);
  graphicsSystem.GetActiveScene()->AddModel(model);

}


// glow 

void Glow::Init()
{
  ShaderBank * myShaderBank = Engine::GetSystem<Graphics>().GetShaderBank();
  MeshBank * myMeshBank = Engine::GetSystem<Graphics>().GetMeshBank();
  TextureBank * myTextureBank = Engine::GetSystem<Graphics>().GetTextureBank();

  // shader 1: horizontal blur
  mBlurHShader = myShaderBank->CreateShader(
    "Blur H S",
    "resources/shaders",
    "fullscreenquad.v.txt",
    "gaussBlurH.f.txt");
  mBlurHShader->BindAttribute(POSITION, "coord3d");
  mBlurHShader->BindAttribute(TEXCOORD, "texcoord");
  mBlurHShader->BindUniform(DIFFUSE_TEXTURE, TEXTURE, "mytexture");
  mBlurHShader->BindUniform(TEXTURE_WIDTH, FLOAT, "texturewidth");

  // shader 2: vertical blur
  mBlurVShader = myShaderBank->CreateShader(
    "Blur V Shader",
    "resources/shaders",
    "fullscreenquad.v.txt", 
    "gaussBlurV.f.txt");
  mBlurVShader->BindAttribute(POSITION, "coord3d");
  mBlurVShader->BindAttribute(TEXCOORD, "texcoord");
  mBlurVShader->BindUniform(DIFFUSE_TEXTURE, TEXTURE, "mytexture");
  mBlurVShader->BindUniform(TEXTURE_HEIGHT, FLOAT, "textureheight");
  
  // shader 3: composite
  mBlendShader = myShaderBank->CreateShader(
    "Blur Composite", 
    "resources/shaders",
    "fullscreenquad.v.txt", 
    "composite.f.txt");
  mBlendShader->BindAttribute(POSITION, "coord3d");
  mBlendShader->BindAttribute(TEXCOORD, "texcoord");
  mBlendShader->BindUniform(DIFFUSE_TEXTURE, TEXTURE, "mytexture");
  mBlendShader->BindUniform(BLEND_FACTOR, FLOAT, "blendFactor");
  
  // fullscreen quad
  GameObject * obj = Engine::Instance().CreateGameObject("Fullscreen Quad");
  obj->AddComponent(new Transform(glm::vec3(0,0,0)));
  Model * model = new Model;
  model->SetMesh(myMeshBank->AddMesh("resources/assets", "unitsquare.obj"));
  obj->AddComponent(model);

  // create a texture for our glow fx
  float w = 200;
  float h = w / Engine::GetSystem<Graphics>().GetAspectRatio();
  mTexture[0] = myTextureBank->CreateTexture("glowTexture0", (int)w, (int)h);
  mTexture[1] = myTextureBank->CreateTexture("glowTexture1", (int)w, (int)h);
  glGenFramebuffers(2, mGlowFrameBuffer);
  
  for (int i = 0; i < 2; ++i)
  {
    // create our framebuffer and friends
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGlowFrameBuffer[i]);

    // attach a texture to the color attachment point
    glFramebufferTexture(
      GL_DRAW_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0,
      mTexture[i]->GetGLuint(), 
      0);

    // not sure if necessary
    GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffers); // tell fragment shader to output to framebuffer

    // todo: attach a renderbuffer for DEPTH info
    //  glFrameBufferRenderBuffer(... GL_DEPTH_ATTACHMENT, ...);
    CheckFramebufferStatus();
  }
}

void Glow::Draw()
{
  
  GameObject * GOmrBlaze = Engine::Instance().FindFirstObjectNamed("Mr. Blaze");
  GameObject * GOquad = Engine::Instance().FindFirstObjectNamed("Fullscreen Quad");
  ErrorIf(!(GOmrBlaze && GOquad), "Can't find things needed for glow effect");

  Model * mrBlazeModel = GOmrBlaze->GetComponent<Model>();
  Model * quad = GOquad->GetComponent<Model>();
  Texture * mrBlazeTexture  = Engine::GetSystem<Graphics>().GetTextureBank()->GetTexture("Blaze.tga");
  
  // draw the objects we want to glow onto t0 using normal shader
  glBindFramebuffer(GL_FRAMEBUFFER, mGlowFrameBuffer[0]);
  glViewport(0,0, mTexture[0]->GetWidth(), mTexture[0]->GetHeight());
  // we don't need to change the viewport matrix 
  // because our aspect ratio is the same
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
  mrBlazeModel->Draw();

  // blur horizontal onto t1
  glBindFramebuffer(GL_FRAMEBUFFER, mGlowFrameBuffer[1]);
  mBlurHShader->SetActive(); // must be active to send data to
  mBlurHShader->SendData(DIFFUSE_TEXTURE, mTexture[0]);
  mBlurHShader->SendData(TEXTURE_WIDTH,  (float)mTexture[0]->GetWidth());
  quad->GetMesh()->RenderGeometryWithoutMaterialData(mBlurHShader);

  // blur vertical onto t0
  glBindFramebuffer(GL_FRAMEBUFFER, mGlowFrameBuffer[0]);
  mBlurVShader->SetActive();
  mBlurVShader->SendData(DIFFUSE_TEXTURE, mTexture[1]);
  mBlurVShader->SendData(TEXTURE_HEIGHT, (float)mTexture[1]->GetHeight());
  quad->GetMesh()->RenderGeometryWithoutMaterialData(mBlurVShader);

  // blend onto backbuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBlendFunc(GL_ONE, GL_ONE); // additive blending
  mBlendShader->SetActive();
  mBlendShader->SendData(DIFFUSE_TEXTURE, mTexture[0]);
  mBlendShader->SendData(BLEND_FACTOR, 1.0f);
  // position, w, h of the screen space viewport
  glViewport(0,0, 
    Engine::GetSystem<Graphics>().GetWidth(), 
    Engine::GetSystem<Graphics>().GetHeight());
  //glViewport(20,20, 200, 200 / Engine::GetSystem<Graphics>().GetAspectRatio()); 
  glDisable(GL_DEPTH_TEST);
  quad->GetMesh()->RenderGeometryWithoutMaterialData(mBlendShader);
}
