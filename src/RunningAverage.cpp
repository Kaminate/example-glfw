#include "RunningAverage.h"

#include <assert.h>
#include <algorithm> // copy
#include <iterator> // ostream_iterator

RunningAverage::RunningAverage(unsigned size) : mAverage(0) , mValues(size) {}

float RunningAverage::GetAverage() const
{
  assert(mPushCount != 0);
  return mAverage;
}

void RunningAverage::Push(float value)
{
  bool overfull = mPushCount >= mValues.size();
  unsigned index = mPushCount % mValues.size();
  if (overfull)
  {
    mAverage -= mValues[index] / mValues.size();
    mValues[index] = value;
    mAverage += value / mValues.size();
  }
  else
  {
    // (A+B+C)/3 - A/12 - B/12 - C/12 + D/4 = (A+B+C+D)/4
    for (unsigned i = 0; i < mPushCount; ++i)
      mAverage -= mValues[i] / (mPushCount * (mPushCount + 1));
    mValues[index] = value;
    mAverage += value / (mPushCount + 1);
  }

  ++mPushCount;
}

std::ostream & RunningAverage::Write( std::ostream & os ) const
{
  unsigned range = mPushCount < mValues.size() ? mPushCount : mValues.size();
  for (unsigned i = 0; i < range; ++i)
    os << mValues[i] << " ";
  return os;
}

std::ostream & operator<<( std::ostream & lhs, const RunningAverage & rhs )
{
  return rhs.Write(lhs);
}
