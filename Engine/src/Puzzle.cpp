
#include "System.h"


pzllst *CreatePzlLst()
{
    pzllst *tmp     = new (pzllst);
    tmp->_list      = CreateMList();
    tmp->stksize    = 0;
    tmp->exec_times = 0;
    return tmp;
}
