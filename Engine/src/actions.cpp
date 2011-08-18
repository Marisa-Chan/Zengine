#include "System.h"

int action_set_screen(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:set_screen  %s\n",params);
#endif
    char *fil = GetFilePath(params);

    if (fil != NULL)
        Rend_LoadGamescr(fil);
    else
        printf("Can't find %s, screen load failed\n",params);

    return ACTION_NORMAL;
}

int action_set_partial_screen(char *params, int aSlot , pzllst *owner)
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

    SDL_Surface *tmp = NULL;

    if (tmp2 != -1)
    {
        int r,g,b;
        b=FiveBitToEightBitLookupTable[((tmp2 >> 10 ) & 0x1F)];
        g=FiveBitToEightBitLookupTable[((tmp2 >> 5 ) & 0x1F)];
        r=FiveBitToEightBitLookupTable[(tmp2 & 0x1F)];
#ifdef TRACE
        printf("        action:set_partial_screen Color Key (%x %x %x)\n",r,g,b);
#endif
        tmp=LoadConvertImg(GetFilePath(file),Rend_MapScreenRGB(r,g,b));
    }
    else
        tmp=LoadConvertImg(GetFilePath(file));

    if (!tmp)
        printf("ERROR:  IMG_Load(%s): %s\n\n",params, IMG_GetError());
    else
    {

        Rend_DrawImageToGamescr(tmp,x,y);

        SDL_FreeSurface(tmp);
    }
    return ACTION_NORMAL;
}

int action_assign(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:assign(%s)\n",params);
#endif
    char tmp1[16],tmp2[16];
    sscanf(params,"%s %s",tmp1,tmp2);

    SetgVarInt(GetIntVal(tmp1),GetIntVal(tmp2));
    return ACTION_NORMAL;
}

int action_timer(char *params, int aSlot, pzllst *owner)
{
#ifdef TRACE
    printf("        action:timer:%d(%s)\n",aSlot,params);
#endif
    char tmp2[16];
    char *s;
    sscanf(params,"%s",tmp2);

    if (getGNode(aSlot) != NULL)
    {
        //SetgVarInt(tmp1,1);
//
//#ifdef TRACE
//       printf("        owned %d\n",GetIntVal(tmp2));
//#endif
        return ACTION_NORMAL;
    }

    struct_action_res *nod = new (struct_action_res);
    nod->nodes.node_timer = 0;

    nod->slot = aSlot;
    nod->owner = owner;
    nod->node_type = NODE_TYPE_TIMER;
    nod->need_delete     = false;

    setGNode(aSlot, nod);

    s = PrepareString(tmp2);
    nod->nodes.node_timer = GetIntVal(s) * 100;
//#ifdef TRACE
//    printf(" %d\n",GetIntVal(s));
//#endif

    ScrSys_AddToActResList(nod);

    SetgVarInt(aSlot,1);

    return ACTION_NORMAL;
}

int action_change_location(char *params, int aSlot , pzllst *owner)
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

    SetNeedLocate(tolower(tmp[0]),tolower(tmp2[0]),tolower(tmp3[0]), tolower(tmp3[1]), GetIntVal(tmp4));

    Rend_SetDelay(2);

    return ACTION_NORMAL;
}

int action_dissolve(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:dissolve()\n");
#endif
    SDL_Surface *surf = Rend_GetGameScreen();
    SDL_FillRect(surf,NULL,Rend_MapScreenRGB(0,0,0));
    return ACTION_NORMAL;
}

int action_disable_control(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:disable_control(%s)\n",params);
#endif

    int slot = GetIntVal(params);

    ScrSys_SetFlag(slot,FLAG_DISABLED);

    return ACTION_NORMAL;
}

int action_enable_control(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:enable_control(%s)\n",params);
#endif

    int slot = GetIntVal(params);

    ScrSys_SetFlag(slot, 0);

    return ACTION_NORMAL;
}

int action_add(char *params, int aSlot , pzllst *owner)
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

int action_debug(char *params, int aSlot , pzllst *owner)
{


    char txt[256],number[16];
    int tmp;
    sscanf(params,"%s %s",txt, number);

    tmp = GetIntVal(number);

#ifdef TRACE
    printf("        action:debug(%s)\n",params);
#endif
    printf("DEBUG :%s\t: %d \n",txt,tmp);

    return ACTION_NORMAL;
}

int action_random(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:random:%d(%s)\n",aSlot,params);
#endif

    int number;
    char chars[16];
    sscanf(params,"%s", chars);
    number=GetIntVal(chars);

    SetgVarInt(aSlot, rand() % (number+1) );

    return ACTION_NORMAL;
}



int action_streamvideo(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:streamvideo(%s)\n",params);
#endif
    char file[255];
    char x[16];
    char y[16];
    char w[16];
    char h[16];
    char u1[16]; //bit-flags
    // 0x1 - in original aspect ratio and size, without scale
    // 0x2 - unknown
    char u2[16]; // skipline 0-off any other - on

    int xx,yy,ww,hh,tmp;

    char *fil;

    sscanf(params,"%s %s %s %s %s %s %s",file,x,y,w,h,u1,u2);

    xx=GetIntVal(x);
    yy=GetIntVal(y);
    ww=GetIntVal(w) - xx +1;
    hh=GetIntVal(h) - yy +1;

    anim_avi *anm=new(anim_avi);
    Mix_Chunk *aud=NULL;

    fil = GetFilePath(file);
    if (fil == NULL)
        return ACTION_NORMAL;
    anm->mpg=SMPEG_new(fil,&anm->inf,0);
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

    file[tmp-1]='b';
    file[tmp-2]='u';
    file[tmp-3]='s';

    struct_subtitles *subs=NULL;
    if (GetgVarInt(SLOT_SUBTITLE_FLAG) == 1)
        subs = sub_LoadSubtitles(file);

    int subtime = 0;


    SMPEG_setdisplay(anm->mpg,anm->img,0,0);
    SMPEG_setdisplayregion(anm->mpg, 0, 0, anm->inf.width,anm->inf.height);
    SMPEG_renderFrame(anm->mpg,1);
    SMPEG_play(anm->mpg);

    if (aud!=NULL)
    {
        tmp=GetFreeChannel();
        Mix_UnregisterAllEffects(tmp);
        Mix_PlayChannel(tmp,aud,0);
        //if (u2 == 0)
        //{
        //    SaveVol();
        //    SilenceVol();
        //}
        Mix_Volume(tmp,127);
    }

    while(SMPEG_status(anm->mpg) != SMPEG_STOPPED  &&  !KeyDown(SDLK_SPACE))
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        UpdateKeyboard();
        DrawImage(anm->img,GAMESCREEN_X+xx,GAMESCREEN_Y+yy); //it's direct rendering without game screen update

        if (subs != NULL)
        {
            SMPEG_getinfo(anm->mpg,&anm->inf);
            sub_ProcessSub(subs,anm->inf.current_frame/2);
        }

        Rend_ProcessSubs();

        Rend_ScreenFlip();
        //SDL_Flip(screen);


        int delay = 15;
        if (anm->inf.current_fps != 0)
            delay = 1000/anm->inf.current_fps;
        if (delay > 200 || delay < 0)
            SDL_Delay(15);
        else
            SDL_Delay(delay);
    }
    if (aud!=NULL)
    {
        //if (u2 == 0)
        //   RestoreVol();
        Mix_HaltChannel(tmp);
        Mix_FreeChunk(aud);
    }

    if (subs != NULL)
        sub_DeleteSub(subs);

    SMPEG_stop(anm->mpg);
    SMPEG_delete(anm->mpg);
    delete anm;

    return ACTION_NORMAL;
}

int action_animplay(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:animplay:%d(%s)\n",aSlot,params);
#endif

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
    char framerate[16]; //framerate or 0 (default)
    sscanf(params,"%s %s %s %s %s %s %s %s %s %s %s %s",file,x,y,w,h,st,en,loop,un1,un2,mask,framerate);



    MList *all = GetAction_res_List();
    StartMList(all);
    while (!eofMList(all))
    {

        struct_action_res *nd = (struct_action_res *)DataMList(all);

        if (nd->node_type == NODE_TYPE_ANIMPLAY)
            if (nd->slot == aSlot)
            {
                anim_DeleteAnimPlay(nd);
                DeleteCurrent(all);
            }

        NextMList(all);
    }

    struct_action_res *glob = anim_CreateAnimPlayNode();

    animnode *nod = glob->nodes.node_anim;

    ScrSys_AddToActResList(glob);


    if (aSlot>0)
    {
        SetgVarInt(aSlot, 1);
        setGNode(aSlot,glob);
    }


    glob->slot  = aSlot;
    glob->owner = owner;

    int mask2,r,g,b;

    mask2=GetIntVal(mask);

    if (mask2 != -1 && mask2 != 0)
    {
    b=FiveBitToEightBitLookupTable[((mask2 >> 10 ) & 0x1F)];
    g=FiveBitToEightBitLookupTable[((mask2 >> 5 ) & 0x1F)];
    r=FiveBitToEightBitLookupTable[(mask2 & 0x1F)];
    mask2=r | g<<8 | b<<16;
    }

    anim_LoadAnim(nod,file,0,0,mask2,GetIntVal(framerate));

    anim_PlayAnim(nod,GetIntVal(x),
                  GetIntVal(y),
                  GetIntVal(w) - GetIntVal(x) +1,
                  GetIntVal(h) - GetIntVal(y) +1,
                  GetIntVal(st),
                  GetIntVal(en),
                  GetIntVal(loop));



    return ACTION_NORMAL;
}

int music_music(char *params, int aSlot, pzllst *owner, bool universe)
{
    int type;
    char file[16];
    char loop[16];
    char vol[16];
    sscanf(params,"%d %s %s %s", &type, file, loop, vol);

    //printf ("%s %d %d\n",file,GetIntVal(vol),SoundVol[GetIntVal(vol)]);

    if (getGNode(aSlot) != NULL)
        return ACTION_NORMAL;



    struct_action_res *nod = snd_CreateWavNode();

    nod->slot = aSlot;
    nod->owner = owner;

    setGNode(nod->slot, nod);

    char *filp=GetFilePath(file);

    nod->nodes.node_music->universe = universe;

    nod->nodes.node_music->chunk = Mix_LoadWAV(filp);

    int st = strlen(file);

    file[st-1] = 'b';
    file[st-2] = 'u';
    file[st-3] = 's';

    if (GetgVarInt(SLOT_SUBTITLE_FLAG) == 1)
        nod->nodes.node_music->sub = sub_LoadSubtitles(file);

    nod->nodes.node_music->chn = GetFreeChannel();
    if (nod->nodes.node_music->chn == -1)
    {
        printf("ERROR, NO CHANNELS!\n");
        snd_DeleteWav(nod);
        return ACTION_NORMAL;
    }

    Mix_UnregisterAllEffects(nod->nodes.node_music->chn);


    if (GetIntVal(loop)==1)
    {
        Mix_PlayChannel(nod->nodes.node_music->chn,nod->nodes.node_music->chunk,-1);
        nod->nodes.node_music->looped = true;
    }
    else
    {
        Mix_PlayChannel(nod->nodes.node_music->chn,nod->nodes.node_music->chunk,0);
        nod->nodes.node_music->looped = false;
    }

    nod->nodes.node_music->volume = GetIntVal(vol);

    Mix_Volume( nod->nodes.node_music->chn, nod->nodes.node_music->volume);


    LockChan(nod->nodes.node_music->chn);




//    printf("chan %d vol %d\n",nod->chn,GetIntVal(vol));

    ScrSys_AddToActResList(nod);

    SetgVarInt(aSlot, 1);

    return ACTION_NORMAL;
}

int action_music(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:music:%d(%s) (%s)\n",aSlot ,params,ScrSys_ReturnListName(owner));
#endif

    return music_music(params,aSlot,owner,false);
}

int action_universe_music(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:universe_music:%d(%s) (%s)\n",aSlot ,params,ScrSys_ReturnListName(owner));
#endif

    return music_music(params,aSlot,owner,true);
}


int action_syncsound(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:syncsound:%d(%s) (%s)\n",aSlot ,params,ScrSys_ReturnListName(owner));
#endif

//slot maybe 0
//params:
//1 - sync with
//2 - type (0 - manual params, 1 from file)
//3 - filename
//4 - Freq
//5 - bits
//6 - 0 mono | 1 stereo
//7 - unknown

    char a1[16];
    char a2[16];
    char a3[16];

    //sscanf(params,"%s %s %s %s %s %s %s",)
    sscanf(params,"%s %s %s",a1,a2,a3);

    int syncto = GetIntVal(a1);

    if (getGNode(syncto) == NULL)
        return ACTION_NORMAL;

    struct_action_res *tmp = snd_CreateSyncNode();

    tmp->owner = owner;
    tmp->slot  = -1;
    //tmp->slot  = aSlot;

    tmp->nodes.node_sync->chn = GetFreeChannel();



    tmp->nodes.node_sync->syncto = syncto;

    if (getGNode(syncto)->node_type == NODE_TYPE_ANIMPRE)
    {
        getGNode(syncto)->nodes.node_animpre->framerate=FPS_DELAY; //~15fps hack
    }

    char *filp=GetFilePath(a3);

    if (tmp->nodes.node_sync->chn == -1 || filp == NULL)
    {
        printf("ERROR, NO CHANNELS OR NO FILE!\n");
        snd_DeleteSync(tmp);
        return ACTION_NORMAL;
    }

    int st = strlen(a3);

    a3[st-1] = 'b';
    a3[st-2] = 'u';
    a3[st-3] = 's';

    if (GetgVarInt(SLOT_SUBTITLE_FLAG) == 1)
        tmp->nodes.node_sync->sub = sub_LoadSubtitles(a3);


    tmp->nodes.node_sync->chunk  = Mix_LoadWAV(filp);

    Mix_UnregisterAllEffects(tmp->nodes.node_sync->chn);

    Mix_PlayChannel(tmp->nodes.node_sync->chn,tmp->nodes.node_sync->chunk,0);
    LockChan(tmp->nodes.node_sync->chn);

    ScrSys_AddToActResList(tmp);

    return ACTION_NORMAL;
}

int action_animpreload(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:animpreload:%d(%s)\n",aSlot,params);
#endif

    if (getGNode(aSlot) != NULL)
        return ACTION_NORMAL;

    char name[64];
    char u1[16];
    char u2[16];
    char u3[16];
    char u4[16];



    struct_action_res *pre = anim_CreateAnimPreNode();

    //%s %d %d %d %f
    //name     ? ? mask framerate
    //in zgi   0 0 0
    sscanf(params,"%s %s %s %s %s", name,u1,u2,u3,u4);

    anim_LoadAnim(pre->nodes.node_animpre,
                  name,
                  0,0,
                  GetIntVal(u3),
                  GetIntVal(u4));

    pre->slot = aSlot;
    pre->owner = owner;

    ScrSys_AddToActResList(pre);

    SetgVarInt(pre->slot,2);

    setGNode(aSlot,pre);

    //printf("AnimPreload \n");

    return ACTION_NORMAL;
}

int action_playpreload(char *params, int aSlot , pzllst *owner)
{
    char sl[16];
    uint32_t slot;
    int x,y,w,h,start,end,loop,sll;
    sscanf(params,"%s %d %d %d %d %d %d %d",sl,&x,&y,&w,&h,&start,&end,&loop);

#ifdef TRACE
    printf("        action:playpreload:%d(%s)\n",aSlot,params);
#endif


    char buff[255];

    slot = GetIntVal(sl);

    struct_action_res *pre = getGNode(slot);


    if (pre == NULL)
    {
#ifdef TRACE
        printf("        not found %d\n",slot);
#endif
        return ACTION_NORMAL;
    }

    if (pre->node_type != NODE_TYPE_ANIMPRE)
        return ACTION_NORMAL;

    struct_action_res *nod = anim_CreateAnimPlayPreNode();

    anim_preplay_node *tmp = nod->nodes.node_animpreplay;

    tmp->playerid = 0;
    tmp->point = pre->nodes.node_animpre;
    tmp->x = x;
    tmp->y = y;
    tmp->w = w-x;
    tmp->h = h-y;
    tmp->start = start;
    tmp->end = end;
    tmp->loop = loop;
    tmp->pointingslot = slot;

    nod->slot = aSlot;
    nod->owner = owner;

    ScrSys_AddToActResList(nod);

    if (aSlot>0)
    {
        setGNode(aSlot,nod);
    }

    //SetgVarInt(GetIntVal(chars),2);

    return ACTION_NORMAL;
}

int action_ttytext(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:ttytext:%d(%s)\n",aSlot,params);
#endif
    char chars[16];
    int32_t delay;
    int32_t x,y,w,h;
    sscanf(params,"%d %d %d %d %s %d",&x,&y,&w,&h,chars,&delay);

    w-=x;
    h-=y;

    char *fil = GetFilePath(chars);

    if (fil == NULL)
    {
#ifdef TRACE
    printf("          +-> file (%s) not found\n",chars);
#endif
        SetgVarInt(aSlot,2);
        return ACTION_NORMAL;
    }


    struct_action_res *nod = txt_CreateTTYtext();

    nod->slot = aSlot;
    nod->owner = owner;

    FILE *fl = fopen(fil,"rb");

    fseek(fl,0,SEEK_END);
    int32_t flsize = ftell(fl);
    fseek(fl,0,SEEK_SET);

    uint8_t *tmp = (uint8_t *)malloc(flsize);

    nod->nodes.tty_text->txtbuf = (char *)malloc(flsize);
    memset(nod->nodes.tty_text->txtbuf,0,flsize);

    fread(tmp,flsize,1,fl);

    fclose(fl);

    int32_t j=0;
    for (int32_t i=0; i<flsize; i++)
        if (tmp[i]!=0x0A && tmp[i]!=0x0D)
        {
            nod->nodes.tty_text->txtbuf[j]=(char)tmp[i];
            j++;
        }
    nod->nodes.tty_text->txtsize = j;
    free(tmp);

    txt_readfontstyle(&nod->nodes.tty_text->style,nod->nodes.tty_text->txtbuf);
    nod->nodes.tty_text->fnt = GetFontByName(nod->nodes.tty_text->style.fontname,nod->nodes.tty_text->style.size);

    nod->nodes.tty_text->w = w;
    nod->nodes.tty_text->h = h;
    nod->nodes.tty_text->x = x;
    nod->nodes.tty_text->y = y;
    nod->nodes.tty_text->delay = delay;


    nod->nodes.tty_text->img = CreateSurface(w,h);

    SetgVarInt(nod->slot, 1);
    setGNode(nod->slot,nod);

    ScrSys_AddToActResList(nod);

    return ACTION_NORMAL;
}


int stopkiller(char *params, int aSlot , pzllst *owner, bool iskillfunc)
{
    int slot;
    char chars[16];
    sscanf(params,"%s",chars);
    if (iskillfunc)
    {
        if (strcasecmp(chars,"\"all\"")==0)
        {
            ScrSys_DeleteAllRes();
            return ACTION_NORMAL;
        }

        if (strcasecmp(chars,"\"anim\"")==0)
        {
            ScrSys_FlushResourcesByType(NODE_TYPE_ANIMPLAY);
            ScrSys_FlushResourcesByType(NODE_TYPE_ANIMPRPL);
            ScrSys_FlushResourcesByType(NODE_TYPE_ANIMPRE);
            return ACTION_NORMAL;
        }

        if (strcasecmp(chars,"\"audio\"")==0)
        {
            ScrSys_FlushResourcesByType(NODE_TYPE_MUSIC);
            //ScrSys_FlushResourcesByType(NODE_TYPE_SYNCSND);
            return ACTION_NORMAL;
        }

        if (strcasecmp(chars,"\"distort\"")==0)
        {
            ScrSys_FlushResourcesByType(NODE_TYPE_DISTORT);
            return ACTION_NORMAL;
        }

        if (strcasecmp(chars,"\"pantrack\"")==0)
        {
            ScrSys_FlushResourcesByType(NODE_TYPE_PANTRACK);
            return ACTION_NORMAL;
        }

        if (strcasecmp(chars,"\"region\"")==0)
        {
            ScrSys_FlushResourcesByType(NODE_TYPE_REGION);
            return ACTION_NORMAL;
        }

        if (strcasecmp(chars,"\"timer\"")==0)
        {
            ScrSys_FlushResourcesByType(NODE_TYPE_TIMER);
            return ACTION_NORMAL;
        }

        if (strcasecmp(chars,"\"ttytext\"")==0)
        {
            ScrSys_FlushResourcesByType(NODE_TYPE_TTYTEXT);
            return ACTION_NORMAL;
        }
    }

    slot = GetIntVal(chars);

    //if (getGNode(slot) == NULL)
        //return ACTION_NOT_FOUND;

    MList *all = GetAction_res_List();

    StartMList(all);
    while(!eofMList(all))
    {
        struct_action_res *nod = (struct_action_res *)DataMList(all);

        if (nod->slot == slot)
        {
            ScrSys_DeleteNode(nod);

            DeleteCurrent(all);
            break;
        }

        NextMList(all);
    }

    return ACTION_NORMAL;
}

int action_kill(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:kill(%s)\n",params);
#endif

    int result = stopkiller(params,aSlot,owner, true);

#ifdef TRACE
    if (result == ACTION_NOT_FOUND)
        printf("Nothing to kill %s\n",params);
#endif

    return ACTION_NORMAL;
}


int action_stop(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:stop(%s)\n",params);
#endif

    int result = stopkiller(params,aSlot,owner, false);

#ifdef TRACE
    if (result == ACTION_NOT_FOUND)
        printf("Nothing to stop %s\n",params);
#endif

    return ACTION_NORMAL;
}


void act_inv_drop(int item)
{
    if (item == 0)
        return;

    if (GetgVarInt(SLOT_INVENTORY_MOUSE) == item)
    {
        SetgVarInt(SLOT_INVENTORY_MOUSE,0);
    }
    else
    {
        for (int i=SLOT_START_SLOT; i<= SLOT_END_SLOT ; i++)
            if (GetgVarInt(i)==item)
            {
                SetgVarInt(i,0);
                break;
            }
    }

}

void act_inv_add(int item)
{
    if (item == 0)
        return;

    /*for (int i=SLOT_START_SLOT; i<= SLOT_END_SLOT; i++)
        if (GetgVarInt(i) == item)
            SetgVarInt(i,0);*/

    for (int i=SLOT_START_SLOT; i<= SLOT_END_SLOT; i++)
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

void act_inv_cycle()
{

}

int action_inventory(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:inventory(%s)\n",params);
#endif

    int item;
    char cmd[16];
    char chars[16];
    sscanf(params,"%s %s",cmd,chars);
    item = GetIntVal(chars);

    if (strcasecmp(cmd,"add")==0)
    {
        act_inv_add(item);
    }
    else if (strcasecmp(cmd,"addi")==0)
    {
        item=GetgVarInt(item);
        act_inv_add(item);
    }
    else if (strcasecmp(cmd,"drop")==0)
    {
        act_inv_drop(item);
    }
    else if (strcasecmp(cmd,"dropi")==0)
    {
        item = GetgVarInt(item);
        act_inv_drop(item);
    }
    else if (strcasecmp(cmd,"cycle")==0)
    {
        act_inv_cycle();
    }
    else
        return ACTION_ERROR;

    return ACTION_NORMAL;
}

int action_crossfade(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:crossfade(%s)\n",params);
#endif
    //crossfade(%d %d %d %d %d %d %ld)
    // item1 item2 fromvol1 fromvol2 tovol1 tovol2 time_in_millisecs
    uint32_t item,item2;
    char slot[16];
    char slot2[16]; //unknown slot
    char frVol1[16];
    char frVol2[16];
    char toVol[16];
    char toVol2[16];
    char tim[16];
    sscanf(params,"%s %s %s %s %s %s %s",slot,slot2,frVol1,frVol2,toVol,toVol2,tim);
    item = GetIntVal(slot);
    item2 = GetIntVal(slot2);

    if (item > 0)
    {
        struct_action_res *tmp = getGNode(item);

        if (tmp != NULL)
            if (tmp->node_type == NODE_TYPE_MUSIC)
            {
                struct_action_res *tnod = tmp;

                tnod->nodes.node_music->crossfade = true;

                if (GetIntVal(frVol1)>=0)
                    tnod->nodes.node_music->volume = GetIntVal(frVol1);

                tnod->nodes.node_music->crossfade_params.times = ceil(GetIntVal(tim) / 33.3) ;
                tnod->nodes.node_music->crossfade_params.deltavolume = ceil((GetIntVal(toVol) - tnod->nodes.node_music->volume) / (float)tnod->nodes.node_music->crossfade_params.times);

                if (Mix_Playing(tnod->nodes.node_music->chn))
                    Mix_Volume(tnod->nodes.node_music->chn, GetLogVol(tnod->nodes.node_music->volume));

            }
    }

    if (item2 > 0)
    {
        struct_action_res *tmp = getGNode(item2);

        if (tmp != NULL)
            if (tmp->node_type == NODE_TYPE_MUSIC)
            {
                struct_action_res *tnod = tmp;

                tnod->nodes.node_music->crossfade = true;

                if (GetIntVal(frVol2)>=0)
                    tnod->nodes.node_music->volume = GetIntVal(frVol2);

                tnod->nodes.node_music->crossfade_params.times = ceil(GetIntVal(tim) / 33.3) ;
                tnod->nodes.node_music->crossfade_params.deltavolume = ceil((GetIntVal(toVol2) - tnod->nodes.node_music->volume) / (float)tnod->nodes.node_music->crossfade_params.times);

                if (Mix_Playing(tnod->nodes.node_music->chn))
                    Mix_Volume(tnod->nodes.node_music->chn, GetLogVol(tnod->nodes.node_music->volume));

            }
    }

    /* MList *wavs = snd_GetWavsList();
     StartMList(wavs);
     while(!eofMList(wavs))
     {
         musicnode *nod = (musicnode *)DataMList(wavs);

    //        if (nod->slot == item)
    //            Mix_Volume(nod->chn , GetLogVol(GetIntVal(toVol)));

     //       if (nod->slot == item2)
     //           Mix_Volume(nod->chn , GetLogVol(GetIntVal(toVol2)));

         NextMList(wavs);
     }*/


    return ACTION_NORMAL;
}


int action_menu_bar_enable(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:menu_bar_enable(%s)\n",params);
#endif

    uint16_t val;
    val = GetIntVal(params);

    menu_SetMenuBarVal(val);

    return ACTION_NORMAL;
}

int action_delay_render(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:delay_render(%s)\n",params);
#endif

    uint16_t val;
    val = GetIntVal(params);

    Rend_SetDelay(val);

    return ACTION_NORMAL;
}


int action_pan_track(char *params, int aSlot , pzllst *owner)
{
    //action:pan_track:XXXXX(TargetSlot X-Pos)
#ifdef TRACE
    printf("        action:pan_track:%d(%s)\n",aSlot,params);
#endif

    if (Rend_GetRenderer() != RENDER_PANA)
        return ACTION_NORMAL;

    int slot;
    int XX = 0;

    sscanf(params,"%d %d",&slot,&XX);




    if (slot > 0)
    {
        struct_action_res *nod = snd_CreatePanTrack();
        nod->nodes.node_pantracking = slot;

        nod->owner = owner;
        nod->slot  = aSlot;

        if (nod->slot > 0)
            setGNode(nod->slot,nod);

        ScrSys_AddToActResList(nod);

        struct_action_res *tmp = getGNode(slot);

        if (tmp != NULL)
            if (tmp->node_type == NODE_TYPE_MUSIC)
            {
                struct_action_res *tnod = tmp;

                tnod->nodes.node_music->pantrack = true;
                tnod->nodes.node_music->pantrack_X = XX;
            }

    }

    return ACTION_NORMAL;
}


int action_attenuate(char *params, int aSlot , pzllst *owner)
{
    //action:attenuate(TargetSlot, pos)
#ifdef TRACE
    printf("        action:attenuate(%s)\n",params);
#endif
    int slot;
    int att;

    sscanf(params,"%d, %d",&slot,&att);

    att = floor(32767.0 / ((float)abs(att)) * 255.0);

    if (slot > 0)
    {
        struct_action_res *tmp = getGNode(slot);

        if (tmp != NULL)
            if (tmp->node_type == NODE_TYPE_MUSIC)
            {

                tmp->nodes.node_music->attenuate = att;
                Mix_SetPosition(tmp->nodes.node_music->chn,
                                tmp->nodes.node_music->pantrack_angle,
                                tmp->nodes.node_music->attenuate);
            }
    }


    return ACTION_NORMAL;
}


int action_cursor(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:cursor(%s)\n",params);
#endif

    if (tolower(params[0]) == 'u')
        Mouse_ShowCursor();
    else if (tolower(params[0]) == 'h')
        Mouse_HideCursor();
    else
        return ACTION_ERROR;

    return ACTION_NORMAL;
}

int action_animunload(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:animunload(%s)\n",params);
#endif

    int slot;

    sscanf(params,"%d",&slot);

    struct_action_res *nod = getGNode(slot);

    if (nod != NULL )
        if (nod->node_type == NODE_TYPE_ANIMPRE)
            nod->need_delete = true;

    return ACTION_NORMAL;
}

int action_flush_mouse_events(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:flush_mouse_events(%s)\n",params);
#endif
    FlushMouseBtn(SDL_BUTTON_LEFT);
    FlushMouseBtn(SDL_BUTTON_RIGHT);

    return ACTION_NORMAL;
}

int action_save_game(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:save_game(%s)\n",params);
#endif

    ScrSys_PrepareSaveBuffer();
    ScrSys_SaveGame(params);

    return ACTION_NORMAL;
}

int action_restore_game(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:restore_game(%s)\n",params);
#endif

    ScrSys_LoadGame(params);

    return ACTION_NORMAL;
}

int action_quit(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:quit(%s)\n",params);
#endif

    if (atoi(params)==1)
        __END();
    else
        ifquit();

    return ACTION_NORMAL;
}

int action_rotate_to(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:rotate_to(%s)\n",params);
#endif

    if (Rend_GetRenderer()!= RENDER_PANA)
        return ACTION_NORMAL;

    int32_t topos;
    int32_t time;

    sscanf(params,"%d, %d",&topos,&time);

    int32_t maxX = Rend_GetPanaWidth();
    int32_t curX = GetgVarInt(SLOT_VIEW_POS);
    int32_t oner = 0;

    if (curX == topos)
        return ACTION_NORMAL;

    if (curX > topos)
    {
        if (curX - topos > maxX/2)
            oner = (topos + (maxX-curX)) / time;
        else
            oner = -(curX-topos) / time;
    }
    else
    {
        if (topos - curX > maxX/2)
            oner = -((maxX -topos) + curX) / time;
        else
            oner = (topos - curX)/ time;
    }

    for (int32_t i=0; i<=time; i++)
    {
        if (i == time)
            curX = topos;
        else
            curX += oner;

        if (curX < 0)
            curX = maxX - curX;
        else if (curX >= maxX)
            curX %= maxX;

        SetDirectgVarInt(SLOT_VIEW_POS,curX);

        Rend_RenderFunc();
        Rend_ScreenFlip();

        SDL_Delay(500/time);

    }

    return ACTION_NORMAL;
}

int action_distort(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:distort(%s)\n",params);
#endif

    if (Rend_GetRenderer() != RENDER_PANA && Rend_GetRenderer() != RENDER_TILT)
        return ACTION_NORMAL;

    int32_t slot,speed;
    float st_angl,en_angl;
    float st_lin,en_lin;
    sscanf(params,"%d %d %f %f %f %f",&slot,&speed,&st_angl,&en_angl,&st_lin,&en_lin);

    if (getGNode(slot)!= NULL)
        return ACTION_NORMAL;

    struct_action_res *act = Rend_CreateDistortNode();
    act->slot = slot;
    act->owner = owner;

    if (slot > 0)
    {
        setGNode(act->slot, act);
        SetgVarInt(act->slot, 1);
    }

    ScrSys_AddToActResList(act);

    act->nodes.distort->speed = speed;
    act->nodes.distort->increase = true;
    act->nodes.distort->rend_angl = Rend_GetRendererAngle();
    act->nodes.distort->rend_lin  = Rend_GetRendererLinscale();
    act->nodes.distort->st_angl = st_angl;
    act->nodes.distort->st_lin  = st_lin;
    act->nodes.distort->end_angl = en_angl;
    act->nodes.distort->end_lin  = en_lin;
    act->nodes.distort->dif_angl = en_angl - st_angl;
    act->nodes.distort->dif_lin  = en_lin - st_lin;
    act->nodes.distort->param1   = (float)speed / 15.0;
    act->nodes.distort->frames = ceil((5.0-act->nodes.distort->param1*2.0) / (act->nodes.distort->param1));
    if (act->nodes.distort->frames <= 0)
        act->nodes.distort->frames = 1;

    return ACTION_NORMAL;
}

int action_preferences(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:preferences(%s)\n",params);
#endif

    if (strCMP(params,"save")== 0)
        ScrSys_SavePreferences();
    else
        ScrSys_LoadPreferences();

    return ACTION_NORMAL;
}

