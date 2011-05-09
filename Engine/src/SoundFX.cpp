
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
                    Mix_HaltChannel(nod->nodes.node_music->chn);
                    Mix_FreeChunk(nod->nodes.node_music->chunk);
                    UnlockChan(nod->nodes.node_music->chn);
                    if (nod->slot != 0)
                        SetgVarInt(nod->slot,2);
                    delete nod->nodes.node_music;
                    delete nod;
                    DeleteCurrent(allres);
                }
        NextMList(allres);
    }
}

int snd_ProcessWav(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_MUSIC)
        return NODE_RET_OK;

        if (!Mix_Playing(nod->nodes.node_music->chn))
        {
            Mix_FreeChunk(nod->nodes.node_music->chunk);
            SetgVarInt(nod->slot,2);
            UnlockChan(nod->nodes.node_music->chn);
            delete nod->nodes.node_music;
            delete nod;
            return NODE_RET_DELETE;
        }

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

void snd_DeleteWavs()
{
    Mix_HaltChannel(-1);

    StartMList(wavs);
    while (!eofMList(wavs))
    {
        musicnode *nod=(musicnode *)DataMList(wavs);
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
