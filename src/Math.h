#ifndef _MATH_H_
#define _MATH_H_

#define GLM_SWIZZLE
#include <glm/glm.hpp> //glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective

#include <assimp/types.h>

#include <iostream>

#define PI 3.14159f
#define TWO_PI PI * 2.0f
#define DEG_TO_RAD PI / 180.0f
#define RAD_TO_DEG 180.0f / PI

void Clamp(float & val, float mini, float maxi);

glm::mat4 CopyaiMat(const aiMatrix4x4 & from);
void CopyaiMat(const aiMatrix4x4 & from, glm::mat4 & to);

std::ostream & operator << (std::ostream & lhs, const aiMatrix4x4 & rhs);
std::ostream & operator << (std::ostream & lhs, const glm::mat4 & rhs);
std::ostream & operator << (std::ostream & lhs, const glm::vec4 & vec);

glm::mat3 RotationMatrixX(float angle);
glm::mat3 RotationMatrixY(float angle);
glm::mat3 RotationMatrixZ(float angle);

aiMatrix4x4 ScaleMatrix(const aiVector3D & vec);
aiMatrix4x4 TranslationMatrix(const aiVector3D & vec);
aiMatrix4x4 ScaleMatrix(float x, float y, float z);
aiMatrix4x4 TranslationMatrix(float x, float y, float z);

void Translate(glm::mat3x3 & matrix, float tx, float ty);
void Rotate(glm::mat3x3 & matrix, float angleRad);

void Translate(glm::mat4 & matrix, float tx, float ty, float tz);
void RotateZ(glm::mat4 & matrix, float angleRad);


// returns a float from 0 to 1
float RandomFloat();
float RandomFloat(float mini, float maxi);
float ToDegrees(float rad);
float ToRadians(float deg);

#endif

