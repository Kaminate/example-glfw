#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>

#define ErrorIf(predicate, ...)       \
  if (predicate)                      \
  {                                   \
    printf("An error has occured! \n" \
           "Function: %s \n"          \
           "File:     %s \n"          \
           "Line #:   %i \n"          \
           "Description: "            \
           , __FUNCTION__             \
           , __FILE__                 \
           , __LINE__);               \
    fprintf(stderr, __VA_ARGS__);     \
    __debugbreak();                   \
  }

#endif
