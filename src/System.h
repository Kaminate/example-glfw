#ifndef _SYSTEM_H_
#define _SYSTEM_H_

class System
{
public:
  virtual ~System() {}
  virtual void Update(float dt) = 0;
  virtual void Init() = 0;
  virtual void Cleanup() = 0;
};

#endif