#ifndef _RUNNING_AVERAGE_H_
#define _RUNNING_AVERAGE_H_

#include <vector>
#include <ostream>

// Great for keepipng track of fps 
class RunningAverage
{
  float mAverage;
  std::vector<float> mValues;
  unsigned mPushCount;
public:
  RunningAverage(unsigned size); // number of values to keep
  float GetAverage() const;
  void Push(float val);
  std::ostream & Write(std::ostream & os) const;
};

std::ostream & operator << (std::ostream & lhs, const RunningAverage & rhs);

#endif
