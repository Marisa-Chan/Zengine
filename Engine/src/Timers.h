#ifndef TIMERS_H_INCLUDED
#define TIMERS_H_INCLUDED

struct timernode
{
    uint32_t slot;
    uint32_t time;
    //uint32_t ownslot;
    pzllst  *owner;
};


void tmr_DeleteTimers();
void tmr_DeleteTimerByOwner(pzllst *owner);
void tmr_ProcessTimers();

void tmr_InitTimerList();
void tmr_DeleteTimerList();
void tmr_AddToTimerList(void *item);

MList *tmr_GetTimerList();

#endif // TIMERS_H_INCLUDED
