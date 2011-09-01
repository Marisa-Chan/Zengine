#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include "Subtitles.h"

#define RENDER_FLAT 0
#define RENDER_PANA 1
#define RENDER_TILT 2


struct struct_SubRect
{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    SDL_Surface *img;
    int32_t id;
    bool todelete;
};

void Rend_DrawImageToScr(SDL_Surface *scr,int x, int y);

void Rend_DrawImageToGamescr(SDL_Surface *scr,int x, int y);
void Rend_DrawImageToGamescr(anim_surf *scr,int x, int y, int frame);

void Rend_LoadGamescr(char *path);

void Rend_ProcessCursor();

int  Rend_GetMouseGameX();
int  Rend_GetMouseGameY();
bool Rend_MouseInGamescr();

int Rend_GetPanaWidth();
void Rend_SetReversePana(bool pana);

void Rend_SetRenderer(int meth);
void Rend_pana_SetTable();
void Rend_pana_SetAngle(float angle);
void Rend_pana_SetLinscale(float linscale);

void Rend_tilt_SetTable();
void Rend_tilt_SetAngle(float angle);
void Rend_tilt_SetLinscale(float linscale);
void Rend_DrawTilt();

float Rend_GetRendererAngle();
float Rend_GetRendererLinscale();
void Rend_SetRendererAngle(float angle);
void Rend_SetRendererLinscale(float lin);
void Rend_SetRendererTable();

void Rend_DrawImageUpGamescr(SDL_Surface *scr,int x, int y);
void Rend_DrawImageUpGamescr(anim_surf *scr,int x, int y, int frame);

int  Rend_GetRenderer();
void Rend_ProcessCursor();

void Rend_PanaMouseInteract();
void Rend_MouseInteractOfRender();
void Rend_tilt_MouseInteract();

void Rend_RenderFunc();
void Rend_InitGraphics(bool fullscreen,char *fontpath);
void Rend_SwitchFullscreen();

void Rend_SetDelay(int32_t delay);


struct_SubRect *Rend_CreateSubRect(int x, int y, int w, int h);
void Rend_DeleteSubRect(struct_SubRect *erect);
void Rend_ClearSubs();
void Rend_InitSubList();
void Rend_ProcessSubs();
struct_SubRect *Rend_GetSubById( int id);

SDL_Surface *Rend_GetGameScreen();
SDL_Surface *Rend_GetWindowSurface();

uint32_t Rend_MapScreenRGB(int r, int g, int b);
void Rend_ScreenFlip();

struct struct_distort
{
    int32_t speed;
    float   st_angl;
    float  end_angl;
    float  dif_angl;
    float  rend_angl;
    float   st_lin;
    float  end_lin;
    float  dif_lin;
    float  rend_lin;
    int32_t frames;
    int32_t cur_frame;
    bool   increase;
    float  param1;
};

struct_action_res *Rend_CreateDistortNode();
int32_t Rend_ProcessDistortNode(struct_action_res *nod);
int32_t Rend_DeleteDistortNode(struct_action_res *nod);

#endif // RENDER_H_INCLUDED
