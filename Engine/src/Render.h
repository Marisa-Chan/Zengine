#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED


#define RENDER_FLAT 0
#define RENDER_PANA 1

void Rend_DrawImageToScr(SDL_Surface *scr,int x, int y);

void Rend_DrawImageToGamescr(SDL_Surface *scr,int x, int y);
void Rend_DrawImageToGamescr(anim_surf *scr,int x, int y, int frame);

void Rend_LoadGamescr(char *path);

void Rend_ProcessCursor();

int Rend_GetMouseGameX();
int Rend_GetMouseGameY();
bool Rend_MouseInGamescr();

void Rend_SetRenderer(int meth);
void SetFishTable(double angl, double k);
void Rend_DrawImageUpGamescr(SDL_Surface *scr,int x, int y);

void FlatRender();
void MakeImageEye(SDL_Surface *srf,SDL_Surface *nw,double dStrength);
void PanaRender();
int Rend_GetRenderer();

void Rend_ProcessCursor();

#endif // RENDER_H_INCLUDED
