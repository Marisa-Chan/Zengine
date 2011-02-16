
#include "System.h"

MList    *wavs  =NULL;


void snd_DeleteLoopedWavsByOwner(pzllst *owner)
{
    StartMList(wavs);
    while (!eofMList(wavs))
    {
        musicnode *nod=(musicnode *)DataMList(wavs);
        if (nod->owner == owner && nod->looped)
        {
            Mix_HaltChannel(nod->chn);
            Mix_FreeChunk(nod->chunk);
            UnlockChan(nod->chn);
            if (nod->slot != 0)
                SetgVarInt(nod->slot,2);
            delete nod;
            DeleteCurrent(wavs);
        }
        NextMList(wavs);
    }
}

void snd_ProcessWavs()
{
    StartMList(wavs);

    while (!eofMList(wavs))
    {
        musicnode *mnod = (musicnode *) DataMList(wavs);

        if (!Mix_Playing(mnod->chn))
        {
            Mix_FreeChunk(mnod->chunk);
            SetgVarInt(mnod->slot,2);
            UnlockChan(mnod->chn);
            delete mnod;
            DeleteCurrent(wavs);
        }

        NextMList(wavs);
    }
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
