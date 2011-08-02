#include "System.h"

int tmr_DeleteTimer(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_TIMER)
        return NODE_RET_NO;

    if (nod->nodes.node_timer < 0)
        SetgVarInt(nod->slot,2);
    else
        SetgVarInt(nod->slot, nod->nodes.node_timer);

    setGNode(nod->slot, NULL);

    delete nod;

    return NODE_RET_DELETE;
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

    if (nod->nodes.node_timer < 0)
    {
#ifdef TRACE
        printf ("Timer #%d End's\n",nod->slot);
#endif
        tmr_DeleteTimer(nod);

        return NODE_RET_DELETE;
    }

    //if (GetNBeat(3)) //10fps
    //    nod->nodes.node_timer--;
    nod->nodes.node_timer -= GetDTime();

return NODE_RET_OK;
}
