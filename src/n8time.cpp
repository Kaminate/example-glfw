#include "n8time.h"

#include <time.h> // clock


float CurrTimeInSecs()
{
  return clock() / (float) CLOCKS_PER_SEC;
}