
#include "System.h"
#include "anims.h"


MList  *preload =NULL;

MList   *anims  =NULL;


MList *anim_getpreloadLst()
{
    return preload;
}

MList *anim_getanimlst()
{
    return anims;
}

void anim_InitAnimLists()
{
    if (preload)
        DeleteMList(preload);

    if (anims)
        DeleteMList(anims);

    preload = CreateMList();

    anims = CreateMList();
}

void anim_ProcessAnims()
{
    StartMList(anims);

    while (!eofMList(anims))
    {
        animnode *nod=(animnode *)DataMList(anims);

        if (nod)
            if (nod->anim)
                if (GetBeat())
                {
                    if (nod->vid)
                    {
                        anim_avi *anm=(anim_avi *)nod->anim;
                        SMPEG_renderFrame(anm->mpg,nod->CurFr+1);

                        Rend_DrawImageToGamescr(anm->img,nod->x,nod->y);
                        nod->CurFr++;
                    }
                    else
                        Rend_DrawImageToGamescr((anim_surf *)nod->anim,nod->x,nod->y,nod->CurFr);

                    nod->CurFr++;

                    if (nod->CurFr > nod->end)
                    {
                        nod->loops++;

                        if (nod->loops<nod->loopcnt || nod->loopcnt == 0)
                        {
                            nod->CurFr=nod->start;
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
                }

        NextMList(anims);
    }
}

void anim_DeleteAnimNod(animnode *nod)
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

void anim_FlushAnims()
{
    StartMList(anims);
    while (!eofMList(anims))
    {
        animnode *nod=(animnode *)DataMList(anims);

        anim_DeleteAnimNod(nod);

        NextMList(anims);
    }

    FlushMList(anims);
}

void anim_FlushPreload()
{
    MList *preload = anim_getpreloadLst();

    if (preload == NULL)
        return;

    StartMList(preload);

    while (!eofMList(preload))
    {
        struct_Preload *pre = (struct_Preload *) DataMList(preload);

        delete pre->fil;
        delete pre;

        NextMList(preload);
    }
    FlushMList(preload);
    //*Getpreload() = NULL;
}
