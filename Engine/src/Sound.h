#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <SDL/SDL_mixer.h>

extern const int SoundVol[101];


void InitMusic();
void DeinitMusic();

int GetFreeChannel();
void LockChan(int i);
void UnlockChan(int i);

void SaveVol();
void SilenceVol();
void RestoreVol();


#endif // SOUND_H_INCLUDED
