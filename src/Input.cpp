#include "Engine.h"
#include "Input.h"

#include <GL/glfw.h>
#include <AntTweakBar.h>


bool Input::IsKeyDown(int key)
{
  // The function returns 
  // GLFW_PRESS if the key is held down
  // GLFW_RELEASE if the key is not held down.
  return glfwGetKey(key) == GLFW_PRESS ? true : false;
}

void Input::Update(float dt)
{
  // do nothing
}


/*
  @param button
    GLFW_MOUSE_BUTTON_LEFT,
    GLFW_MOUSE_BUTTON_RIGHT,
    GLFW_MOUSE_BUTTON_MIDDLE
  @param action
    GLFW_PRESS
    GLFW_RELEASE.
*/
void Input::MouseButtonCallback( int button, int action )
{
  // alert anttweakbar
  TwEventMouseButtonGLFW(button, action);
}

void Input::MousePosCallback( int x, int y )
{
  // alert antweakbar
  TwEventMousePosGLFW(x, y);
  Engine::GetSystem<Input>().mMouseX = x;
  Engine::GetSystem<Input>().mMouseY = y;
}

void Input::MouseWheelCallback( int pos )
{
  TwEventMouseWheelGLFW(pos);
  Engine::GetSystem<Input>().mMouseWheelPos = pos;
}

/*
  @param glfwChar
    ex: 'A' or GLFW_KEY_SPACE
  @param action
    GLFW_PRESS
    GLFW_RELEASE
*/
void Input::CharCallback( int glfwChar, int action )
{
  TwEventCharGLFW(glfwChar, action);
  // todo: more things
}

void Input::KeyCallback( int glfwKey, int action )
{
  TwEventKeyGLFW(glfwKey, action);
}

void Input::Init()
{
  glfwSetMouseButtonCallback(MouseButtonCallback);
  glfwSetMousePosCallback(MousePosCallback);
  glfwSetMouseWheelCallback(MouseWheelCallback);
  glfwSetCharCallback(CharCallback);
  glfwSetKeyCallback(KeyCallback);
}

void Input::Cleanup()
{
  glfwSetMouseButtonCallback(0);
  glfwSetMousePosCallback(0);
  glfwSetMouseWheelCallback(0);
  glfwSetCharCallback(0);
  glfwSetKeyCallback(0);
}
