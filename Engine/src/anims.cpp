#include "System.h"
#include "anims.h"


struct_action_res *anim_CreateAnimNode()
{
    struct_action_res *tmp;
    tmp = new (struct_action_res);

    tmp->slot            = 0;
    tmp->node_type       = NODE_TYPE_ANIM;
    tmp->owner           = NULL;
    tmp->need_delete     = false;

    tmp->nodes.node_anim = new (animnode);
    tmp->nodes.node_anim->anim.avi= NULL;
    tmp->nodes.node_anim->CurFr   = 0;
    tmp->nodes.node_anim->end     = 0;
    tmp->nodes.node_anim->h       = 0;
    tmp->nodes.node_anim->loopcnt = 0;
    tmp->nodes.node_anim->loops   = 0;
    tmp->nodes.node_anim->start   = 0;
    tmp->nodes.node_anim->unk1    = 0;
    tmp->nodes.node_anim->unk2    = 0;
    tmp->nodes.node_anim->mask    = 0;
    tmp->nodes.node_anim->framerate = 0;
    tmp->nodes.node_anim->vid     = false;
    tmp->nodes.node_anim->w       = 0;
    tmp->nodes.node_anim->x       = 0;
    tmp->nodes.node_anim->y       = 0;
    tmp->nodes.node_anim->nexttick = 0;
    return tmp;
}

struct_action_res *anim_CreateAnimPreNode()
{
    struct_action_res *tmp;
    tmp = new (struct_action_res);

    tmp->slot            = 0;
    tmp->node_type       = NODE_TYPE_ANIMPRE;
    tmp->owner           = NULL;
    tmp->need_delete     = false;

    tmp->nodes.node_animpre = new (animprenode);
    tmp->nodes.node_animpre->fil = NULL;
    tmp->nodes.node_animpre->u1  = 0;
    tmp->nodes.node_animpre->u2  = 0;
    tmp->nodes.node_animpre->mask      = 0;
    tmp->nodes.node_animpre->framerate = 0;
    return tmp;
}


int anim_ProcessAnim(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIM)
        return NODE_RET_OK;

    animnode *mnod = nod->nodes.node_anim;

    if (mnod)
        if (GetBeat())
        {
            mnod->nexttick--;

            if (mnod->nexttick <= 0)
            {
                mnod->nexttick = mnod->framerate;

                if (mnod->vid)
                {
                    SMPEG_renderFrame(mnod->anim.avi->mpg,
                                      mnod->CurFr+1);

                    Rend_DrawImageToGamescr(mnod->anim.avi->img,
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
                        printf ("Animplay #%d End's\n",nod->slot);
#endif
                        anim_DeleteAnimNod(nod);
                        return NODE_RET_DELETE;
                    }
                }
            }
        }
    return NODE_RET_OK;
}

int anim_DeleteAnimNod(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIM)
        return NODE_RET_NO;

    if (nod->nodes.node_anim->vid)
    {
        SDL_FreeSurface(nod-> nodes.node_anim-> anim.avi ->img);
        SMPEG_stop(     nod-> nodes.node_anim-> anim.avi ->mpg);
        SMPEG_delete(   nod-> nodes.node_anim-> anim.avi ->mpg);
        delete nod->nodes.node_anim->anim.avi;
    }
    else
        FreeAnimImage(nod->nodes.node_anim->anim.rlf);

    if (nod->slot >= 0)
        SetgVarInt(nod->slot,2);

    setGNode(nod->slot, NULL);

    delete nod->nodes.node_anim;
    delete nod;

    return NODE_RET_DELETE;
}

int anim_DeleteAnimPreNod(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_ANIMPRE)
        return NODE_RET_NO;

    setGNode(nod->slot, NULL);

    delete nod->nodes.node_animpre->fil;
    delete nod->nodes.node_animpre;
    delete nod;

    return NODE_RET_DELETE;
}
