#ifndef SOUNDFX_H_INCLUDED
#define SOUNDFX_H_INCLUDED


struct musicnode
{
    uint32_t    slot;
    Mix_Chunk  *chunk;
    int         chn;
    bool        looped;
    pzllst     *owner;
};

void snd_DeleteLoopedWavsByOwner(pzllst *owner);
void snd_ProcessWavs();
void snd_DeleteWavs();

void snd_AddToWavsList(void *item);
void snd_InitWavsList();
void snd_DeleteWavsList();

MList *snd_GetWavsList();

#endif // SOUNDFX_H_INCLUDED
