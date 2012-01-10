#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#ifdef GAME_ZGI
//TITLE
#define TITLE       "Z-Engine: Zork Grand Inquisitor (RC1) [build: "__DATE__" "__TIME__"]"

//ScriptSystem params
#define VAR_SLOTS_MAX      20000 //For nemesis 30000
#define STATEBOX_STACK_MAX 1024
#define MaxStateBoxEnts    0x800 //0xFF is very small
#define pzlSTACK 0x800


//speed tune
#define FPS         15
#define FPS_DELAY   66 //millisecs  1000/FPS

//Mouse
#define Mouse_LoadCursors      Mouse_LoadCursors_zgi
#define Mouse_LoadObjCursor    Mouse_LoadObjCursor_zgi


//Preferences
#define pref_FileName      "./INQUIS.INI"
#define pref_TagString     "ZorkGrandInquisitor"


//Graphics
#define GAME_W   640
#define GAME_H   480
#define GAME_BPP 32

#define GAMESCREEN_W   640
#define GAMESCREEN_P   60    //Panorama turn area
#define GAMESCREEN_H   344
#define GAMESCREEN_X   0
#define GAMESCREEN_Y   68
#define GAMESCREEN_FLAT_X 0

#define GAMESCREEN_H_2  (GAMESCREEN_H >> 1)
#define GAMESCREEN_W_2  (GAMESCREEN_W >> 1)


//// WIDESCREEN SUPPORT
#ifdef WIDESCREEN

#undef GAME_W
#define GAME_W 854

#undef GAMESCREEN_W
#define GAMESCREEN_W 854

#undef GAMESCREEN_FLAT_X
#define GAMESCREEN_FLAT_X 107
#endif

//timer
#define TIMER_DELAY   100

//MENU//
#define menu_UpdateMenuBar menu_UpdateMenuBar_zgi
#define menu_DrawMenuBar   menu_DrawMenuBar_zgi
#define menu_LoadGraphics  menu_LoadGraphics_zgi
#define menu_HOT_Y         40
#define menu_MAIN_X        ((GAME_W-580)>>1)
#define menu_MAIN_CENTER   (GAME_W >> 1)
#define menu_SCROLL_DELAY  5

//Script names
#define SystemWorld 'g'
#define SystemRoom  'j'

#define SaveWorld   SystemWorld
#define SaveRoom    SystemRoom
#define SaveNode    's'
#define SaveView    'e'

#define LoadWorld   SystemWorld
#define LoadRoom    SystemRoom
#define LoadNode    'r'
#define LoadView    'e'

#define PrefWorld   SystemWorld
#define PrefRoom    SystemRoom
#define PrefNode    'p'
#define PrefView    'e'

#define InitWorld   SystemWorld
#define InitRoom    'a'
#define InitNode    'r'
#define InitView    'y'

//Save
#define SAVEBUFFER_SIZE     VAR_SLOTS_MAX*4+12+16+8+8+100*16
#define MAX_SAVES           100
#define SAVE_NAME_MAX_LEN   20

#define SYS_STRINGS_FILE    "INQUIS.STR"

//Slot-controls
#define CTRL_SLOT_FILE_NAME "g0z%1.1su%2.2x1.tga"

//Lever-control
#define CTRL_LEVER_MAX_FRAMES  128
#define CTRL_LEVER_MAX_DIRECTS 4
#define CTRL_LEVER_ANGL_TIME   50
#define CTRL_LEVER_ANGL_DELTA  30
#define CTRL_LEVER_AUTO_DELAY  FPS_DELAY

//fist-control
#define CTRL_FIST_MAX_FISTS  4
#define CTRL_FIST_MAX_BOXES  6
#define CTRL_FIST_MAX_ENTRS  64






#elif GAME_NEMESIS





//TITLE
#define TITLE       "Z-Engine: Zork Nemesis (dev) [build: "__DATE__" "__TIME__"]"

//ScriptSystem params
#define VAR_SLOTS_MAX      30100 //For nemesis 30000
#define STATEBOX_STACK_MAX 1024
#define MaxStateBoxEnts    0x800 //0xFF is very small
#define pzlSTACK 0x800



//speed tune
#define FPS         15
#define FPS_DELAY   66 //millisecs  1000/FPS

//Mouse
#define Mouse_LoadCursors      Mouse_LoadCursors_znemesis
#define Mouse_LoadObjCursor    Mouse_LoadObjCursor_znemesis


//Preferences
#define pref_FileName      "./ZNEMESIS.INI"
#define pref_TagString     "ZorkNemesis"


//Graphics
#define GAME_W   640
#define GAME_H   480
#define GAME_BPP 32

#define GAMESCREEN_W   512
#define GAMESCREEN_P   60    //Panorama turn area
#define GAMESCREEN_H   320
#define GAMESCREEN_X   64     //(640-512)/2
#define GAMESCREEN_Y   80    //(480-320)/2
#define GAMESCREEN_FLAT_X 0

#define GAMESCREEN_H_2  (GAMESCREEN_H >> 1)
#define GAMESCREEN_W_2  (GAMESCREEN_W >> 1)


//timer
#define TIMER_DELAY   1000


//MENU//
#define menu_UpdateMenuBar menu_UpdateMenuBar_znemesis
#define menu_DrawMenuBar   menu_DrawMenuBar_znemesis
#define menu_LoadGraphics  menu_LoadGraphics_znemesis
#define menu_HOT_Y         40
#define menu_MAIN_X       ((GAME_W-512)>>1)
#define menu_MAIN_CENTER  (GAME_W >> 1)
#define menu_SCROLL_DELAY  15


//Script names
#define SystemWorld 'g'
#define SystemRoom  'j'

#define SaveWorld   SystemWorld
#define SaveRoom    SystemRoom
#define SaveNode    's'
#define SaveView    'e'

#define LoadWorld   SystemWorld
#define LoadRoom    SystemRoom
#define LoadNode    'r'
#define LoadView    'e'

#define PrefWorld   SystemWorld
#define PrefRoom    SystemRoom
#define PrefNode    'p'
#define PrefView    'e'

#define InitWorld   SystemWorld
#define InitRoom    'a'
#define InitNode    'r'
#define InitView    'y'

//Save
#define SAVEBUFFER_SIZE     VAR_SLOTS_MAX*4+12+16+8+8+100*16
#define MAX_SAVES           100
#define SAVE_NAME_MAX_LEN   20

#define SYS_STRINGS_FILE    "NEMESIS.STR"

//Slot-controls
#define CTRL_SLOT_FILE_NAME "%d%sOBJ.TGA"

//Lever-control
#define CTRL_LEVER_MAX_FRAMES  128
#define CTRL_LEVER_MAX_DIRECTS 4
#define CTRL_LEVER_ANGL_TIME   50
#define CTRL_LEVER_ANGL_DELTA  30
#define CTRL_LEVER_AUTO_DELAY  FPS_DELAY

//fist-control
#define CTRL_FIST_MAX_FISTS  4
#define CTRL_FIST_MAX_BOXES  6
#define CTRL_FIST_MAX_ENTRS  64

#else

#error Please specify game.

#endif



#define DBL_CLK_TIME       250


#define SYSTEM_STRINGS_NUM 64
#define TRY_CHANNELS 16
#define FILE_LN_BUF 0x400

#define CHANGELOCATIONDELAY 2
#define MINIBUFSZ           32

#define EFFECTS_MAX_CNT     32

#endif // CONFIG_H_INCLUDED
