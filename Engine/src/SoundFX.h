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

    struct_subtitles *sub;
};

void snd_DeleteLoopedWavsByOwner(pzllst *owner);
void snd_DeleteNoUniverse(pzllst *owner);

int snd_ProcessWav(struct_action_res *nod);
int snd_DeleteWav(struct_action_res *nod);

struct_action_res *snd_CreateWavNode();






struct struct_syncnode
{
    int         syncto;
    Mix_Chunk  *chunk;
    int         chn;

    struct_subtitles *sub;
};

int snd_ProcessSync(struct_action_res *nod);
int snd_DeleteSync(struct_action_res *nod);
struct_action_res *snd_CreateSyncNode();




struct_action_res *snd_CreatePanTrack();
int snd_DeletePanTrack(struct_action_res *nod);

#endif // SOUNDFX_H_INCLUDED
