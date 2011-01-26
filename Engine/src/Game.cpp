#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "Graph.h"

#include "Game.h"
#include "mylist.h"
#include "System.h"



//#define MAX_DO_ME_NOW 1024
//puzzlenode *DO_ME_NOW_LIST[MAX_DO_ME_NOW];
//int DO_ME_NOWS;


pzllst *uni      =NULL;

//uint8_t     wo  =0;
pzllst   *world  =NULL;
//uint8_t     ro  =0;
pzllst   *room   =NULL;
//uint16_t    vi  =0;
pzllst   *view   =NULL;

Locate  Location= {0,0,0,0};



MList    *ctrl  =NULL;

MList  *timers  =NULL;

MList    *wavs  =NULL;

MList   *anims  =NULL;

MList  *preload =NULL;


//18 default cursors
#define NUM_CURSORS 18
char *(CurNames[])= {"active","arrow","backward","downarrow","forward","handpt","handpu","hdown","hleft",\
                     "hright","hup","idle","leftarrow","rightarrow","suggest_surround","suggest_tilt","turnaround","zuparrow"
                    };

char *(CurFiles[])= {"g0gbc011.zcr","g0gac001.zcr","g0gac021.zcr","g0gac031.zcr","g0gac041.zcr","g0gac051.zcr","g0gac061.zcr","g0gac071.zcr","g0gac081.zcr",\
                     "g0gac091.zcr","g0gac101.zcr","g0gac011.zcr","g0gac111.zcr","g0gac121.zcr","g0gac131.zcr","g0gac141.zcr","g0gac151.zcr","g0gac161.zcr"
                    };

Cursor *CurDefault[NUM_CURSORS];

#define CURSOR_ACTIVE 0
#define CURSOR_HANDPU 6
#define CURSOR_IDLE   11
#define CURSOR_LEFT   12
#define CURSOR_RIGH   13



SDL_Surface *screen;
SDL_Surface *scrbuf=NULL;
Cursor *cur;

int current_obj_cur=0;
Cursor *objcur[2];

//int GAME_X=0;
int GAME_Y=68;


bool NeedToLoadScript=false;
Locate Need_Locate = {0,0,0,0};

Locate Current_Locate = {0,0,0,0};

#define PREV_STACK_MAX   10
Locate Previos_Locate[PREV_STACK_MAX];
uint8_t PrevCount = 0;


#define RENDER_FLAT 0
#define RENDER_PANA 1
uint8_t     Renderer = RENDER_FLAT;


#define VAR_SLOTS_MAX 0xFFFF
void *gVars[VAR_SLOTS_MAX];



#define STATEBOX_STACK_MAX 1024

puzzlenode * StateBoxStk[STATEBOX_STACK_MAX];
uint32_t     StateBoxStkSz = 0;

StateBoxEnt *StateBox[VAR_SLOTS_MAX];


uint8_t  Flags[VAR_SLOTS_MAX];



int32_t RenderDelay = 0;
int32_t View_start_Loops = 0;

int8_t  SaveSlot = 0;




char * ReturnListName(pzllst *lst)
{
    if (lst == world)
        return "world";

    if (lst == uni)
        return "universe";

    if (lst == room)
        return "room";

    if (lst == view)
        return "view";
}

pzllst *CreatePzlLst()
{
    pzllst *tmp     = new (pzllst);
    tmp->_list      = CreateMList();
    tmp->stksize    = 0;
    tmp->exec_times = 0;
    return tmp;
}

void SetgVarInt(int indx, int var)
{
    gVars[indx]=NULL;
    gVars[indx]=(void *)var;

    ShakeStateBox(indx);
}

//void SetgVarInt(int indx, int var)
//{
//    gVars[indx]=NULL;
//    gVars[indx]=(void *)var;
//
//    if (StateBox[indx] != NULL)
//    {
//        for (int i=StateBox[indx]->cnt-1; i >= 0; i--)
//        {
//            if (StateBoxStkSz < STATEBOX_STACK_MAX)
//            {
//                puzzlenode *nod = StateBox[indx]->nod[i];
//
//                bool DO=false;
//
//                pushMList(nod->CritList);
//
//                StartMList(nod->CritList);
//                while (!eofMList(nod->CritList))
//                {
//                    MList *criteries=(MList *)DataMList(nod->CritList);
//
//                    DO |= ProcessCriteries(criteries);
//
//                    NextMList(nod->CritList);
//                }
//
//                popMList(nod->CritList);
//
//                if (DO)
//                {
//                    /*
//                    if (StateBoxStkSz>0)
//                        if (StateBoxStk[StateBoxStkSz-1] == StateBox[indx]->nod[i])
//                             return;
//                    */
//                    StateBoxStk[StateBoxStkSz] = StateBox[indx]->nod[i];
//                    StateBoxStkSz++;
//                }
//
//            }
//            else
//            {
//                //printf("StateBox Stack overflow!");
//            }
//
//        }
//    }
//}

int GetgVarInt(int indx)
{
    return (int)gVars[indx];
}



void SetgVarInt(void **Vars, int indx, int var)
{
    Vars[indx]=NULL;
    Vars[indx]=(void *)var;
}

int GetgVarInt(void **Vars, int indx)
{
    return (int)Vars[indx];
}

void SaveGame(char *file)
{
    void *tmpVars[VAR_SLOTS_MAX];
    memcpy(tmpVars,gVars,VAR_SLOTS_MAX*sizeof(void *));

    StartMList(timers);
    while (!eofMList(timers))
    {
        timernode *nod=(timernode *)DataMList(timers);
        SetgVarInt(tmpVars,nod->slot,2);
        NextMList(timers);
    }

    StartMList(wavs);
    while (!eofMList(wavs))
    {
        musicnode *nod=(musicnode *)DataMList(wavs);
        SetgVarInt(tmpVars,nod->slot,2);
        NextMList(wavs);
    }

    StartMList(anims);
    while (!eofMList(anims))
    {
        animnode *nod=(animnode *)DataMList(anims);
        SetgVarInt(tmpVars,nod->slot,2);
        NextMList(anims);
    }

    StartMList(room->_list);
    while (!eofMList(room->_list))
    {
        puzzlenode *nod=(puzzlenode *)DataMList(room->_list);
        SetgVarInt(tmpVars,nod->slot,0);
        NextMList(room->_list);
    }

    StartMList(view->_list);
    while (!eofMList(view->_list))
    {
        puzzlenode *nod=(puzzlenode *)DataMList(view->_list);
        SetgVarInt(tmpVars,nod->slot,0);
        NextMList(view->_list);
    }

    StartMList(world->_list);
    while (!eofMList(world->_list))
    {
        puzzlenode *nod=(puzzlenode *)DataMList(world->_list);
        SetgVarInt(tmpVars,nod->slot,0);
        NextMList(world->_list);
    }

    StartMList(uni->_list);
    while (!eofMList(uni->_list))
    {
        puzzlenode *nod=(puzzlenode *)DataMList(uni->_list);
        SetgVarInt(tmpVars,nod->slot,0);
        NextMList(uni->_list);
    }

    FILE *fil=fopen(file, "wb");

    fwrite(Previos_Locate,PREV_STACK_MAX,sizeof(Locate),fil);
    fwrite(&PrevCount,1,sizeof(PrevCount),fil);
    fwrite(&Location,1,sizeof(Location),fil);
    fwrite(&Current_Locate,1,sizeof(Locate),fil);


    fwrite(tmpVars,VAR_SLOTS_MAX,sizeof(void *),fil);
    fwrite(Flags,VAR_SLOTS_MAX,sizeof(uint8_t),fil);

    fclose(fil);
}


void LoadGame(char *file)
{
    Locate tmp;


    FILE *fil=fopen(file, "rb");
    if (!fil)
        return;

//    DeleteAnims(anims);
    DeleteWavs(wavs);
    DeleteTimers(timers);
//    DeletePuzzleList(view);
//    DeletePuzzleList(room);
//    DeletePuzzleList(world);
//    DeletePuzzleList(uni);
//    DeleteControlList(ctrl);

    timers=CreateMList();
    wavs=CreateMList();

    memset(&Current_Locate,0,sizeof(Locate));




    fread(Previos_Locate,PREV_STACK_MAX,sizeof(Locate),fil);
    fread(&PrevCount,1,sizeof(PrevCount),fil);
    fread(&tmp,1,sizeof(tmp),fil);

    memset(&Location,0,sizeof(Location));



    //memcpy(&Location,&tmp,sizeof(Location));

    fread(&Current_Locate,1,sizeof(Locate),fil);



    fread(gVars,VAR_SLOTS_MAX,sizeof(void *),fil);
    fread(Flags,VAR_SLOTS_MAX,sizeof(uint8_t),fil);

    fclose(fil);

    ChangeLocation(tmp.World,tmp.Room,tmp.View,tmp.X+320);


}



void InitScriptsEngine()
{
    memset(gVars,0x0,VAR_SLOTS_MAX * sizeof(void *));
    timers=CreateMList();
    wavs=CreateMList();

    memset(StateBox,0x0,VAR_SLOTS_MAX * sizeof(StateBoxEnt *));
    StateBoxStkSz = 0;

    memset(Flags,0x0,VAR_SLOTS_MAX * sizeof(uint8_t));
}

bool Eligeblity(int obj, slotnode *slut)
{
    bool eli = false;

    for (int i=0; i< slut->eligable_cnt; i++)
        if (obj == slut->eligible_objects[i])
        {
            eli = true;
            break;
        }
    return eli;
}

//Don't call it from loops for mylists!! it's cause error
bool SlotIsOwned(int i)
{
    StartMList(timers);
    while (!eofMList(timers))
    {
        timernode *nod= (timernode *)DataMList(timers);

        if (nod->slot == i)
            return true;

        NextMList(timers);
    }

    StartMList(wavs);
    while (!eofMList(wavs))
    {
        musicnode *nod= (musicnode *)DataMList(wavs);

        if (nod->slot == i)
            return true;

        NextMList(wavs);
    }


    return false;
}




char * PrepareString(char *buf)
{
    for (int i=strlen(buf)-1; i>-1; i--)
        if (buf[i]==0x0A || buf[i]==0x0D || buf[i]=='#')
            buf[i]=0x00;

    char *str=buf;
    for (int i=0; i<strlen(buf); i++)
        if (buf[i]!=0x20 && buf[i]!=0x09)
        {
            str=buf + i;
            break;
        }
    return str;
}

char * GetParams(char *str)
{
    for (int i=strlen(str)-1; i>-1; i--)
    {
        if (str[i]==')')
            str[i]=0x0;
        else if (str[i]=='(')
        {
            return str+i+1;
        }
    }
}

#define strCMP(X,Y) strncasecmp(X,Y,strlen(Y))

int GetIntVal(char *chr)
{
    if (chr[0]=='[')
    {
        return GetgVarInt(atoi(chr+1));
    }
    else
        return atoi(chr);
}




void action_set_screen(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:set_screen  %s\n",params);
#endif

    if (scrbuf)
        SDL_FreeSurface(scrbuf);

    scrbuf=IMG_Load(GetFilePath(params));
    if (!scrbuf)
        printf("ERROR:  IMG_Load(%s): %s\n\n",params, IMG_GetError());
    else
    {
        ConvertImage(&scrbuf);

        Current_Locate.World = Location.World;
        Current_Locate.Room  = Location.Room;
        Current_Locate.View  = Location.View;
        Current_Locate.X = Location.X + (Renderer == RENDER_PANA ? 320 : 0 );
    }
}

void action_set_partial_screen(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:set_partial_screen(%s)\n",params);
#endif
    int x,y,tmp1,tmp2;
    char xx[16],yy[16],tmp11[16],tmp22[16];
    char file[255];
    sscanf(params,"%s %s %s %s %s",xx,yy,file,tmp11,tmp22);

    x=GetIntVal(xx);
    y=GetIntVal(yy);
    tmp1=GetIntVal(tmp11);
    tmp2=GetIntVal(tmp22);

    SDL_Surface *tmp=IMG_Load(GetFilePath(file));
    if (!tmp)
        printf("ERROR:  IMG_Load(%s): %s\n\n",params, IMG_GetError());
    else
    {
        SDL_Surface *tmp_1=SDL_ConvertSurface(tmp,screen->format,0);
        SDL_FreeSurface(tmp);
        tmp=tmp_1;

        if (tmp2 != -1)
        {
            int r,g,b;
            b=FiveBitToEightBitLookupTable[((tmp2 >> 10 ) & 0x1F)];
            g=FiveBitToEightBitLookupTable[((tmp2 >> 5 ) & 0x1F)];
            r=FiveBitToEightBitLookupTable[(tmp2 & 0x1F)];
#ifdef TRACE
            printf("        action:set_partial_screen Color Key (%x %x %x)\n",r,g,b);
#endif
            SDL_SetColorKey(tmp,SDL_SRCCOLORKEY ,SDL_MapRGB(tmp->format,r,g,b));
        }

        //if (tmp1==0)
        DrawImageToSurf(tmp,x,y,scrbuf);
        //else if (tmp1==1)
        //    DrawImageToSurf(tmp,x-tmp->w/2,y-tmp->h/2,scrbuf);
        SDL_FreeSurface(tmp);

    }
}

void action_assign(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:assign(%s)\n",params);
#endif
    char tmp1[16],tmp2[16];
    sscanf(params,"%s %s",tmp1,tmp2);

    SetgVarInt(GetIntVal(tmp1),GetIntVal(tmp2));
}

void action_timer(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:timer(%s)\n",params);
#endif
    int tmp1,tmp3;
    char tmp2[16];
    char *s;
    sscanf(params,"%d %s",&tmp1,tmp2);

    if (SlotIsOwned(tmp1))
    {
        //SetgVarInt(tmp1,1);
//#ifdef TRACE
//        printf("        owned %d\n",GetIntVal(tmp2));
//#endif
        return;
    }


    timernode *nod = new (timernode);
    nod->slot = tmp1;

    nod->owner = owner;

    s=PrepareString(tmp2);

    nod->time = GetTickCount() + GetIntVal(s);
//#ifdef TRACE
//    printf(" %d\n",GetIntVal(s));
//#endif
    AddToMList(timers,nod);

    SetgVarInt(tmp1,1);

}

void action_change_location(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:change_location(%s)\n",params);
#endif
    char tmp[4];
    char tmp2[4];
    char tmp3[4];
    char tmp4[16];
    sscanf(params,"%c, %c, %c%c, %s",tmp,tmp2,tmp3,tmp3+1,tmp4);

    NeedToLoadScript=true;
    Need_Locate.World=toupper(tmp[0]);
    Need_Locate.Room=toupper(tmp2[0]);
    Need_Locate.View=toupper(tmp3[1]) | (toupper(tmp3[0]) << 8);
    Need_Locate.X=GetIntVal(tmp4);
}

void action_dissolve(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:dissolve()\n");
#endif

    Current_Locate.World = Location.World;
    Current_Locate.Room  = Location.Room;
    Current_Locate.View  = Location.View;
    Current_Locate.X = Location.X + (Renderer == RENDER_PANA ? 320 : 0 );
}

void action_disable_control(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:disable_control(%s)\n",params);
#endif



    //StartMList(ctrl);

    int slot = GetIntVal(params);
    //sscanf(params,"%d",&slot);

    Flags[slot] = FLAG_DISABLED;

    /*while(!eofMList(ctrl))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(ctrl);
        if (nod->slot == slot)
        {
            nod->enable = false;
            break;
        }
        NextMList(ctrl);
    }*/
}

void action_enable_control(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:enable_control(%s)\n",params);
#endif

    //StartMList(ctrl);

    int slot = GetIntVal(params);
    //sscanf(params,"%d",&slot);

    Flags[slot] = 0;

    /*while(!eofMList(ctrl))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(ctrl);
        if (nod->slot == slot)
        {
            nod->enable = true;
            break;
        }
        NextMList(ctrl);
    }*/
}

void action_add(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:add(%s)\n",params);
#endif

    char slot[16],number[16];
    int tmp;
    sscanf(params,"%s %s",slot, number);

    tmp = GetIntVal(slot);
    SetgVarInt(tmp, GetgVarInt(tmp) + GetIntVal(number));
}

void action_debug(char *params, pzllst *owner)
{


    char txt[256],number[16];
    int tmp;
    sscanf(params,"%s %s",txt, number);

    tmp = GetIntVal(number);

#ifdef TRACE
    printf("DEBUG :%s\t: %d \n",txt,tmp);
#endif
}

void action_random(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:random(%s)\n",params);
#endif

    int slot,number;
    char chars[16];
    sscanf(params,"%d %s",&slot, chars);
    number=GetIntVal(chars);

    SetgVarInt(slot, rand() % (number+1) );
}



void action_streamvideo(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:streamvideo(%s)\n",params);
#endif
    char file[255];
    char x[16];
    char y[16];
    char w[16];
    char h[16];
    char u1[16];
    char u2[16];

    int xx,yy,ww,hh,tmp;

    char *fil;

    sscanf(params,"%s %s %s %s %s %s %s",file,x,y,w,h,u1,u2);

    xx=GetIntVal(x);
    yy=GetIntVal(y);
    ww=GetIntVal(w) - xx +1;
    hh=GetIntVal(h) - yy +1;

    anim_avi *anm=new(anim_avi);
    Mix_Chunk *aud=NULL;
    anm->mpg=SMPEG_new(GetFilePath(file),&anm->inf,0);
    anm->img = CreateSurface(ww,hh);


    tmp=strlen(file);
    file[tmp-1]='v';
    file[tmp-2]='a';
    file[tmp-3]='w';

    fil=GetFilePath(file);

    if (fil!=NULL)
    {
        aud = Mix_LoadWAV(fil);
    }


    SMPEG_setdisplay(anm->mpg,anm->img,0,0);
    SMPEG_setdisplayregion(anm->mpg, 0, 0, anm->inf.width,anm->inf.height);
    SMPEG_renderFrame(anm->mpg,1);
    SMPEG_play(anm->mpg);
    if (aud!=NULL)
    {
        tmp=GetFreeChannel();
        Mix_PlayChannel(tmp,aud,0);
        if (u2 == 0)
        {
            SaveVol();
            SilenceVol();
        }
        Mix_Volume(tmp,127);
    }

    while(SMPEG_status(anm->mpg) != SMPEG_STOPPED  &&  !KeyDown(SDLK_SPACE))
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        UpdateKeyboard();
        DrawImage(anm->img,xx,GAME_Y+yy);
        SDL_Flip(screen);
        SDL_Delay(1000/anm->inf.current_fps);
    }
    if (aud!=NULL)
    {
        if (u2 == 0)
            RestoreVol();
        Mix_HaltChannel(tmp);
        Mix_FreeChunk(aud);
    }

    SMPEG_stop(anm->mpg);
    SMPEG_delete(anm->mpg);
    delete anm;
}

void action_animplay(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:animplay(%s)\n",params);
#endif

    int slot;
    char file[255];
    char x[16];
    char y[16];
    char w[16];
    char h[16];
    char st[16];
    char en[16];
    char un1[16];
    char loop[16];
    char un2[16];
    char mask[16];
    char un4[16];
    sscanf(params,"%d %s %s %s %s %s %s %s %s %s %s %s %s",&slot,file,x,y,w,h,st,en,loop,un1,un2,mask,un4);

    StartMList(anims);
    while (!eofMList(anims))
    {

        animnode *nd = (animnode *)DataMList(anims);

        if (nd->slot == slot)
            {
                DeleteAnimNod(nd);
                DeleteCurrent(anims);
            }

        NextMList(anims);
    }



    SetgVarInt(slot, 1);




    animnode *nod = new (animnode);
    anim_avi *anm;
    AddToMList(anims,nod);





    nod->slot = slot;

    nod->nexttick = millisec();
    nod->x = GetIntVal(x);
    nod->y = GetIntVal(y);
    nod->w = GetIntVal(w) - nod->x +1;
    nod->h = GetIntVal(h) - nod->y +1;
    nod->loopcnt = GetIntVal(loop);

    nod->loops=0;

    if (strcasestr(file,"avi")!=NULL)
    {
        anm=new(anim_avi);
        nod->anim = (void *) anm;
        anm->mpg=SMPEG_new(GetFilePath(file),&anm->inf,0);
        anm->img = CreateSurface(nod->w,nod->h);
        SMPEG_setdisplay(anm->mpg,anm->img,0,0);
        SMPEG_setdisplayregion(anm->mpg, 0, 0, anm->inf.width,anm->inf.height);

        //if (nod->loopcnt == 0 )
        //  {

        //     SMPEG_play(anm->mpg);

        //    anm->pld=true;
        //    anm->loop=true;
        //}
        //if (anm->inf.width != nod->w)
        //   SMPEG_scaleXY(anm->mpg,1,1);
        nod->vid=true;
    }
    else
    {


        nod->vid=false;


        int r=GetIntVal(mask),g,b;
        b=FiveBitToEightBitLookupTable[((r >> 10 ) & 0x1F)];
        g=FiveBitToEightBitLookupTable[((r >> 5 ) & 0x1F)];
        r=FiveBitToEightBitLookupTable[(r & 0x1F)];

        nod->anim = LoadAnimImage(file,r | g<<8 | b<<16);
    }

    if (nod->vid)
    {
        nod->start= GetIntVal(st) *2;
        nod->end= GetIntVal(en) *2;

        SMPEG_renderFrame(anm->mpg,nod->start+1);
    }
    else
    {
        nod->start= GetIntVal(st);
        nod->end= GetIntVal(en);
    }

    nod->CurFr = nod->start;
}

void action_music(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:music(%s)\n",params);
#endif

    int slot;
    char chars[16];
    sscanf(params,"%d %s",&slot, chars);



    timernode *nod = new (timernode);
    nod->slot = slot;

    nod->owner = owner;


    nod->time = GetTickCount() + 10;
    AddToMList(timers,nod);

    SetgVarInt(slot, 1);
}

void action_universe_music(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:universe_music(%s) (%s)\n",params,ReturnListName(owner));
#endif

    int slot;
    char unk1[16];
    char file[16];
    char loop[16];
    char vol[16];
    sscanf(params,"%d %s %s %s %s",&slot, unk1, file, loop, vol);

    //printf ("%s %d %d\n",file,GetIntVal(vol),SoundVol[GetIntVal(vol)]);

    if (SlotIsOwned(slot))
        return;



    musicnode *nod = new (musicnode);
    nod->slot = slot;

    nod->chunk = Mix_LoadWAV(GetFilePath(file));

    nod->chn = GetFreeChannel();
    if (nod->chn == -1)
    {
        printf("ERROR, NO CHANNELS!\n");
        Mix_FreeChunk(nod->chunk);
        delete nod;
        return;
    }

    if (GetIntVal(loop)==1)
    {
        Mix_PlayChannel(nod->chn,nod->chunk,-1);
        nod->looped = true;
    }
    else
    {
        Mix_PlayChannel(nod->chn,nod->chunk,0);
        nod->looped = false;
    }


    LockChan(nod->chn);

    Mix_Volume(nod->chn,SoundVol[GetIntVal(vol)]);
//    printf("chan %d vol %d\n",nod->chn,GetIntVal(vol));

    nod->owner = owner;

    AddToMList(wavs,nod);

    SetgVarInt(slot, 1);
}


void action_syncsound(char *params, pzllst *owner)
{
    printf("PlayPreload \n");
}

void action_animpreload(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:animpreload(%s)\n",params);
#endif
    if (preload == NULL)
        preload = CreateMList();

    int      slot;
    char name[64];

    struct_Preload *pre = new (struct_Preload);

    sscanf(params,"%d %s", &slot, name);

    pre->fil = new (char[255]);

    strcpy(pre->fil,name);
    pre->slot = slot;

    AddToMList(preload,pre);

    SetgVarInt(pre->slot,2);

    //printf("AnimPreload \n");
}

void action_playpreload(char *params, pzllst *owner)
{
    char sl[16];
    int x,y,w,h,start,end,loop,slot,sll;
    sscanf(params,"%d %s %d %d %d %d %d %d %d",&sll,sl,&x,&y,&w,&h,&start,&end,&loop);

#ifdef TRACE
    printf("        action:playpreload(%s)\n",params);
#endif

    char buff[255];
    bool found = false;

    struct_Preload *pre;

    slot = GetIntVal(sl);

    StartMList(preload);
    while (!eofMList(preload))
    {
        pre = (struct_Preload *)DataMList(preload);
        if (pre->slot == slot)
        {
            found = true;
            break;
        }

        NextMList(preload);
    }

    if (!found)
        return;

    if (sll == 0)
        sll = slot;

    sprintf(buff,"%d %s %d %d %d %d %d %d %d %d %d %d %d",sll ,pre->fil,\
            x, y, w, h, start, end, loop, 0, 0, 0, 0);

    action_animplay(buff,owner);
    //SetgVarInt(GetIntVal(chars),2);
}

void action_ttytext(char *params, pzllst *owner)
{
    char chars[16];
    sscanf(params,"%s",chars);
    SetgVarInt(GetIntVal(chars),2);
}

void action_kill(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:kill(%s)\n",params);
#endif

    int slot;
    char chars[16];
    sscanf(params,"%s",chars);

    if (strcasecmp(chars,"\"all\"")==0)
    {
        DeleteAnims(anims);
        anims = CreateMList();

        StartMList(wavs);
        while(!eofMList(wavs))
        {
            musicnode *nod = (musicnode *)DataMList(wavs);
            Mix_HaltChannel(nod->chn);
            UnlockChan(nod->chn);
            SetgVarInt(nod->slot, 2);
            delete nod;
            DeleteCurrent(wavs);


            NextMList(wavs);
        }

        StartMList(timers);
        while(!eofMList(timers))
        {
            musicnode *nod = (musicnode *)DataMList(timers);

            delete nod;
            DeleteCurrent(timers);
            SetgVarInt(slot, 2);
            NextMList(timers);
        }

        return;
    }

    if (strcasecmp(chars,"\"audio\"")==0)
    {

        StartMList(wavs);
        while(!eofMList(wavs))
        {
            musicnode *nod = (musicnode *)DataMList(wavs);
            if (nod->slot == slot)
            {
                Mix_HaltChannel(nod->chn);
                UnlockChan(nod->chn);
                delete nod;
                DeleteCurrent(wavs);
                SetgVarInt(slot, 2);
                return;
            }

            NextMList(wavs);
        }

        return;
    }

    slot = GetIntVal(chars);

    StartMList(anims);
    while(!eofMList(anims))
    {
        animnode *nod = (animnode *)DataMList(anims);
        if (nod->slot == slot)
        {
            if (nod->vid)
            {
                SDL_FreeSurface(((anim_avi *)nod->anim)->img);
                SMPEG_delete(((anim_avi *)nod->anim)->mpg);
            }
            else
                FreeAnimImage((anim_surf *)nod->anim);
            delete nod;
            DeleteCurrent(anims);
            SetgVarInt(slot, 2);
            return;
        }

        NextMList(anims);
    }

    StartMList(wavs);
    while(!eofMList(wavs))
    {
        musicnode *nod = (musicnode *)DataMList(wavs);
        if (nod->slot == slot)
        {
            Mix_HaltChannel(nod->chn);
            UnlockChan(nod->chn);
            delete nod;
            DeleteCurrent(wavs);
         //   SetgVarInt(slot, 2);
            return;
        }

        NextMList(wavs);
    }

    StartMList(timers);
    while(!eofMList(timers))
    {
        musicnode *nod = (musicnode *)DataMList(timers);
        if (nod->slot == slot)
        {
            delete nod;
            DeleteCurrent(timers);
            SetgVarInt(slot, 2);
            return;
        }

        NextMList(timers);
    }

#ifdef TRACE
    printf("Nothing to kill %d\n",slot);
#endif
}


void action_stop(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:stop(%s)\n",params);
#endif

    int slot;
    char chars[16];
    sscanf(params,"%s",chars);
    slot = GetIntVal(chars);

    StartMList(timers);
    while(!eofMList(timers))
    {
        timernode *nod = (timernode *)DataMList(timers);
        if (nod->slot == slot)
        {
            delete nod;
            DeleteCurrent(timers);
            SetgVarInt(slot, 2);
            return;
        }

        NextMList(timers);
    }

    StartMList(anims);
    while(!eofMList(anims))
    {
        animnode *nod = (animnode *)DataMList(anims);
        if (nod->slot == slot)
        {
            if (nod->vid)
            {
                SDL_FreeSurface(((anim_avi *)nod->anim)->img);
                SMPEG_stop(((anim_avi *)nod->anim)->mpg);
                SMPEG_delete(((anim_avi *)nod->anim)->mpg);
            }
            else
                FreeAnimImage((anim_surf *)nod->anim);

            delete nod;
            DeleteCurrent(anims);
            SetgVarInt(slot, 2);
            return;
        }

        NextMList(anims);
    }

    StartMList(wavs);
    while(!eofMList(wavs))
    {
        musicnode *nod = (musicnode *)DataMList(wavs);
        if (nod->slot == slot)
        {
            Mix_HaltChannel(nod->chn);
            UnlockChan(nod->chn);
            delete nod;
            DeleteCurrent(wavs);
            SetgVarInt(slot, 2);
            return;
        }

        NextMList(wavs);
    }

    printf("Nothing to stop %d\n",slot);
}


void action_inventory(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:inventory(%s)\n",params);
#endif

    int item;
    char cmd[16];
    char chars[16];
    sscanf(params,"%s %s",cmd,chars);
    item = GetIntVal(chars);
    int i;

    if (strcasecmp(cmd,"add")==0)
    {
        for (i=SLOT_START_SLOT; i<= SLOT_END_SLOT ; i++)
            if (GetgVarInt(i)==0)
            {
                if (GetgVarInt(SLOT_INVENTORY_MOUSE)!=0)
                {
                    SetgVarInt(i,GetgVarInt(SLOT_INVENTORY_MOUSE));
                    SetgVarInt(SLOT_INVENTORY_MOUSE,0);
                }
                else
                    break;
            }

        SetgVarInt(SLOT_INVENTORY_MOUSE,item);
    }


    if (strcasecmp(cmd,"addi")==0)
    {
        item=GetgVarInt(item);
        for (i=SLOT_START_SLOT; i<= SLOT_END_SLOT ; i++)
            if (GetgVarInt(i)==0)
            {
                if (GetgVarInt(SLOT_INVENTORY_MOUSE)!=0)
                {
                    SetgVarInt(i,GetgVarInt(SLOT_INVENTORY_MOUSE));
                    SetgVarInt(SLOT_INVENTORY_MOUSE,0);
                }
                else
                    break;
            }

        SetgVarInt(SLOT_INVENTORY_MOUSE,item);
    }

    if (strcasecmp(cmd,"drop")==0)
    {
        for (i=SLOT_START_SLOT; i<= SLOT_END_SLOT ; i++)
            if (GetgVarInt(i)==item)
            {
                SetgVarInt(i,0);
                break;
            }
        if (GetgVarInt(SLOT_INVENTORY_MOUSE) == item)
            SetgVarInt(SLOT_INVENTORY_MOUSE,0);
    }

    if (strcasecmp(cmd,"dropi")==0)
    {
        SetgVarInt(item,0);
    }


}

void action_crossfade(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:crossfade(%s)\n",params);
#endif

    int item,item2;
    char slot[16];
    char slot2[16]; //unknown slot
    char unk1[16];
    char unk2[16];
    char toVol[16];
    char toVol2[16];
    char unk4[16];
    sscanf(params,"%s %s %s %s %s %s %s",slot,slot2,unk1,unk2,toVol,toVol2,unk4);
    item = GetIntVal(slot);
    item2 = GetIntVal(slot2);

    StartMList(wavs);
    while(!eofMList(wavs))
    {
        musicnode *nod = (musicnode *)DataMList(wavs);

        if (nod->slot == item)
            Mix_Volume(nod->chn , SoundVol[GetIntVal(toVol)]);

        if (nod->slot == item2)
            Mix_Volume(nod->chn , SoundVol[GetIntVal(toVol2)]);

        NextMList(wavs);
    }

}


void ParsePuzzle(char *instr, MList *lst)
{
    char *str;
    char buf[255];
    bool HaveDts=false;

    func_node *nod;
    char *params;

    if (strCMP(instr,"action:")==0)
    {
        str=instr+7;

        memset(buf,0,255);

        int end_s=strlen(str);

        for (int i=0; i<strlen(str); i++)
        {
            if (str[i]!='(' && str[i]!=0x20 && str[i]!=0x09 && str[i]!='#' && str[i]!=0x00 && str[i]!=':')
                buf[i]=_tolower(str[i]);
            else
            {
                if (str[i]==':')
                    HaveDts=true;
                end_s=i;
                break;
            }
        }

        if (strCMP(buf,"set_screen")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_set_screen;
            return;
        }

        if (strCMP(buf,"debug")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_debug;
            return;
        }

        if (strCMP(buf,"assign")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_assign;
            return;
        }
        if (strCMP(buf,"timer")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            char buff[255];
            int tmp1=0;

            sscanf(str+end_s+1,"%d",&tmp1);

            sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

            nod->param=(char *)malloc(strlen(buff)+1);
            strcpy(nod->param,buff);

            nod->func=action_timer;
            return;
        }
        if (strCMP(buf,"set_partial_screen")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_set_partial_screen;
            return;
        }

        if (strCMP(buf,"change_location")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_change_location;
            return;
        }

        if (strCMP(buf,"dissolve")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            //params=GetParams(str+end_s);
            nod->param=NULL;
            //strcpy(nod->param,params);

            nod->func=action_dissolve; //make save prev W R VI
            return;
        }

        if (strCMP(buf,"disable_control")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_disable_control;
            return;
        }
        if (strCMP(buf,"enable_control")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_enable_control;
            return;
        }
        if (strCMP(buf,"add")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_add;
            return;
        }
        if (strCMP(buf,"random")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            char buff[255];
            int tmp1=0;

            sscanf(str+end_s+1,"%d",&tmp1);

            sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

            nod->param=(char *)malloc(strlen(buff)+1);
            strcpy(nod->param,buff);

            nod->func=action_random;
            return;
        }
        if (strCMP(buf,"animplay")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            char buff[255];
            int tmp1=0;

            sscanf(str+end_s+1,"%d",&tmp1);

            //SetgVarInt(tmp1,0); //hack ?

            sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

            nod->param=(char *)malloc(strlen(buff)+1);
            strcpy(nod->param,buff);

            nod->func=action_animplay;
            return;
        }
        if (strCMP(buf,"universe_music")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            char buff[255];
            int tmp1=0;

            sscanf(str+end_s+1,"%d",&tmp1);

            sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

            nod->param=(char *)malloc(strlen(buff)+1);
            strcpy(nod->param,buff);

            nod->func=action_universe_music;
            return;
        }
        if (strCMP(buf,"music")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            char buff[255];
            int tmp1=0;

            sscanf(str+end_s+1,"%d",&tmp1);

            sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

            nod->param=(char *)malloc(strlen(buff)+1);
            strcpy(nod->param,buff);

            nod->func=action_universe_music;
            return;
        }

        if (strCMP(buf,"kill")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_kill;
            return;
        }

        if (strCMP(buf,"stop")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_stop;
            return;
        }

        if (strCMP(buf,"inventory")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_inventory;
            return;
        }

        if (strCMP(buf,"crossfade")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_crossfade;
            return;
        }

        if (strCMP(buf,"streamvideo")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_streamvideo;
            return;
        }

        if (strCMP(buf,"animpreload")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            char buff[255];
            int tmp1=0;

            sprintf(buff,"%d %s",atoi(str+end_s+1),GetParams(str+end_s+1));

            nod->param=(char *)malloc(strlen(buff)+1);
            strcpy(nod->param,buff);

            nod->func=action_animpreload;
            return;
        }
        if (strCMP(buf,"playpreload")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            char buff[255];

            if (HaveDts)
                sprintf(buff,"%d %s",atoi(str+end_s+1),GetParams(str+end_s+1));
            else
                sprintf(buff,"%d %s",0,GetParams(str+end_s));


            //sprintf(buff,"%d %s",atoi(str+end_s+1),GetParams(str+end_s+1));

            nod->param=(char *)malloc(strlen(buff)+1);
            strcpy(nod->param,buff);

            nod->func=action_playpreload;
            return;
        }
        if (strCMP(buf,"syncsound")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            params=GetParams(str+end_s);
            nod->param=(char *)malloc(strlen(params)+1);
            strcpy(nod->param,params);

            nod->func=action_syncsound;
            return;
        }

        if (strCMP(buf,"ttytext")==0)
        {
            nod=new(func_node);
            AddToMList(lst,nod);

            char buff[255];
            int tmp1=0;

            sscanf(str+end_s+1,"%d",&tmp1);

            sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

            nod->param=(char *)malloc(strlen(buff)+1);
            strcpy(nod->param,buff);

            nod->func=action_ttytext;
            return;
        }

    }
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


    char buf[0x400];
    char *str,*str2,*str3;



    while(!feof(fl))
    {
        fgets(buf,0x400,fl);

        str=PrepareString(buf);


        if (strCMP(str,"puzzle")==0)
        {
            uint32_t    slot;
            sscanf(str,"puzzle:%d",&slot); //read slot number;

#ifdef FULLTRACE
            printf("puzzle:%d Creating object\n",slot);
#endif

            //SetgVarInt(slot,0); /////////////////////////////////////

            puzzlenode *pzl=new(puzzlenode);

            pzl->owner = lst;

            pzl->slot=slot;

            AddToMList(lst->_list,pzl);

            pzl->CritList = CreateMList();
            pzl->ResList = CreateMList();



            pzl->flags = 0;

            for (;;)
            {
                fgets(buf,0x400,fl);
                str2=PrepareString(buf);

                if (str2[0] == '}')
                    break;
                else if (strCMP(str2,"criteria")==0) //PARSE CRITERIA
                {
#ifdef FULLTRACE
                    printf("Creating criteria\n");
#endif
                    MList *crit_nodes_lst=CreateMList();

                    AddToMList(pzl->CritList,crit_nodes_lst);

                    for (;;)
                    {
                        fgets(buf,0x400,fl);
                        str3=PrepareString(buf);

                        if (str3[0] == '}')
                            break;
                        else if (str3[0] == '[')
                        {
                            crit_node *nod=new(crit_node);
                            AddToMList(crit_nodes_lst,nod);

                            sscanf(&str3[1],"%d",&nod->slot1);

                            int ij;
                            for (ij=0; ij<strlen(str3); ij++)
                            {
                                if (str3[ij]=='!')
                                {
                                    nod->oper=CRIT_OP_NOT;
                                    break;
                                }
                                else if (str3[ij]=='>')
                                {
                                    nod->oper=CRIT_OP_GRE;
                                    break;
                                }
                                else if (str3[ij]=='<')
                                {
                                    nod->oper=CRIT_OP_LEA;
                                    break;
                                }
                                else if (str3[ij]=='=')
                                {
                                    nod->oper=CRIT_OP_EQU;
                                    break;
                                }
                            }


                            for (ij++; ij<strlen(str3); ij++)
                            {
                                if (str3[ij]=='[')
                                {
                                    sscanf(&str3[ij+1],"%d",&nod->slot2);
                                    nod->var2=true;
                                    break;
                                }
                                else if (str3[ij]!=0x20 && str3[ij]!=0x09)
                                {
                                    sscanf(&str3[ij],"%d",&nod->slot2);
                                    nod->var2=false;
                                    break;
                                }
                            }


                        }
                        else
                        {
                            printf("Warning!!! %s\n",str3);
                        }
                    }
                }
                else if (strCMP(str2,"results")==0) //RESULTS
                {
#ifdef FULLTRACE
                    printf("Creating results\n");
#endif
                    for (;;)
                    {
                        fgets(buf,0x400,fl);
                        str3=PrepareString(buf);

                        if (str3[0] == '}')
                            break;
                        else
                            ParsePuzzle(str3,pzl->ResList);
                    }
                }
                else if (strCMP(str2,"flags")==0)  // FLAGS
                {
#ifdef FULLTRACE
                    printf("Reading flags\n");
#endif
                    for (;;)
                    {
                        fgets(buf,0x400,fl);
                        str3=PrepareString(buf);

                        if (str3[0] == '}')
                            break;
                        else if (strCMP(str3,"once_per_inst")==0)
                        {
                            pzl->flags |= FLAG_ONCE_PER_I;
                        }
                        else if (strCMP(str3,"do_me_now")==0)
                        {
                            pzl->flags |= FLAG_DO_ME_NOW;
                        }
                        else if (strCMP(str3,"disabled")==0)
                        {
                            pzl->flags |= FLAG_DISABLED;
                        }
                    }
                }


            }

            if ((pzl->flags & FLAG_ONCE_PER_I ))// || (pzl->flags & FLAG_DO_ME_NOW ))
                SetgVarInt(slot,0); /////////////////////////////////////


        }
        else if (strCMP(str,"control")==0 && control )
        {
            uint32_t    slot;
            char      ctrltp[100];
            memset(ctrltp,0,100);
            sscanf(str,"control:%d %s",&slot,ctrltp); //read slot number;



#ifdef FULLTRACE
            printf("Creating control:%d %s Creating object\n",slot,ctrltp);
#endif


            if (strCMP(ctrltp,"flat")==0)
            {
                Renderer = RENDER_FLAT;
#ifdef FULLTRACE
                printf("    Flat Rendere\n");
#endif
            }
            else if (strCMP(ctrltp,"pana")==0)
            {
                Renderer = RENDER_PANA;
                Location.X -= 320;
#ifdef FULLTRACE
                printf("    Panorama Rendere\n");
#endif
            }
            else if (strCMP(ctrltp,"push_toggle")==0)
            {

                SetgVarInt(slot,0);

                ctrlnode *ctnode = new (ctrlnode);
                pushnode *psh = new (pushnode);

                AddToMList(controlst,ctnode);
                ctnode->type = CTRL_PUSH;
                ctnode->node.push = psh;
                ctnode->slot = slot;
                //ctnode->enable=true;

                fgets(buf,0x400,fl);
                str2=PrepareString(buf);

                if (strCMP(str2,"warp_hotspot")==0)
                    psh->flat=false;
                else
                    psh->flat=true; //flat_hotspot

                str2=GetParams(str2);

                sscanf(str2,"%d, %d, %d, %d",&psh->x,&psh->y,&psh->w,&psh->h);

#ifdef FULLTRACE
                printf("    Push %d %d %d %d %d\n",psh->x,psh->y,psh->w,psh->h,psh->flat);
#endif

                fgets(buf,0x400,fl);
                str2=PrepareString(buf);

                str2=GetParams(str2); //cursor
                psh->cursor=CURSOR_IDLE;

                for (int i=0; i<NUM_CURSORS; i++)
                    if (strcasecmp(str2,CurNames[i]) == 0)
                    {
                        psh->cursor = i;
                        break;
                    }



            }
            else if (strCMP(ctrltp,"input")==0)
            {

            }
            else if (strCMP(ctrltp,"slot")==0)
            {
                ctrlnode *ctnode = new (ctrlnode);
                slotnode *slut = new (slotnode);

                AddToMList(controlst,ctnode);
                ctnode->type = CTRL_SLOT;
                ctnode->node.slot = slut;
                ctnode->slot = slot;
                //ctnode->enable=true;
                slut->srf = NULL;

                for (;;)
                {
                    fgets(buf,0x400,fl);
                    str2=PrepareString(buf);

                    if (strCMP(str2,"}")==0)
                        break;
                    else if (strCMP(str2,"rectangle")==0)
                    {
                        str2=GetParams(str2);
                        sscanf(str2,"%d %d %d %d",&slut->rectangle.x,&slut->rectangle.y,&slut->rectangle.w,&slut->rectangle.h);
                    }
                    else if (strCMP(str2,"hotspot")==0)
                    {
                        str2=GetParams(str2);
                        sscanf(str2,"%d %d %d %d",&slut->hotspot.x,&slut->hotspot.y,&slut->hotspot.w,&slut->hotspot.h);
                    }
                    else if (strCMP(str2,"cursor")==0)
                    {
                        str2=GetParams(str2);
                        for (int i=0; i<NUM_CURSORS; i++)
                            if (strcasecmp(str2,CurNames[i]) == 0)
                            {
                                slut->cursor = i;
                                break;
                            }
                    }
                    else if (strCMP(str2,"eligible_objects")==0)
                    {
                        str2=GetParams(str2);
                        int tmpobj=0;
                        int strl=strlen(str2);
                        for (int i=0; i < strl; i++)
                            if (str2[i] == ' ')
                                tmpobj++;

                        tmpobj++;

                        slut->eligable_cnt = tmpobj;
                        slut->eligible_objects = (int *)malloc (tmpobj * sizeof(int));
                        int i=0;
                        tmpobj=0;

                        for (;;)
                        {
                            if (i>=strl)
                                break;
                            if (str2[i] != ' ')
                            {
                                slut->eligible_objects[tmpobj] = atoi(str2 + i);
                                tmpobj++;

                                while (i<strl && str2[i] != ' ')
                                    i++;
                            }
                            i++;
                        }

                    }



                }

            }
        }
    }
    fclose(fl);
}

void DeletePuzzleList(pzllst *lst)
{
    StartMList(lst->_list);
    while (!eofMList(lst->_list))
    {
        puzzlenode *nod=(puzzlenode *)DataMList(lst->_list);


#ifdef FULLTRACE
        printf("Deleting Puzzle #%d\n",nod->slot);
#endif

        StartMList(nod->CritList);
        while (!eofMList(nod->CritList))
        {

            MList *criteries=(MList *)DataMList(nod->CritList);

            StartMList(criteries);
            while (!eofMList(criteries))
            {
                crit_node *critnd=(crit_node *)DataMList(criteries);
                delete critnd;
                NextMList(criteries);
            }
            DeleteMList(criteries);

            NextMList(nod->CritList);
        }
        DeleteMList(nod->CritList);


        StartMList(nod->ResList);
        while (!eofMList(nod->ResList))
        {
            func_node *fun=(func_node *)DataMList(nod->ResList);
            if (fun->param != NULL)
                free(fun->param);
            delete fun;
            NextMList(nod->ResList);
        }
        DeleteMList(nod->ResList);




        delete nod;

        NextMList(lst->_list);
    }

    DeleteMList(lst->_list);
    delete lst;
}

void DeleteControlList(MList *lst)
{
    pushnode *psh;
    slotnode *slt;

    StartMList(lst);
    while (!eofMList(lst))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(lst);


        switch (nod->type)
        {
        case CTRL_PUSH:
            psh=nod->node.push;
            delete psh;
            break;
        case CTRL_SLOT:
            slt=nod->node.slot;
            if (slt->srf)
                SDL_FreeSurface(slt->srf);
            delete slt;
            break;
        }


        delete nod;

        NextMList(lst);
    }

    DeleteMList(lst);
}



bool ProcessCriteries2(MList *lst)
{
    bool tmp=true;

    StartMList(lst);
    while (!eofMList(lst))
    {
        crit_node *critnd=(crit_node *)DataMList(lst);
#ifdef TRACE
        printf("        [%d] %d [%d] %d\n",critnd->slot1,critnd->oper,critnd->slot2,critnd->var2);
#endif
        int tmp1=GetgVarInt(critnd->slot1);
        int tmp2;

        if (critnd->var2)
            tmp2=GetgVarInt(critnd->slot2);
        else
            tmp2=critnd->slot2;

        switch (critnd->oper)
        {
        case CRIT_OP_EQU:
            tmp &= (tmp1 == tmp2);
            break;
        case CRIT_OP_GRE:
            tmp &= (tmp1 > tmp2);
            break;
        case CRIT_OP_LEA:
            tmp &= (tmp1 < tmp2);
            break;
        case CRIT_OP_NOT:
            tmp &= (tmp1 != tmp2);
            break;
        }

        if (!tmp) break;

        NextMList(lst);
    }
    return tmp;
}

bool ProcessCriteries(MList *lst)
{
    bool tmp=true;

    StartMList(lst);
    while (!eofMList(lst))
    {
        crit_node *critnd=(crit_node *)DataMList(lst);
#ifdef FULLTRACE
        printf("        [%d] %d [%d] %d\n",critnd->slot1,critnd->oper,critnd->slot2,critnd->var2);
#endif
        int tmp1=GetgVarInt(critnd->slot1);
        int tmp2;

        if (critnd->var2)
            tmp2=GetgVarInt(critnd->slot2);
        else
            tmp2=critnd->slot2;

        switch (critnd->oper)
        {
        case CRIT_OP_EQU:
            tmp &= (tmp1 == tmp2);
            break;
        case CRIT_OP_GRE:
            tmp &= (tmp1 > tmp2);
            break;
        case CRIT_OP_LEA:
            tmp &= (tmp1 < tmp2);
            break;
        case CRIT_OP_NOT:
            tmp &= (tmp1 != tmp2);
            break;
        }

        if (!tmp) break;

        NextMList(lst);
    }
    return tmp;
}


void ProcessControls(MList *ctrlst)
{

    pushnode *psh;//=(pushnode *) nod->node;
    slotnode *slut;
    bool mousein=false;

    LastMList(ctrlst);

    while (!eofMList(ctrlst))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(ctrlst);

#ifdef FULLTRACE
        printf("Control, slot:%d \n",nod->slot);
#endif


        if (!(Flags[nod->slot] & FLAG_DISABLED))  //(nod->enable)
            switch (nod->type)
            {
            case CTRL_PUSH:
#ifdef FULLTRACE
                printf("Push_toggle\n");
#endif

                psh = nod->node.push;
                mousein = false;

                if (Renderer == RENDER_FLAT)
                {


                    if (    (psh->x         <= MouseX())    &&\
                            (psh->x+psh->w   >= MouseX())    &&\
                            (psh->y+GAME_Y          <= MouseY())    &&\
                            (psh->y+GAME_Y+psh->h   >= MouseY()))
                        mousein = true;

                }
                else if (Renderer == RENDER_PANA)
                {
                    if (MouseX()>=20 && MouseX()<=620)
                    {

                        if (    (psh->x-Location.X          <= MouseX())    &&\
                                (psh->x-Location.X+psh->w   >= MouseX())    &&\
                                (psh->y+GAME_Y          <= MouseY())    &&\
                                (psh->y+GAME_Y+psh->h   >= MouseY()))
                            mousein = true;

                        if (Location.X > scrbuf->w - screen->w)
                        {
                            if (    (psh->x+scrbuf->w-Location.X  <= MouseX())    &&\
                                    (psh->x+scrbuf->w-Location.X+psh->w   >= MouseX())    &&\
                                    (psh->y+GAME_Y          <= MouseY())    &&\
                                    (psh->y+GAME_Y+psh->h   >= MouseY()))
                                mousein = true;
                        }

                    }
                    else if (MouseX()<20)
                        cur=CurDefault[CURSOR_LEFT];
                    else if (MouseX()>620)
                        cur=CurDefault[CURSOR_RIGH];
                }


                if (mousein)
                {
                    if (cur == CurDefault[CURSOR_IDLE])
                        cur=CurDefault[psh->cursor];

                    if (MouseUp(SDL_BUTTON_LEFT))
                    {
#ifdef TRACE
                        printf("Pushed #%d\n",nod->slot);
#endif
                        SetgVarInt(nod->slot,1);

                        FlushMouseBtn(SDL_BUTTON_LEFT);

                    }

                }


                break;
            case CTRL_SLOT:

                slut = nod->node.slot;
                mousein = false;

                if (    (slut->hotspot.x          <= MouseX())    &&\
                        (slut->hotspot.w          >= MouseX())    &&\
                        (slut->hotspot.y+GAME_Y   <= MouseY())    &&\
                        (GAME_Y+slut->hotspot.h   >= MouseY()))
                    mousein = true;

                if (mousein)
                {

                    if (GetgVarInt(nod->slot)!=0)
                        if (cur == CurDefault[CURSOR_IDLE])
                            cur=CurDefault[slut->cursor];

                    if (MouseUp(SDL_BUTTON_LEFT))
                    {
                        if (GetgVarInt(SLOT_INVENTORY_MOUSE)==0)
                        {
                            SetgVarInt(SLOT_INVENTORY_MOUSE,GetgVarInt(nod->slot));
                            SetgVarInt(nod->slot,0);

                        }
                        else if (Eligeblity(GetgVarInt(SLOT_INVENTORY_MOUSE),slut))
                        {
                            int te=GetgVarInt(nod->slot);
                            SetgVarInt(nod->slot,GetgVarInt(SLOT_INVENTORY_MOUSE));
                            SetgVarInt(SLOT_INVENTORY_MOUSE,te);
                            SDL_FreeSurface(slut->srf);
                            slut->srf=NULL;
                        }
#ifdef TRACE
                        printf("Pushed\n");
                        printf("Slot #%d to 1\n",nod->slot);
                        FlushMouseBtn(SDL_BUTTON_LEFT);
#endif

                    }

                }


                break;
            }

        PrevMList(ctrlst);
    }
}

void DrawSlots()
{
    StartMList(ctrl);

    while (!eofMList(ctrl))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(ctrl);
        if (nod->type == CTRL_SLOT)
        {
            slotnode *slut = nod->node.slot;

            //rectangleRGBA(screen,slut->hotspot.x,slut->hotspot.y+GAME_Y,slut->hotspot.w,slut->hotspot.h+GAME_Y,255,0,0,255);


            int tmp1 = GetgVarInt(nod->slot);
            //printf("%d %d\n",nod->slot,tmp1);
            bool in = Eligeblity(tmp1,slut);

            if (in && tmp1!=GetgVarInt(9))
            {
                if (slut->srf==NULL)
                {
                    char bff[16];
                    sprintf(bff,"G0ZYU%2.2x1.tga",tmp1);
                    slut->srf=IMG_Load(GetFilePath(bff));
                    ConvertImage(&slut->srf);
                    SDL_SetColorKey(slut->srf,SDL_SRCCOLORKEY ,SDL_MapRGB(slut->srf->format,0,0,0));
                }

                DrawImage(slut->srf,    Location.X +slut->rectangle.x  ,  GAME_Y + slut->rectangle.y);
            }
            else
            {
                if (slut->srf != NULL)
                {
                    SDL_FreeSurface(slut->srf);
                    slut->srf=NULL;
                }

            }




        }
        NextMList(ctrl);
    }
}


void ProcessTimers()
{
    StartMList(timers);

    while (!eofMList(timers))
    {
        timernode *nod=(timernode *)DataMList(timers);

        if (nod)
            if (nod->time<GetTickCount())
            {
                SetgVarInt(nod->slot,2);
#ifdef TRACE
                printf ("Timer #%d End's\n",nod->slot);
#endif
                delete nod;
                DeleteCurrent(timers);

            }

        NextMList(timers);
    }
}



void InitGraphics()
{
    screen=InitGraphicAndSound(640,480,32);

    for (int i=0; i<18; i++)
    {
        CurDefault[i]=new(Cursor);
        LoadCursor(CurFiles[i],CurDefault[i]);
    }

    cur=CurDefault[CURSOR_IDLE];

    //cur=new(Cursor);//"g0gac011.zcr"));
    //LoadCursor("g0gac011.zcr",cur);

    SDL_ShowCursor(SDL_DISABLE);

}




void FlatRender()
{
    SDL_FillRect(screen,0,0);
    DrawImage(scrbuf,0,GAME_Y);
}

void PanaRender()
{
    if (MouseX() > 620)
        Location.X +=30;

    if (MouseX() < 20)
        Location.X -=30;

    if (Location.X >= scrbuf->w)
        Location.X %= scrbuf->w;
    if (Location.X < 0)
        Location.X = scrbuf->w + Location.X;

    SDL_FillRect(screen,0,0);
    DrawImage(scrbuf,-Location.X,GAME_Y);
    if (Location.X > scrbuf->w - screen->w)
        DrawImage(scrbuf,scrbuf->w-Location.X,GAME_Y);
}



void RenderFunc()
{

    if (Renderer == RENDER_FLAT)
        FlatRender();
    else
        PanaRender();


    DrawSlots();

    ProcessCursor();

    //SDL_Flip(screen);
}


void LoadCursor(char *file, Cursor *cur)
{
    char tmp[64];
    char *tmp2;
    strcpy(tmp,file);
    int len=strlen(tmp);
    tmp[len-1]='g';
    tmp[len-2]='n';
    tmp[len-3]='p';

    tmp2=GetExactFilePath(tmp);
    if (tmp2==NULL)
        return;

    cur->img = IMG_Load(tmp2);
    if (cur->img)
    {
        SDL_Surface *z=SDL_ConvertSurface(cur->img,screen->format,0);
        SDL_FreeSurface(cur->img);
        cur->img=z;
        SDL_SetColorKey(cur->img,SDL_SRCCOLORKEY ,SDL_MapRGB(cur->img->format,0,0,0));
    }
    tmp[len-2]='o';
    tmp[len-1]='i';
    tmp[len]='n';
    tmp[len+1]='t';
    tmp[len+2]=0x0;

    tmp2=GetExactFilePath(tmp);
    if (tmp2==NULL)
        return;

    FILE *f=fopen(tmp2,"rb");
    fread(&cur->ox,1,2,f);
    fread(&cur->oy,1,2,f);
    fclose(f);
}

void DrawCursor(Cursor *cur, int x, int y)
{
    DrawImage(cur->img,x-cur->ox,y-cur->oy);
}

void DeleteCursor(Cursor *cur)
{
    SDL_FreeSurface(cur->img);
    delete cur;
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

void ChangeLocation(uint8_t w, uint8_t r, uint16_t v, int32_t X) // world / room / view
{
    Locate temp;
    if (w=='0' && r=='0' && v=='00')
    {
        PrevCount--;
        memcpy(&temp,&Previos_Locate[PrevCount],sizeof(temp));
    }
    else
    {
        // If dissolve was called
        if (Current_Locate.World != 0 &&\
                Current_Locate.Room  != 0 &&\
                Current_Locate.View  != 0 )
        {
            if (PrevCount<PREV_STACK_MAX)
            {
                memcpy(&Previos_Locate[PrevCount],&Current_Locate,sizeof(Current_Locate));
                PrevCount++;
            }
            else
            {
                for (int i=0; i<PREV_STACK_MAX-1; i++)
                    memcpy(&Previos_Locate[i],&Previos_Locate[i+1],sizeof(Previos_Locate[i]));

                memcpy(&Previos_Locate[PrevCount-1],&Current_Locate,sizeof(Current_Locate));
                //PrevCount++;
            }
        }
        temp.World =w;
        temp.Room  =r;
        temp.View  =v;
        temp.X     =X;
        for (int i=PrevCount-1; i>=0; i--)
        {
            if (Previos_Locate[i].World==w &&\
                    Previos_Locate[i].Room ==r &&\
                    Previos_Locate[i].View ==v /*&&\
                    Previos_Locate[i].X    ==X */ )
            {
                PrevCount=i;
                break;
            }
            //Previos_Locate[i].==w &&\)
        }
    }

    ////////State box routine////////////
    for (int i=0; i<VAR_SLOTS_MAX; i++)
    {
        if (StateBox[i] != NULL)
            delete StateBox[i];
    }

    memset(StateBox,0,VAR_SLOTS_MAX * sizeof(StateBoxEnt *));
    StateBoxStkSz=0;
    ////////---State box routine---//////////////

    memset (&Current_Locate,0,sizeof(Current_Locate));

    char buf[32];
    char tm[5];

    Location.X=temp.X;

    RenderDelay = 2;
    View_start_Loops = 1;


//    ClearUsedOnOPIPuzz(view); //Really needed??
//    ClearUsedOnOPIPuzz(room); //Really needed??
//    ClearUsedOnOPIPuzz(world); //Really needed??

    if (temp.View != Location.View || temp.Room != Location.Room || temp.World != Location.World)
    {
        if (view)
        {
            DeleteTimerByOwner(timers,view);
            DeleteLoopedWavsByOwner(wavs,view);

            DeletePuzzleList(view);
            DeleteControlList(ctrl);
            DeleteAnims(anims);
        }

        tm[0]=temp.World;
        tm[1]=temp.Room;
        tm[2]=(temp.View >> 8) & 0xFF;
        tm[3]=temp.View & 0xFF;
        tm[4]=0;
        sprintf(buf,"%s.scr",tm);
        view=CreatePzlLst();
        ctrl=CreateMList();
        anims=CreateMList();
        LoadScriptFile(view,GetExactFilePath(buf),true,ctrl);
        Location.View=temp.View;


    }

    if (temp.Room != Location.Room || temp.World != Location.World)
    {
        if (room)
        {
            DeleteLoopedWavsByOwner(wavs,room);

            DeletePuzzleList(room);
        }


        tm[0]=temp.World;
        tm[1]=temp.Room;
        tm[2]=0;
        sprintf(buf,"%s.scr",tm);
        room=CreatePzlLst();
        LoadScriptFile(room,GetExactFilePath(buf),false,NULL);
        Location.Room=temp.Room;
    }

    if (temp.World != Location.World)
    {
        if (world)
        {
            DeleteLoopedWavsByOwner(wavs,world);

            DeletePuzzleList(world);
        }
        tm[0]=temp.World;
        tm[1]=0;
        sprintf(buf,"%s.scr",tm);
        world=CreatePzlLst();
        LoadScriptFile(world,GetExactFilePath(buf),false,NULL);
        Location.World=temp.World;
    }

    //
    FillStateBoxFromList(uni);
    FillStateBoxFromList(view);
    FillStateBoxFromList(room);
    FillStateBoxFromList(world);

    // FillStateBoxFromList(room);
    //FillStateBoxFromList(world);
    //FillStateBoxFromList(uni);

}

void InitGameLoop()
{
    uni = CreatePzlLst();
    LoadScriptFile(uni,GetExactFilePath("universe.scr"),false,NULL);

    ChangeLocation('g','a','ry',0);
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

bool examine_criterias(puzzlenode *nod)
{
    StartMList(nod->CritList);

    while (!eofMList(nod->CritList))
    {
        MList *criteries=(MList *)DataMList(nod->CritList);

        if (ProcessCriteries(criteries))
            return true;

        NextMList(nod->CritList);
    }
    return false;
}

void execute_puzzle_node(puzzlenode *nod)
{
    StartMList(nod->ResList);
    while (!eofMList(nod->ResList))
    {
        func_node *fun=(func_node *)DataMList(nod->ResList);
        fun->func(fun->param, nod->owner);

        NextMList(nod->ResList);
    }
}

void puzzle_try_exec(puzzlenode *pzlnod) //, pzllst *owner)
{
    if (pzlnod->flags & FLAG_DISABLED)
        return;

    if (GetgVarInt(pzlnod->slot) != 1)
    {
        if (pzlnod->owner->exec_times == 0)
            if (! pzlnod->flags & FLAG_DO_ME_NOW)
                return;
        if (examine_criterias(pzlnod))
        {

#ifdef TRACE
printf("Puzzle: %d (%s) \n",pzlnod->slot,ReturnListName(pzlnod->owner));
#endif

            SetgVarInt(pzlnod->slot, 1);

            execute_puzzle_node(pzlnod);
        }
    }
}

void exec_puzzle_list(pzllst *lst)
{
    if (lst->exec_times<2)
    {
        StartMList(lst->_list);
        while (!eofMList(lst->_list))
        {
            puzzle_try_exec( (puzzlenode *)DataMList(lst->_list) );// , lst);
            NextMList(lst->_list);
        }
        lst->exec_times++;
    }
    else
    {
        int i=0,j=lst->stksize;

        while ( i < j)
        {
            puzzle_try_exec( lst->stack[i] );
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



void GameLoop()
{

    //SetgVarInt(067,1);

    //printf("%d,%d,%d\n",GetgVarInt(14000),GetgVarInt(1001),GetgVarInt(5180));

    cur=CurDefault[CURSOR_IDLE];

    SetgVarInt(18,0);
    SetgVarInt(10,0);
    if (MouseUp(SDL_BUTTON_RIGHT))
        SetgVarInt(18,1);
    if (MouseHit(SDL_BUTTON_LEFT))
        SetgVarInt(10,1);


    ProcessTimers();
    ProcessWavs();
    ProcessAnims();



    //if (View_start_Loops>0)
    //    ProcessTriggers(uni);

    //ProcessStateBoxStack();
    //ProcessTriggers(world);
    //ProcessTriggers(room);
    //if (View_start_Loops>0)
    //{
    //   ProcessTriggers(view);
    //    View_start_Loops--;
    //}

    exec_puzzle_list(world);
    exec_puzzle_list(room);
    exec_puzzle_list(view);
    exec_puzzle_list(uni);



    ProcessControls(ctrl);


    if (RenderDelay<0)
        RenderFunc();
    else
        RenderDelay--;

    if (NeedToLoadScript)
    {
        NeedToLoadScript=false;
        ChangeLocation(Need_Locate.World,Need_Locate.Room,Need_Locate.View,Need_Locate.X);
    }


    char savefile[16];

    sprintf(savefile,"Save%d.sav",SaveSlot);

    if (KeyHit(SDLK_F6))
        SaveSlot--;
    if (KeyHit(SDLK_F7))
        SaveSlot++;

    if (SaveSlot > 40)
        SaveSlot = 0;
    if (SaveSlot < 0)
        SaveSlot = 40;

    if (KeyHit(SDLK_F5))
        SaveGame(savefile);
    if (KeyHit(SDLK_F8))
        LoadGame(savefile);

    stringColor(screen,0,470,savefile,0xFFFFFFFF);

    SDL_Flip(screen);
}




void ProcessAnims()
{
    StartMList(anims);

    while (!eofMList(anims))
    {
        animnode *nod=(animnode *)DataMList(anims);

        if (nod)
            if (nod->anim)
                if (nod->nexttick<millisec())
                    //if (GetTick())
                {
                    if (nod->vid)
                    {
                        anim_avi *anm=(anim_avi *)nod->anim;
                        //if (!anm->pld)
                        // {
                        SMPEG_renderFrame(anm->mpg,nod->CurFr+1);
                        //  }

                        /*if (anm->loop == true && SMPEG_status(anm->mpg) == SMPEG_STOPPED)
                            {
                                SMPEG_rewind(anm->mpg);
                                SMPEG_play(anm->mpg);
                            }*/

                        DrawImageToSurf(anm->img,nod->x,nod->y,scrbuf);
                    }
                    else
                        DrawAnimImageToSurf((anim_surf *)nod->anim,nod->x,nod->y,nod->CurFr,scrbuf);
                    nod->CurFr++;

                    if (nod->CurFr > nod->end)
                    {
                        nod->loops++;

                        if (nod->loops<nod->loopcnt || nod->loopcnt == 0)
                        {
                            nod->CurFr=nod->start;
                            if (nod->vid)
                            {
                                //nod->nexttick=millisec() + 1.0/(((anim_avi *)nod->anim)->inf.current_fps) * 1000.0;
                                nod->nexttick=millisec() + (1.0/30.0) * 1000.0;
                            }
                            else
                                nod->nexttick=millisec()+((anim_surf *)nod->anim)->info.time;
                        }

                        else
                        {
#ifdef TRACE
                            printf ("Animplay #%d End's\n",nod->slot);
#endif
                            if (nod->slot != 0)
                                SetgVarInt(nod->slot,2);

                            if (nod->vid)
                            {
                                SDL_FreeSurface(((anim_avi *)nod->anim)->img);
                                SMPEG_stop(((anim_avi *)nod->anim)->mpg);
                                SMPEG_delete(((anim_avi *)nod->anim)->mpg);
                            }
                            else
                                FreeAnimImage((anim_surf *)nod->anim);
                            delete nod;
                            DeleteCurrent(anims);
                        }
                    }
                    else
                    {
                        if (nod->vid)
                        {
                            //nod->nexttick=millisec() + 1.0/(((anim_avi *)nod->anim)->inf.current_fps) * 1000.0;
                            nod->nexttick=millisec() + (1.0/30.0) * 1000.0;
                        }
                        else
                            nod->nexttick=millisec()+((anim_surf *)nod->anim)->info.time;
                    }

                }

        NextMList(anims);
    }
}

void DeleteAnimNod(animnode *nod)
{
    if (nod->vid)
    {
        SDL_FreeSurface(((anim_avi *)nod->anim)->img);
        SMPEG_stop(((anim_avi *)nod->anim)->mpg);
        SMPEG_delete(((anim_avi *)nod->anim)->mpg);
    }
    else
        FreeAnimImage((anim_surf *)nod->anim);

    SetgVarInt(nod->slot,2);

    delete nod;
}

void DeleteAnims(MList *lst)
{
    StartMList(lst);
    while (!eofMList(lst))
    {
        animnode *nod=(animnode *)DataMList(lst);

        DeleteAnimNod(nod);

        NextMList(lst);
    }

    DeleteMList(lst);
}

void DeleteWavs(MList *lst)
{
    Mix_HaltChannel(-1);

    StartMList(lst);
    while (!eofMList(lst))
    {
        musicnode *nod=(musicnode *)DataMList(lst);
        Mix_FreeChunk(nod->chunk);
        UnlockChan(nod->chn);
        delete nod;


        NextMList(lst);
    }

    DeleteMList(lst);
}

void DeleteLoopedWavsByOwner(MList *lst,pzllst *owner)
{
    StartMList(lst);
    while (!eofMList(lst))
    {
        musicnode *nod=(musicnode *)DataMList(lst);
        if (nod->owner == owner && nod->looped)
        {
            Mix_HaltChannel(nod->chn);
            Mix_FreeChunk(nod->chunk);
            UnlockChan(nod->chn);
            if (nod->slot != 0)
                SetgVarInt(nod->slot,2);
            delete nod;
            DeleteCurrent(lst);
        }
        NextMList(lst);
    }
}

void DeleteTimers(MList *lst)
{

    StartMList(lst);
    while (!eofMList(lst))
    {
        timernode *nod=(timernode *)DataMList(lst);
        delete nod;

        NextMList(lst);
    }

    DeleteMList(lst);
}

void DeleteTimerByOwner(MList *lst,pzllst *owner)
{
    StartMList(lst);
    while (!eofMList(lst))
    {
        timernode *nod=(timernode *)DataMList(lst);
        if (nod->owner == owner)
        {
            if (nod->slot != 0)
            {
                SetgVarInt(nod->slot, nod->time - GetTickCount());
                //printf("deleted timer %d, ost %d",nod->slot,  nod->time - GetTickCount());
            }
            //printf("deleted timer %d, ost %d %d \n",nod->slot,  nod->time - GetTickCount(),nod->ownslot);
            delete nod;

            DeleteCurrent(lst);
        }

        NextMList(lst);
    }
}

void ProcessCursor()
{
    if (GetgVarInt(SLOT_INVENTORY_MOUSE) != 0)
    {
        if (GetgVarInt(SLOT_INVENTORY_MOUSE) != current_obj_cur)
        {
            if (objcur[0]!=NULL)
                DeleteCursor(objcur[0]);
            if (objcur[1]!=NULL)
                DeleteCursor(objcur[1]);

            objcur[0]=new(Cursor);
            objcur[1]=new(Cursor);

            current_obj_cur=GetgVarInt(SLOT_INVENTORY_MOUSE);

            char buf[16];
            sprintf(buf,"g0bac%2.2x1.tga",current_obj_cur);
            LoadCursor(buf,objcur[0]);
            sprintf(buf,"g0bbc%2.2x1.tga",current_obj_cur);
            LoadCursor(buf,objcur[1]);
        }
        if (cur == CurDefault[CURSOR_ACTIVE] || cur == CurDefault[CURSOR_HANDPU] || cur == CurDefault[CURSOR_IDLE])
        {
            if (cur == CurDefault[CURSOR_ACTIVE] || cur == CurDefault[CURSOR_HANDPU])
                cur=objcur[1];
            else
                cur=objcur[0];
        }
    }
    DrawCursor(cur,MouseX(),MouseY());
}



void ProcessWavs()
{
    StartMList(wavs);

    while (!eofMList(wavs))
    {
        musicnode *mnod = (musicnode *) DataMList(wavs);

        if (!Mix_Playing(mnod->chn))
        {
            Mix_FreeChunk(mnod->chunk);
            SetgVarInt(mnod->slot,2);
            UnlockChan(mnod->chn);
            delete mnod;
            DeleteCurrent(wavs);
        }

        NextMList(wavs);
    }
}

void DeleteAllPreload()
{
    StartMList(preload);

    while (!eofMList(preload))
    {
        struct_Preload *pre = (struct_Preload *) DataMList(preload);

        delete pre->fil;
        delete pre;

        NextMList(preload);
    }
    DeleteMList(preload);
}


