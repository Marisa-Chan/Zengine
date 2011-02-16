#ifndef SCRIPTSYSTEM_H_INCLUDED
#define SCRIPTSYSTEM_H_INCLUDED

#include "Puzzle.h"


#define VAR_SLOTS_MAX 0xFFFF
#define STATEBOX_STACK_MAX 1024



#define SLOT_LOCATION_CUR_WO    3
#define SLOT_LOCATION_CUR_RO    4
#define SLOT_LOCATION_CUR_V1    5
#define SLOT_LOCATION_CUR_V2    6
#define SLOT_LOCATION_CUR_X     7
#define SLOT_INVENTORY_MOUSE    9
#define SLOT_START_SLOT       151
#define SLOT_END_SLOT         170

#define FLAG_ONCE_PER_I 1
#define FLAG_DISABLED   2
#define FLAG_DO_ME_NOW  4

#define CRIT_OP_EQU   0  //=
#define CRIT_OP_LEA   1  //<
#define CRIT_OP_GRE   2  //>
#define CRIT_OP_NOT   3  //!

void    ScrSys_SetSystemClass(char World, char Room);
uint8_t ScrSys_GetSystemWorld();
uint8_t ScrSys_GetSystemRoom();

uint8_t ScrSys_GetFlag(uint32_t indx);
void    ScrSys_SetFlag(uint32_t indx, uint8_t newval);

bool ScrSys_SlotIsOwned(uint32_t i);

void SetgVarInt(uint32_t indx, int var);
int  GetgVarInt(uint32_t indx);
void SetDirectgVarInt(uint32_t indx, int var);
int *getdirectvar(uint32_t indx);



void LoadScriptFile(pzllst *lst, char *filename, bool control, MList *controlst);
void InitScriptsEngine();

void FillStateBoxFromList(pzllst *lst);


/////  Depricated
int *DGetGVars();
uint8_t *DGetFlags();
/////  /Depricated


#endif // SCRIPTSYSTEM_H_INCLUDED
