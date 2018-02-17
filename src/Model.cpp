#include "Model.h"
#include "Engine.h"
#include "Graphics.h"
#include "debug.h" // errorif
#include "Transform.h"
#include "Shader.h"

#include "GraphicsErrors.h"

#include <iostream>

#include "Math.h"

// Model
Model::Model() 
  : mMesh(0)
  //, mJoint(0)
  , mVisible(true)
{
  
}

Model::~Model()
{
  
}

void Model::SetMesh( Mesh * mesh )
{
  mMesh = mesh;
}

Mesh * Model::GetMesh()
{
  return mMesh;
}

void Model::ConstructWorldMatrix()
{
  Transform * myTransform = GetSibling<Transform>();
  mModelToWorld = myTransform->ConstructMatrix();
}

void Model::Draw()
{
  if (!mVisible) 
    return;
  
  mMesh->Render(this);
}

//void Model::DrawWithShader(Shader * shader)
//{
//  // swap our shaders, draw, then swap back
//  Material * myMaterial = GetMesh()->GetMaterial();
//  Shader * currShader = myMaterial->GetShader();
//  myMaterial->SetShader(shader);
//  Draw();
//  myMaterial->SetShader(currShader);
//}

const glm::mat4 & Model::GetModelToWorld() const
{
  return mModelToWorld;
}



bool Model::GetVisible() const
{
  return mVisible;
}

void Model::SetVisible( bool visible )
{
  mVisible = visible;
}

void Model::Update(float dt)
{
  ConstructWorldMatrix();

  // todo: not static
  static float timeElapsed = 0;
  timeElapsed += dt;
  
  if (mMesh->IsAnimated())
  {
    mMesh->GetSketalTransforms(mMesh->GetScene(), timeElapsed);
  }
}
