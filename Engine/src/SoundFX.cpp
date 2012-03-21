
#include "System.h"

#define pi180 0.0174

void snd_DeleteNoUniverse(pzllst *owner)
{
    MList *allres = GetAction_res_List();
    StartMList(allres);
    while (!eofMList(allres))
    {
        struct_action_res *nod=(struct_action_res *)DataMList(allres);
        if (nod->node_type == NODE_TYPE_MUSIC)
            if (nod->owner == owner && nod->nodes.node_music->universe == false)
                {
                    snd_DeleteWav(nod);
                    DeleteCurrent(allres);
                }
        NextMList(allres);
    }
}

void snd_DeleteLoopedWavsByOwner(pzllst *owner)
{
    MList *allres = GetAction_res_List();
    StartMList(allres);
    while (!eofMList(allres))
    {
        struct_action_res *nod=(struct_action_res *)DataMList(allres);
        if (nod->node_type == NODE_TYPE_MUSIC)
            if (nod->owner == owner && nod->nodes.node_music->looped)
                {
                    snd_DeleteWav(nod);
                    DeleteCurrent(allres);
                }
        NextMList(allres);
    }
}

int snd_ProcessWav(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_MUSIC)
        return NODE_RET_OK;

    musicnode *mnod = nod->nodes.node_music;

        if (!Mix_Playing(mnod->chn))
        {
            snd_DeleteWav(nod);
            return NODE_RET_DELETE;
        }

        if (mnod->crossfade)
        {
            if (mnod->crossfade_params.times > 0)
            {
                if (GetBeat())
                {
                    mnod->volume += mnod->crossfade_params.deltavolume;
                    if (Mix_Playing(mnod->chn))
                        Mix_Volume(mnod->chn, GetLogVol(mnod->volume));
                    mnod->crossfade_params.times--;
                }
            }
            else
                mnod->crossfade = false;
        }

        if (Rend_GetRenderer() != RENDER_PANA)
        {
            mnod->pantrack = false;
            Mix_SetPosition(mnod->chn, 0, mnod->attenuate);
        }

        if (mnod->pantrack)
        {
            int PlX = GetgVarInt(SLOT_VIEW_POS);
            float pixangle = (360.0 / Rend_GetPanaWidth());
            int soundpos   = floor((PlX - mnod->pantrack_X) * pixangle);
            if (soundpos < 0)
                soundpos+=360;

            soundpos = 360 - soundpos;
            mnod->pantrack_angle = soundpos;

            int tempdist = mnod->attenuate;
            if (soundpos > 90 && soundpos < 270)
                tempdist += (-cos(soundpos * pi180)*96.0);

            if (tempdist > 255)
                tempdist = 255;

            Mix_SetPosition(mnod->chn, mnod->pantrack_angle, tempdist);

        }

        if (mnod->sub != NULL)
            sub_ProcessSub(mnod->sub,GetChanTime(mnod->chn) / 100);


    return NODE_RET_OK;
}

int snd_DeleteWav(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_MUSIC)
        return NODE_RET_NO;

    if (nod->nodes.node_music->chn >= 0)
    {
        if (Mix_Playing(nod->nodes.node_music->chn))
            Mix_HaltChannel(nod->nodes.node_music->chn);

        Mix_UnregisterAllEffects(nod->nodes.node_music->chn);
        UnlockChan(nod->nodes.node_music->chn);
    }
    Mix_FreeChunk(nod->nodes.node_music->chunk);
    if (nod->slot != 0)
        SetgVarInt(nod->slot,2);

    if (nod->nodes.node_music->sub != NULL)
            sub_DeleteSub(nod->nodes.node_music->sub);

    setGNode(nod->slot,NULL);

    delete nod->nodes.node_music;
    delete nod;

    return NODE_RET_DELETE;
}

struct_action_res *snd_CreateWavNode()
{
    struct_action_res *tmp;
    tmp = ScrSys_CreateActRes(NODE_TYPE_MUSIC);

    tmp->nodes.node_music = new (musicnode);
    tmp->nodes.node_music->chn = -1;
    tmp->nodes.node_music->chunk  = NULL;
    tmp->nodes.node_music->volume = 100;
    tmp->nodes.node_music->looped = false;
    tmp->nodes.node_music->crossfade = false;
    tmp->nodes.node_music->pantrack  = false;
    tmp->nodes.node_music->crossfade_params.deltavolume = 0;
    tmp->nodes.node_music->crossfade_params.times       = 0;
    tmp->nodes.node_music->pantrack_X = 0;
    tmp->nodes.node_music->pantrack_angle = 0;
    tmp->nodes.node_music->attenuate  = 0;
    tmp->nodes.node_music->universe   = false;
    tmp->nodes.node_music->sub        = NULL;

    return tmp;
}


/// SoundSync


struct_action_res *snd_CreateSyncNode()
{
    struct_action_res *tmp;
    tmp = ScrSys_CreateActRes(NODE_TYPE_SYNCSND);

    tmp->nodes.node_sync = new (struct_syncnode);
    tmp->nodes.node_sync->chn    =  -1;
    tmp->nodes.node_sync->chunk  = NULL;
    tmp->nodes.node_sync->syncto =  0;
    tmp->nodes.node_sync->sub        = NULL;

    return tmp;
}


int snd_DeleteSync(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_SYNCSND)
        return NODE_RET_NO;

    if (nod->nodes.node_sync->chn >= 0)
    {
        if (Mix_Playing(nod->nodes.node_sync->chn))
            Mix_HaltChannel(nod->nodes.node_sync->chn);

        Mix_UnregisterAllEffects(nod->nodes.node_sync->chn);
        UnlockChan(nod->nodes.node_sync->chn);
    }
    Mix_FreeChunk(nod->nodes.node_sync->chunk);

    if (nod->nodes.node_sync->sub != NULL)
            sub_DeleteSub(nod->nodes.node_sync->sub);

    delete nod->nodes.node_sync;
    delete nod;

    return NODE_RET_DELETE;
}

int snd_ProcessSync(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_SYNCSND)
        return NODE_RET_OK;

    struct_syncnode *mnod = nod->nodes.node_sync;


    if (mnod->sub != NULL)
        sub_ProcessSub(mnod->sub,GetChanTime(mnod->chn) / 100);

    if (!Mix_Playing(mnod->chn) || getGNode(mnod->syncto) == NULL)
        {
#ifdef TRACE
            if (!Mix_Playing(mnod->chn))
                printf("Not Played chan %d syncto %d \n",mnod->chn,mnod->syncto);
            else
                printf("NULL \n");
#endif
            snd_DeleteSync(nod);
            return NODE_RET_DELETE;
        }

    return NODE_RET_OK;
}



//// Pantracking
struct_action_res *snd_CreatePanTrack()
{
    struct_action_res *tmp;
    tmp = ScrSys_CreateActRes(NODE_TYPE_PANTRACK);

    tmp->nodes.node_pantracking = 0;
    return tmp;
}

int snd_ProcessPanTrack(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_PANTRACK)
        return NODE_RET_OK;

    struct_action_res *tr_nod = getGNode(nod->nodes.node_pantracking);
    if ( tr_nod == NULL)
        {
            snd_DeletePanTrack(nod);
            return NODE_RET_DELETE;
        }

    return NODE_RET_OK;
}

int snd_DeletePanTrack(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_PANTRACK)
        return NODE_RET_NO;

    struct_action_res *tr_nod = getGNode(nod->nodes.node_pantracking);
    if ( tr_nod != NULL)
        if (tr_nod->node_type == NODE_TYPE_MUSIC)
        {
            tr_nod->nodes.node_music->pantrack = false;
            Mix_SetPosition(tr_nod->nodes.node_music->chn, 0, tr_nod->nodes.node_music->attenuate);
            tr_nod->need_delete = true;
        }

    if (nod->slot > 0)
        setGNode(nod->slot,NULL);

    delete nod;

    return NODE_RET_DELETE;
}
