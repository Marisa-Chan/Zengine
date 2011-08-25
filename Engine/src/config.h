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

//MENU//
#define menu_UpdateMenuBar menu_UpdateZGIMenuBar
#define menu_DrawMenuBar   menu_DrawZGIMenuBar

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

#define GAMESCREEN_H_2  (GAMESCREEN_H >> 1)
#define GAMESCREEN_W_2  (GAMESCREEN_W >> 1)


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

#endif


#ifdef GAME_NEMESIS
//TITLE
#define TITLE       "Z-Engine: Zork Nemesis (dev) [build: "__DATE__" "__TIME__"]"

//ScriptSystem params
#define VAR_SLOTS_MAX      30000 //For nemesis 30000
#define STATEBOX_STACK_MAX 1024
#define MaxStateBoxEnts    0x800 //0xFF is very small
#define pzlSTACK 0x800


//speed tune
#define FPS         15
#define FPS_DELAY   66 //millisecs  1000/FPS

//MENU//
#define menu_UpdateMenuBar menu_UpdateZGIMenuBar
#define menu_DrawMenuBar   menu_DrawZGIMenuBar

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

#define GAMESCREEN_H_2  (GAMESCREEN_H >> 1)
#define GAMESCREEN_W_2  (GAMESCREEN_W >> 1)


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

#endif





#define SYSTEM_STRINGS_NUM 64
#define TRY_CHANNELS 16
#define FILE_LN_BUF 0x400

#define CHANGELOCATIONDELAY 2
#define MINIBUFSZ           16

#endif // CONFIG_H_INCLUDED
