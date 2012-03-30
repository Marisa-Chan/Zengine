#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <inttypes.h>
#include "mylist.h"
#include "Graph.h"
#include "Sound.h"

#include <math.h>

#define SYSTEM_STR_SAVEEXIST   23
#define SYSTEM_STR_SAVED       4
#define SYSTEM_STR_SAVEEMPTY   21
#define SYSTEM_STR_EXITPROMT   6

struct Locate
{
    uint8_t World;
    uint8_t Room;
    uint8_t Node;
    uint8_t View;
    int16_t X;
};


void InitScriptsEngine();


void GameLoop();
void InitGameLoop();

void SetNeedLocate(uint8_t w, uint8_t r,uint8_t v1, uint8_t v2, int32_t X);

void ReadSystemStrings(char *filename);
char *GetSystemString(int32_t indx);

void game_timed_debug_message(int32_t milsecs,const char * str);
void game_timed_message(int32_t milsecs,const char * str);
void game_delay_message(int32_t milsecs,const char * str);
bool game_question_message(const char * str);
void ifquit();

#endif // GAME_H_INCLUDED
