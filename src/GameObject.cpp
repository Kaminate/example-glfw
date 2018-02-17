#include "GameObject.h"
#include "Component.h"
#include "Engine.h"
#include <string>
GameObject::GameObject(const char * name) : m_name(name)
{
   
}

// none of this is tested btw
GameObject::~GameObject()
{
  // delete all our components
  for (auto it = m_components.begin(); it != m_components.end(); ++it)
    delete *it;

  // remove ourselves from the multimap
  typedef std::multimap<std::string, GameObject *> GameObjectMap;

  std::pair < GameObjectMap::iterator, GameObjectMap::iterator> ret;
  ret = Engine::Instance().m_gameObjects.equal_range(this->m_name);

  // look for ourself linearly...
  for (GameObjectMap::iterator it=ret.first; it!=ret.second; ++it) // todo: auto (recommended)
  {
    const std::pair<std::string, GameObject *> & myPair = *it;
    if (myPair.second == this)
    {
      Engine::Instance().m_gameObjects.erase(it);
      return;
    }
  }
}

void GameObject::AddComponent(Component * component)
{
  component->SetOwner(this);
  m_components.push_back(component);
}

