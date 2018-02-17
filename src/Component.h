#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "GameObject.h"
class Component
{
public:
  Component();
  virtual ~Component();

  GameObject * GetOwner();

  template <typename ComponentType>
  ComponentType * GetSibling(); // returns 0 if not found

  template <typename ComponentType>
  const ComponentType * GetSibling() const; // returns 0 if not found


  void SetOwner(GameObject * owner);
private:
  GameObject * m_owner;
};

template<typename ComponentType> 
ComponentType * Component::GetSibling()
{
  return m_owner->GetComponent<ComponentType>();
}

template<typename ComponentType> 
const ComponentType * Component::GetSibling() const
{
  return m_owner->GetComponent<ComponentType>();
}


#endif