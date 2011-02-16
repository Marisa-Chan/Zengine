#ifndef ACTIONS_H_INCLUDED
#define ACTIONS_H_INCLUDED

#include "Puzzle.h"

void action_set_screen(char *params, pzllst *owner);
void action_set_partial_screen(char *params, pzllst *owner);
void action_assign(char *params, pzllst *owner);
void action_timer(char *params, pzllst *owner);
void action_change_location(char *params, pzllst *owner);
void action_dissolve(char *params, pzllst *owner);
void action_disable_control(char *params, pzllst *owner);
void action_enable_control(char *params, pzllst *owner);
void action_add(char *params, pzllst *owner);
void action_debug(char *params, pzllst *owner);
void action_random(char *params, pzllst *owner);
void action_streamvideo(char *params, pzllst *owner);
void action_animplay(char *params, pzllst *owner);
void action_music(char *params, pzllst *owner);
void action_universe_music(char *params, pzllst *owner);
void action_syncsound(char *params, pzllst *owner);
void action_animpreload(char *params, pzllst *owner);
void action_playpreload(char *params, pzllst *owner);
void action_ttytext(char *params, pzllst *owner);
void action_kill(char *params, pzllst *owner);
void action_stop(char *params, pzllst *owner);
void action_inventory(char *params, pzllst *owner);
void action_crossfade(char *params, pzllst *owner);

#endif // ACTIONS_H_INCLUDED
