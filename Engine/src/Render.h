#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED


#define RENDER_FLAT 0
#define RENDER_PANA 1

void Rend_DrawImageToScr(SDL_Surface *scr,int x, int y);

void Rend_DrawImageToGamescr(SDL_Surface *scr,int x, int y);
void Rend_DrawImageToGamescr(anim_surf *scr,int x, int y, int frame);

void Rend_LoadGamescr(char *path);

void Rend_ProcessCursor();

int  Rend_GetMouseGameX();
int  Rend_GetMouseGameY();
bool Rend_MouseInGamescr();

void Rend_SetRenderer(int meth);
void Rend_SetFishTable(double angl, double k);
void Rend_DrawImageUpGamescr(SDL_Surface *scr,int x, int y);

int  Rend_GetRenderer();
void Rend_ProcessCursor();

void Rend_RenderFunc();
void Rend_InitGraphics(bool fullscreen);

void Rend_SetDelay(int delay);

#define GAMESCREEN_W 640
#define GAMESCREEN_P 20
#define GAMESCREEN_H 344

#endif // RENDER_H_INCLUDED
