#include <iostream>
#include <fstream>

#include "shader_utils.h"
#include "debug.h"


file_read::file_read(const char * filename) : m_source(0)
{
  std::ifstream myfile(filename, std::ifstream::binary);
  ErrorIf(!myfile.is_open(), "Either your filepath or filename is wrong!");
  myfile.seekg(0, myfile.end);
  unsigned length = (unsigned) myfile.tellg(); // problems if tellg() > MAX_UINT
  myfile.seekg(0, myfile.beg);

  m_source = new char[length + 1]; // allocate memory:
  m_source[length] = 0; // null termination

  myfile.read (m_source,length);
  myfile.close();
}
file_read::~file_read(){if (m_source) delete [] m_source;}
const char * file_read::GetSource() const {return m_source;}

void print_log(GLuint obj)
{
  GLint logLen = 0;
  if (glIsShader(obj)) glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLen);
  else if (glIsProgram(obj)) glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &logLen);
  else ErrorIf(true, "printlog: Not a shader or a program\n");

  char * log = new char[logLen];

  if (glIsShader(obj)) glGetShaderInfoLog(obj, logLen, NULL, log);
  else if (glIsProgram(obj)) glGetProgramInfoLog(obj, logLen, NULL, log);

  ErrorIf(true, "%s", log);
  delete [] log;
}

GLuint create_shader(const char * filename, GLenum type)
{
  file_read fileReader(filename);
  GLuint result = glCreateShader(type);
  
  const GLchar* sources[] = {
    // Define GLSL version
#ifdef GL_ES_VERSION_2_0
    "#version 100\n"
#else
    "#version 120\n"
#endif
    ,
    // GLES2 precision specifiers
#ifdef GL_ES_VERSION_2_0
    // Define default float precision for fragment shaders:
    (type == GL_FRAGMENT_SHADER) ?
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "precision highp float;           \n"
    "#else                            \n"
    "precision mediump float;         \n"
    "#endif                           \n"
    : ""
    // Note: OpenGL ES automatically defines this:
    // #define GL_ES
#else
    // Ignore GLES 2 precision specifiers:
    "#define lowp   \n"
    "#define mediump\n"
    "#define highp  \n"
#endif
    ,
    fileReader.GetSource()};

    glShaderSource(result, 3, sources, 0);
    
    glCompileShader(result);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(result, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
      fprintf(stderr, "%s:", filename);
      print_log(result);
      glDeleteShader(result);
      return 0;
    }

    return result;
}
