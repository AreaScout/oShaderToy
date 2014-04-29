/*
  Copyright (C) 2012 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received m_a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "EGLWindow.h"
#include "EGLConfig.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <linux/fb.h>

// instrument this a bit like QGLWidget by adding a
// a context so we can choose GL version rgb size etc.
// This can be done with a context object at some stage.
EGLWindow::EGLWindow(EGLconfig *_config)
{
 // toggle we don't yet have an active surface
 m_activeSurface=false;
 // set default to not upscale the screen resolution
 m_upscale=false;
 // set our display values to 0 (not once ported to cx11 will use nullptr but
 // current pi default compiler doesn't support it yet
 m_display=0;
 m_context=0;
 m_surface=0;
 
  // if we have a user defined config we will use that else we need to create one
 if (_config == 0)
 {
    std::cout<<"making new config\n";
    m_config= new EGLconfig();
 }
 else
 {
    m_config=_config;
 }

  // this code actually creates the surface
 makeSurface(0,0);

}

void EGLWindow::setScreen(uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h)
{
	// destroy our surface the make a new one
	destroySurface();
	makeSurface(_x,_y);
}

void EGLWindow::makeSurface(uint32_t _x, uint32_t _y)
{
	// this code does the main window creation
	EGLBoolean result;

        Window native_window;

	// config you use OpenGL ES2.0 by default
	static const EGLint context_attributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	XSetWindowAttributes XWinAttr;
	Atom XWMDeleteMessage;
	Window XRoot;

	XDisplay = XOpenDisplay(NULL);
	if (!XDisplay) {
		fprintf(stderr, "Error: failed to open X display.\n");
		return;
	}

	int screen_num = DefaultScreen(XDisplay);
	uint32_t w = DisplayWidth(XDisplay, screen_num);
	uint32_t h = DisplayHeight(XDisplay, screen_num);
	
        m_maxWidth=w;
        m_maxHeight=h;	
	
	XRoot = DefaultRootWindow(XDisplay);

	XWinAttr.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;

	native_window = XCreateWindow(XDisplay, XRoot, 0, 0, w, h, 0,
				CopyFromParent, InputOutput,
				CopyFromParent, CWEventMask, &XWinAttr);

	XWMDeleteMessage = XInternAtom(XDisplay, "WM_DELETE_WINDOW", False);

        XEvent xev;
        Atom wm_state   = XInternAtom(XDisplay, "_NET_WM_STATE", False);
        Atom fullscreen = XInternAtom(XDisplay, "_NET_WM_STATE_FULLSCREEN", False);

        memset(&xev, 0, sizeof(xev));
        xev.type = ClientMessage;
        xev.xclient.window = native_window;
        xev.xclient.message_type = wm_state;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = 1;
        xev.xclient.data.l[1] = fullscreen;
        xev.xclient.data.l[2] = 0;
	XMapWindow(XDisplay, native_window);
        XSendEvent(XDisplay, DefaultRootWindow(XDisplay), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
        XFlush(XDisplay);
	XStoreName(XDisplay, native_window, "oShaderToy");
	XSetWMProtocols(XDisplay, native_window, &XWMDeleteMessage, 1);

	// get an EGL display connection
	m_display = eglGetDisplay((EGLNativeDisplayType) XDisplay);
	if(m_display == EGL_NO_DISPLAY)
	{
		std::cerr<<"error getting display\n";
		exit(EXIT_FAILURE);
	}
	// initialize the EGL display connection
	int major,minor;

	result = eglInitialize(m_display, &major, &minor);
	std::cout<<"EGL init version "<<major<<"."<<minor<<"\n";
	if(result == EGL_FALSE)
	{
		std::cerr<<"error initialising display\n";
		exit(EXIT_FAILURE);
	}
	// get our config from the config class
	m_config->chooseConfig(m_display);
	EGLConfig config=m_config->getConfig();
	// bind the OpenGL API to the EGL
	result = eglBindAPI(EGL_OPENGL_ES_API);
	if(result ==EGL_FALSE)
	{
		std::cerr<<"error binding API\n";
		exit(EXIT_FAILURE);
	}
	// create an EGL rendering context
	m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, context_attributes);
	if(m_context ==EGL_NO_CONTEXT)
	{
		std::cerr<<"couldn't get a valid context\n";
		exit(EXIT_FAILURE);
	}
	// finally we can create a new surface using this config and window
	m_surface = eglCreateWindowSurface( m_display, config, (NativeWindowType)native_window, NULL );
	assert(m_surface != EGL_NO_SURFACE);
	// connect the context to the surface
	result = eglMakeCurrent(m_display, m_surface, m_surface, m_context);
	assert(EGL_FALSE != result);
	m_activeSurface=true;

}

void EGLWindow::destroySurface()
{
	if(m_activeSurface == true)
	{
		eglSwapBuffers(m_display, m_surface);
		// here we free up the context and display we made earlier
		eglMakeCurrent( m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
		eglDestroySurface( m_display, m_surface );
		eglDestroyContext( m_display, m_context );
		eglTerminate( m_display );
		m_activeSurface=false;
	}
}

EGLWindow::~EGLWindow()
{
	// clear screen
	std::cout<<"dtor called\n";
	glClear( GL_COLOR_BUFFER_BIT );
	destroySurface();
}

void EGLWindow::swapBuffers() const
{
	eglSwapBuffers(m_display, m_surface);
}

void EGLWindow::resizeScreen(uint32_t _w, uint32_t _h)
{
	destroySurface();
	makeSurface(0,0);
}
