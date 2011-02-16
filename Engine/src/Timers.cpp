
#include "System.h"

MList  *timers  = NULL;

void tmr_DeleteTimers()
{
    StartMList(timers);
    while (!eofMList(timers))
    {
        timernode *nod=(timernode *)DataMList(timers);
        delete nod;

        NextMList(timers);
    }

    FlushMList(timers);
}

void tmr_DeleteTimerByOwner(pzllst *owner)
{
    StartMList(timers);
    while (!eofMList(timers))
    {
        timernode *nod=(timernode *)DataMList(timers);
        if (nod->owner == owner)
        {
            if (nod->slot != 0)
            {
                SetgVarInt(nod->slot, nod->time - GetTickCount());
                //printf("deleted timer %d, ost %d",nod->slot,  nod->time - GetTickCount());
            }
            //printf("deleted timer %d, ost %d %d \n",nod->slot,  nod->time - GetTickCount(),nod->ownslot);
            delete nod;

            DeleteCurrent(timers);
        }

        NextMList(timers);
    }
}

void tmr_ProcessTimers()
{
    StartMList(timers);

    while (!eofMList(timers))
    {
        timernode *nod=(timernode *)DataMList(timers);

        if (nod)
            if (nod->time<GetTickCount())
            {
                SetgVarInt(nod->slot,2);
#ifdef TRACE
                printf ("Timer #%d End's\n",nod->slot);
#endif
                delete nod;
                DeleteCurrent(timers);

            }

        NextMList(timers);
    }
}

void tmr_InitTimerList()
{
    timers = CreateMList();
}

void tmr_DeleteTimerList()
{
    tmr_DeleteTimers();
    DeleteMList(timers);
    timers = NULL;
}

void tmr_AddToTimerList(void *item)
{
    AddToMList(timers,item);
}

MList *tmr_GetTimerList()
{
    return timers;
}
