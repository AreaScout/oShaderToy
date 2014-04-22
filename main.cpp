// by dff180

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "MyGLWindow.h"

int main(int argc, char ** argv)
{

  // Parse command line
  std::string fragmentShader;
  std::string textureName;

  if (argc >= 2)
    fragmentShader = argv[1];

  if (argc >= 3)
    textureName = argv[2];

  EGLconfig *config = new EGLconfig();
  config->setRGBA(8,8,8,8);
  
  config->setDepth(16);
  
  MyGLWindow win(config, fragmentShader, textureName);

  while(1)
  {
    static int count = 0;
    count++;
    if(count == 1000)
      break;
    win.paintGL();
    sleep(0.01);
  }
}

