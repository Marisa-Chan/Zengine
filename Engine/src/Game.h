#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <inttypes.h>
#include "mylist.h"
#include "Graph.h"
#include "Sound.h"

#include <math.h>







#define CTRL_PUSH     0
#define CTRL_INPUT    1
#define CTRL_SLOT     2
#define CTRL_PANA     3
#define CTRL_FLAT     4






struct Locate
{
    uint8_t World;
    uint8_t Room;
    uint8_t View1;
    uint8_t View2;
    int16_t X;
} __attribute__((__packed__)) ;

struct Rect
{
    int x;
    int y;
    int w;
    int h;
};




struct crit_node
{
    uint32_t    slot1;
    uint32_t    slot2;
    uint8_t     oper;
    bool        var2; //if true: slot2 is slot; false: slot2 - number
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
    int *eligible_objects;
    int eligable_cnt;
    uint16_t cursor;
    SDL_Surface *srf;
};

struct ctrlnode
{
    uint32_t slot;
    uint8_t  type;
    union
    {
        slotnode *slot;
        pushnode *push;
    } node;
    void   (*func)(ctrlnode *);
};




void ProcessTriggers(MList *lst);
void InitScriptsEngine();
void ProcessControls(MList *ctrlst);

bool ProcessCriteries(MList *lst);

void DeletePuzzleList(pzllst *lst);
void DeleteControlList(MList *lst);
void ChangeLocation(uint8_t w, uint8_t r, uint8_t v1, uint8_t v2, int32_t X);

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
void DeleteAnimNod(animnode *nod);
void DeleteAllPreload();




void InitGraphics(bool fullscreen);
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





#define MaxStateBoxEnts 0x400 //0xFF is very small

struct StateBoxEnt{
    puzzlenode * nod[MaxStateBoxEnts];
    uint32_t     cnt;
};

void ShakeStateBox(uint32_t indx);


struct struct_Preload{
    uint32_t  slot;
    char    *fil;
    int32_t   u1;
    int32_t   u2;
    int32_t   u3;
    int32_t   u4;
};




bool examine_criterias(puzzlenode *nod);



#endif // GAME_H_INCLUDED
