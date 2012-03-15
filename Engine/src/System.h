#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <inttypes.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <errno.h>

#ifdef WIN32
#include "win_funct.h"
#endif

#include "mylist.h"

#include "config.h"
#include "types.h"

#include "Graph.h"
#include "Render.h"
#include "Text.h"
#include "Subtitles.h"
#include "Sound.h"
#include "Mouse.h"
#include "Puzzle.h"
#include "SoundFX.h"
#include "Timers.h"
#include "Control.h"
#include "ScriptSystem.h"
#include "inventory.h"
#include "actions.h"
#include "anims.h"
#include "menu.h"
#include "Game.h"


void END();
void UpdateGameSystem();

//System time functions
//uint64_t millisec();
#define millisec SDL_GetTicks

#define SDL_TTF_NUM_VERSION (SDL_TTF_MAJOR_VERSION * 10000 \
                               + SDL_TTF_MINOR_VERSION * 100 \
                               + SDL_TTF_PATCHLEVEL)

//Game timer functions
void InitMTime(float fps);
void ProcMTime();
bool GetBeat();
bool GetNBeat(int n);
bool Get2thBeat();
bool Get4thBeat();
uint64_t GetBeatCount();

//Keyboard functions
void FlushKeybKey(SDLKey key);
void FlushHits();
void UpdateKeyboard();
bool KeyDown(SDLKey key);
bool KeyAnyHit();
int MouseX();
int MouseY();
bool MouseDown(int btn);
bool MouseHit(int btn);
bool MouseUp(int btn);
bool MouseDblClk();
void FlushMouseBtn(int btn);
void SetHit(SDLKey key);
bool KeyHit(SDLKey key);

bool MouseInRect(int x, int y, int w, int h);


void InitVkKeys();
uint8_t GetWinKey(SDLKey key);
SDLKey GetLastKey();

struct FManNode
{
    char *File;
    char *Path;
};

struct FManRepNode
{
    char *ext;
    char *ext2;
};

void InitFileManage();
void ListDir(char *dir);
const char *GetFilePath(const char *chr);
const char *GetExactFilePath(const char *chr);
void AddReplacer(const char *ext, const char *ext2);

int GetKeyBuffered(int indx);
bool CheckKeyboardMessage(const char *msg, int len);

bool FileExist(char * fil);


void UpdateDTime();
uint32_t GetDTime();

char * PrepareString(char *buf);
char * TrimLeft(char *buf);
char * TrimRight(char *buf);
char * GetParams(char *str);
int GetIntVal(char *chr);

int8_t GetUtf8CharSize(char chr);
uint16_t ReadUtf8Char(char *chr);

#define strCMP(X,Y) strncasecmp(X,Y,strlen(Y))


struct BinTreeNd;

struct BinTreeNd
{
    BinTreeNd *zero;
    BinTreeNd *one;
    FManNode  *nod;
};

BinTreeNd *CreateBinTreeNd();
void AddToBinTree(FManNode *nod);
FManNode *FindInBinTree(const char *chr);


double round(double r);

void SetAppPath(const char *pth);
char *GetAppPath();

#endif // SYSTEM_H_INCLUDED
