#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
#include <smpeg/smpeg.h>

extern const int FiveBitToEightBitLookupTable [32];

SDL_Surface *InitGraphicAndSound(uint16_t width, uint16_t height, uint16_t bpp,bool ful, char *fontsdir);
void ConvertImage(SDL_Surface **tmp);
SDL_Surface *CreateSurface(uint16_t w,uint16_t h);

void DrawImage(SDL_Surface *surf, int16_t x, int16_t y);
void DrawImageToSurf(SDL_Surface *surf, int16_t x, int16_t y,SDL_Surface *dest);


struct Anim_info
{
    uint32_t w;
    uint32_t h;
    uint32_t time;
    uint32_t frames;
};

struct anim_surf
{
    SDL_Surface **img;
    Anim_info    info;
};

struct anim_avi
{
    SDL_Surface *img;
    SMPEG       *mpg;
    SMPEG_Info   inf;
    bool         pld;
    bool        loop;
};

struct struct_graph_font
{
    char Name[64];
    char path[255];
};

anim_surf *LoadAnimImage(char *file,int32_t mask);
void DrawAnimImage(anim_surf *anim, int x, int y, int frame);
void DrawAnimImageToSurf(anim_surf *anim, int x, int y, int frame,SDL_Surface *surf);
void FreeAnimImage(anim_surf *anim);


SDL_Surface *LoadConvertImg(char *file);
SDL_Surface *LoadConvertImg(char *file,uint32_t key);

TTF_Font *GetFontByName(char *name,int size);

#endif // GRAPH_H_INCLUDED
