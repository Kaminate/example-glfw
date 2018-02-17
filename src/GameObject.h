#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <vector>
#include "debug.h"

class Component;
class Engine;
class GameObject
{
  const char * m_name;
  GameObject(const char * name); 
  friend class Engine;
public:
  
  ~GameObject(); // deletes all components

  void AddComponent(Component * component);

  template<typename ComponentType>
  ComponentType * GetComponent(); // return 0 if not found

  template<typename ComponentType>
  const ComponentType * GetComponent() const; // return 0 if not found

private:
  std::vector<Component *> m_components; // owned by GameObject // todo: unique_ptr
};

// template functions
template<typename ComponentType> 
ComponentType * GameObject::GetComponent() // todo: make not slow
{
  for (auto it = m_components.begin(); it != m_components.end(); ++it)
  {
    ComponentType * component = dynamic_cast<ComponentType*>(*it);
    if (component)
      return component;
  }
  ErrorIf(true, "GameObject doesn't have this component type");
  return 0;
}

template<typename ComponentType> 
const ComponentType * GameObject::GetComponent() const // todo: won't compile because of const (just remove)
{
  for (auto it = m_components.begin(); it != m_components.end(); ++it)
  {
    ComponentType * component = dynamic_cast<ComponentType*>(*it);
    if (component)
      return component;
  }
  ErrorIf(true, "GameObject doesn't have this component type");
  return 0;
}

#endif