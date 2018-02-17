#ifndef _INPUT_H_
#define _INPUT_H_

#include "System.h"
#include <GL/glfw.h>
  
// todo: a hierarchical callback system
// see: http://thefullbrightcompany.com/2013/02/15/gone-homes-input-system/


class Input : public System
{
  
  // static callbacks (from glfw)
  static void CharCallback(int glfwChar, int action);
  static void KeyCallback(int glfwKey, int action);
  static void MouseButtonCallback(int button, int action);
  int mMouseX;
  int mMouseY;
  static void MousePosCallback(int x, int y);
  int mMouseWheelPos;
  static void MouseWheelCallback(int pos);
  
public:
  bool IsKeyDown(int key); // 'a', ('A'?)
  virtual void Init();
  virtual void Update(float dt);
  virtual void Cleanup();

/* reference:
  see page 25: http://www.glfw.org/GLFWReference278.pdf
  
  GLFW_KEY_SPACE Space
  GLFW_KEY_ESC Escape
  GLFW_KEY_Fn Function key n (n can be in the range 1..25)
  GLFW_KEY_UP Cursor up
  GLFW_KEY_DOWN Cursor down
  GLFW_KEY_LEFT Cursor left
  GLFW_KEY_RIGHT Cursor right
  GLFW_KEY_LSHIFT Left shift key
  GLFW_KEY_RSHIFT Right shift key
  GLFW_KEY_LCTRL Left control key
  GLFW_KEY_RCTRL Right control key
  GLFW_KEY_LALT Left alternate function key
  GLFW_KEY_RALT Right alternate function key
  GLFW_KEY_LSUPER Left super key, WinKey, or command key
  GLFW_KEY_RSUPER Right super key, WinKey, or command key
  GLFW_KEY_TAB Tabulator
  GLFW_KEY_ENTER Enter
  GLFW_KEY_BACKSPACE Backspace
  GLFW_KEY_INSERT Insert
  GLFW_KEY_DEL Delete
  GLFW_KEY_PAGEUP Page up
  GLFW_KEY_PAGEDOWN Page down
  GLFW_KEY_HOME Home
  GLFW_KEY_END End
  GLFW_KEY_KP_n Keypad numeric key n (n can be in the range 0..9)
  GLFW_KEY_KP_DIVIDE Keypad divide ()
  GLFW_KEY_KP_MULTIPLY Keypad multiply ()
  GLFW_KEY_KP_SUBTRACT Keypad subtract ()
  GLFW_KEY_KP_ADD Keypad add (+)
  GLFW_KEY_KP_DECIMAL Keypad decimal (. or ,)
  GLFW_KEY_KP_EQUAL Keypad equal (=)
  GLFW_KEY_KP_ENTER Keypad enter
  GLFW_KEY_KP_NUM_LOCK Keypad num lock
  GLFW_KEY_CAPS_LOCK Caps lock
  GLFW_KEY_SCROLL_LOCK Scroll lock
  GLFW_KEY_PAUSE Pause key
  GLFW_KEY_MENU
*/
};

#endif
