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

void tmr_DeleteTimer(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_TIMER)
        return;

    if (nod->nodes.node_timer->time < 0)
        SetgVarInt(nod->slot,2);
    else
        SetgVarInt(nod->slot, nod->nodes.node_timer->time);

    delete nod->nodes.node_timer;
    delete nod;
}

void tmr_DeleteTimerByOwner(pzllst *owner)
{
    MList *allres = GetAction_res_List();
    StartMList(allres);
    while (!eofMList(allres))
    {
        struct_action_res *nod=(struct_action_res *)DataMList(allres);
        if (nod->node_type == NODE_TYPE_TIMER)
        if (nod->owner == owner)
        {
            tmr_DeleteTimer(nod);

            DeleteCurrent(allres);
        }

        NextMList(allres);
    }
}

int tmr_ProcessTimer(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_TIMER)
        return NODE_RET_OK;

    if (nod->nodes.node_timer->time < 0)
    {
#ifdef TRACE
        printf ("Timer #%d End's\n",nod->slot);
#endif
        tmr_DeleteTimer(nod);

        return NODE_RET_DELETE;
    }

    if (GetBeat())
        nod->nodes.node_timer->time--;

return NODE_RET_OK;
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
