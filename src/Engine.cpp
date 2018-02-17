#include "Engine.h"

#include "debug.h"
#include "n8time.h" // framerate control

#include "System.h"
#include "GameObject.h"

#include <GL/glfw.h>
//Engine * Engine::s_instance(0);

Engine::Engine() : mTimestep(1/60.0f), mElapsedTime(0)
{
  ErrorIf(!glfwInit(), "Failed to init glfw");
}


void Engine::InitAux()
{
  for (auto it = mSystems.begin(); it != mSystems.end(); ++it)
  {
    System * mySystem = (*it).get();
    mySystem->Init();
  }
}
void Engine::Init() { Instance().InitAux(); }
void Engine::RunAux()
{
  mRunning = true;
  mLastUpdateTime = CurrTimeInSecs();

  while (mRunning)
  {
    glfwPollEvents();

    float currTime = CurrTimeInSecs();
    float timeDifference = currTime - mLastUpdateTime;

    if (timeDifference >= mTimestep)
    {
      mLastUpdateTime = currTime;
      for (auto it = mSystems.begin(); it != mSystems.end(); ++it)
      {
        System * system = (*it).get();
        system->Update(mTimestep);
      }
      mElapsedTime += mTimestep;
    }
    mRunning = glfwGetWindowParam( GLFW_OPENED ) != 0;
  }
}
void Engine::Run() { Instance().RunAux(); }
void Engine::CleanupAux()
{
  for (auto it = mSystems.begin(); it != mSystems.end(); ++it)
  {
    System * currSystem = (*it).get();
    currSystem->Cleanup();
  }
  mSystems.clear(); // unique_ptr deletes systems

  for (auto it = mManagedSystems.begin(); it != mManagedSystems.end(); ++it)
  {
    bool * isManagedFlag = *it;
    *isManagedFlag = false;
  }
  mManagedSystems.clear();

}
void Engine::Cleanup() { Instance().CleanupAux(); }

GameObject * Engine::CreateGameObject(const char * name)
{
  GameObject * obj = new GameObject(name);
  m_gameObjects.insert(std::make_pair(name, obj)); // todo: look up more std:: stuff
  return obj;
}

GameObject * Engine::FindFirstObjectNamed( const char * name ) // todo: rename this cuz its a multimap (ex: find first obj)
{
  auto it = m_gameObjects.find(name); // todo: speed
  if (it == m_gameObjects.end())
    return 0;
  return (*it).second;  
}

std::list<GameObject *> Engine::FindAllObjectsNamed( const char * name ) // todo: move the std::list by reference
{
  typedef std::multimap<std::string, GameObject *> GameObjectMap;

  std::pair <GameObjectMap::iterator, GameObjectMap::iterator> ret
    = m_gameObjects.equal_range(name);
  
  std::list<GameObject *> objectList;
  for(auto it = ret.first; it != ret.second; ++it)
  {
    objectList.push_back(it->second);
  }
  return objectList;
}

float Engine::GetElapsedTime() const
{
  return mElapsedTime;
}
