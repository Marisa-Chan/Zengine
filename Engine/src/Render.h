#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include "Subtitles.h"

#define RENDER_FLAT 0
#define RENDER_PANA 1
#define RENDER_TILT 2


#define COLOR_RGBA32(a,r,g,b) \
        r = a & 0xFF; \
        g = (a >> 8)  & 0xFF; \
        b = (a >> 16) & 0xFF;

#define COLOR_RGB16_565(a,r,g,b) \
        r = a & 0x1F; \
        g = (a >> 5 ) & 0x3F; \
        b = (a >> 11) & 0x1F;

#define COLOR_RGBA16_5551(a,r,g,b) \
        r = a & 0x1F; \
        g = (a >> 5 ) & 0x1F; \
        b = (a >> 10) & 0x1F;

#define COLOR_RGBA16_4444(a,r,g,b) \
        r = a & 0xF; \
        g = (a >> 4 ) & 0xF; \
        b = (a >> 8) & 0xF;


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
void Rend_pana_SetZeroPoint(int32_t point);

void Rend_tilt_SetTable();
void Rend_tilt_SetAngle(float angle);
void Rend_tilt_SetLinscale(float linscale);
void Rend_DrawTilt();
void Rend_DrawTilt_pre();

float Rend_GetRendererAngle();
float Rend_GetRendererLinscale();
void Rend_SetRendererAngle(float angle);
void Rend_SetRendererLinscale(float lin);
void Rend_SetRendererTable();

void Rend_DrawImageUpGamescr(SDL_Surface *scr,int x, int y);
void Rend_DrawImageUpGamescr(anim_surf *scr,int x, int y, int frame);

void Rend_DrawScalerToGamescr(scaler *scl,int16_t x, int16_t y);

int  Rend_GetRenderer();
void Rend_ProcessCursor();

void Rend_PanaMouseInteract();
void Rend_MouseInteractOfRender();
void Rend_tilt_MouseInteract();

void Rend_RenderFunc();
void Rend_InitGraphics(bool fullscreen,char *fontpath);
void Rend_SwitchFullscreen();

void Rend_SetDelay(int32_t delay);

SDL_Surface *Rend_GetLocationScreenImage();

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
int Rend_deleteRegion(struct_action_res *nod);


int8_t Rend_GetScreenPart(int32_t *x, int32_t *y, int32_t w, int32_t h, SDL_Surface *dst);


/***************Effects section******************/

#define EFFECT_WAVE  1
#define EFFECT_LIGH  2
#define EFFECT_9     4

void Effects_Init();
void Effects_Process();
int32_t Effects_FindFree();
void Effects_Delete(uint32_t index);
struct_effect * Effects_GetEf(uint32_t index);
int32_t Effects_AddEffect(int32_t type);
int32_t Effects_GetColor(uint32_t x, uint32_t y);

int32_t Rend_EF_Wave_Setup(int32_t delay, int32_t frames, int32_t s_x, int32_t s_y, float apml, float waveln, float spd);
int32_t Rend_EF_Light_Setup(char *string, int32_t x, int32_t y, int32_t w, int32_t h, int32_t delay,int32_t steps);
int32_t Rend_EF_9_Setup(char *mask, char *clouds, int32_t delay,int32_t x, int32_t y, int32_t w, int32_t h);
void Rend_EF_9_Draw(struct_effect *ef);


void Rend_Effect(SDL_Surface *srf);//test-wave effect



struct effect0 //water
{
    int32_t frame;
    int32_t frame_cnt;
    int8_t  **ampls;
    SDL_Surface *surface;
};

struct effect1 //lightning
{
    int8_t  *map; // 0 - no; !0 - draw
    int8_t  sign;
    int32_t stp;
    int32_t maxstp;
    SDL_Surface *surface;
};


struct effect9
{
    int8_t *cloud_mod;
    SDL_Surface *cloud;
    SDL_Surface *mask;
    SDL_Surface *mapping;
};

struct struct_effect
{
    int32_t type;
    int32_t delay;
    int32_t time;
    int32_t w;
    int32_t h;
    int32_t x;
    int32_t y;
    union effect
    {
        effect0 ef0;
        effect1 ef1;
        effect9 ef9;
    } effect;
};

#endif // RENDER_H_INCLUDED
