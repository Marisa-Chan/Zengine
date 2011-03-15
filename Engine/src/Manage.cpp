
#include "System.h"


pzllst   *uni    =NULL;

pzllst   *world  =NULL;
pzllst   *room   =NULL;
pzllst   *view   =NULL;

Locate  Location= {0,0,0,0};



MList    *ctrl  =NULL;

MList   *anims  =NULL;






char * ReturnListName(pzllst *lst)
{
    if (lst == world)
        return "world";

    if (lst == uni)
        return "universe";

    if (lst == room)
        return "room";

    if (lst == view)
        return "view";
}



pzllst **GetUni()
{
    return &uni;
}

pzllst **Getworld()
{
    return &world;
}

pzllst **Getroom()
{
    return &room;
}

pzllst **Getview()
{
    return &view;
}

MList **Getctrl()
{
    return &ctrl;
}

MList **Getanims()
{
    return &anims;
}



