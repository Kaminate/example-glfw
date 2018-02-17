// Nathan Park 2013

// engine include
#include "Engine.h"

// system includes
#include "Graphics.h"
#include "Temporary.h"
#include "Input.h"

int main()
{



  Engine::AddSystem(new Graphics);
  Engine::AddSystem(new Input);
  Engine::AddSystem(new Temporary);
  Engine::Init();
  Engine::Run();
  Engine::Cleanup();
  
  return 0;
}
