
#include "System.h"
#include "Sound.h"

#define TRY_CHANNELS 16
int CHANNELS=0;

bool ChanStatus[TRY_CHANNELS];

int audio_rate = 44100;
uint16_t audio_format = MIX_DEFAULT_FORMAT; /* 16-bit stereo */
int audio_channels = MIX_DEFAULT_CHANNELS;
int audio_buffers = 1024;

void InitMusic()
{
    Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers);
    memset(ChanStatus,0,TRY_CHANNELS * sizeof(bool));
    CHANNELS=Mix_AllocateChannels(TRY_CHANNELS);
}

void DeinitMusic()
{
    Mix_CloseAudio();
}

int GetFreeChannel()
{
    for (int i=0; i<CHANNELS; i++)
        if (ChanStatus[i] == false)
            return i;

    return -1;
}

void LockChan(int i)
{
    ChanStatus[i]=true;
}

void UnlockChan(int i)
{
    ChanStatus[i]=false;
}
