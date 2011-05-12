#ifndef SOUNDFX_H_INCLUDED
#define SOUNDFX_H_INCLUDED


struct musicnode
{
    Mix_Chunk  *chunk;
    int         chn;
    int         volume;
    bool        looped;

    bool        crossfade;
    struct crossfade_params
    {
        int deltavolume;
        int times;
    } crossfade_params;

    int         attenuate;

    bool        pantrack;
    int         pantrack_X;

    int         pantrack_angle;

    bool        universe; //universe_music or music
};

void snd_DeleteLoopedWavsByOwner(pzllst *owner);
void snd_DeleteNoUniverse(pzllst *owner);
int snd_ProcessWav(struct_action_res *nod);
void snd_DeleteWavs();
void snd_DeleteWav(struct_action_res *nod);
struct_action_res *snd_CreateWavNode();

void snd_AddToWavsList(void *item);
void snd_InitWavsList();
void snd_DeleteWavsList();

MList *snd_GetWavsList();

#endif // SOUNDFX_H_INCLUDED
