CC = gcc
CXX = g++
MINGW = /usr/i486-mingw32
CC_WIN = i486-mingw32-gcc
CXX_WIN = i486-mingw32-g++
FLAGS = -Wall
LIBS = -lSDL -lSDL_image -lSDL_gfx -lSDLmain -lSDL_mixer -lSDL_ttf -lsmpeg -lrt
LIBS_WIN = -L$(MINGW)/lib/ -lmingw32 -lSDL -lSDL_image -lSDL_gfx -lSDLmain -lSDL_mixer -lSDL_ttf -lsmpeg -lSDL.dll -mwindows
TARGET = Zengine
OBJS =  \
	actions.cpp\
	Control.cpp\
	Game.cpp\
	Graph.cpp\
	main.cpp\
	System.cpp\
	Mouse.cpp\
	mylist.cpp\
	Puzzle.cpp\
	Render.cpp\
	ScriptSystem.cpp\
	SoundFX.cpp\
	Timers.cpp\
 	Sound.cpp\
	anims.cpp\
	menu.cpp\
	Subtitles.cpp\
	Text.cpp\
	Optimiz.cpp\
	intro.cpp\
	inventory.cpp
	
WINOBJ = \
	win_funct.cpp

linux_zgi: $(OBJS)
	$(CXX) $(FLAGS) -DGAME_ZGI -O3 -Os -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/
	$(CXX) $(FLAGS) -DWIDESCREEN -DGAME_ZGI -O3 -Os -o $(TARGET)_wide $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

linux_znem: $(OBJS)
	$(CXX) $(FLAGS) -DGAME_NEMESIS -O3 -Os -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

win32_zgi: $(OBJS) $(WINOBJ)
	$(CXX_WIN) $(FLAGS) -DGAME_ZGI -O3 -Os -o $(TARGET).exe $(OBJS) $(WINOBJ) $(LIBS_WIN) -I $(MINGW)/include/ -I $(MINGW)/include/SDL/
	$(CXX_WIN) $(FLAGS) -DWIDESCREEN -DGAME_ZGI -O3 -Os -o $(TARGET)_wide.exe $(OBJS) $(WINOBJ) $(LIBS_WIN) -I $(MINGW)/include/ -I $(MINGW)/include/SDL/

win32_znem: $(OBJS) $(WINOBJ)
	$(CXX_WIN) $(FLAGS) -DGAME_NEMESIS -O3 -Os -o $(TARGET_WIN) $(OBJS) $(WINOBJ) $(LIBS_WIN) -I $(MINGW)/include/ -I $(MINGW)/include/SDL/

clean:
	rm -f $(TARGET) $(TARGET_DINGOO) *.o
