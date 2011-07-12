#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED


//// TITLE
#define TITLE       "Z-Engine: Zork Grand Inquisitor (Beta 1)"
////


#define VAR_SLOTS_MAX      20000 //For nemesis 22320
#define STATEBOX_STACK_MAX 1024
#define MaxStateBoxEnts    0x400 //0xFF is very small

#define pzlSTACK 0x800

#define TRY_CHANNELS 16

#define FILE_LN_BUF 0x400



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

#endif // CONFIG_H_INCLUDED
