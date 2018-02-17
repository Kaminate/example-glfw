#include "ShaderVariableTypes.h"
#include "debug.h"

unsigned GetComponentsPerAttribute(VertAttrib type)
{
  switch(type)
  {
  case COLOR:
    return 4;

  case POSITION:
  case NORMAL:
  case TANGENT:
  case BITANGENT:
    return 3;

  case TEXCOORD:
    return 2;
  case BONE_ID:
    return 4;
  case BONE_WEIGHT:
    return 4;

  default:
    ErrorIf(true, "Vertex attribute with unknows components per attribute");
    
  }
  return 1337; // quell warning
}