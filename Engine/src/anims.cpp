#include "System.h"
#include "anims.h"

int32_t AnnimID = 0;

animnode *anim_CreateAnim()
{
    animnode *tmp;
    tmp = new (animnode);
    tmp->scal    = NULL;
    tmp->anim.avi= NULL;
    tmp->CurFr   = 0;
    tmp->end     = 0;
    tmp->h       = 0;
    tmp->loopcnt = 0;
    tmp->loops   = 0;
    tmp->start   = 0;
    tmp->unk1    = 0;
    tmp->unk2    = 0;
    tmp->mask    = 0;
    tmp->framerate = 0;
    tmp->vid     = false;
    tmp->w       = 0;
    tmp->x       = 0;
    tmp->y       = 0;
    tmp->rel_h   = 0;
    tmp->rel_w   = 0;
    tmp->nexttick = 0;
    tmp->playID  = 0;
    tmp->playing  = false;
    return tmp;
}

struct_action_res *anim_CreateAnimPlayNode()
{
    struct_action_res *tmp = ScrSys_CreateActRes(NODE_TYPE_ANIMPLAY);

    tmp->nodes.node_anim = anim_CreateAnim();

    return tmp;
}

struct_action_res *anim_CreateAnimPreNode()
{
    struct_action_res *tmp;
    tmp = ScrSys_CreateActRes(NODE_TYPE_ANIMPRE);

    tmp->nodes.node_animpre = anim_CreateAnim();

    return tmp;
}

struct_action_res *anim_CreateAnimPlayPreNode()
{
    struct_action_res *tmp;
    tmp = ScrSys_CreateActRes(NODE_TYPE_ANIMPRPL);

    tmp->nodes.node_animpreplay = new(anim_preplay_node);
    tmp->nodes.node_animpreplay ->playerid = 0;
    tmp->nodes.node_animpreplay ->pointingslot = 0;
    tmp->nodes.node_animpreplay ->point = NULL;
    tmp->nodes.node_animpreplay ->end     = 0;
    tmp->nodes.node_animpreplay ->h       = 0;
    tmp->nodes.node_animpreplay ->loop = 0;
    tmp->nodes.node_animpreplay ->start   = 0;
    tmp->nodes.node_animpreplay ->w       = 0;
    tmp->nodes.node_animpreplay ->x       = 0;
    tmp->nodes.node_animpreplay ->y       = 0;

    return tmp;
}

void anim_LoadAnim(animnode *nod,char *filename,int u1, int u2, int32_t mask, int framerate)
{

    if (framerate != 0)
        nod->framerate = 1000.0/framerate;
    else
        nod->framerate = 0;

    nod->nexttick = 0;
    nod->loops=0;

    if (strcasestr(filename,"avi")!=NULL || strcasestr(filename,"mpg")!=NULL)
    {
        nod->anim.avi = new(anim_avi);
        nod->vid=true;

        nod->anim.avi->mpg=SMPEG_new(GetFilePath(filename),&nod->anim.avi->inf,0);
        int16_t w,h;

        w = nod->anim.avi->inf.width;
        h = nod->anim.avi->inf.height;

        nod->anim.avi->img = CreateSurface(w,h);

        SMPEG_setdisplay(nod->anim.avi->mpg,nod->anim.avi->img,0,0);
        SMPEG_setdisplayregion(nod->anim.avi->mpg, 0, 0, nod->anim.avi->inf.width,nod->anim.avi->inf.height);

        nod->anim.avi->lastfrm = -1;

        if (nod->framerate == 0)
            nod->framerate = FPS_DELAY; //~15fps

        nod->rel_h = h;
        nod->rel_w = w;

    }
    else
    {
        if (strcasestr(filename,"rlf")!=NULL)
            nod->anim.rlf = loader_LoadRlf(filename,Rend_GetRenderer() == RENDER_PANA,mask);
        else
            nod->anim.rlf = LoadAnimImage(filename,mask);

        nod->vid=false;

        if (nod->framerate == 0)
            nod->framerate = nod->anim.rlf->info.time;

        nod->rel_h = nod->anim.rlf->info.h;
        nod->rel_w = nod->anim.rlf->info.w;
    }
}


void anim_ProcessAnim(animnode *mnod)
{
    if (!mnod->playing)
        return;

    if (mnod)
    {

        mnod->nexttick -= GetDTime();

        if (mnod->nexttick <= 0)
        {
            mnod->nexttick = mnod->framerate;

            if (mnod->vid)
            {
                SMPEG_renderFrame(mnod->anim.avi->mpg,
                                  mnod->CurFr+1);

                Rend_DrawScalerToGamescr(mnod->scal,
                                        mnod->x,
                                        mnod->y);
                mnod->CurFr++;
            }
            else
                Rend_DrawImageToGamescr(mnod->anim.rlf,
                                        mnod->x,
                                        mnod->y,
                                        mnod->CurFr);

            mnod->CurFr++;

            if (mnod->CurFr > mnod->end)
            {
                mnod->loops++;

                if (mnod->loops < mnod->loopcnt || mnod->loopcnt == 0)
                {
                    mnod->CurFr=mnod->start;
                    /*                            if (nod->vid)
                                                {
                                                    //nod->nexttick=millisec() + 1.0/(((anim_avi *)nod->anim)->inf.current_fps) * 1000.0;
                                                    nod->nexttick=millisec() + (1.0/30.0) * 1000.0;
                                                }
                                                else
                                                    nod->nexttick=millisec()+((anim_surf *)nod->anim)->info.time;*/
                }

                else
                {
#ifdef TRACE
//                        printf ("Animplay #%d End's\n",nod->slot);
#endif
//                        anim_DeleteAnimNod(nod);
                    mnod->playing = false;
                }
            }
        }
    }
}

int anim_ProcessAnimPlayNode(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIMPLAY)
        return NODE_RET_OK;

    anim_ProcessAnim(nod->nodes.node_anim);

    if (!nod->nodes.node_anim->playing)
    {
        anim_DeleteAnimPlay(nod);
        return NODE_RET_DELETE;
    }

    return NODE_RET_OK;
}

int anim_ProcessAnimPreNode(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIMPRE)
        return NODE_RET_OK;

    anim_ProcessAnim(nod->nodes.node_animpre);

    return NODE_RET_OK;
}

int anim_ProcessAnimPrePlayNode(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIMPRPL)
        return NODE_RET_OK;

    anim_preplay_node *pre = nod->nodes.node_animpreplay;

    if (pre->playerid == 0)
    {
        pre->playerid = anim_PlayAnim(pre->point,pre->x,
                                      pre->y,
                                      pre->w,
                                      pre->h,
                                      pre->start,
                                      pre->end,
                                      pre->loop);
        SetgVarInt(pre->pointingslot,1);
        if (nod->slot > 0)
            SetgVarInt(nod->slot,1);
    }
    else
    {
        if (!pre->point->playing)
        {
            SetgVarInt(nod->slot,2);
            anim_DeleteAnimPrePlayNode(nod);
            return NODE_RET_DELETE;
        }

    }


    return NODE_RET_OK;
}

int anim_PlayAnim(animnode *nod,int x, int y, int w, int h, int start, int end, int loop)
{
    nod->playing = true;
    AnnimID++;
    nod->playID = AnnimID;

    nod->w = w;
    nod->h = h;
    nod->x = x;
    nod->y = y;

    if (nod->vid)
    {
        if (nod->scal != NULL)
            DeleteScaler(nod->scal);

        nod->scal = CreateScaler(nod->anim.avi->img,nod->w,nod->h);

        nod->start= (start+1) *2;
        nod->end= (end+1) *2;

        SMPEG_renderFrame(nod->anim.avi->mpg,nod->start);
    }
    else
    {
        nod->start= start;
        nod->end= end;
    }


    nod->CurFr = nod->start;
    nod->loopcnt = loop;

    return nod->playID;
}

void anim_RenderAnimFrame(animnode *mnod,int16_t x, int16_t y,int16_t w, int16_t h, int16_t frame)
{

    if (mnod)
    {
        if (mnod->vid)
        {
            if (mnod->anim.avi != NULL)
            {
                if (mnod->anim.avi->lastfrm != frame)
                {
                    SMPEG_renderFrame(mnod->anim.avi->mpg, frame*2);
                    SMPEG_renderFrame(mnod->anim.avi->mpg, frame*2+1);
                    if (mnod->anim.avi->lastfrm > frame)
                        SMPEG_renderFinal(mnod->anim.avi->mpg,mnod->anim.avi->img,0,0);
                }


                mnod->anim.avi->lastfrm = frame;

                if (mnod->scal)
                    if (mnod->scal->w != w || mnod->scal->h != h)
                    {
                        DeleteScaler(mnod->scal);
                        mnod->scal = NULL;
                    }

                if (!mnod->scal)
                    mnod->scal = CreateScaler(mnod->anim.avi->img,w,h);

                Rend_DrawScalerToGamescr(mnod->scal,x,y);

                //Rend_DrawImageToGamescr(mnod->anim.avi->img, x, y);
            }
        }
        else
            if (mnod->anim.rlf != NULL)
                Rend_DrawImageToGamescr(mnod->anim.rlf, x, y, frame);
    }
}

void anim_DeleteAnim(animnode *nod)
{
    if (nod->scal != NULL)
        DeleteScaler(nod->scal);

    if (nod->vid)
    {
        if (nod->anim.avi->img)
            SDL_FreeSurface(nod-> anim.avi ->img);
        SMPEG_stop(     nod-> anim.avi ->mpg);
        SMPEG_delete(   nod-> anim.avi ->mpg);
        delete nod->anim.avi;
    }
    else
        FreeAnimImage(nod->anim.rlf);

    delete nod;
}

int anim_DeleteAnimPlay(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIMPLAY)
        return NODE_RET_NO;

    anim_DeleteAnim(nod->nodes.node_anim);

    if (nod->slot > 0)
    {
        SetgVarInt(nod->slot,2);
        setGNode(nod->slot, NULL);
    }

    delete nod;

    return NODE_RET_DELETE;
}

int anim_DeleteAnimPreNod(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIMPRE)
        return NODE_RET_NO;

    MList *lst = GetAction_res_List();
    pushMList(lst);
    StartMList(lst);
    while(!eofMList(lst))
    {
        struct_action_res *nod2 = (struct_action_res *)DataMList(lst);

        if (nod2->node_type == NODE_TYPE_ANIMPRPL)
        {
            anim_preplay_node *tmp = nod2->nodes.node_animpreplay;
            if (tmp->pointingslot == nod->slot)
                nod2->need_delete = true;
        }

        NextMList(lst);
    }
    popMList(lst);

    setGNode(nod->slot, NULL);

    delete nod->nodes.node_animpre;
    delete nod;

    return NODE_RET_DELETE;
}

int anim_DeleteAnimPrePlayNode(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIMPRPL)
        return NODE_RET_NO;

    if (nod->slot > 0)
    {
        //SetgVarInt(nod->slot,2);
        setGNode(nod->slot,NULL);
    }

    SetgVarInt(nod->nodes.node_animpreplay->pointingslot, 2);

    delete nod->nodes.node_animpreplay;
    delete nod;

    return NODE_RET_DELETE;
}


