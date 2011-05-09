#ifndef SOUNDFX_H_INCLUDED
#define SOUNDFX_H_INCLUDED


struct musicnode
{
    Mix_Chunk  *chunk;
    int         chn;
    bool        looped;
};

void snd_DeleteLoopedWavsByOwner(pzllst *owner);
int snd_ProcessWav(struct_action_res *nod);
void snd_DeleteWavs();
void snd_DeleteWav(struct_action_res *nod);

void snd_AddToWavsList(void *item);
void snd_InitWavsList();
void snd_DeleteWavsList();

MList *snd_GetWavsList();

#endif // SOUNDFX_H_INCLUDED
