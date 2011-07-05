#ifndef PUZZLE_H_INCLUDED
#define PUZZLE_H_INCLUDED

#include "config.h"
#include "types.h"
#include "ScriptSystem.h"


struct puzzlenode
{
    uint16_t    slot; //puzzle slot
    MList  *CritList; //Criteria list of lists criteria
    MList   *ResList; //results list
    pzllst    *owner;
};


struct func_node
{
    int          (*func)(char*, int , pzllst *);
    char         *param;
    puzzlenode   *owner;
    int           slot;
};

struct crit_node
{
    uint32_t    slot1;
    uint32_t    slot2;
    uint8_t     oper;
    bool        var2; //if true: slot2 is slot; false: slot2 - number
};


pzllst *CreatePzlLst();
int Parse_Puzzle(pzllst *lst,FILE *fl,char *ctstr);
int Puzzle_try_exec(puzzlenode *pzlnod);

void FlushPuzzleList(pzllst *lst);


#endif // PUZZLE_H_INCLUDED
