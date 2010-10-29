#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <inttypes.h>
#include "mylist.h"
#include "Graph.h"
#include "Sound.h"



#define FLAG_ONCE_PER_I 1
#define FLAG_DISABLED   2
#define FLAG_DO_ME_NOW  4

#define CRIT_OP_EQU   0  //=
#define CRIT_OP_LEA   1  //<
#define CRIT_OP_GRE   2  //>
#define CRIT_OP_NOT   3  //!

#define CTRL_PUSH     0
#define CTRL_INPUT    1
#define CTRL_SLOT     2
#define CTRL_PANA     3
#define CTRL_FLAT     4

#define SLOT_INVENTORY_MOUSE    9
#define SLOT_START_SLOT       151
#define SLOT_END_SLOT         170

struct Rect
{
    int x;
    int y;
    int w;
    int h;
};

struct puzzlenode
{
    uint16_t    slot; //puzzle slot
    MList  *CritList; //Criteria list of lists criteria
    MList   *ResList; //results list
    uint8_t    flags; //flags
};

struct crit_node
{
    uint32_t    slot1;
    uint32_t    slot2;
    uint8_t     oper;
    bool        var2; //if true: slot2 is slot; false: slot2 - number
};

struct func_node
{
    void (*func)(char*);
    char         *param;
};

struct pushnode
{
    bool flat; //true - flat, false - warp
    int x;
    int y;
    int w;
    int h;
    uint16_t cursor;
};

struct slotnode
{
    bool flat; //true - flat, false - warp
    Rect rectangle;
    //int warp;
    //int distance_id;
    Rect hotspot;
    //int do_skip;
    char *eligible_objects;
    uint16_t cursor;
    SDL_Surface *srf;
};

struct ctrlnode
{
    uint32_t slot;
    uint8_t  type;
    void    *node;
    bool   enable;
};

struct timernode
{
    uint32_t slot;
    uint32_t time;
};


void LoadScriptFile(MList *lst, char *filename, bool control, MList *controlst);
void ProcessTriggers(MList *lst);
void InitScriptsEngine();
void ProcessControls(MList *ctrlst);
void ProcessTimers();



struct animnode
{
    uint32_t slot;
    int      x;
    int      y;
    int      w;
    int      h;
    int  start;
    int    end;
    int   loopcnt;
    int   unk1; //???
    int   unk2;
    int   unk3;
    int   unk4;
    int  CurFr;
    uint32_t nexttick;
    int  loops;
    void *anim;
    bool   vid;
};

void ProcessAnims();
void DeleteAnims(MList *lst);


void InitGraphics();
void RenderFunc();


void DrawImageToSurf(SDL_Surface *surf, int16_t x, int16_t y,SDL_Surface *dest);

struct Cursor
{
    SDL_Surface *img;
    int16_t ox;
    int16_t oy;
};


void LoadCursor(char *file, Cursor *cur);
void DrawCursor(Cursor *cur, int x, int y);
void ProcessCursor();


void GameLoop();
void InitGameLoop();



struct musicnode
{
    int         slot;
    Mix_Chunk *chunk;
    int         chn;
};

void ProcessWavs();

#endif // GAME_H_INCLUDED
