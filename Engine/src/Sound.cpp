#include "System.h"
#include "Sound.h"


int CHANNELS=0;

uint8_t chanvol[TRY_CHANNELS];

uint32_t chantime[TRY_CHANNELS];

bool ChanStatus[TRY_CHANNELS];

int audio_rate = 44100;
uint16_t audio_format = MIX_DEFAULT_FORMAT; /* 16-bit stereo */
int audio_channels = MIX_DEFAULT_CHANNELS;
int audio_buffers = 1024;

const int SoundVol[101] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,\
                           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,\
                           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2,\
                           3, 3, 3, 4, 4, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11, 12, 14, 15, 17,\
                           18, 20, 23, 26, 29, 32, 36, 41, 46, 51, 57, 64, 72, 81, 91, 102, 114, 127};

int GetLogVol(uint8_t linear)
{
    if (linear < 101)
        return SoundVol[linear];
    else if (linear > 100)
        return SoundVol[100];
    else
        return 0;
}

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
    if (i>=0 && i < CHANNELS)
        {
            ChanStatus[i]=true;
            chantime[i]=SDL_GetTicks();
        }

}

void UnlockChan(int i)
{
    if (i>=0 && i < CHANNELS)
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

uint32_t GetChanTime(int i)
{
    if (i>=0 && i < CHANNELS)
        if  (ChanStatus[i])
            return SDL_GetTicks() - chantime[i];
    return 0;
}
