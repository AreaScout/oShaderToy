// by dff180

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "MyGLWindow.h"

#include  <X11/Xlib.h>
#include  <X11/Xutil.h>

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
  win.bIsRunning = true;
  
  while(win.bIsRunning)
  {
    if(XPending(win.XDisplay) > 0)
    {
      XEvent ev;
      XNextEvent(win.XDisplay, &ev);
      switch(ev.type)
      {
        case KeyPress:
          win.bIsRunning = false;
        break;
      }
    }    
    win.paintGL();
    sleep(0.01);
  }
  delete config;
}

