CC = gcc
CXX = g++
MINGW = /usr/i486-mingw32
CC_WIN = i486-mingw32-gcc
CXX_WIN = i486-mingw32-g++
LIBS = -Bstatic -lSDL -lSDL_image -lSDL_gfx -lSDLmain -lSDL_mixer -lSDL_ttf
LIBS_WIN = -L$(MINGW)/lib/ -lmingw32  -lSDL  -lSDL_image -lSDL_gfx -lSDLmain -lSDL_mixer -lSDL_ttf -lSDL.dll -mwindows
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
	loader.cpp\
	mfile.cpp\
	inventory.cpp\
	intro.cpp\
	avi_duck/simple_avi.cpp\
	avi_duck/truemotion1.cpp
	
WINOBJ = \
	win_funct.cpp

linux_zgi: $(OBJS)
	$(CXX) -Wno-write-strings -DGAME_ZGI -O3 -Os -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/
	$(CXX) -Wno-write-strings -DWIDESCREEN -DGAME_ZGI -O3 -Os -o $(TARGET)_wide $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

linux_zgi_trace: $(OBJS)
	$(CXX) -Wno-write-strings -DTRACE -DGAME_ZGI -g -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

linux_zgi_trace_path: $(OBJS)
	$(CXX) -Wno-write-strings -DDEBUG_LOADER -DTRACE -DGAME_ZGI -g -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

linux_znem: $(OBJS)
	$(CXX) -Wno-write-strings -DGAME_NEMESIS -O3 -Os -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

linux_znem_trace: $(OBJS)
	$(CXX) -Wno-write-strings -DTRACE -DGAME_NEMESIS -g -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

linux_znem_trace_path: $(OBJS)
	$(CXX) -Wno-write-strings -DDEBUG_LOADER -DTRACE -DGAME_NEMESIS -g -o $(TARGET) $(OBJS) $(LIBS) -I /usr/include/ -I /usr/include/SDL/

win32_zgi_trace: $(OBJS) $(WINOBJ)
	$(CXX_WIN) -Wno-write-strings -DTRACE -DGAME_ZGI -O3 -Os -o $(TARGET).exe $(OBJS) $(WINOBJ) $(LIBS_WIN) -I $(MINGW)/include/ -I $(MINGW)/include/SDL/

win32_zgi: $(OBJS) $(WINOBJ)
	$(CXX_WIN) -Wno-write-strings -DGAME_ZGI -O3 -Os -o $(TARGET).exe $(OBJS) $(WINOBJ) $(LIBS_WIN) -I $(MINGW)/include/ -I $(MINGW)/include/SDL/
	$(CXX_WIN) -Wno-write-strings -DWIDESCREEN -DGAME_ZGI -O3 -Os -o $(TARGET)_wide.exe $(OBJS) $(WINOBJ) $(LIBS_WIN) -I $(MINGW)/include/ -I $(MINGW)/include/SDL/

win32_znem_trace: $(OBJS) $(WINOBJ)
	$(CXX_WIN) -Wno-write-strings -DTRACE -DGAME_NEMESIS -O3 -Os -o $(TARGET).exe $(OBJS) $(WINOBJ) $(LIBS_WIN) -I $(MINGW)/include/ -I $(MINGW)/include/SDL/

win32_znem: $(OBJS) $(WINOBJ)
	$(CXX_WIN) -Wno-write-strings -DGAME_NEMESIS -O3 -Os -o $(TARGET).exe $(OBJS) $(WINOBJ) $(LIBS_WIN) -I $(MINGW)/include/ -I $(MINGW)/include/SDL/

clean:
	rm -f $(TARGET) $(TARGET_DINGOO) *.o