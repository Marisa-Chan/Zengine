

#include "System.h"


uint8_t SystemWorld;
uint8_t SystemRoom;



int          gVars   [VAR_SLOTS_MAX];

uint8_t      Flags   [VAR_SLOTS_MAX];

StateBoxEnt *StateBox[VAR_SLOTS_MAX];

puzzlenode  *StateBoxStk[STATEBOX_STACK_MAX];
uint32_t     StateBoxStkSz = 0;




void SetgVarInt(uint32_t indx, int var)
{
    gVars[indx]=var;

    ShakeStateBox(indx);
}

void SetDirectgVarInt(uint32_t indx, int var)
{
    gVars[indx]=var;
}

int GetgVarInt(uint32_t indx)
{
    return gVars[indx];
}

int *getdirectvar(uint32_t indx)
{
    return &gVars[indx];
}


uint8_t ScrSys_GetFlag(uint32_t indx)
{
    return Flags[indx];
}

void ScrSys_SetFlag(uint32_t indx, uint8_t newval)
{
    Flags[indx] = newval;
}

void ScrSys_SetSystemClass(char World, char Room)
{
    SystemWorld = toupper(World);
    SystemRoom  = toupper(Room);
}

uint8_t ScrSys_GetSystemWorld()
{
    return SystemWorld;
}

uint8_t ScrSys_GetSystemRoom()
{
    return SystemRoom;
}



void InitScriptsEngine()
{
    memset(gVars,0x0,VAR_SLOTS_MAX * sizeof(void *));
    *Gettimers()=CreateMList();
    *Getwavs()=CreateMList();

    memset(StateBox,0x0,VAR_SLOTS_MAX * sizeof(StateBoxEnt *));
    StateBoxStkSz = 0;

    memset(Flags,0x0,VAR_SLOTS_MAX * sizeof(uint8_t));
}




void LoadScriptFile(pzllst *lst, char *filename, bool control, MList *controlst)
{
#ifdef TRACE
    printf("Loading script file %s\n",filename);
#endif


    FILE *fl=fopen(filename,"rb");
    if (fl == NULL)
    {
        printf("Error opening file %s\n",filename);
        exit(1);
        return;
    }

    char buf[FILE_LN_BUF];

    while(!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);

        char *str=PrepareString(buf);


        if (strCMP(str,"puzzle")==0)
        {
            Parse_Puzzle(lst,fl,str);
        }
        else if (strCMP(str,"control")==0 && control )
        {
            Parse_Control(controlst,fl,str);
        }
    }

    fclose(fl);
}

void ChangeLocation(uint8_t w, uint8_t r,uint8_t v1, uint8_t v2, int32_t X) // world / room / view
{
    //Needed reverse from 0x004246C7

    if (GetgVarInt(3) != 'G' &&
        GetgVarInt(4) != 'J' )
    {
        if (w == 'G' &&
            r == 'J' )
        {
            SetgVarInt(45,GetgVarInt(3));
            SetgVarInt(46,GetgVarInt(4));
            SetgVarInt(47,GetgVarInt(5));
            SetgVarInt(48,GetgVarInt(6));
            SetgVarInt(49,GetgVarInt(7));
        }
        else
        {
            SetgVarInt(40,GetgVarInt(3));
            SetgVarInt(41,GetgVarInt(4));
            SetgVarInt(42,GetgVarInt(5));
            SetgVarInt(43,GetgVarInt(6));
            SetgVarInt(44,GetgVarInt(7));
        }
    }






    Locate temp;
    temp.World =w;
        temp.Room  =r;
        temp.View1 =v1;
        temp.View2 =v2;
        temp.X     =X;


    ////////State box routine////////////
    for (int i=0; i<VAR_SLOTS_MAX; i++)
    {
        if (StateBox[i] != NULL)
            delete StateBox[i];
    }

    memset(StateBox,0,VAR_SLOTS_MAX * sizeof(StateBoxEnt *));
    StateBoxStkSz=0;
    ////////---State box routine---//////////////

    char buf[32];
    char tm[5];


//    RenderDelay = 2;
//    View_start_Loops = 1;

    if (temp.View1 != GetgVarInt(6) || temp.View2 != GetgVarInt(5) || temp.Room != GetgVarInt(4) || temp.World != GetgVarInt(3))
    {
        if (*Getview())
        {
            DeleteTimerByOwner(*Gettimers(),*Getview());
            DeleteLoopedWavsByOwner(*Getwavs(),*Getview());

            DeletePuzzleList(*Getview());
            DeleteControlList(*Getctrl());
            DeleteAnims(*Getanims());
            DeleteAllPreload();
        }

        tm[0]=temp.World;
        tm[1]=temp.Room;
        tm[2]=temp.View1;
        tm[3]=temp.View2;
        tm[4]=0;
        sprintf(buf,"%s.scr",tm);
        *Getview()=CreatePzlLst();
        *Getctrl()=CreateMList();
        *Getanims()=CreateMList();
        LoadScriptFile(*Getview(),GetExactFilePath(buf),true,*Getctrl());

    }

    if (temp.Room != GetgVarInt(4) || temp.World != GetgVarInt(3))
    {
        if (*Getroom())
        {
            DeleteLoopedWavsByOwner(*Getwavs(),*Getroom());

            DeletePuzzleList(*Getroom());
        }


        tm[0]=temp.World;
        tm[1]=temp.Room;
        tm[2]=0;
        sprintf(buf,"%s.scr",tm);
        *Getroom()=CreatePzlLst();
        LoadScriptFile(*Getroom(),GetExactFilePath(buf),false,NULL);
    }

    if (temp.World != GetgVarInt(3))
    {
        if (*Getworld())
        {
            DeleteLoopedWavsByOwner(*Getwavs(),*Getworld());

            DeletePuzzleList(*Getworld());
        }
        tm[0]=temp.World;
        tm[1]=0;
        sprintf(buf,"%s.scr",tm);
        *Getworld()=CreatePzlLst();
        LoadScriptFile(*Getworld(),GetExactFilePath(buf),false,NULL);
    }

    FillStateBoxFromList(*GetUni());
    FillStateBoxFromList(*Getview());
    FillStateBoxFromList(*Getroom());
    FillStateBoxFromList(*Getworld());

    SetgVarInt(3,w);
    SetgVarInt(4,r);
    SetgVarInt(5,v1);
    SetgVarInt(6,v2);
    SetgVarInt(7,X);
}


void AddPuzzleToStateBox(int slot, puzzlenode *pzlnd)
{
    StateBoxEnt *ent = StateBox[slot];

    if (ent == NULL)
    {
        StateBox[slot] = ent = new (StateBoxEnt);
        ent->cnt = 0;
    }
    if (ent->cnt < MaxStateBoxEnts)
    {
        ent->nod[ent->cnt] = pzlnd;
        ent->cnt++;
    }
}

void FillStateBoxFromList(pzllst *lst)
{
    StartMList(lst->_list);
    while (!eofMList(lst->_list))
    {
        puzzlenode *pzlnod=(puzzlenode *)DataMList(lst->_list);

        if (pzlnod->flags & FLAG_ONCE_PER_I)
            AddPuzzleToStateBox(pzlnod->slot,pzlnod);

        StartMList(pzlnod->CritList);
        while (!eofMList(pzlnod->CritList))
        {
            MList *CriteriaLst= (MList *) DataMList(pzlnod->CritList);

            int prevslot=0;
            StartMList(CriteriaLst);
            while (!eofMList(CriteriaLst))
            {
                crit_node *crtnod = (crit_node *)DataMList(CriteriaLst);

                if (prevslot != crtnod->slot1)
                    AddPuzzleToStateBox(crtnod->slot1,pzlnod);

                prevslot = crtnod->slot1;

                NextMList(CriteriaLst);
            }

            NextMList(pzlnod->CritList);
        }
        NextMList(lst->_list);
    }
}

//Function clears trigger status for once_per_inst triggers
void ClearUsedOnOPIPuzz(MList *lst)
{
    if (!lst)
        return;
    StartMList(lst);
    while (!eofMList(lst))
    {
        puzzlenode *nod=(puzzlenode *)DataMList(lst);
        if (nod->flags & FLAG_ONCE_PER_I)
            SetgVarInt(nod->slot,0);
        NextMList(lst);
    }
}

void AddStateBoxToStk(puzzlenode *pzl)
{
    pzllst *owner = pzl->owner;
    if (owner->stksize < pzlSTACK)
    {
        if (owner->stksize > 0)
            if (owner->stack[owner->stksize - 1] == pzl)
                return;

        owner->stack[owner->stksize] = pzl;
        owner->stksize++;
    }
    else
    {
#ifdef TRACE
        printf("Can't add pzl# %d to Stack\n",pzl->slot);
#endif
    }
}

void ShakeStateBox(uint32_t indx)
{
    if (StateBox[indx] != NULL)
    {
        for (int i=StateBox[indx]->cnt-1; i >= 0; i--)
        {
            //if (examine_criterias(StateBox[indx]->nod[i])) //may cause bug's
            AddStateBoxToStk(StateBox[indx]->nod[i]);
        }
    }
}


////Depricated

int *DGetGVars()
{
    return gVars;
}

uint8_t *DGetFlags()
{
    return Flags;
}


