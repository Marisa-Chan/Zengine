#ifndef MANAGE_H_INCLUDED
#define MANAGE_H_INCLUDED

#include "Puzzle.h"


char * ReturnListName(pzllst *lst);



pzllst **GetUni();
pzllst **Getworld();
pzllst **Getroom();
pzllst **Getview();
MList **Getctrl();

MList **Getanims();
MList **Getpreload();


#endif // MANAGE_H_INCLUDED
