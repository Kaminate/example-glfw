#ifndef _ACTIONLIST_H
#define _ACTIONLIST_H

#include <memory>
#include <set>
#include <list>

struct Action
{
  Action(bool blocking);
  virtual void Update(float dt) = 0;
  bool IsFinished() const;
  bool IsBlocking() const;
protected:
  void SetFinished();
private:
  bool m_finished;
  bool m_blocking;
};

class ActionList
{
public:
  void Clear();
  ActionList * Push(Action * action);
  bool Empty() const;
private:
  void Update(float dt);
  std::list<std::unique_ptr<Action> > m_actions;
};

#endif
