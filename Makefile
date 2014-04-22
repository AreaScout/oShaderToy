CC=g++
CFLAGS=-c -g -Wall -O3  -I/usr/include 
LDFLAGS=-L/usr/lib -lX11 -lGLESv2 -lEGL -lSOIL
SOURCES=EGLConfig.cpp EGLWindow.cpp main.cpp MyGLWindow.cpp shader_utils.cpp
OBJECTS=$(SOURCES:%.cpp=%.o)
EXECUTABLE=oShaderToy

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean :
	rm -f *.o $(EXECUTABLE)
