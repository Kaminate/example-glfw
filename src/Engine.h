#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <list>

#include "debug.h" // errorif
#include "Singleton.h" 

class System;
class GameObject;

class Engine : public Singleton<Engine>
{
  void InitAux();
  void RunAux();
  void CleanupAux();
  template <typename SystemType> void AddSystemAux(SystemType* system);
  template <class SystemType> SystemType & GetSystemAux();

public:
  static void Init();
  static void Run();
  static void Cleanup();

  template <typename SystemType>
  static void AddSystem(SystemType* system);

  template <class SystemType>
  static SystemType & GetSystem();

  GameObject * CreateGameObject(const char * name);
  GameObject * FindFirstObjectNamed(const char * name); // 0 if not found
  std::list<GameObject *> FindAllObjectsNamed(const char * name);

  float GetElapsedTime() const;

private:
  
  Engine(); friend class Singleton<Engine>;
  bool mRunning;

  float mLastUpdateTime;
  float mTimestep;
  float mElapsedTime;
  std::vector<std::unique_ptr<System> > mSystems;
  std::multimap<std::string, GameObject*> m_gameObjects; 
  friend class GameObject;
  std::vector<bool*> mManagedSystems;
};


template <class SystemType>
SystemType & Engine::GetSystemAux() // will error if system not in engine
{
  static SystemType * system = 0;
  static bool isManaged = false; // dirty means that the engine needs to manage this system

  if (!isManaged)
  {
    isManaged = true;
    mManagedSystems.push_back(&isManaged);

    bool found = false;

    for(auto it = mSystems.begin(); it != mSystems.end(); ++it)
    {
      System * currSystem = (*it).get();
      SystemType * ourSystemType = dynamic_cast<SystemType*>(currSystem);
      if (ourSystemType)
      {
        found = true;
        system = ourSystemType;

        break;
      }
    }

    ErrorIf(!found, "Engine doesn't have this system");
  }

  return *system;
}

template <class SystemType>
SystemType & Engine::GetSystem(){return Instance().GetSystemAux<SystemType>();}

template <typename SystemType>
void Engine::AddSystemAux( SystemType * system )
{
  mSystems.push_back(std::unique_ptr<SystemType>(system));
}

template <typename SystemType>
void Engine::AddSystem( SystemType* system ){Instance().AddSystemAux(system);}

#endif