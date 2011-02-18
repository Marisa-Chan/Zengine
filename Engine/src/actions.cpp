
#include "System.h"

extern SDL_Surface *scrbuf;
extern SDL_Surface *screen;
extern bool NeedToLoadScript;
extern Locate Need_Locate;
extern int32_t RenderDelay;
extern int32_t View_start_Loops;
extern int GAME_Y;

int action_set_screen(char *params, pzllst *owner)
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
    }

    return ACTION_NORMAL;
}

int action_set_partial_screen(char *params, pzllst *owner)
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
    return ACTION_NORMAL;
}

int action_assign(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:assign(%s)\n",params);
#endif
    char tmp1[16],tmp2[16];
    sscanf(params,"%s %s",tmp1,tmp2);

    SetgVarInt(GetIntVal(tmp1),GetIntVal(tmp2));
    return ACTION_NORMAL;
}

int action_timer(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:timer(%s)\n",params);
#endif
    int tmp1;
    char tmp2[16];
    char *s;
    sscanf(params,"%d %s",&tmp1,tmp2);

    if (ScrSys_SlotIsOwned(tmp1))
    {
        //SetgVarInt(tmp1,1);
//#ifdef TRACE
//        printf("        owned %d\n",GetIntVal(tmp2));
//#endif
        return ACTION_NORMAL;
    }


    timernode *nod = new (timernode);
    nod->slot = tmp1;

    nod->owner = owner;

    s=PrepareString(tmp2);

    nod->time = GetTickCount() + GetIntVal(s);
//#ifdef TRACE
//    printf(" %d\n",GetIntVal(s));
//#endif

    tmr_AddToTimerList(nod);

    SetgVarInt(tmp1,1);

    return ACTION_NORMAL;
}

int action_change_location(char *params, pzllst *owner)
{

    //need reverse from 0x00409891

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
    Need_Locate.View1=toupper(tmp3[0]);
    Need_Locate.View2=toupper(tmp3[1]);
    Need_Locate.X=GetIntVal(tmp4);


    if (Need_Locate.World == '0')
    {
        if (GetgVarInt(3) == 'G' &&
            GetgVarInt(4) == 'J')
        {
            Need_Locate.World = GetgVarInt(45);
            Need_Locate.Room  = GetgVarInt(46);
            Need_Locate.View1 = GetgVarInt(47);
            Need_Locate.View2 = GetgVarInt(48);
            Need_Locate.X     = GetgVarInt(49);
        }
        else
        {
            Need_Locate.World = GetgVarInt(40);
            Need_Locate.Room  = GetgVarInt(41);
            Need_Locate.View1 = GetgVarInt(42);
            Need_Locate.View2 = GetgVarInt(43);
            Need_Locate.X     = GetgVarInt(44);
        }
    }


    //depricated
    RenderDelay = 2;
    View_start_Loops = 1;

    return ACTION_BREAK;
}

int action_dissolve(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:dissolve()\n");
#endif

    return ACTION_NORMAL;
}

int action_disable_control(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:disable_control(%s)\n",params);
#endif

    int slot = GetIntVal(params);

    ScrSys_SetFlag(slot,FLAG_DISABLED);

    return ACTION_NORMAL;
}

int action_enable_control(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:enable_control(%s)\n",params);
#endif

    int slot = GetIntVal(params);

    ScrSys_SetFlag(slot, 0);

    return ACTION_NORMAL;
}

int action_add(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:add(%s)\n",params);
#endif

    char slot[16],number[16];
    int tmp;
    sscanf(params,"%s %s",slot, number);

    tmp = GetIntVal(slot);
    SetgVarInt(tmp, GetgVarInt(tmp) + GetIntVal(number));

    return ACTION_NORMAL;
}

int action_debug(char *params, pzllst *owner)
{


    char txt[256],number[16];
    int tmp;
    sscanf(params,"%s %s",txt, number);

    tmp = GetIntVal(number);

#ifdef TRACE
    printf("DEBUG :%s\t: %d \n",txt,tmp);
#endif

    return ACTION_NORMAL;
}

int action_random(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:random(%s)\n",params);
#endif

    int slot,number;
    char chars[16];
    sscanf(params,"%d %s",&slot, chars);
    number=GetIntVal(chars);

    SetgVarInt(slot, rand() % (number+1) );

    return ACTION_NORMAL;
}



int action_streamvideo(char *params, pzllst *owner)
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

    return ACTION_NORMAL;
}

int action_animplay(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:animplay(%s)\n",params);
#endif

    uint32_t slot;
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

    MList *anims = *Getanims();
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

    return ACTION_NORMAL;
}

int action_music(char *params, pzllst *owner)
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

    tmr_AddToTimerList(nod);

    SetgVarInt(slot, 1);

    return ACTION_NORMAL;
}

int action_universe_music(char *params, pzllst *owner)
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

    if (ScrSys_SlotIsOwned(slot))
        return ACTION_NORMAL;



    musicnode *nod = new (musicnode);
    nod->slot = slot;

    nod->chunk = Mix_LoadWAV(GetFilePath(file));

    nod->chn = GetFreeChannel();
    if (nod->chn == -1)
    {
        printf("ERROR, NO CHANNELS!\n");
        Mix_FreeChunk(nod->chunk);
        delete nod;
        return ACTION_NORMAL;
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

    Mix_Volume( nod->chn, GetLogVol(GetIntVal(vol)) );
//    printf("chan %d vol %d\n",nod->chn,GetIntVal(vol));

    nod->owner = owner;

    snd_AddToWavsList(nod);

    SetgVarInt(slot, 1);

    return ACTION_NORMAL;
}


int action_syncsound(char *params, pzllst *owner)
{
    printf("PlayPreload \n");

    return ACTION_NORMAL;
}

int action_animpreload(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:animpreload(%s)\n",params);
#endif
    MList *preload = *Getpreload();
    if (!preload)
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

    return ACTION_NORMAL;
}

int action_playpreload(char *params, pzllst *owner)
{
    char sl[16];
    uint32_t slot;
    int x,y,w,h,start,end,loop,sll;
    sscanf(params,"%d %s %d %d %d %d %d %d %d",&sll,sl,&x,&y,&w,&h,&start,&end,&loop);

#ifdef TRACE
    printf("        action:playpreload(%s)\n",params);
#endif

    MList *preload = *Getpreload();
    if (!preload)
        return ACTION_NORMAL;

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
        return ACTION_NORMAL;

    if (sll == 0)
        sll = slot;

    sprintf(buff,"%d %s %d %d %d %d %d %d %d %d %d %d %d",sll ,pre->fil,\
            x, y, w, h, start, end, loop, 0, 0, 0, 0);

    action_animplay(buff,owner);
    //SetgVarInt(GetIntVal(chars),2);

    return ACTION_NORMAL;
}

int action_ttytext(char *params, pzllst *owner)
{
    char chars[16];
    sscanf(params,"%s",chars);

    timernode *nod = new (timernode);
    nod->slot = GetIntVal(chars);

    nod->owner = owner;


    nod->time = GetTickCount() + 15;

    tmr_AddToTimerList(nod);

    SetgVarInt(GetIntVal(chars), 1);

    return ACTION_NORMAL;
}

int action_kill(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:kill(%s)\n",params);
#endif

    int slot;
    char chars[16];
    sscanf(params,"%s",chars);

    if (strcasecmp(chars,"\"all\"")==0)
    {
        MList *anims = *Getanims();
        DeleteAnims(anims);
        *Getanims() = CreateMList();

        MList *wavs = snd_GetWavsList();
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

        MList *timers = tmr_GetTimerList();
        StartMList(timers);
        while(!eofMList(timers))
        {
            musicnode *nod = (musicnode *)DataMList(timers);

            delete nod;
            DeleteCurrent(timers);
            SetgVarInt(slot, 2);
            NextMList(timers);
        }

        return ACTION_NORMAL;
    }

    if (strcasecmp(chars,"\"audio\"")==0)
    {
        MList *wavs = snd_GetWavsList();
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
                return ACTION_NORMAL;
            }

            NextMList(wavs);
        }

        return ACTION_NORMAL;
    }

    slot = GetIntVal(chars);

    MList *anims = *Getanims();
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
            return ACTION_NORMAL;
        }

        NextMList(anims);
    }

    MList *wavs = snd_GetWavsList();
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
            return ACTION_NORMAL;
        }

        NextMList(wavs);
    }

    MList *timers = tmr_GetTimerList();
    StartMList(timers);
    while(!eofMList(timers))
    {
        musicnode *nod = (musicnode *)DataMList(timers);
        if (nod->slot == slot)
        {
            delete nod;
            DeleteCurrent(timers);
            SetgVarInt(slot, 2);
            return ACTION_NORMAL;
        }

        NextMList(timers);
    }

#ifdef TRACE
    printf("Nothing to kill %d\n",slot);
#endif

    return ACTION_NORMAL;
}


int action_stop(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:stop(%s)\n",params);
#endif

    int slot;
    char chars[16];
    sscanf(params,"%s",chars);
    slot = GetIntVal(chars);

    MList *timers = tmr_GetTimerList();
    StartMList(timers);
    while(!eofMList(timers))
    {
        timernode *nod = (timernode *)DataMList(timers);
        if (nod->slot == slot)
        {
            delete nod;
            DeleteCurrent(timers);
            SetgVarInt(slot, 2);
            return ACTION_NORMAL;
        }

        NextMList(timers);
    }

    MList *anims = *Getanims();
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
            return ACTION_NORMAL;
        }

        NextMList(anims);
    }

    MList *wavs = snd_GetWavsList();
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
            return ACTION_NORMAL;
        }

        NextMList(wavs);
    }

    printf("Nothing to stop %d\n",slot);

    return ACTION_NORMAL;
}


int action_inventory(char *params, pzllst *owner)
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
            if (GetgVarInt(i) == item)
            {
                SetgVarInt(i,0);
            }
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

        if (GetgVarInt(SLOT_INVENTORY_MOUSE) == item)
        {
            SetgVarInt(SLOT_INVENTORY_MOUSE,0);
        }
        else
        {
            for (i=SLOT_START_SLOT; i<= SLOT_END_SLOT ; i++)
                if (GetgVarInt(i)==item)
                {
                    SetgVarInt(i,0);
                    break;
                }
        }

    }

    if (strcasecmp(cmd,"dropi")==0)
    {
        SetgVarInt(item,0);
    }

    return ACTION_NORMAL;
}

int action_crossfade(char *params, pzllst *owner)
{
#ifdef TRACE
    printf("        action:crossfade(%s)\n",params);
#endif

    uint32_t item,item2;
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

    MList *wavs = snd_GetWavsList();
    StartMList(wavs);
    while(!eofMList(wavs))
    {
        musicnode *nod = (musicnode *)DataMList(wavs);

        if (nod->slot == item)
            Mix_Volume(nod->chn , GetLogVol(GetIntVal(toVol)));

        if (nod->slot == item2)
            Mix_Volume(nod->chn , GetLogVol(GetIntVal(toVol2)));

        NextMList(wavs);
    }


    return ACTION_NORMAL;
}
