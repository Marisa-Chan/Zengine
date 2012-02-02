
#include "System.h"
#include "intro.h"
#include "Optimiz.h"

int8_t intro_play = 1;

#define ZUB 14
#define ZPT (ZUB *4)
#define ZCor 0.5
float z_rs[ZPT][2];

#define ZEpt 30
float z_ers[ZEpt][2] = {{0,0},{0,1},{0.52,1},{0.79,1},{1.05,1},{1.57,1},{2.09,1},{2.35,1},
                        {2.62,1},{3.14,1},{3.67,1},{3.93,1},{4.19,1},{4.71,1},{5.24,1},{5.49,1},
                        {5.49,0.5},{5.24,0.5},{4.71,0.45},{4.19,0.4},{3.93,0.4},{3.67,0.35},{3.14,0.35},{2.62,0.4},
                        {2.35,0.4},{2.09,0.45},{1.57,0.5},{1.05,0.5},{0.79,0.5},{1.57,0.35}};

#define Z_Zpt 10
float z_zrs[Z_Zpt][2] = {{0.07,0},{0.81,0},{0.20,0.92},{0.64,0.92},{0.81,0.68},
                         {0.74,1.0},{0,1.0},{0.61,0.1},{0.15,0.1},{0,0.33}};

Sint16 zzx[Z_Zpt],zzy[Z_Zpt];

#define Z_Npt 14
float z_nrs[Z_Npt][2] = {{0,0.2},{0.14,0.2},{0.14,0.3},{0.25,0.20},{0.45,0.2},
                         {0.65,0.43},{0.65,1.0},{0.51,1.0},{0.51,0.43},{0.42,0.33},
                         {0.27,0.33},{0.14,0.46},{0.14,1.0},{0,1.0}};

Sint16 znx[Z_Npt],zny[Z_Npt];
Sint16 zn2x[Z_Npt],zn2y[Z_Npt];

#define Z_Gpt 39
float z_grs[Z_Gpt][2] = {{0.56,0.19},{0.56,0.69},{0.49,0.81},{0.41,0.88},{0.28,0.88},
                         {0.18,0.80},{0.13,0.67},{0.13,0.49},{0.19,0.36},{0.28,0.30},
                         {0.41,0.30},{0.50,0.36},{0.56,0.45},{0.56,0.32},{0.46,0.21},
                         {0.35,0.18},{0.29,0.18},{0.20,0.20},{0.07,0.30},{0.01,0.46},
                         {0.01,0.71},{0.05,0.82},{0.17,0.94},{0.27,0.98},{0.39,0.98},
                         {0.48,0.93},{0.56,0.85},{0.56,1.00},{0.50,1.11},{0.41,1.17},
                         {0.22,1.17},{0.09,1.12},{0.09,1.23},{0.21,1.27},{0.41,1.27},
                         {0.51,1.24},{0.62,1.14},{0.69,0.96},{0.69,0.19}};

Sint16 zgx[Z_Gpt],zgy[Z_Gpt];


#define Z_Ipt 4
float z_irs[Z_Ipt][2] = {{0.13,1.00},{0.00,1.00},{0.00,0.20},
                         {0.13,0.20}};

Sint16 zix[Z_Ipt],ziy[Z_Ipt];


#define Z_E2pt 30
float z_e2rs[Z_E2pt][2] = {{0.71,0.63},{0.71,0.48},{0.62,0.28},{0.43,0.18},{0.30,0.18},
                           {0.12,0.26},{0.02,0.42},{0.00,0.60},{0.06,0.82},{0.17,0.95},
                           {0.33,1.00},{0.47,1.00},{0.59,0.97},{0.68,0.94},{0.68,0.83},
                           {0.58,0.87},{0.45,0.90},{0.29,0.89},{0.19,0.81},{0.13,0.67},
                           {0.13,0.52},{0.16,0.40},{0.23,0.32},{0.33,0.29},{0.41,0.29},
                           {0.51,0.34},{0.57,0.42},{0.58,0.52},{0.13,0.52},{0.13,0.63}};

Sint16 ze2x[Z_E2pt],ze2y[Z_E2pt];

int32_t vrx;



float in_an = 0;

int32_t icx,icy;

int32_t introstage = 0;
int32_t introtime = 0;

uint32_t scrn_color = 0xFFFFFFFF;
int32_t intr_param = 0;



#define intr_pauses 2000

void intro_setup()
{
    int32_t min = GAME_H;

    if (GAME_H > GAME_W)
        min = GAME_W;

    icx=GAME_W / 2;
    icy=GAME_H / 2;

    float scale = min/10;

    float ZRad = scale;
    float ZZub = ZRad * 0.1;

    for (int32_t i=0;i<ZUB;i++)
    {
        float stangl=6.2831852 / float(ZUB);
        float curangl = stangl * float(i);

        float k_1 = stangl / 2.0;
        float k_2 = k_1 * ZCor;

        z_rs[i*4 + 0][0] = curangl;
        z_rs[i*4 + 1][0] = curangl+k_1-k_2;
        z_rs[i*4 + 2][0] = curangl+k_1;
        z_rs[i*4 + 3][0] = curangl+k_1+k_1-k_2;
        z_rs[i*4 + 0][1] = ZRad+ZZub;
        z_rs[i*4 + 1][1] = ZRad+ZZub;
        z_rs[i*4 + 2][1] = ZRad-ZZub;
        z_rs[i*4 + 3][1] = ZRad-ZZub;
    }

    for (int32_t i=0;i<ZEpt;i++)
        z_ers[i][1] *= scale*0.65;

    float tx_width = scale*8.4;

    vrx = icx -(tx_width/2.0) +(scale*1.5)+scale*0.9;

    for (int32_t i=0;i<Z_Zpt;i++)
    {
        zzx[i] = icx-(tx_width/2.0) + z_zrs[i][0] * scale*1.5;
        zzy[i] = icy-(scale*0.75)+z_zrs[i][1] * scale*1.5;
    }
    for (int32_t i=0;i<Z_Npt;i++)
    {
        znx[i] = icx-(tx_width/2.0) +(scale*3.6) + z_nrs[i][0] * scale*1.5;
        zny[i] = icy-(scale*0.75)+z_nrs[i][1] * scale*1.5;

        zn2x[i] = icx-(tx_width/2.0) +(scale*6.3) + z_nrs[i][0] * scale*1.5;
        zn2y[i] = zny[i];
    }

    for (int32_t i=0;i<Z_Gpt;i++)
    {
        zgx[i] = icx-(tx_width/2.0) +(scale*4.7) +z_grs[i][0] * scale*1.5;
        zgy[i] = icy-(scale*0.75)+z_grs[i][1] * scale*1.5;
    }

    for (int32_t i=0;i<Z_Ipt;i++)
    {
        zix[i] = icx-(tx_width/2.0) +(scale*5.9) +z_irs[i][0] * scale*1.5;
        ziy[i] = icy-(scale*0.75)+z_irs[i][1] * scale*1.5;
    }

    for (int32_t i=0;i<Z_E2pt;i++)
    {
        ze2x[i] = icx-(tx_width/2.0) +(scale*7.4) +z_e2rs[i][0] * scale*1.5;
        ze2y[i] = icy-(scale*0.75)+z_e2rs[i][1] * scale*1.5;
    }

    intr_param = 0;
    scrn_color = SDL_MapRGBA(Rend_GetWindowSurface()->format,0xFF,0xFF,0xFF,0xFF);
}



void intro_draw_radcoord(int32_t x, int32_t y, float angl, float points[][2], int32_t num_pts,int32_t r, int32_t g, int32_t b, int32_t a)
{
    Sint16 tmpx[100],tmpy[100];

    for (int32_t i=0;i<num_pts;i++)
    {
        tmpx[i] = x + sin(points[i][0] + angl) *(points[i][1]);
        tmpy[i] = y + cos(points[i][0] + angl) *(points[i][1]);
    }
    filledPolygonRGBA(Rend_GetWindowSurface(),tmpx,tmpy,num_pts,r,g,b,a);
}

void intro_draw()
{
    SDL_FillRect(Rend_GetWindowSurface(),NULL,scrn_color);

    introtime -= GetDTime();

    in_an += 0.03;

    switch(introstage)
    {
    case 0:
    {
        int32_t fl_cl = 0xFF-intr_param *9;
        intro_draw_radcoord(vrx,icy,in_an,z_rs,ZPT,fl_cl,fl_cl,fl_cl,0xFF);
        intro_draw_radcoord(vrx,icy,in_an,z_ers,ZEpt,0xFF,fl_cl,fl_cl,0xFF);

        filledPolygonRGBA(Rend_GetWindowSurface(),zzx,zzy,Z_Zpt,fl_cl,fl_cl,fl_cl,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),znx,zny,Z_Npt,fl_cl,fl_cl,fl_cl,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zgx,zgy,Z_Gpt,fl_cl,fl_cl,fl_cl,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zix,ziy,Z_Ipt,fl_cl,fl_cl,fl_cl,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zn2x,zn2y,Z_Npt,fl_cl,fl_cl,fl_cl,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),ze2x,ze2y,Z_E2pt,fl_cl,fl_cl,fl_cl,0xFF);

        if (introtime <= 0)
        {
            introtime = 1000/30;
            if (intr_param < 28)
                intr_param ++;
            else
            {
                intr_param = 0;
                introtime = intr_pauses;
                introstage = 1;
            }

        }

    }

    break;

    case 1:
    {
        intro_draw_radcoord(vrx,icy,in_an,z_rs,ZPT,0,0,0,0xFF);
        intro_draw_radcoord(vrx,icy,in_an,z_ers,ZEpt,0xFF,0,0,0xFF);

        filledPolygonRGBA(Rend_GetWindowSurface(),zzx,zzy,Z_Zpt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),znx,zny,Z_Npt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zgx,zgy,Z_Gpt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zix,ziy,Z_Ipt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zn2x,zn2y,Z_Npt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),ze2x,ze2y,Z_E2pt,0,0,0,0xFF);

        if (introtime <= 0)
        {
            introtime = 1000/30;
            intr_param = 0;
            introstage = 2;
        }
    }
    break;
    case 2:
    {
        intro_draw_radcoord(vrx,icy,in_an,z_rs,ZPT,0,0,0,0xFF);
        intro_draw_radcoord(vrx,icy,in_an,z_ers,ZEpt,0xFF,0,0,0xFF);

        filledPolygonRGBA(Rend_GetWindowSurface(),zzx,zzy,Z_Zpt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),znx,zny,Z_Npt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zgx,zgy,Z_Gpt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zix,ziy,Z_Ipt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),zn2x,zn2y,Z_Npt,0,0,0,0xFF);
        filledPolygonRGBA(Rend_GetWindowSurface(),ze2x,ze2y,Z_E2pt,0,0,0,0xFF);

        int32_t fl_cl = 0xFF-intr_param *9;
        scrn_color = SDL_MapRGBA(Rend_GetWindowSurface()->format,fl_cl,fl_cl,fl_cl,0xFF);

        if (introtime <= 0)
        {
            introtime = 1000/30;
            if (intr_param < 28)
                intr_param ++;
            else
            {
                intr_param = 0;
                introtime = intr_pauses;
                introstage = 3;
                scrn_color = SDL_MapRGBA(Rend_GetWindowSurface()->format,0,0,0,0xFF);
            }

        }
    }
    break;
    case 3:
    {

        intro_draw_radcoord(vrx,icy,in_an,z_ers,ZEpt,0xFF,0,0,0xFF);


        if (introtime <= 0)
        {
            introtime = 1000/30;
            intr_param = 0;
            introstage = 4;
        }
    }
    break;
    case 4:
    {


        int32_t fl_cl = 0xFF-intr_param *9;
        intro_draw_radcoord(vrx,icy,in_an,z_ers,ZEpt,fl_cl,0,0,0xFF);

        if (introtime <= 0)
        {
            introtime = 1000/30;
            if (intr_param < 28)
                intr_param ++;
            else
            {
                intr_param = 0;
                intro_play = 0;
                SDL_FillRect(Rend_GetWindowSurface(),NULL,SDL_MapRGBA(Rend_GetWindowSurface()->format,0,0,0,0xFF));
            }

        }
    }
    break;

    }


    Rend_ScreenFlip();

    if (KeyHit(SDLK_SPACE))
    {
        FlushKeybKey(SDLK_SPACE);
        SDL_FillRect(Rend_GetWindowSurface(),NULL,SDL_MapRGBA(Rend_GetWindowSurface()->format,0,0,0,0xFF));
        intro_play = 0;
    }


}

int8_t intro_ended()
{
    return intro_play;
}
