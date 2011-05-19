#ifndef ACTIONS_H_INCLUDED
#define ACTIONS_H_INCLUDED

#include "Puzzle.h"

#define ACTION_NOT_FOUND   1
#define ACTION_NORMAL      0
#define ACTION_ERROR      -1
#define ACTION_BREAK      -2

int action_set_screen(char *params, int aSlot , pzllst *owner);
int action_set_partial_screen(char *params, int aSlot , pzllst *owner);
int action_assign(char *params, int aSlot , pzllst *owner);
int action_timer(char *params, int aSlot , pzllst *owner);
int action_change_location(char *params, int aSlot , pzllst *owner);
int action_dissolve(char *params, int aSlot , pzllst *owner);
int action_disable_control(char *params, int aSlot , pzllst *owner);
int action_enable_control(char *params, int aSlot , pzllst *owner);
int action_add(char *params, int aSlot , pzllst *owner);
int action_debug(char *params, int aSlot , pzllst *owner);
int action_random(char *params, int aSlot , pzllst *owner);
int action_streamvideo(char *params, int aSlot , pzllst *owner);
int action_animplay(char *params, int aSlot , pzllst *owner);
int action_music(char *params, int aSlot , pzllst *owner);
int action_universe_music(char *params, int aSlot , pzllst *owner);
int action_syncsound(char *params, int aSlot , pzllst *owner);
int action_animpreload(char *params, int aSlot , pzllst *owner);
int action_playpreload(char *params, int aSlot , pzllst *owner);
int action_ttytext(char *params, int aSlot , pzllst *owner);
int action_kill(char *params, int aSlot , pzllst *owner);
int action_stop(char *params, int aSlot , pzllst *owner);
int action_inventory(char *params, int aSlot , pzllst *owner);
int action_crossfade(char *params, int aSlot , pzllst *owner);
int action_menu_bar_enable(char *params, int aSlot , pzllst *owner);
int action_delay_render(char *params, int aSlot , pzllst *owner);
int action_cursor(char *params, int aSlot , pzllst *owner);
int action_attenuate(char *params, int aSlot , pzllst *owner);
int action_pan_track(char *params, int aSlot , pzllst *owner);

#endif // ACTIONS_H_INCLUDED
