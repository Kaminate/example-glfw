#ifndef __SINGLETON_H_
#define __SINGLETON_H_

template<typename ClassType>
class Singleton
{
public:
  static ClassType & Instance()
  {
    static ClassType * sInstance = new ClassType();
    return *sInstance;
  }
};

#endif
