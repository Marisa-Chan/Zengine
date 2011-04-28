#ifndef PUZZLE_H_INCLUDED
#define PUZZLE_H_INCLUDED


#define pzlSTACK 0x800

struct pzllst;

struct puzzlenode
{
    uint16_t    slot; //puzzle slot
    MList  *CritList; //Criteria list of lists criteria
    MList   *ResList; //results list
    uint8_t    flags; //flags
    pzllst    *owner;
};

struct pzllst{
    MList      *_list;
    puzzlenode *stack[pzlSTACK];
    int16_t     stksize;
    uint8_t     exec_times;
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
