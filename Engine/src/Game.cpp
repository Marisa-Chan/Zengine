#include "System.h"


Locate Need_Locate;
bool NeedToLoadScript=false;


int32_t View_start_Loops = 0;

int8_t  SaveSlot = 0;

char *SystemStrings[SYSTEM_STRINGS_NUM];


void SetNeedLocate(uint8_t w, uint8_t r,uint8_t v1, uint8_t v2, int32_t X)
{
    NeedToLoadScript  = true;
    Need_Locate.World = tolower(w);
    Need_Locate.Room  = tolower(r);
    Need_Locate.Node  = tolower(v1);
    Need_Locate.View  = tolower(v2);
    Need_Locate.X     = X;

    if (Need_Locate.World == '0')
    {
        if (GetgVarInt(SLOT_WORLD) == tolower(SystemWorld) &&
                GetgVarInt(SLOT_ROOM) == tolower(SystemRoom))
        {
            Need_Locate.World = GetgVarInt(SLOT_MENU_LASTWORLD);
            Need_Locate.Room  = GetgVarInt(SLOT_MENU_LASTROOM);
            Need_Locate.Node  = GetgVarInt(SLOT_MENU_LASTNODE);
            Need_Locate.View  = GetgVarInt(SLOT_MENU_LASTVIEW);
            Need_Locate.X     = GetgVarInt(SLOT_MENU_LASTVIEW_POS);
        }
        else
        {
            Need_Locate.World = GetgVarInt(SLOT_LASTWORLD);
            Need_Locate.Room  = GetgVarInt(SLOT_LASTROOM);
            Need_Locate.Node  = GetgVarInt(SLOT_LASTNODE);
            Need_Locate.View  = GetgVarInt(SLOT_LASTVIEW);
            Need_Locate.X     = GetgVarInt(SLOT_LASTVIEW_POS);
        }
    }

}




void InitGameLoop()
{
    pzllst *uni = GetUni();
    LoadScriptFile(uni,GetExactFilePath("universe.scr"),false,NULL);

    ScrSys_ChangeLocation(InitWorld,InitRoom,InitNode,InitView,0,true);

    //Hack
    SetDirectgVarInt(SLOT_LASTWORLD, InitWorld);
    SetDirectgVarInt(SLOT_LASTROOM , InitRoom);
    SetDirectgVarInt(SLOT_LASTNODE , InitNode);
    SetDirectgVarInt(SLOT_LASTVIEW , InitView);
    SetDirectgVarInt(SLOT_MENU_LASTWORLD, InitWorld);
    SetDirectgVarInt(SLOT_MENU_LASTROOM , InitRoom);
    SetDirectgVarInt(SLOT_MENU_LASTNODE , InitNode);
    SetDirectgVarInt(SLOT_MENU_LASTVIEW , InitView);
    SetDirectgVarInt(53,250);
    //\Hack



}





void EasterEggsAndDebug()
{
    if (KeyAnyHit())
    {
        if (GetgVarInt(SLOT_DBG_GOTO_LOCATION) == 1)
            if (CheckKeyboardMessage("GO????",6))
            {
                NeedToLoadScript = true;
                SetNeedLocate(GetKeyBuffered(3),
                              GetKeyBuffered(2),
                              GetKeyBuffered(1),
                              GetKeyBuffered(0),0);
            }

        if (CheckKeyboardMessage("M?????T?",8))
            {
                char abc[6];
                abc[0] = GetKeyBuffered(6);
                abc[1] = GetKeyBuffered(5);
                abc[2] = GetKeyBuffered(4);
                abc[3] = GetKeyBuffered(3);
                abc[4] = GetKeyBuffered(2);
                abc[5] = 0;
                int ii = atoi(abc);
                abc[0] = GetKeyBuffered(0);
                abc[1] = 0;
                int ii2 = atoi(abc);
                SetgVarInt(ii,ii2);
            }

        if (CheckKeyboardMessage("DBGONOFF",8))
            SetDirectgVarInt(SLOT_DBG_GOTO_LOCATION, 1 - GetgVarInt(SLOT_DBG_GOTO_LOCATION));

        if (CheckKeyboardMessage("COMPUTERARCH",12))
            {
                //TODO: var-watcher
            }

        if (CheckKeyboardMessage("FFRAME",6))
            {
                //TODO: print current fps
            }

        if (CheckKeyboardMessage("IMNOTDEAFF",10))
            {
                //TODO: unknown
            }

        if (CheckKeyboardMessage("3100OPB",7))
                printf("Current Location: %c%c%c%c\n",GetgVarInt(SLOT_WORLD),
                                                      GetgVarInt(SLOT_ROOM),
                                                      GetgVarInt(SLOT_NODE),
                                                      GetgVarInt(SLOT_VIEW));


        if (CheckKeyboardMessage("KILLMENOW",9))
            {
                //TODO: go to hell :3
                NeedToLoadScript  = true;
                SetNeedLocate('g', 'j', 'd', 'e', 0);
                SetgVarInt(2201,35);
            }


        if (CheckKeyboardMessage("MIKESPANTS",10) )
        {
            NeedToLoadScript  = true;
            SetNeedLocate('g', 'j', 't', 'm', 0);
        }
    }
}


void GameLoop()
{


    Mouse_SetCursor(CURSOR_IDLE);

    if (GetgVarInt(18) != 0)
        SetgVarInt(18,0);

    if (GetgVarInt(10) != 0)
        SetgVarInt(10,0);

    if (KeyAnyHit())
        if (GetLastKey() != SDLK_FIRST)
            SetgVarInt(8,GetWinKey(GetLastKey()));

    if (Rend_MouseInGamescr())
    {
        if (MouseUp(SDL_BUTTON_RIGHT))
            SetgVarInt(18,1);
        if (MouseHit(SDL_BUTTON_LEFT))
            SetgVarInt(10,1);
    }

    ScrSys_ProcessAllRes();

    pzllst *room  = Getroom();
    pzllst *view  = Getview();
    pzllst *world = Getworld();
    pzllst *uni   =  GetUni();

    if (!ScrSys_BreakExec())
        ScrSys_exec_puzzle_list(world);

    if (!ScrSys_BreakExec())
        ScrSys_exec_puzzle_list(room);

    if (!ScrSys_BreakExec())
        ScrSys_exec_puzzle_list(view);

    if (!ScrSys_BreakExec())
        ScrSys_exec_puzzle_list(uni);


    MList *ctrl = Getctrl();

    if (!ScrSys_BreakExec())
        ProcessControls(ctrl);

    if (!ScrSys_BreakExec())
        Rend_RenderFunc();

    if (NeedToLoadScript)
    {
        NeedToLoadScript=false;
        ScrSys_ChangeLocation(Need_Locate.World,Need_Locate.Room,Need_Locate.Node,Need_Locate.View,Need_Locate.X, false);
    }

    EasterEggsAndDebug();

    Rend_ScreenFlip();
}

void ReadSystemStrings(char *filename)
{
    memset(SystemStrings,0,sizeof(SystemStrings[0])*SYSTEM_STRINGS_NUM);

    FILE *fl = fopen(filename,"rb");

    int32_t ii=0;

    char buf[FILE_LN_BUF];

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
        char *str = TrimRight(buf);
        if (ii<SYSTEM_STRINGS_NUM)
        {
            SystemStrings[ii] = (char *)malloc (strlen(str)+1);
            strcpy(SystemStrings[ii],str);
        }
        ii++;
    }

    fclose(fl);
}

char *GetSystemString(int32_t indx)
{
    if (indx<SYSTEM_STRINGS_NUM)
        return SystemStrings[indx];
    return NULL;
}



void ifquit()
{
    struct_SubRect *zzz=Rend_CreateSubRect(0,412,640,68);
    txt_DrawTxtInOneLine(GetSystemString(6),zzz->img);
    Rend_ProcessSubs();
    Rend_ScreenFlip();
    while (!KeyDown(SDLK_y) && !KeyDown(SDLK_n))
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        UpdateKeyboard();
    }
    Rend_DeleteSubRect(zzz);
    if (KeyDown(SDLK_y))
        __END();
}
