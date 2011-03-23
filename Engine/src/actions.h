#ifndef ACTIONS_H_INCLUDED
#define ACTIONS_H_INCLUDED

#include "Puzzle.h"

#define ACTION_NORMAL      0
#define ACTION_ERROR      -1
#define ACTION_BREAK      -2

int action_set_screen(char *params, pzllst *owner);
int action_set_partial_screen(char *params, pzllst *owner);
int action_assign(char *params, pzllst *owner);
int action_timer(char *params, pzllst *owner);
int action_change_location(char *params, pzllst *owner);
int action_dissolve(char *params, pzllst *owner);
int action_disable_control(char *params, pzllst *owner);
int action_enable_control(char *params, pzllst *owner);
int action_add(char *params, pzllst *owner);
int action_debug(char *params, pzllst *owner);
int action_random(char *params, pzllst *owner);
int action_streamvideo(char *params, pzllst *owner);
int action_animplay(char *params, pzllst *owner);
int action_music(char *params, pzllst *owner);
int action_universe_music(char *params, pzllst *owner);
int action_syncsound(char *params, pzllst *owner);
int action_animpreload(char *params, pzllst *owner);
int action_playpreload(char *params, pzllst *owner);
int action_ttytext(char *params, pzllst *owner);
int action_kill(char *params, pzllst *owner);
int action_stop(char *params, pzllst *owner);
int action_inventory(char *params, pzllst *owner);
int action_crossfade(char *params, pzllst *owner);
int action_menu_bar_enable(char *params, pzllst *owner);

#endif // ACTIONS_H_INCLUDED
