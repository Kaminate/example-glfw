#ifndef _SHADER_VARIABLE_TYPES_
#define _SHADER_VARIABLE_TYPES_

/*

  Here's how this shit's gonna go down.

  1. When we bind the attributes and uniforms from a shader, we will associate
  with it a shader attribute and store it in an instance of the shader class

  2. When drawing the object, the material will iterate through each shader
  variable type, and will know what to do with each variable type 
  (material knows where to find the actual data)
  (material knows what glFunciton to call to send it to gpu)
  (data may be a private variable, ex: class SpecialMaterial : public Material)
  
 */


// thoughts: wrap it in a struct
//
// struct VertAttrib
// {
//    enum AttribType
//    enum DataType
// }

enum VertAttrib 
{
  COLOR,

  POSITION,
  NORMAL,
  TANGENT,
  BITANGENT,

  TEXCOORD,
  
  BONE_ID, 
  BONE_WEIGHT,
  ANIMATION_MESH,
};

unsigned GetComponentsPerAttribute(VertAttrib type);

enum ShaderUniform // add more if needed
{
  MVP_MATRIX, 
  DIFFUSE_TEXTURE,
  RESOLUTION_WIDTH,
  RESOLUTION_HEIGHT,
  BLEND_FACTOR,
  TEXTURE_WIDTH,
  TEXTURE_HEIGHT,
  BONE_MATRIXES,
};

enum ShaderDataType
{
  MATRIX,
  MATRIX_ARRAY,
  TEXTURE,
  FLOAT,
};



/* thoughts: 
  Change "ShaderUniform" to "DefaultUniforms", 
  then add additional ones inside the files of the child classes that use them
*/

#endif
