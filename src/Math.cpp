#include "Math.h"

#include <iomanip> // std::setw
#include <random>
// aiMatrix4x4
// a1 a2 a3 a4
// b1 b2 b3 b4
// c1 c2 c3 c4
// d1 d2 d3 d4

// aiMatrix4x4 (row major)
// vec4 rows[4];
// in memory: a1 a2 a3 a4 b1 b2 b3 b4 c1 c2 c3 c4 d1 d2 d3 d4

// glm::mat4 (column major)
// vec4 cols[4]
// in memory: a1 b1 c1 d1 a2 b2 c2 d2 a3 b3 c3 d3 a4 b4 c4 d4

// http://ephenationopengl.blogspot.com/2012/06/doing-animations-in-opengl.html
void CopyaiMat(const aiMatrix4x4 & from, glm::mat4 & to)
{  
  // [col][row] 
  to[0][0] = from.a1; 
  to[1][0] = from.a2;
  to[2][0] = from.a3; 
  to[3][0] = from.a4;
  to[0][1] = from.b1; 
  to[1][1] = from.b2;
  to[2][1] = from.b3; 
  to[3][1] = from.b4;
  to[0][2] = from.c1; 
  to[1][2] = from.c2;
  to[2][2] = from.c3; 
  to[3][2] = from.c4;
  to[0][3] = from.d1; 
  to[1][3] = from.d2;
  to[2][3] = from.d3; 
  to[3][3] = from.d4;
}

glm::mat4 CopyaiMat( const aiMatrix4x4 & from )
{
  return glm::mat4(
    from.a1, from.b1, from.c1, from.d1,
    from.a2, from.b2, from.c2, from.d2,
    from.a3, from.b3, from.c3, from.d3,
    from.a4, from.b4, from.c4, from.d4);
}

std::ostream & operator << (std::ostream & lhs, const aiMatrix4x4 & rhs)
{
  
  for (unsigned r = 0; r < 4; ++r)
  {
    for (unsigned c = 0; c < 4; ++c)
    {
      float val = rhs[r][c];
      if (abs(val) < 0.001f) val = 0;
      lhs << std::setw(4) << std::setprecision(2) << val << " ";
    }
    if (r != 3)
      lhs << std::endl;
  }
  return lhs;
}

std::ostream & operator<<( std::ostream & lhs, const glm::mat4 & rhs )
{
  for (unsigned r = 0; r < 4; ++r)
  {
    for (unsigned c = 0; c < 4; ++c)
    {
      float val = rhs[c][r];
      if (abs(val) < 0.001f)
        val = 0;

      lhs.width(4);
      lhs.precision(2);
      lhs << val << " ";
    }
    if (r != 3) 
      lhs << std::endl;
  }
  return lhs;
}

std::ostream & operator << (std::ostream & lhs, const glm::vec4 & vec)
{

  lhs << "(";
  for (unsigned i = 0; i < 4; ++i)
  {
    float val = vec[i];
    if (abs(val) < 0.001f)
      val = 0;

    lhs.width(4);
    lhs.precision(2);
    
    lhs << val;
    if (i != 3)
      lhs << ", ";
  }
  return lhs << ")"; 
}

aiMatrix4x4 ScaleMatrix( const aiVector3D & vec )
{
  return ScaleMatrix(vec.x, vec.y, vec.z);
}

//aiMatrix4x4 RotateMatrix( const aiVector3D & vec )
//{
//  return RotateMatrix(vec.x, vec.y, vec.z);
//}
aiMatrix4x4 TranslationMatrix( const aiVector3D & vec )
{
  return TranslationMatrix(vec.x, vec.y, vec.z);
}

aiMatrix4x4 ScaleMatrix( float x, float y, float z )
{
  return aiMatrix4x4(
    x,0,0,0,
    0,y,0,0,
    0,0,z,0,
    0,0,0,1);
}

//aiMatrix4x4 RotateMatrix( float x, float y, float z )
//{
//  return aiMatrix4x4(
//    x,0,0,0,
//    0,y,0,0,
//    0,0,z,0,
//    0,0,0,1);
//}
aiMatrix4x4 TranslationMatrix( float x, float y, float z )
{
  return aiMatrix4x4(
    1,0,0,x,
    0,1,0,y,
    0,0,1,z,
    0,0,0,1);
}

// returns a float from 0 to 1
float RandomFloat()
{
  float randomFloat = (float) rand();
  randomFloat /= RAND_MAX;
  return randomFloat;
}

float RandomFloat( float mini, float maxi )
{
  float random0to1 = RandomFloat();
  float difference = maxi - mini;
  return mini + random0to1 * difference;
}

float ToDegrees( float rad )
{
  return rad * RAD_TO_DEG;
}

float ToRadians( float deg )
{
  return deg * DEG_TO_RAD;
}

glm::mat3 RotationMatrixY( float angle )
{
  float cosAngle = cos(angle);
  float sinAngle = sin(angle);
  return glm::mat3x3
    ( glm::vec3(cosAngle,  0, sinAngle)
    , glm::vec3(0,         1,         0)
    , glm::vec3(-sinAngle, 0, cosAngle));  
}

glm::mat3 RotationMatrixX( float angle )
{
  float cosAngle = cos(angle);
  float sinAngle = sin(angle);

  return glm::mat3x3
    ( glm::vec3(1,        0,         0)
    , glm::vec3(0, cosAngle, -sinAngle)
    , glm::vec3(0, sinAngle,  cosAngle));
}

glm::mat3 RotationMatrixZ( float angle )
{
  float cosAngle = cos(angle);
  float sinAngle = sin(angle);

  return glm::mat3x3
    ( glm::vec3(cosAngle,-sinAngle, 0)
    , glm::vec3(sinAngle, cosAngle, 0)
    , glm::vec3(       0,        0, 1));
}

void Translate( glm::mat3x3 & matrix, float tx, float ty )
{
  // col 0         // col 1         // col 2
  matrix[0].x = 1; matrix[1].x = 0; matrix[2].x = tx;
  matrix[0].y = 0; matrix[1].y = 1; matrix[2].y = ty;
  matrix[0].z = 0; matrix[1].z = 0; matrix[2].z = 1;
}

void Rotate( glm::mat3x3 & matrix, float angleRad )
{
  float s = sin(angleRad);
  float c = cos(angleRad);

  // col 0         // col 1         // col 2
  matrix[0].x = c; matrix[1].x =-s; matrix[2].x = 0;
  matrix[0].y = s; matrix[1].y = c; matrix[2].y = 0;
  matrix[0].z = 0; matrix[1].z = 0; matrix[2].z = 1;
}

void Clamp( float & val, float mini, float maxi )
{
  if (val < mini) 
    val = mini;
  else if (val > maxi)
    val = maxi;
}


/* for testing the order of glm

  glm::mat4 foo = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(0,0,1));
  std::cout << foo << std::endl << std::endl;

  // 1st 4 values are col 0
  ./ 2nd 4 values are col 1
  // 3rd 4 values are col 2
  // 4th 4 values are col 3
  glm::mat4 bar (0,1,0,0,-1,0,0,0,0,0,1,0,0,0,0,1);

  std::cout << bar << std::endl;

  THE OUTPUT OF FOO AND BAR SHOULD MATCH


*/