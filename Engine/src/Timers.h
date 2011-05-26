#ifndef TIMERS_H_INCLUDED
#define TIMERS_H_INCLUDED

void tmr_DeleteTimers();
void tmr_DeleteTimerByOwner(pzllst *owner);
int  tmr_ProcessTimer(struct_action_res *nod);

int tmr_DeleteTimer(struct_action_res *nod);


#endif // TIMERS_H_INCLUDED
