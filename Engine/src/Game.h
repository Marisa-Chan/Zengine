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
};



void ProcessTriggers(MList *lst);
void InitScriptsEngine();
void ProcessControls(MList *ctrlst);

bool ProcessCriteries(MList *lst);

void DeletePuzzleList(pzllst *lst);
void DeleteControlList(MList *lst);


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


void GameLoop();
void InitGameLoop();








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
