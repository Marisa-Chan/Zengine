#include "System.h"
#include "Sound.h"

#define TRY_CHANNELS 16
int CHANNELS=0;

uint8_t chanvol[TRY_CHANNELS];

bool ChanStatus[TRY_CHANNELS];

int audio_rate = 44100;
uint16_t audio_format = MIX_DEFAULT_FORMAT; /* 16-bit stereo */
int audio_channels = MIX_DEFAULT_CHANNELS;
int audio_buffers = 1024;

const int SoundVol[101] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,\
                           2, 2, 2, 3, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 13, 14, 16,\
                           18, 20, 23, 26, 29, 32, 36, 41, 46, 51, 57, 64, 72, 81, 91, 102, 114, 127};



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


void SaveVol()
{
    for (int i=0; i < CHANNELS; i++)
        chanvol[i] = Mix_Volume(i,-1);
}

void SilenceVol()
{
    for (int i=0; i < CHANNELS; i++)
        Mix_Volume(i,0);
}

void RestoreVol()
{
    for (int i=0; i < CHANNELS; i++)
        Mix_Volume(i,chanvol[i]);
}
