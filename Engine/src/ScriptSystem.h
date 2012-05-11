#ifndef SCRIPTSYSTEM_H_INCLUDED
#define SCRIPTSYSTEM_H_INCLUDED

#include "types.h"
#include "config.h"

#include "Puzzle.h"


#define SLOT_WORLD              3
#define SLOT_ROOM               4
#define SLOT_NODE               5
#define SLOT_VIEW               6
#define SLOT_VIEW_POS           7
#define SLOT_KEY_PRESS          8 //keycode in slot
#define SLOT_INVENTORY_MOUSE    9 //code of item in slot
#define SLOT_MOUSE_DOWN        10 //1 if clicked
#define SLOT_ROUNDS            12
#define SLOT_VENUS             13
#define SLOT_MOUSE_RIGHT_CLICK 18 //1 if right button of mouse
#define SLOT_MENU_STATE        19
#define SLOT_JUST_RESTORED     20
#define SLOT_ON_QUIT           39 //1 - quitting
#define SLOT_LASTWORLD         40
#define SLOT_LASTROOM          41
#define SLOT_LASTNODE          42
#define SLOT_LASTVIEW          43
#define SLOT_LASTVIEW_POS      44
#define SLOT_MENU_LASTWORLD    45
#define SLOT_MENU_LASTROOM     46
#define SLOT_MENU_LASTNODE     47
#define SLOT_MENU_LASTVIEW     48
#define SLOT_MENU_LASTVIEW_POS 49
#define SLOT_KBD_ROTATE_SPEED  50
#define SLOT_SUBTITLE_FLAG     51 //ShowSubtitles
#define SLOT_STREAMSKIP_KEY    52
#define SLOT_PANAROTATE_SPEED  53
#define SLOT_MASTER_VOLUME     56
#define SLOT_QSOUND_ENABLE     57
#define SLOT_VENUSENABLED      58
#define SLOT_HIGH_QUIALITY     59
#define SLOT_LINE_SKIP_VIDEO   65
#define SLOT_PLATFORM          66
#define SLOT_INSTALL_LEVEL     67
#define SLOT_COUNTRY_CODE      68
#define SLOT_CPU               69
#define SLOT_MOVIE_CURSOR      70
#define SLOT_TURN_OFF_ANIM     71 //NoAnimWhileTurning
#define SLOT_WIN958            72
#define SLOT_SHOWERRORDIALOG   73
#define SLOT_DEBUGCHEATS       74 //if set to 1, we may type GOXXXX while game, and it will be changed location to XXXX
                                    //To change - type DBGONOFF
#define SLOT_JAPANESEFONTS     75
#define SLOT_BRIGHTNESS        77
#define SLOT_EF9_B             91
#define SLOT_EF9_G             92
#define SLOT_EF9_R             93
#define SLOT_EF9_SPEED         94
#define SLOT_INV_STORAGE_S1   100
#define SLOT_INV_STORAGE_0    101
#define SLOT_INV_STORAGE_1    102
#define SLOT_INV_STORAGE_2    103
#define SLOT_INV_STORAGE_3    104
#define SLOT_INV_STORAGE_4    105
#define SLOT_INV_STORAGE_5    106
#define SLOT_INV_STORAGE_6    107
#define SLOT_INV_STORAGE_50   149
#define SLOT_TOTAL_INV_AVAIL  150

#define SLOT_START_SLOT       151
#define SLOT_END_SLOT         170


#define SLOT_SPELL_1          191
#define SLOT_SPELL_2          192
#define SLOT_SPELL_3          193
#define SLOT_SPELL_4          194
#define SLOT_SPELL_5          195
#define SLOT_SPELL_6          196
#define SLOT_SPELL_7          197
#define SLOT_SPELL_8          198
#define SLOT_SPELL_9          199
#define SLOT_SPELL_10         200
#define SLOT_SPELL_11         201
#define SLOT_SPELL_12         202
#define SLOT_USER_CHOSE_THIS_SPELL 205
#define SLOT_REVERSED_SPELLBOOK    206

#define FLAG_ONCE_PER_I 1
#define FLAG_DISABLED   2
#define FLAG_DO_ME_NOW  4

#define CRIT_OP_EQU   0  //=
#define CRIT_OP_LEA   1  //<
#define CRIT_OP_GRE   2  //>
#define CRIT_OP_NOT   3  //!

#define NODE_TYPE_MUSIC    0
#define NODE_TYPE_TIMER    1
#define NODE_TYPE_ANIMPLAY 2
#define NODE_TYPE_ANIMPRE  3
#define NODE_TYPE_ANIMPRPL 4
#define NODE_TYPE_PANTRACK 5
#define NODE_TYPE_TTYTEXT  6
#define NODE_TYPE_SYNCSND  7
#define NODE_TYPE_DISTORT  8
#define NODE_TYPE_REGION   9

#define NODE_RET_OK        0
#define NODE_RET_DELETE    1
#define NODE_RET_NO        2


struct StateBoxEnt{
    puzzlenode * nod[MaxStateBoxEnts];
    int32_t     cnt;
};

struct pzllst{
    MList      *_list;
    puzzlenode *stack[pzlSTACK];
    int16_t     stksize;
    uint8_t     exec_times;
};

struct struct_action_res
{
    int32_t    slot;
    pzllst     *owner;
    int8_t     node_type;
    union nodes{
        musicnode       *node_music;
        animnode        *node_anim;
        animnode        *node_animpre;
        anim_preplay_node *node_animpreplay;
        struct_syncnode *node_sync;
        int32_t          node_timer;
        struct_ttytext  *tty_text;
        struct_distort  *distort;

        int32_t          node_pantracking;
        int32_t          node_region;
        void        *node_unknow;
    } nodes;
    bool       need_delete;
    bool       first_process;
};

pzllst *GetUni();
pzllst *Getworld();
pzllst *Getroom();
pzllst *Getview();
MList  *Getctrl();
MList  *GetAction_res_List();

void ScrSys_AddToActResList(void *);

const char * ScrSys_ReturnListName(pzllst *lst);

MList *ScrSys_FindResAllBySlot(int32_t slot);

void ScrSys_ProcessAllRes();

void    ScrSys_SetSystemClass(char World, char Room);
uint8_t ScrSys_GetSystemWorld();
uint8_t ScrSys_GetSystemRoom();

void ScrSys_DeleteAllRes();

uint8_t ScrSys_GetFlag(uint32_t indx);
void    ScrSys_SetFlag(uint32_t indx, uint8_t newval);

bool ScrSys_SlotIsOwned(uint32_t i);

void SetgVarInt(int32_t indx, int var);
int  GetgVarInt(int32_t indx);
void SetDirectgVarInt(uint32_t indx, int var);
int *getdirectvar(uint32_t indx);



void LoadScriptFile(pzllst *lst, FManNode *filename, bool control, MList *controlst);
void InitScriptsEngine();

void FillStateBoxFromList(pzllst *lst);

void ScrSys_ChangeLocation(uint8_t w, uint8_t r, uint8_t v1, uint8_t v2, int32_t X,bool force_all);

void ScrSys_exec_puzzle_list(pzllst *lst);
int ScrSys_DeleteNode(struct_action_res *nod);

void ScrSys_FlushResourcesByOwner(pzllst *owner);
void ScrSys_FlushResourcesByType(int type);

void ShakeStateBox(uint32_t indx);

bool ScrSys_BreakExec();
void ScrSys_SetBreak();


bool ScrSys_SlotIsOwned2(int32_t i);
void ScrSys_FlushGNodes();
void ScrSys_RereadGNodes();
struct_action_res *getGNode(int32_t indx);
void setGNode(int32_t indx, struct_action_res *data);

void ScrSys_SaveGame(char *file);
void ScrSys_LoadGame(char *file);
void ScrSys_PrepareSaveBuffer();

struct_action_res *ScrSys_CreateActRes(int type);

void ScrSys_LoadPreferences();
void ScrSys_SavePreferences();

#endif // SCRIPTSYSTEM_H_INCLUDED

