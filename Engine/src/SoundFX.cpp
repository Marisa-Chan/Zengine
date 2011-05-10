
#include "System.h"

MList    *wavs  =NULL;



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


    return NODE_RET_OK;
}

void snd_DeleteWav(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_MUSIC)
        return;

    if (Mix_Playing(nod->nodes.node_music->chn))
        Mix_HaltChannel(nod->nodes.node_music->chn);

    Mix_FreeChunk(nod->nodes.node_music->chunk);
    SetgVarInt(nod->slot,2);
    UnlockChan(nod->nodes.node_music->chn);
    delete nod->nodes.node_music;
    delete nod;

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
    tmp->nodes.node_music->attenuate  = 0;

    return tmp;
}

void snd_DeleteWavs()
{
    Mix_HaltChannel(-1);

    StartMList(wavs);
    while (!eofMList(wavs))
    {
        musicnode *nod=(musicnode *)DataMList(wavs);
        if (!nod->chunk)
            Mix_FreeChunk(nod->chunk);
        UnlockChan(nod->chn);
        delete nod;

        NextMList(wavs);
    }

    FlushMList(wavs);
}

void snd_InitWavsList()
{
    wavs = CreateMList();
}

void snd_DeleteWavsList()
{
    snd_DeleteWavs();
    DeleteMList(wavs);
    wavs = NULL;
}

void snd_AddToWavsList(void *item)
{
    AddToMList(wavs,item);
}

MList * snd_GetWavsList()
{
    return wavs;
}
