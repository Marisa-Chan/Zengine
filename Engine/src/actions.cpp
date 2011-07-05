#include "System.h"

extern SDL_Surface *screen;

extern int GAME_Y;

int action_set_screen(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:set_screen  %s\n",params);
#endif

    SDL_Surface **scrbuf = Rend_GetGameScreen();

    if (*scrbuf)
        SDL_FreeSurface(*scrbuf);

    *scrbuf=IMG_Load(GetFilePath(params));
    if (!*scrbuf)
        printf("ERROR:  IMG_Load(%s): %s\n\n",params, IMG_GetError());
    else
    {
        ConvertImage(scrbuf);
    }

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
        Rend_DrawImageToGamescr(tmp,x,y);
        //else if (tmp1==1)
        //    DrawImageToSurf(tmp,x-tmp->w/2,y-tmp->h/2,scrbuf);
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
    nod->nodes.node_timer = GetIntVal(s);
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

    //depricated
    Rend_SetDelay(2);
    // View_start_Loops = 1;

    return ACTION_BREAK;
}

int action_dissolve(char *params, int aSlot , pzllst *owner)
{
#ifdef TRACE
    printf("        action:dissolve()\n");
#endif

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
    printf("DEBUG :%s\t: %d \n",txt,tmp);
#endif

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
        DrawImage(anm->img,xx,GAME_Y+yy);

        if (subs != NULL)
        {
            SMPEG_getinfo(anm->mpg,&anm->inf);
            sub_ProcessSub(subs,anm->inf.current_frame/2);
        }

        Rend_ProcessSubs();

        SDL_Flip(screen);
        SDL_Delay(1000/anm->inf.current_fps);
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

        if (nd->node_type == NODE_TYPE_ANIM)
            if (nd->slot == aSlot)
            {
                anim_DeleteAnimNod(nd);
                DeleteCurrent(all);
            }

        NextMList(all);
    }



    SetgVarInt(aSlot, 1);



    struct_action_res *glob = anim_CreateAnimNode();

    animnode *nod = glob->nodes.node_anim;

    ScrSys_AddToActResList(glob);



    setGNode(aSlot,glob);

    glob->slot  = aSlot;
    glob->owner = owner;

    nod->x = GetIntVal(x);
    nod->y = GetIntVal(y);
    nod->w = GetIntVal(w) - nod->x +1;
    nod->h = GetIntVal(h) - nod->y +1;
    nod->mask = GetIntVal(mask);
    nod->loopcnt   = GetIntVal(loop);
    nod->framerate = GetIntVal(framerate);

    if (nod->framerate != 0)
        nod->framerate = ceil(30.0 / float(nod->framerate));

    nod->nexttick = 0;//nod->framerate;
    nod->loops=0;

    if (strcasestr(file,"avi")!=NULL)
    {
        nod->anim.avi = new(anim_avi);
        nod->vid=true;

        nod->anim.avi->mpg=SMPEG_new(GetFilePath(file),&nod->anim.avi->inf,0);
        nod->anim.avi->img = CreateSurface(nod->w,nod->h);
        SMPEG_setdisplay(nod->anim.avi->mpg,nod->anim.avi->img,0,0);
        SMPEG_setdisplayregion(nod->anim.avi->mpg, 0, 0, nod->anim.avi->inf.width,nod->anim.avi->inf.height);

        nod->start= GetIntVal(st) *2;
        nod->end= GetIntVal(en) *2;

        SMPEG_renderFrame(nod->anim.avi->mpg,nod->start+1);
        SMPEG_Info inf;
        SMPEG_getinfo(nod->anim.avi->mpg,&inf);

        if (nod->framerate == 0)
            nod->framerate = ceil(30.0*(inf.current_fps/ 10000.0))+1;

    }
    else
    {
        int r=GetIntVal(mask),g,b;
        b=FiveBitToEightBitLookupTable[((r >> 10 ) & 0x1F)];
        g=FiveBitToEightBitLookupTable[((r >> 5 ) & 0x1F)];
        r=FiveBitToEightBitLookupTable[(r & 0x1F)];

        nod->anim.rlf = LoadAnimImage(file,r | g<<8 | b<<16);
        nod->vid=false;

        nod->start= GetIntVal(st);
        nod->end= GetIntVal(en);

        if (nod->framerate == 0)
            nod->framerate = ceil(30.0*(nod->anim.rlf->info.frames / 10000.0))+1;

    }

    nod->CurFr = nod->start;

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
    //name     ? ? ?   framerate
    //in zgi   0 0 0
    sscanf(params,"%s %s %s %s %s", name,u1,u2,u3,u4);

    pre->nodes.node_animpre->fil = (char *)malloc(255);

    strcpy(pre->nodes.node_animpre->fil,name);
    pre->slot = aSlot;

    pre->nodes.node_animpre->framerate = GetIntVal(u4);

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

    sprintf(buff,"%s %d %d %d %d %d %d %d %d %d %d %d" ,pre->nodes.node_animpre->fil,\
            x, y, w, h, start, end, loop, 0, 0, 0, pre->nodes.node_animpre->framerate);

    if (aSlot == 0)
        action_animplay(buff,slot,owner);
    else
        action_animplay(buff,aSlot,owner);

    //SetgVarInt(GetIntVal(chars),2);

    return ACTION_NORMAL;
}

int action_ttytext(char *params, int aSlot , pzllst *owner)
{
    char chars[16];
    sscanf(params,"%s",chars);
    printf("STUB     action:ttytext:%d(%s) \n>>>   ",aSlot,params);
    action_timer("15",aSlot,owner);

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
            ScrSys_FlushResourcesByType(NODE_TYPE_ANIM);
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

    if (getGNode(slot) == NULL)
        return ACTION_NOT_FOUND;

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

    for (int i=SLOT_START_SLOT; i<= SLOT_END_SLOT; i++)
        if (GetgVarInt(i) == item)
            SetgVarInt(i,0);

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
        ScrSys_AddToActResList(nod);

        struct_action_res *tmp = getGNode(slot);

        if (tmp != NULL)
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

