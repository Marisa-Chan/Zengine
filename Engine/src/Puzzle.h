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
    void (*func)(char*, pzllst *);
    char         *param;
    puzzlenode   *owner;
};



pzllst *CreatePzlLst();
int Parse_Puzzle(pzllst *lst,FILE *fl,char *ctstr);



#endif // PUZZLE_H_INCLUDED
