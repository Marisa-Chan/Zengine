#include "System.h"




int32_t      gVars   [VAR_SLOTS_MAX];

struct_action_res *gNodes [VAR_SLOTS_MAX];

uint8_t      Flags   [VAR_SLOTS_MAX];

StateBoxEnt *StateBox[VAR_SLOTS_MAX];

puzzlenode  *StateBoxStk[STATEBOX_STACK_MAX];
uint32_t     StateBoxStkSz = 0;

bool BreakExecute = false;



pzllst   *uni    =NULL; //universe script
pzllst   *world  =NULL; //world script
pzllst   *room   =NULL; //room script
pzllst   *view   =NULL; //view script

MList    *ctrl   =NULL; //contorls

MList    *actres =NULL; //sounds, animations, ttytexts and other.

uint8_t  *SaveBuffer = NULL;
uint32_t SaveCurrentSize=0;

const char * ScrSys_ReturnListName(pzllst *lst)
{
    if (lst == world)
        return "world";

    if (lst == uni)
        return "universe";

    if (lst == room)
        return "room";

    if (lst == view)
        return "view";

    return "unknown";
}

pzllst *GetUni()
{
    return uni;
}

pzllst *Getworld()
{
    return world;
}

pzllst *Getroom()
{
    return room;
}

pzllst *Getview()
{
    return view;
}

MList *Getctrl()
{
    return ctrl;
}

MList *GetAction_res_List()
{
    return actres;
}

void ScrSys_AddToActResList(void *nod)
{
    if (actres != NULL)
        AddToMList(actres,nod);
}

void SetgVarInt(int32_t indx, int var)
{
    if (indx > 0 && indx < VAR_SLOTS_MAX )
    {
        gVars[indx]=var;

        ShakeStateBox(indx);
    }

}

void SetDirectgVarInt(uint32_t indx, int var)
{
    if (indx > 0 && indx < VAR_SLOTS_MAX )
        gVars[indx]=var;
}

int GetgVarInt(int32_t indx)
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

//Don't call it from loops for mylists!! it's cause error
bool ScrSys_SlotIsOwned(uint32_t i)
{
    MList *res = ScrSys_FindResAllBySlot(i);

    if (res != NULL)
    {
        delete res;
        return true;
    }

    return false;
}

bool ScrSys_SlotIsOwned2(int32_t i)
{
    if (getGNode(i) != NULL)
        return true;
    return false;
}

void ScrSys_FlushGNodes()
{
    memset(gNodes,0x0,VAR_SLOTS_MAX * sizeof(struct_action_res *));
}

void ScrSys_RereadGNodes()
{
    ScrSys_FlushGNodes();

    MList *all = GetAction_res_List();
    StartMList(all);
    while (!eofMList(all))
    {
        struct_action_res *nod = (struct_action_res *)DataMList(all);

        setGNode(nod->slot,nod);

        NextMList(all);
    }
}

struct_action_res *getGNode(int32_t indx)
{
    if (indx >= 0 && indx < VAR_SLOTS_MAX)
        return gNodes[indx];
    else
        return NULL;
}

void setGNode(int32_t indx, struct_action_res *data)
{
    if (indx >= 0 && indx < VAR_SLOTS_MAX)
        gNodes[indx] = data;
}

void InitScriptsEngine()
{
    memset(gVars,0x0,VAR_SLOTS_MAX * sizeof(gVars[0]));
    ScrSys_FlushGNodes();

    view  = CreatePzlLst();
    room  = CreatePzlLst();
    world = CreatePzlLst();
    uni   = CreatePzlLst();

    ctrl  = CreateMList();

    actres = CreateMList();


    memset(StateBox,0x0,VAR_SLOTS_MAX * sizeof(StateBoxEnt *));
    StateBoxStkSz = 0;

    memset(Flags,0x0,VAR_SLOTS_MAX * sizeof(uint8_t));

    //needed for znemesis
    SetDirectgVarInt(SLOT_CPU, 1);
    SetDirectgVarInt(SLOT_PLATFORM, 0);
    SetDirectgVarInt(SLOT_WIN958, 0);

    ScrSys_LoadPreferences();
}





void LoadScriptFile(pzllst *lst,FManNode *filename, bool control, MList *controlst)
{
#ifdef TRACE
    printf("Loading script file %s\n",filename->File);
#endif

    if (control)
    {
        Rend_SetRenderer(RENDER_FLAT);
    }

    mfile *fl=mfopen(filename);
    if (fl == NULL)
    {
        printf("Error opening file %s\n",filename->File);
        exit(1);
        return;
    }

    char buf[FILE_LN_BUF];

    while(!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);

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

    mfclose(fl);
}

void ScrSys_ClearStateBox()
{
    for (int i=0; i<VAR_SLOTS_MAX; i++)
    {
        if (StateBox[i] != NULL)
            delete[] StateBox[i];
        StateBox[i] = NULL;
    }

    //memset(StateBox,0,VAR_SLOTS_MAX * sizeof(StateBoxEnt *));
    StateBoxStkSz=0;
}

void ScrSys_PrepareSaveBuffer()
{
    if (SaveBuffer == NULL)
            SaveBuffer = (uint8_t *)malloc(SAVEBUFFER_SIZE);

        int buffpos=0;

        SaveBuffer[0] = 'Z';
        SaveBuffer[1] = 'N';
        SaveBuffer[2] = 'S';
        SaveBuffer[3] = 'G';
        SaveBuffer[4] =  4;
        SaveBuffer[5] =  0;
        SaveBuffer[6] =  0;
        SaveBuffer[7] =  0;
        SaveBuffer[12]= 'L';
        SaveBuffer[13]= 'O';
        SaveBuffer[14]= 'C';
        SaveBuffer[15]= ' ';
        SaveBuffer[16] =  8;
        SaveBuffer[17] =  0;
        SaveBuffer[18] =  0;
        SaveBuffer[19] =  0;
        SaveBuffer[20] =  tolower(GetgVarInt(SLOT_WORLD));
        SaveBuffer[21] =  tolower(GetgVarInt(SLOT_ROOM));
        SaveBuffer[22] =  tolower(GetgVarInt(SLOT_NODE));
        SaveBuffer[23] =  tolower(GetgVarInt(SLOT_VIEW));

        int16_t *tmp2 = (int16_t *)&SaveBuffer[24];
        *tmp2 = GetgVarInt(SLOT_VIEW_POS);

        buffpos=28;

        MList *lst = GetAction_res_List();
        StartMList(lst);
        while(!eofMList(lst))
        {
            struct_action_res *nod = (struct_action_res *)DataMList(lst);
            if (nod->node_type == NODE_TYPE_TIMER)
            {
                SaveBuffer[buffpos]   = 'T';
                SaveBuffer[buffpos+1] = 'I';
                SaveBuffer[buffpos+2] = 'M';
                SaveBuffer[buffpos+3] = 'R';
                SaveBuffer[buffpos+4] =  8;
                SaveBuffer[buffpos+5] =  0;
                SaveBuffer[buffpos+6] =  0;
                SaveBuffer[buffpos+7] =  0;

                int32_t *tmp = (int32_t *)&SaveBuffer[buffpos+8];
                *tmp = nod->slot;

                tmp = (int32_t *)&SaveBuffer[buffpos+12];

                *tmp = nod->nodes.node_timer;

                buffpos+=16;
            }

            NextMList(lst);
        }

        SaveBuffer[buffpos]   = 'F';
        SaveBuffer[buffpos+1] = 'L';
        SaveBuffer[buffpos+2] = 'A';
        SaveBuffer[buffpos+3] = 'G';

        buffpos+=4;

        int32_t *tmp = (int32_t *)&SaveBuffer[buffpos];

        *tmp = VAR_SLOTS_MAX * 2; //16bits

        buffpos+=4;

        tmp2 = (int16_t *)&SaveBuffer[buffpos];

        for (int i=0; i<VAR_SLOTS_MAX; i++)
            tmp2[i] = ScrSys_GetFlag(i);

        buffpos+=VAR_SLOTS_MAX*2;



        SaveBuffer[buffpos]   = 'P';
        SaveBuffer[buffpos+1] = 'U';
        SaveBuffer[buffpos+2] = 'Z';
        SaveBuffer[buffpos+3] = 'Z';

        buffpos+=4;

        tmp = (int32_t *)&SaveBuffer[buffpos];

        *tmp = VAR_SLOTS_MAX * 2; //16bits

        buffpos+=4;

        tmp2 = (int16_t *)&SaveBuffer[buffpos];

        for (int i=0; i<VAR_SLOTS_MAX; i++)
            tmp2[i] = GetgVarInt(i);

        lst = GetAction_res_List();
        StartMList(lst);
        while(!eofMList(lst))
        {
            struct_action_res *nod = (struct_action_res *)DataMList(lst);
            if (nod->node_type == NODE_TYPE_MUSIC)
                if (nod->slot > 0)
                    tmp2[nod->slot] = 2;

            NextMList(lst);
        }

        buffpos+=VAR_SLOTS_MAX*2;

        SaveCurrentSize = buffpos;
}

void ScrSys_SaveGame(char *file)
{
    if (SaveBuffer == NULL)
        return;

    FILE *f = fopen(file,"wb");

    fwrite(SaveBuffer,SaveCurrentSize,1,f);

    fclose(f);
}

void ScrSys_LoadGame(char *file)
{

    FILE *f = fopen(file,"rb");

    if (!f)
        return;

    ScrSys_DeleteAllRes();

    uint32_t tmp;
    fread(&tmp,4,1,f);
    if (tmp != 0x47534E5A)
    {
        printf("Error in save file %s\n",file);
        exit(-1);
    }

    fread(&tmp,4,1,f);
    fread(&tmp,4,1,f);
    fread(&tmp,4,1,f);
    fread(&tmp,4,1,f);

    uint8_t w,r,n,v;
    int16_t pos;

    fread(&w,1,1,f);
    fread(&r,1,1,f);
    fread(&n,1,1,f);
    fread(&v,1,1,f);

    fread(&pos,2,1,f);





    fread(&tmp,2,1,f);

    while(!feof(f))
    {
        fread(&tmp,4,1,f);
        if (tmp != 0x524D4954)
            break;

        fread(&tmp,4,1,f);

        int slot,time;
        fread(&slot,4,1,f);
        fread(&time,4,1,f);

        char buf[32];
        sprintf(buf,"%d",time/100);
        action_timer(buf,slot,view);
    }

    fread(&tmp,4,1,f);

    if (tmp != VAR_SLOTS_MAX*2)
    {
        printf("Error in save file %s (FLAGS VAR_SLOTS_MAX)\n",file);
        exit(-1);
    }
    for (int i=0; i<VAR_SLOTS_MAX;i++)
    {
        int16_t tmp2;
        fread(&tmp2,2,1,f);
        ScrSys_SetFlag(i,tmp2);
    }

    fread(&tmp,4,1,f);
    fread(&tmp,4,1,f);
    if (tmp != VAR_SLOTS_MAX*2)
    {
        printf("Error in save file %s (PUZZLE VAR_SLOTS_MAX)\n",file);
        exit(-1);
    }
    for (int i=0; i<VAR_SLOTS_MAX;i++)
    {
        int16_t tmp2;
        fread(&tmp2,2,1,f);
        SetDirectgVarInt(i,tmp2);
    }

    Rend_SetDelay(2);

    ScrSys_ChangeLocation(w,r,n,v,pos,true);

    SetgVarInt(SLOT_JUST_RESTORED, 1);

    fclose(f);

    ScrSys_LoadPreferences();
}

void ScrSys_ChangeLocation(uint8_t w, uint8_t r,uint8_t v1, uint8_t v2, int32_t X, bool force_all) // world / room / view
{
    //reversed from 0x004246C7

    Locate temp;
    temp.World = w;
    temp.Room  = r;
    temp.Node  = v1;
    temp.View  = v2;
    temp.X     = X;

    if (GetgVarInt(SLOT_WORLD)!= SystemWorld ||
        GetgVarInt(SLOT_ROOM) != SystemRoom  )
    {
        if (temp.World == SystemWorld &&
            temp.Room  == SystemRoom)
        {
            SetDirectgVarInt(SLOT_MENU_LASTWORLD    , GetgVarInt(SLOT_WORLD));
            SetDirectgVarInt(SLOT_MENU_LASTROOM     , GetgVarInt(SLOT_ROOM));
            SetDirectgVarInt(SLOT_MENU_LASTNODE     , GetgVarInt(SLOT_NODE));
            SetDirectgVarInt(SLOT_MENU_LASTVIEW     , GetgVarInt(SLOT_VIEW));
            SetDirectgVarInt(SLOT_MENU_LASTVIEW_POS , GetgVarInt(SLOT_VIEW_POS));
        }
        else
        {
            SetDirectgVarInt(SLOT_LASTWORLD    , GetgVarInt(SLOT_WORLD));
            SetDirectgVarInt(SLOT_LASTROOM     , GetgVarInt(SLOT_ROOM));
            SetDirectgVarInt(SLOT_LASTNODE     , GetgVarInt(SLOT_NODE));
            SetDirectgVarInt(SLOT_LASTVIEW     , GetgVarInt(SLOT_VIEW));
            SetDirectgVarInt(SLOT_LASTVIEW_POS , GetgVarInt(SLOT_VIEW_POS));
        }
    }


    ScrSys_ClearStateBox();

    if (temp.World == SaveWorld && temp.Room  == SaveRoom  &&
        temp.Node  == SaveNode  && temp.View  == SaveView)
    {
        //Save all to buffer
        ScrSys_PrepareSaveBuffer();
    }

    char buf[32];
    char tm[5];


    if (temp.View  != GetgVarInt(SLOT_VIEW)  ||
        temp.Node  != GetgVarInt(SLOT_NODE)  ||
        temp.Room  != GetgVarInt(SLOT_ROOM)  ||
        temp.World != GetgVarInt(SLOT_WORLD) ||
        force_all  || view == NULL)
    {

        ScrSys_FlushResourcesByOwner(view);

        FlushPuzzleList(view);
        FlushControlList(ctrl);

        tm[0]=temp.World;
        tm[1]=temp.Room;
        tm[2]=temp.Node;
        tm[3]=temp.View;
        tm[4]=0;
        sprintf(buf,"%s.scr",tm);
        FManNode *fil = FindInBinTree(buf);
        if (fil != NULL)
            LoadScriptFile(view,fil,true,ctrl);

    }

    if (temp.Room  != GetgVarInt(SLOT_ROOM)   ||
        temp.World != GetgVarInt(SLOT_WORLD)  ||
        force_all  || room == NULL)
    {
        ScrSys_FlushResourcesByOwner(room);

        FlushPuzzleList(room);

        tm[0]=temp.World;
        tm[1]=temp.Room;
        tm[2]=0;
        sprintf(buf,"%s.scr",tm);

        FManNode *fil = FindInBinTree(buf);
        if (fil != NULL)
            LoadScriptFile(room,fil,false,NULL);
    }


    if (temp.World != GetgVarInt(SLOT_WORLD) ||
        force_all  || world == NULL)
    {
        ScrSys_FlushResourcesByOwner(world);

        FlushPuzzleList(world);

        tm[0]=temp.World;
        tm[1]=0;
        sprintf(buf,"%s.scr",tm);

        FManNode *fil = FindInBinTree(buf);
        if (fil != NULL)
            LoadScriptFile(world,fil,false,NULL);

        Mouse_ShowCursor();
    }

    FillStateBoxFromList(uni);
    FillStateBoxFromList(view);
    FillStateBoxFromList(room);
    FillStateBoxFromList(world);

    SetgVarInt(SLOT_WORLD    , w);
    SetgVarInt(SLOT_ROOM     , r);
    SetgVarInt(SLOT_NODE     , v1);
    SetgVarInt(SLOT_VIEW     , v2);
    SetgVarInt(SLOT_VIEW_POS , X);

    menu_SetMenuBarVal(0xFFFF);

    BreakExecute = false;
}


void AddPuzzleToStateBox(int slot, puzzlenode *pzlnd)
{
    StateBoxEnt *ent = StateBox[slot];

    if (ent == NULL)
    {
        ent = new (StateBoxEnt);
        StateBox[slot] = ent;
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
        if (ScrSys_GetFlag(nod->slot) & FLAG_ONCE_PER_I)
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
        {
            int32_t numb = 0;
            for (int32_t i=owner->stksize - 1; i>=0 && owner->stack[i] != NULL; i--)
            {
                if (owner->stack[i] == pzl)
                {
                    if (numb < MAX_PUZZLS_IN_STACK)
                        numb++;
                    else
                        return;
                }
            }
        }

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
//Nemesis don't use statebox, but this engine does, well make for nemesis it non revert.
    if (StateBox[indx] != NULL)
    {
        #ifdef GAME_NEMESIS
        for (int i=0; i < StateBox[indx]->cnt; i++)
        {
            //if (examine_criterias(StateBox[indx]->nod[i])) //may cause bug's
            AddStateBoxToStk(StateBox[indx]->nod[i]);
        }
        #else
        for (int i=StateBox[indx]->cnt-1; i >= 0; i--)
        {
            //if (examine_criterias(StateBox[indx]->nod[i])) //may cause bug's
            AddStateBoxToStk(StateBox[indx]->nod[i]);
        }
        #endif
    }
}




void ScrSys_exec_puzzle_list(pzllst *lst)
{
    if (lst->exec_times<2)
    {
        StartMList(lst->_list);
        while (!eofMList(lst->_list))
        {
            if (Puzzle_try_exec( (puzzlenode *)DataMList(lst->_list) ) == ACTION_BREAK )
            {
                BreakExecute=true;
                break;
            }
            NextMList(lst->_list);
        }
        lst->exec_times++;
    }
    else
    {
        int i=0,j=lst->stksize;

        while ( i < j)
        {
            puzzlenode *to_exec = lst->stack[i];

            lst->stack[i] = NULL;

            if ( Puzzle_try_exec(to_exec) == ACTION_BREAK )
            {
                BreakExecute=true;
                break;
            }

            i++;
        }

        int z=0;
        for (i = j; i < lst->stksize; i++)
        {
            lst->stack[z] = lst->stack[i];
            z++;
        }
        lst->stksize = z;
    }
}

bool ScrSys_BreakExec()
{
    return BreakExecute;
}

void ScrSys_SetBreak()
{
    BreakExecute=true;
}

void ScrSys_ProcessAllRes()
{
    MList *lst = GetAction_res_List();

    int result=NODE_RET_OK;

    StartMList(lst);
    while (!eofMList(lst))
    {
        struct_action_res *nod = (struct_action_res *)DataMList(lst);

        nod->first_process = true;

        if (!nod->need_delete)
        {
            switch(nod->node_type)
            {
            case NODE_TYPE_MUSIC:
                result = snd_ProcessWav(nod);
                break;
            case NODE_TYPE_TIMER:
                result = tmr_ProcessTimer(nod);
                break;
            case NODE_TYPE_ANIMPLAY:
                result = anim_ProcessAnimPlayNode(nod);
                break;
            case NODE_TYPE_ANIMPRE:
                result = anim_ProcessAnimPreNode(nod);
                break;
            case NODE_TYPE_ANIMPRPL:
                result = anim_ProcessAnimPrePlayNode(nod);
                break;
            case NODE_TYPE_SYNCSND:
                result = snd_ProcessSync(nod);
                break;
            case NODE_TYPE_PANTRACK:
                result = snd_ProcessPanTrack(nod);
                break;
            case NODE_TYPE_TTYTEXT:
                result = txt_ProcessTTYtext(nod);
                break;
            case NODE_TYPE_DISTORT:
                result = Rend_ProcessDistortNode(nod);
                break;
            case NODE_TYPE_REGION:
                result = NODE_RET_OK;
                break;

            default:
                result=NODE_RET_OK;
                break;
            };
        }
        else
        {
            result=NODE_RET_DELETE;
            ScrSys_DeleteNode(nod);
        }

        if (result == NODE_RET_DELETE)
            DeleteCurrent(lst);

        NextMList(lst);
    }
}

MList *ScrSys_FindResAllBySlot(int32_t slot)
{
    MList *lst = new (MList);
    *lst = *GetAction_res_List();

    StartMList(lst);
    while (!eofMList(lst))
    {
        struct_action_res *nod = (struct_action_res *) DataMList(lst);
        if (nod->slot == slot)
            return lst;
        NextMList(lst);
    }
    delete lst;
    return NULL;
}

int ScrSys_DeleteNode(struct_action_res *nod)
{
    switch (nod->node_type)
    {
    case NODE_TYPE_MUSIC:
        return snd_DeleteWav(nod);
        break;
    case NODE_TYPE_TIMER:
        return tmr_DeleteTimer(nod);
        break;
    case NODE_TYPE_ANIMPLAY:
        return anim_DeleteAnimPlay(nod);
        break;
    case NODE_TYPE_ANIMPRE:
        return anim_DeleteAnimPreNod(nod);
        break;
    case NODE_TYPE_ANIMPRPL:
        return anim_DeleteAnimPrePlayNode(nod);
        break;
    case NODE_TYPE_SYNCSND:
        return snd_DeleteSync(nod);
        break;
    case NODE_TYPE_PANTRACK:
        return snd_DeletePanTrack(nod);
        break;
    case NODE_TYPE_TTYTEXT:
        return txt_DeleteTTYtext(nod);
        break;
    case NODE_TYPE_DISTORT:
        return Rend_DeleteDistortNode(nod);
        break;
    case NODE_TYPE_REGION:
        return Rend_deleteRegion(nod);
        break;
    }

    return NODE_RET_NO;
}

void ScrSys_DeleteAllRes()
{
    MList *all = GetAction_res_List();

    StartMList(all);
    while(!eofMList(all))
    {
        struct_action_res *nod = (struct_action_res *)DataMList(all);

        ScrSys_DeleteNode(nod);

        NextMList(all);
    }

    FlushMList(all);

}

void ScrSys_FlushResourcesByOwner(pzllst *owner)
{
    MList *all = GetAction_res_List();
    int result;

    StartMList(all);
    while(!eofMList(all))
    {
        struct_action_res *nod = (struct_action_res *)DataMList(all);

        if (nod->owner == owner)
        {
            result = NODE_RET_OK;

            if (nod->node_type == NODE_TYPE_MUSIC)
            {
                if (nod->nodes.node_music->universe == false)
                    result = snd_DeleteWav(nod);
            }
            else
                result = ScrSys_DeleteNode(nod);


            if (result == NODE_RET_DELETE)
                DeleteCurrent(all);
        }

        NextMList(all);
    }

}

void ScrSys_FlushResourcesByType(int type)
{
    MList *all = GetAction_res_List();

    StartMList(all);
    while(!eofMList(all))
    {
        struct_action_res *nod = (struct_action_res *)DataMList(all);

        if (nod->node_type == type && nod->first_process == true)
            if (ScrSys_DeleteNode(nod) == NODE_RET_DELETE)
                DeleteCurrent(all);

        NextMList(all);
    }

}

void ScrSys_HardFlushResourcesByType(int type)
{
    MList *all = GetAction_res_List();

    StartMList(all);
    while(!eofMList(all))
    {
        struct_action_res *nod = (struct_action_res *)DataMList(all);

        if (nod->node_type == type)
            if (ScrSys_DeleteNode(nod) == NODE_RET_DELETE)
                DeleteCurrent(all);

        NextMList(all);
    }

}

struct_action_res *ScrSys_CreateActRes(int type)
{
    struct_action_res *tmp = new(struct_action_res);

    tmp->nodes.node_unknow = NULL;
    tmp->node_type         = type;
    tmp->owner             = NULL;
    tmp->slot              = 0;
    tmp->need_delete       = false;
    tmp->first_process     = false;

    return tmp;
}








#define pref_COUNT 17

const struct {const char *name;int slot;} prefs[pref_COUNT] =
{
{"KeyboardTurnSpeed",SLOT_KBD_ROTATE_SPEED},
{"PanaRotateSpeed",SLOT_PANAROTATE_SPEED},
{"QSoundEnabled",SLOT_QSOUND_ENABLE},
{"VenusEnabled",SLOT_VENUSENABLED},
{"HighQuality",SLOT_HIGH_QUIALITY},
{"Platform",SLOT_PLATFORM},
{"InstallLevel",SLOT_INSTALL_LEVEL},
{"CountryCode",SLOT_COUNTRY_CODE},
{"CPU",SLOT_CPU},
{"MovieCursor",SLOT_MOVIE_CURSOR},
{"NoAnimWhileTurning",SLOT_TURN_OFF_ANIM},
{"Win958",SLOT_WIN958},
{"ShowErrorDialogs",SLOT_SHOWERRORDIALOG},
{"ShowSubtitles",SLOT_SUBTITLE_FLAG},
{"DebugCheats",SLOT_DEBUGCHEATS},
{"JapaneseFonts",SLOT_JAPANESEFONTS},
{"Brightness",SLOT_BRIGHTNESS}
};


void ScrSys_LoadPreferences()
{
FILE *fl = fopen(pref_FileName,"rb");
if (fl == NULL)
    return;

char buffer[128];
char *str;

while(!feof(fl))
{
    fgets(buffer,128,fl);
    str=TrimLeft(TrimRight(buffer));
    if (str != NULL)
    if (strlen(str)>0)
        for (int16_t i=0; i<pref_COUNT; i++ )
            if (strCMP(str,prefs[i].name)==0)
            {
                str=strstr(str,"=");
                if (str != NULL)
                {
                    str++;
                    str=TrimLeft(str);
                    SetDirectgVarInt(prefs[i].slot,atoi(str));
                }
                break;
            }
};

fclose(fl);
}

void ScrSys_SavePreferences()
{
FILE *fl = fopen(pref_FileName,"wb");
if (fl == NULL)
    return;
fprintf(fl,"[%s]\r\n",pref_TagString);
for (int16_t i=0; i<pref_COUNT; i++ )
    fprintf(fl,"%s=%d\r\n",prefs[i].name,GetgVarInt(prefs[i].slot));

fclose(fl);
}
