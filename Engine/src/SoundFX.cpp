
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

        if (Rend_GetRenderer() != RENDER_PANA)
        {
            mnod->pantrack = false;
            Mix_SetPosition(mnod->chn, 0, mnod->attenuate);
        }

        if (mnod->pantrack)
        {
            int PlX = GetgVarInt(SLOT_LOCATION_CUR_X);
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
    delete nod->nodes.node_music;
    delete nod;

    return NODE_RET_DELETE;
}

struct_action_res *snd_CreateWavNode()
{
    struct_action_res *tmp;
    tmp = new (struct_action_res);
    tmp->slot = 0;
    tmp->node_type = NODE_TYPE_MUSIC;
    tmp->owner = NULL;

    tmp->nodes.node_music = new (musicnode);
    tmp->nodes.node_music->chn = 0;
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

    return tmp;
}


