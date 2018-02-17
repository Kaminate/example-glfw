
#include "ActionList.h"

// Action
Action::Action(bool blocking) : m_blocking(blocking), m_finished(false) {}
bool Action::IsFinished() const {return m_finished;}
bool Action::IsBlocking() const {return m_blocking;}
void Action::SetFinished() {m_finished = true;}

void ActionList::Clear()
{
  m_actions.clear();
}
ActionList * ActionList::Push(Action * action)
{
  m_actions.push_back(std::unique_ptr<Action>(action));
  return this;
}
void ActionList::Update(float dt)
{
  auto it = m_actions.begin();
  while(it != m_actions.end())
  {
    Action * action = (*it).get();
    action->Update(dt);
    if (action->IsFinished())
    {
      it = m_actions.erase(it);
    }
    else if (action->IsBlocking()) return;
    else ++it;
  }
}
bool ActionList::Empty() const
{
  return m_actions.empty();
}