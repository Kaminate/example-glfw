#include "Component.h"


Component::Component() : m_owner(0)
{

}


Component::~Component()
{

}


void Component::SetOwner( GameObject * owner )
{
  m_owner = owner;
}

GameObject * Component::GetOwner()
{
  return m_owner;
}
