#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H
#include <GL\glew.h>
#include "debug.h"

// reads a file into memory (ie: for passing shaders to OpenGL)
class file_read
{
public:
  file_read(const char * filename);
  ~file_read();
  const char * GetSource() const;
private:
  char * m_source;
};

void print_log(GLuint obj);

#define ErrorIfGL(predicate, glObj, ...)\
if (predicate)\
{\
  print_log(glObj);\
}\
ErrorIf(predicate, __VA_ARGS__)

GLuint create_shader(const char * filename, GLenum type); // returns 0 on fail

#endif