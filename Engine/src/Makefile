CC = gcc
CXX = g++
LIBS = -lSDL -lSDL_image -lSDL_gfx -lSDLmain -lSDL_mixer -lsmpeg -lrt
TARGET=Zengine
TARGET_DINGOO=$(TARGET).dge
OBJS =  \
	actions.cpp\
	Control.cpp\
	Game.cpp\
	Graph.cpp\
	Manage.cpp\
	main.cpp\
	System.cpp\
	Mouse.cpp\
	mylist.cpp\
	Puzzle.cpp\
	Render.cpp\
	ScriptSystem.cpp\
	SoundFX.cpp\
	Timers.cpp\
 	Sound.cpp
	


pc: $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

clean:
	rm -f $(TARGET) $(TARGET_DINGOO) *.o
