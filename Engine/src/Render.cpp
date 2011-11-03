#include "System.h"
#include "Optimiz.h"


uint8_t     Renderer = RENDER_FLAT;


MList *sublist = NULL;
int32_t subid=0;

//Game window surface
SDL_Surface *screen;

//Surface loaded by setscreen, all changes by setpartialscreen and other similar modify this surface.
SDL_Surface *scrbuf=NULL;

//This surface used for effects(region action) and control draws.
SDL_Surface *tempbuf=NULL;

//This surface used for rendered viewport image with renderer processing.
SDL_Surface *viewportbuf=NULL;

int32_t RenderDelay = 0;


struct xy
{
    int32_t x;
    int32_t y;
};

xy render_table[GAMESCREEN_W][GAMESCREEN_H];
int32_t new_render_table[GAMESCREEN_W*GAMESCREEN_H];

int32_t  *view_X;

int32_t pana_PanaWidth=1800;
bool    pana_ReversePana = false;
float   pana_angle=60.0, pana_linscale=1.00;



void Rend_pana_SetAngle(float angle)
{
    pana_angle = angle;
}

void Rend_pana_SetLinscale(float linscale)
{
    pana_linscale = linscale;
}

void Rend_SetDelay(int32_t delay)
{
    RenderDelay = delay;
}

void Rend_indexer()
{
    int32_t previndx= new_render_table[0];

    for(int32_t ff=1; ff<GAMESCREEN_H*GAMESCREEN_W; ff++)
    {
        int32_t curindx=new_render_table[ff];
        new_render_table[ff]=curindx-previndx;
        previndx = curindx;
    }
}

void Rend_pana_SetTable()
{
    float angl = pana_angle;
    float k = pana_linscale;
    memset(render_table,0,sizeof(xy)*GAMESCREEN_W*GAMESCREEN_H);

    int32_t yy=GAMESCREEN_H;
    int32_t ww=GAMESCREEN_W;

    double half_w = (double)ww / 2.0;
    double half_h = (double)yy / 2.0;

    double angle  = (angl * 3.14159265 / 180.0 );
    double hhdtan = half_h / tan(angle);
    double tandhh = tan(angle)/half_h;

    for (int32_t x=0; x<ww; x++)
    {
        double poX = (double)x - half_w +0.01; //0.01 - for zero tan/atan issue (vertical line on half of screen)

        double tmx = atan(poX*tandhh);
        double nX  = k * hhdtan * tmx;
        double nn  = cos(tmx);
        double nhw = half_h * nn * hhdtan * tandhh*2.0;

        int32_t relx   = floor(nX);// + half_w);
        double yk  = nhw / (double)yy;

        double et2=((double)yy-nhw)/2.0;

        for (int32_t y=0; y<yy; y++)
        {
            double et1=(double)y*yk;

            int32_t newx= relx;
            render_table[x][y].x = newx; //pixel index

            newx+=GAMESCREEN_W_2;

            int32_t newy= floor(et2+et1);

            render_table[x][y].y = newy; //pixel index

            if (newx<0)
                newx = 0;
            if (newx>=GAMESCREEN_W)
                newx = GAMESCREEN_W-1;
            if (newy<0)
                newy = 0;
            if (newy>=GAMESCREEN_H)
                newy = GAMESCREEN_H-1;

            new_render_table[x+y*GAMESCREEN_W] = newx + newy*GAMESCREEN_W; //pixel index
        }
    }

    Rend_indexer();
}

void Rend_InitGraphics(bool fullscreen, char *fontsdir)
{
    screen=InitGraphicAndSound(GAME_W,GAME_H,GAME_BPP,fullscreen,fontsdir);


    Rend_InitSubList();


    tempbuf = CreateSurface(GAMESCREEN_W,GAMESCREEN_H);
    viewportbuf = CreateSurface(GAMESCREEN_W,GAMESCREEN_H);



    Mouse_LoadCursors();

    SDL_ShowCursor(SDL_DISABLE);

    view_X = getdirectvar(SLOT_VIEW_POS);

}

void Rend_SwitchFullscreen()
{
    screen = SwitchFullscreen();
}

void Rend_DrawImageToGamescr(SDL_Surface *scr,int x, int y)
{
    if (scrbuf)
        DrawImageToSurf(scr,x,y,scrbuf);
}

void Rend_DrawImageToGamescr(anim_surf *scr,int x, int y, int frame)
{
    if (scrbuf)
        DrawAnimImageToSurf(scr,x,y,frame,scrbuf);
}

void Rend_DrawImageUpGamescr(SDL_Surface *scr,int x, int y)
{
    if (tempbuf)
        DrawImageToSurf(scr,x,y,tempbuf);
}

void Rend_DrawImageUpGamescr(anim_surf *scr,int x, int y, int frame)
{
    if (tempbuf)
        DrawAnimImageToSurf(scr,x,y,frame,tempbuf);
}

void Rend_DrawImageToScr(SDL_Surface *scr,int x, int y)
{
    if (screen)
        DrawImageToSurf(scr,x,y,screen);
}

void Rend_LoadGamescr(char *path)
{
    if (scrbuf)
        SDL_FreeSurface(scrbuf);

    scrbuf=LoadConvertImg(path);

    SDL_FillRect(tempbuf,0,0);

    if (!scrbuf)
        printf("ERROR:  IMG_Load(%s): %s\n\n",path, IMG_GetError());

    if (!scrbuf) // no errors if no screen
        scrbuf=CreateSurface(GAMESCREEN_W,GAMESCREEN_H);

    if (Renderer != RENDER_TILT)
        pana_PanaWidth = scrbuf->w;
    else
        pana_PanaWidth = scrbuf->h;
}


void Rend_ProcessCursor()
{
    if (GetgVarInt(SLOT_INVENTORY_MOUSE) != 0)
    {
        if (GetgVarInt(SLOT_INVENTORY_MOUSE) != Mouse_GetCurrentObjCur())
            Mouse_LoadObjCursor( GetgVarInt(SLOT_INVENTORY_MOUSE) );

        if (Mouse_IsCurrentCur(CURSOR_ACTIVE) || Mouse_IsCurrentCur(CURSOR_HANDPU) || Mouse_IsCurrentCur(CURSOR_IDLE))
        {
            if (Mouse_IsCurrentCur(CURSOR_ACTIVE) || Mouse_IsCurrentCur(CURSOR_HANDPU))
                Mouse_SetCursor(CURSOR_OBJ_1);
            else
                Mouse_SetCursor(CURSOR_OBJ_0);
        }
    }


    if (Renderer == RENDER_PANA)
        if (Rend_MouseInGamescr())
        {
            if (MouseX() < GAMESCREEN_X+GAMESCREEN_P)
                Mouse_SetCursor(CURSOR_LEFT);
            if (MouseX() > GAMESCREEN_X+GAMESCREEN_W - GAMESCREEN_P)
                Mouse_SetCursor(CURSOR_RIGH);
        }
    if (Renderer == RENDER_TILT)
        if (Rend_MouseInGamescr())
        {
            if (MouseY() < GAMESCREEN_Y + GAMESCREEN_P)
                Mouse_SetCursor(CURSOR_UPARR);
            if (MouseY() > GAMESCREEN_Y + GAMESCREEN_H - GAMESCREEN_P)
                Mouse_SetCursor(CURSOR_DWNARR);
        }


    Mouse_DrawCursor(MouseX(),MouseY());
}

bool Rend_MouseInGamescr()
{
    return MouseInRect(GAMESCREEN_X,GAMESCREEN_Y,GAMESCREEN_W,GAMESCREEN_H);
}

int Rend_GetMouseGameX()
{
    int32_t tmpl;
    int32_t tmp;
    switch(Renderer)
    {
    case RENDER_FLAT:
        return MouseX() - GAMESCREEN_X - GAMESCREEN_FLAT_X;
        break;
    case RENDER_PANA:
        tmp = MouseY() - GAMESCREEN_Y;
        tmpl = MouseX() - GAMESCREEN_X;

        if (tmp >= 0 && tmp < GAMESCREEN_H && tmpl >= 0 && tmpl < GAMESCREEN_W)
            tmpl = render_table[tmpl][tmp].x;
        else
            tmpl = 0;

        tmpl += *view_X;
        if (tmpl < 0)
            tmpl += pana_PanaWidth;
        else if (tmpl > pana_PanaWidth)
            tmpl -= pana_PanaWidth;

        return tmpl;
        break;

    case RENDER_TILT:
        tmp = MouseY() - GAMESCREEN_Y;
        tmpl = MouseX() - GAMESCREEN_X;

        if (tmp >= 0 && tmp < GAMESCREEN_H && tmpl >= 0 && tmpl < GAMESCREEN_W)
            tmpl = render_table[tmpl][tmp].x;
        else
            tmpl = 0;

        //tmpl += GAMESCREEN_W_2;

        if (tmpl < 0)
            tmpl += GAMESCREEN_W;
        else if (tmpl > GAMESCREEN_W)
            tmpl -= GAMESCREEN_W;

        return tmpl;

        break;

    default:
        return MouseX() - GAMESCREEN_X;
    }
}

int Rend_GetMouseGameY()
{
    int32_t tmpl;
    int32_t tmp;
    switch(Renderer)
    {
    case RENDER_FLAT:
        return MouseY() - GAMESCREEN_Y;
        break;

    case RENDER_PANA:
        tmp = MouseY() - GAMESCREEN_Y;
        tmpl = MouseX() - GAMESCREEN_X;
        if (tmp >= 0 && tmp < GAMESCREEN_H && tmpl >= 0 && tmpl < GAMESCREEN_W)
            return render_table[tmpl][tmp].y;
        else
            return tmp;

        break;

    case RENDER_TILT:
        tmp = MouseY() - GAMESCREEN_Y;
        tmpl = MouseX() - GAMESCREEN_X;
        if (tmp >= 0 && tmp < GAMESCREEN_H && tmpl >= 0 && tmpl < GAMESCREEN_W)
            tmpl = render_table[tmpl][tmp].y;
        else
            tmpl = 0;

        tmpl+=*view_X;

        if (tmpl>pana_PanaWidth)
            tmpl=pana_PanaWidth;
        else if (tmpl<0)
            tmpl=0;

        return tmpl;

        break;

    default:
        return MouseY() - GAMESCREEN_Y;
    }
}

void Rend_SetRenderer(int meth)
{
    Renderer = meth;
    pana_ReversePana = false;
}

void Rend_SetReversePana(bool pana)
{
    pana_ReversePana = pana;
}

int Rend_GetRenderer()
{
    return Renderer;
}

void Rend_FlatRender()
{
    DrawImageToSurf(tempbuf,0,0,viewportbuf);
}

void Rend_FlatRender_pre()
{
    DrawImageToSurf(scrbuf,GAMESCREEN_FLAT_X,0,tempbuf);
}

//void Rend_DrawPanorama2()
//{
//    //hehe
//    //float hhx = 5.0/10.0;
//    /////
//    int asddsa= SDL_GetTicks();
//
//    SDL_LockSurface(tempbuf);
//    SDL_LockSurface(scrbuf);
//    if (GAME_BPP == 32)
//    {
//    int *nww = ((int *)tempbuf->pixels) ;
//    for(int y = 0; y < GAMESCREEN_H>>2; y++)
//    {
//        //int *nww = ((int *)tempbuf->pixels) + (tempbuf->w * y*4); // only for 32 bit color
//
//        for(int x = 0; x < GAMESCREEN_W>>2; x++)
//        {
//            // int *nww = (int *)screen->pixels;
//            int *old = (int *)scrbuf->pixels;    // only for 32 bit color
//
//            int newx = fishtable[(x<<2)][(y<<2)].x  /* *hhx */   + *PanaX;
//
//            if (newx < 0)
//                newx += scrbuf->w;
//            else if (newx > scrbuf->w)
//                newx -= scrbuf->w;
//
//            *nww = old[newx + fishtable[(x<<2)][(y<<2)].y * scrbuf->w];
//            nww++; //more faster than mul %)
//            *nww = old[newx + fishtable[(x<<2)+1][(y<<2)].y * scrbuf->w];
//            nww++;
//            *nww = old[newx + fishtable[(x<<2)+2][(y<<2)].y * scrbuf->w];
//            nww++; //more faster than mul %)
//            *nww = old[newx + fishtable[(x<<2)+3][(y<<2)].y * scrbuf->w];
//            nww++;
//            int *nw2=nww;
//
//            nw2+=tempbuf->w-4;
//            *nw2 = old[newx + fishtable[(x<<2)][(y<<2)+1].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+1][(y<<2)+1].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+2][(y<<2)+1].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+3][(y<<2)+1].y * scrbuf->w];
//
//            nw2+=tempbuf->w-3;
//            *nw2 = old[newx + fishtable[(x<<2)][(y<<2)+2].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+1][(y<<2)+2].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+2][(y<<2)+2].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+3][(y<<2)+2].y * scrbuf->w];
//
//            nw2+=tempbuf->w-3;
//            *nw2 = old[newx + fishtable[(x<<2)][(y<<2)+3].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+1][(y<<2)+3].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+2][(y<<2)+3].y * scrbuf->w];
//            nw2++; //more faster than mul %)
//            *nw2 = old[newx + fishtable[(x<<2)+3][(y<<2)+3].y * scrbuf->w];
//
//        }
//        nww+=tempbuf->w+tempbuf->w+tempbuf->w;
//    }
//    }
//    else if (GAME_BPP == 16)
//    {
//    for(int y = 0; y < GAMESCREEN_H; y++)
//    {
//        short *nww = ((short *)tempbuf->pixels) + (tempbuf->w * y); // only for 16 bit color
//
//        for(int x = 0; x < GAMESCREEN_W; x++)
//        {
//            // int *nww = (int *)screen->pixels;
//            short *old = (short *)scrbuf->pixels;    // only for 16 bit color
//
//            int newx = fishtable[x][y].x + *PanaX;
//
//            if (newx < 0)
//                newx += scrbuf->w;
//            else if (newx > scrbuf->w)
//                newx -= scrbuf->w;
//
//            *nww = old[newx + fishtable[x][y].y * scrbuf->w];
//            nww++; //more faster than mul %)
//        }
//    }
//    }
//    else
//    {
//        printf("Write panorama code for %d bpp\n",GAME_BPP);
//        exit(0);
//    }
//    SDL_UnlockSurface(tempbuf);
//    SDL_UnlockSurface(scrbuf);
//
//    printf("%d\n",SDL_GetTicks()-asddsa);
//}

void Rend_DrawPanorama_pre()
{
    DrawImageToSurf(scrbuf,GAMESCREEN_W_2-*view_X,0,tempbuf);
    if (*view_X<GAMESCREEN_W_2)
        DrawImageToSurf(scrbuf,GAMESCREEN_W_2-(*view_X+pana_PanaWidth),0,tempbuf);
    else if (pana_PanaWidth-*view_X<GAMESCREEN_W_2)
        DrawImageToSurf(scrbuf,GAMESCREEN_W_2+pana_PanaWidth-*view_X,0,tempbuf);

}

void Rend_DrawPanorama()
{
    SDL_LockSurface(tempbuf);
    SDL_LockSurface(viewportbuf);
    if (GAME_BPP == 32)
    {
        uint32_t *nww = (uint32_t *)viewportbuf->pixels;
        uint32_t *old = (uint32_t *)tempbuf->pixels;
        int32_t *ofs = new_render_table;
        for (int32_t ai=0;ai<GAMESCREEN_H*GAMESCREEN_W;ai++)
        {
            old+=*ofs;
            *nww = *old;
            nww++;
            ofs++;
        }
    }
    else if (GAME_BPP == 16)
    {
        uint16_t *nww = (uint16_t *)viewportbuf->pixels;
        uint16_t *old = (uint16_t *)tempbuf->pixels;
        int32_t *ofs = new_render_table;
        for (int32_t ai=0;ai<GAMESCREEN_H*GAMESCREEN_W;ai++)
        {
            old+=*ofs;
            *nww = *old;
            nww++;
            ofs++;
        }
    }
    else
    {
        printf("Write panorama code for %d bpp\n",GAME_BPP);
        exit(0);
    }
    SDL_UnlockSurface(tempbuf);
    SDL_UnlockSurface(viewportbuf);

//    SDL_LockSurface(tempbuf);
//    SDL_LockSurface(viewportbuf);
//    if (GAME_BPP == 32)
//    {
//    int *nww = ((int *)tempbuf->pixels);
//    int *old = (int *)scrbuf->pixels;    // only for 32 bit color
//    for(int y = 0; y < GAMESCREEN_H; y++)
//    {
//         // only for 32 bit color
//
//        for(int x = 0; x < GAMESCREEN_W; x++)
//        {
//            // int *nww = (int *)screen->pixels;
//
//            int newx = render_table[x][y].x  /* *hhx */  + GAMESCREEN_W2;
//
//            if (newx < 0)
//                newx += scrbuf->w;
//            else if (newx > scrbuf->w)
//                newx -= scrbuf->w;
//
//            *nww = old[newx + render_table[x][y].y * scrbuf->w];
//            nww++; //more faster than mul %)
//        }
//    }
//    }
//    else if (GAME_BPP == 16)
//    {
//    short *nww = ((short *)tempbuf->pixels); // only for 16 bit color
//    short *old = (short *)scrbuf->pixels;    // only for 16 bit color
//    for(int y = 0; y < GAMESCREEN_H; y++)
//    {
//        for(int x = 0; x < GAMESCREEN_W; x++)
//        {
//            // int *nww = (int *)screen->pixels;
//
//            int newx = render_table[x][y].x + GAMESCREEN_W2;
//
//            if (newx < 0)
//                newx += scrbuf->w;
//            else if (newx > scrbuf->w)
//                newx -= scrbuf->w;
//
//            *nww = old[newx + render_table[x][y].y * scrbuf->w];
//            nww++; //more faster than mul %)
//        }
//    }
//    }
//    else
//    {
//        printf("Write panorama code for %d bpp\n",GAME_BPP);
//        exit(0);
//    }
//    SDL_UnlockSurface(tempbuf);
//    SDL_UnlockSurface(viewportbuf);
   // printf("%d\n",SDL_GetTicks()-asddsa);
}

void Rend_PanaMouseInteract()
{
if (Rend_MouseInGamescr())
    {
        if (MouseX() > GAMESCREEN_X + GAMESCREEN_W - GAMESCREEN_P)
        {
            int32_t mspeed = GetgVarInt(SLOT_PANAROTATE_SPEED) >> 4;
            int32_t param  = (((MouseX() - GAMESCREEN_X - GAMESCREEN_W + GAMESCREEN_P) << 7 ) / GAMESCREEN_P * mspeed) >> 7;

            *view_X += (pana_ReversePana == false ? param: -param);
        }

        if (MouseX() < GAMESCREEN_X + GAMESCREEN_P)
        {
            int32_t mspeed = GetgVarInt(SLOT_PANAROTATE_SPEED) >> 4;
            int32_t param  = (((GAMESCREEN_X + GAMESCREEN_P - MouseX()) << 7) / GAMESCREEN_P * mspeed) >> 7;

            *view_X -= (pana_ReversePana == false ? param: -param);
        }
    }

    if (*view_X >= pana_PanaWidth)
        *view_X %= pana_PanaWidth;
    if (*view_X < 0)
        *view_X += pana_PanaWidth;
}

void Rend_MouseInteractOfRender()
{
    if (Renderer == RENDER_PANA)
        Rend_PanaMouseInteract();
    else if (Renderer == RENDER_TILT)
        Rend_tilt_MouseInteract();
}

int Rend_GetPanaWidth()
{
    return pana_PanaWidth;
}


void Rend_RenderFunc()
{
    if (RenderDelay>0)
    {
        //if (GetBeat())
        RenderDelay--;
        return;
    }

    SDL_FillRect(screen,0,0);

    //pre-rendered
    if (Renderer == RENDER_FLAT)
        Rend_FlatRender_pre();
    else if (Renderer == RENDER_PANA)
        Rend_DrawPanorama_pre();
    else if (Renderer == RENDER_TILT)
        Rend_DrawTilt();

    //draw dynamic controls
    Ctrl_DrawControls();

    //effect-processor
    Rend_Effect(tempbuf);

    //Apply renderer distortion
    if (Renderer == RENDER_FLAT)
        Rend_FlatRender();
    else if (Renderer == RENDER_PANA)
        Rend_DrawPanorama();
    else if (Renderer == RENDER_TILT)
        Rend_DrawTilt();

    //output viewport
    DrawImage(viewportbuf,GAMESCREEN_X,GAMESCREEN_Y);


    Rend_ProcessSubs();

    menu_DrawMenuBar();

    Rend_ProcessCursor();

}




struct_SubRect *Rend_CreateSubRect(int x, int y, int w, int h)
{
    struct_SubRect *tmp = new(struct_SubRect);

    tmp->h = h;
    tmp->w = w;
    tmp->x = x;
    tmp->y = y;
    tmp->todelete = false;
    tmp->id       = subid;

    subid++;

    tmp->img = CreateSurface(w,h);

    AddToMList(sublist,tmp);

    return tmp;
}

void Rend___DeleteSubRect(struct_SubRect *erect)
{
    SDL_FreeSurface(erect->img);
    delete erect;
}

void Rend_DeleteSubRect(struct_SubRect *erect)
{
    erect->todelete = true;
}

void Rend_ClearSubs()
{
    StartMList(sublist);
    while(!eofMList(sublist))
    {
        struct_SubRect *subrec = (struct_SubRect *)DataMList(sublist);
        Rend___DeleteSubRect(subrec);
        NextMList(sublist);
    }
    FlushMList(sublist);
}

void Rend_InitSubList()
{
    if (sublist == NULL)
        sublist = CreateMList();
    else
        Rend_ClearSubs();
    subid = 0;
}

void Rend_ProcessSubs()
{
    StartMList(sublist);
    while(!eofMList(sublist))
    {
        struct_SubRect *subrec = (struct_SubRect *)DataMList(sublist);

        if (subrec->todelete)
        {
            Rend___DeleteSubRect(subrec);
            DeleteCurrent(sublist);
        }
        else
            DrawImage(subrec->img,subrec->x+GAMESCREEN_FLAT_X,subrec->y);

        NextMList(sublist);
    }
}

struct_SubRect *Rend_GetSubById( int id)
{
    StartMList(sublist);
    while(!eofMList(sublist))
    {
        struct_SubRect *subrec = (struct_SubRect *)DataMList(sublist);
        if (subrec->id == id)
        {
            return subrec;
            break;
        }
        NextMList(sublist);
    }

    return NULL;
}

SDL_Surface *Rend_GetGameScreen()
{
    return tempbuf;
}

SDL_Surface *Rend_GetWindowSurface()
{
    return screen;
}

uint32_t Rend_MapScreenRGB(int r, int g, int b)
{
    return SDL_MapRGB(screen->format,r,g,b);
}

void Rend_ScreenFlip()
{
    SDL_Flip(screen);
}





float tilt_angle=60.0;
float tilt_linscale=1.0;
bool  tilt_Reverse = false;

void Rend_tilt_SetAngle(float angle)
{
    tilt_angle = angle;
}

void Rend_tilt_SetLinscale(float linscale)
{
    tilt_linscale = linscale;
}

void Rend_tilt_SetTable()
{
    float angl = tilt_angle;
    float k = tilt_linscale;
    memset(render_table,0,sizeof(xy)*GAMESCREEN_W*GAMESCREEN_H);

    int32_t yy=GAMESCREEN_H;
    int32_t xx=GAMESCREEN_W;

    double half_w = (double)xx / 2.0;
    double half_h = (double)yy / 2.0;

    double angle  = (angl * 3.14159265 / 180.0 );
    double hhdtan = half_w / tan(angle);
    double tandhh = tan(angle)/half_w;

    for (int32_t y=0; y<yy; y++)
    {
        double poY = (double)y - half_h +0.01; //0.01 - for zero tan/atan issue (vertical line on half of screen)

        double tmx = atan(poY*tandhh);
        double nX  = k * hhdtan * tmx;
        double nn  = cos(tmx);
        double nhw = half_w * nn * hhdtan * tandhh*2.0;

        int32_t rely   = floor(nX);// + half_w);
        double xk  = nhw / (double)xx;

        double et2=((double)xx-nhw)/2.0;

        for (int32_t x=0; x<xx; x++)
        {
            double et1=(double)x*xk;

            int32_t newy= rely;
            int32_t newx= floor(et2+et1);

            render_table[x][y].y = newy;
            render_table[x][y].x = newx;

            newy+=GAMESCREEN_H_2;

            if (newx<0)
                newx = 0;
            if (newx>=GAMESCREEN_W)
                newx = GAMESCREEN_W-1;
            if (newy<0)
                newy = 0;
            if (newy>=GAMESCREEN_H)
                newy = GAMESCREEN_H-1;

            new_render_table[x+y*GAMESCREEN_W] = newx + newy*GAMESCREEN_W; //pixel index
        }
    }

    Rend_indexer();
}

void Rend_DrawTilt()
{
    SDL_LockSurface(tempbuf);
    SDL_LockSurface(viewportbuf);
    if (GAME_BPP == 32)
    {
        uint32_t *nww = (uint32_t *)viewportbuf->pixels;
        uint32_t *old = (uint32_t *)tempbuf->pixels;
        int32_t *ofs = new_render_table;
        for (int32_t ai=0;ai<GAMESCREEN_H*GAMESCREEN_W;ai++)
        {
            old+=*ofs;
            *nww = *old;
            nww++;
            ofs++;
        }
    }
    else if (GAME_BPP == 16)
    {
        uint16_t *nww = (uint16_t *)viewportbuf->pixels;
        uint16_t *old = (uint16_t *)tempbuf->pixels;
        int32_t *ofs = new_render_table;
        for (int32_t ai=0;ai<GAMESCREEN_H*GAMESCREEN_W;ai++)
        {
            old+=*ofs;
            *nww = *old;
            nww++;
            ofs++;
        }
    }
    else
    {
        printf("Write tilt code for %d bpp\n",GAME_BPP);
        exit(0);
    }
    SDL_UnlockSurface(tempbuf);
    SDL_UnlockSurface(viewportbuf);
//    //float hhx = (float)(rand()%5+1)/10.0;
//    SDL_LockSurface(tempbuf);
//    SDL_LockSurface(scrbuf);
//    if (GAME_BPP == 32)
//    {
//        int32_t maxIndx = scrbuf->w*scrbuf->h;
//    int *nww = ((int *)tempbuf->pixels);
//    int *old = (int *)scrbuf->pixels;    // only for 32 bit color
//    for(int y = 0; y < GAMESCREEN_H; y++)
//    {
//         // only for 32 bit color
//
//        for(int x = 0; x < GAMESCREEN_W; x++)
//        {
//            // int *nww = (int *)screen->pixels;
//
//            int newy = render_table[x][y].y  /* *hhx */  + *view_X;
//
//            if (newy < 0)
//                newy += scrbuf->h;
//            else if (newy > scrbuf->h)
//                newy -= scrbuf->h;
//
//            int32_t index = render_table[x][y].x + newy * scrbuf->w;
//            if (index > maxIndx)
//                index %= maxIndx;
//            *nww = old[index];
//            nww++; //more faster than mul %)
//        }
//    }
//    }
//    else
//    {
//        printf("Write tilt code for %d bpp\n",GAME_BPP);
//        exit(0);
//    }
//    SDL_UnlockSurface(tempbuf);
//    SDL_UnlockSurface(scrbuf);
}


void Rend_tilt_MouseInteract()
{
if (Rend_MouseInGamescr())
    {
        if (tilt_Reverse == false)
        {
            if (MouseY() > GAMESCREEN_Y + GAMESCREEN_H - GAMESCREEN_P)
                *view_X +=GetgVarInt(SLOT_PANAROTATE_SPEED)/100;
            if (MouseY() < GAMESCREEN_Y + GAMESCREEN_P)
                *view_X -=GetgVarInt(SLOT_PANAROTATE_SPEED)/100;
        }
        else
        {
            if (MouseY() > GAMESCREEN_Y + GAMESCREEN_H - GAMESCREEN_P)
                *view_X -=GetgVarInt(SLOT_PANAROTATE_SPEED)/100;
            if (MouseY() < GAMESCREEN_Y + GAMESCREEN_P)
                *view_X +=GetgVarInt(SLOT_PANAROTATE_SPEED)/100;
        }
    }

    if (*view_X >= (pana_PanaWidth - GAMESCREEN_H_2))
        *view_X = pana_PanaWidth - GAMESCREEN_H_2;
    if (*view_X <= GAMESCREEN_H_2)
        *view_X = GAMESCREEN_H_2;
}

float Rend_GetRendererAngle()
{
    if (Renderer == RENDER_PANA)
        return pana_angle;
    else if (Renderer == RENDER_TILT)
        return tilt_angle;
    else
        return 1.0;
}

float Rend_GetRendererLinscale()
{
    if (Renderer == RENDER_PANA)
        return pana_linscale;
    else if (Renderer == RENDER_TILT)
        return tilt_linscale;
    else
        return 1.0;
}

void Rend_SetRendererAngle(float angle)
{
    if (Renderer == RENDER_PANA)
        pana_angle = angle;
    else if (Renderer == RENDER_TILT)
        tilt_angle = angle;
}

void Rend_SetRendererLinscale(float lin)
{
    if (Renderer == RENDER_PANA)
        pana_linscale = lin;
    else if (Renderer == RENDER_TILT)
        tilt_linscale = lin;
}

void Rend_SetRendererTable()
{
    if (Renderer == RENDER_PANA)
        Rend_pana_SetTable();
    else if (Renderer == RENDER_TILT)
        Rend_tilt_SetTable();
}

struct_action_res *Rend_CreateDistortNode()
{
    struct_action_res *act = ScrSys_CreateActRes(NODE_TYPE_DISTORT);
    act->nodes.distort = new (struct_distort);
    act->nodes.distort->cur_frame = 0;
    act->nodes.distort->increase = true;
    act->nodes.distort->frames = 0;
    act->nodes.distort->speed = 0;
    act->nodes.distort->param1 = 0.0;
    act->nodes.distort->dif_angl = 0.0;
    act->nodes.distort->st_angl = 0.0;
    act->nodes.distort->end_angl = 0.0;
    act->nodes.distort->rend_angl = 0.0;
    act->nodes.distort->st_lin = 0.0;
    act->nodes.distort->end_lin = 0.0;
    act->nodes.distort->dif_lin = 0.0;
    act->nodes.distort->rend_lin = 0.0;

    return act;
}

int32_t Rend_ProcessDistortNode(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_DISTORT)
        return NODE_RET_OK;

    if (Rend_GetRenderer() != RENDER_PANA && Rend_GetRenderer() != RENDER_TILT)
        return NODE_RET_OK;

    struct_distort *dist = nod->nodes.distort;

    if (dist->increase)
        dist->cur_frame+=rand()%dist->frames;
    else
        dist->cur_frame-=rand()%dist->frames;

    if (dist->cur_frame >= dist->frames)
        {
            dist->increase = false;
            dist->cur_frame = dist->frames;
        }
    else if (dist->cur_frame <= 1)
        {
            dist->cur_frame = 1;
            dist->increase = true;
        }

    float diff=(1.0/(5.0-((float)dist->cur_frame * dist->param1)))/(5.0-dist->param1);

    Rend_SetRendererAngle(dist->st_angl+diff*dist->dif_angl);
    Rend_SetRendererLinscale(dist->st_lin+diff*dist->dif_lin);
    Rend_SetRendererTable();
    return NODE_RET_OK;
}

int32_t Rend_DeleteDistortNode(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_DISTORT)
        return NODE_RET_NO;

    Rend_SetRendererAngle(nod->nodes.distort->rend_angl);
    Rend_SetRendererLinscale(nod->nodes.distort->rend_lin);
    Rend_SetRendererTable();

    if (nod->slot > 0)
    {
        setGNode(nod->slot, NULL);
        SetgVarInt(nod->slot, 2);
    }


    delete nod->nodes.distort;
    delete nod;

    return NODE_RET_DELETE;
}


void Rend_DrawScalerToGamescr(scaler *scl,int16_t x, int16_t y)
{
    if (scrbuf)
        DrawScaler(scl,x,y,scrbuf);
}


float phase = 0;
SDL_Surface *am = NULL;

int ss=0;

void Rend_Effect(SDL_Surface *srf)//test-wave effect
{
    int tt=SDL_GetTicks();
    if (!am)
    am = CreateSurface(srf->w,srf->h);

    SDL_LockSurface(am);
    SDL_LockSurface(srf);

    phase += 0.12;

    int scalex=3;
    int scaley=5;
    float amplitude = 2;
    float wavelength = 1.5;


    int ww = (srf->w/2)/2;
    int hh = (srf->h/2)/2;
    int ww2 = srf->w/2;
    int hh2 = srf->h/2;

    for(int j=0;j<srf->h/2;j++)
    {
        int32_t *abc = ((int32_t *)am->pixels) + j*srf->w;
        int32_t *abc2 = ((int32_t *)am->pixels) + (j+hh2)*srf->w;
        int32_t *abc3 = ((int32_t *)am->pixels) + j*srf->w+ww2;
        int32_t *abc4 = ((int32_t *)am->pixels) + (j+hh2)*srf->w+ww2;
        for(int i=0;i<srf->w/2;i++)
        {
            int dx = (i - ww);
            int dy = (j - hh);
            float d = fastSqrt(dx * dx / scalex + dy * dy / scaley);

            float pi = 3.1415926;
            int amnt = amplitude * fastSin (d / (-wavelength * pi) + phase);

            //int needx = (dx + amnt) + ww;
            //int needy = (dy + amnt) + hh;
            int needx = amnt+i;
            int needy = amnt+j;

            if (needx < 0 )
                needx=0;
            if (needx >= srf->w )
                needx=srf->w-1;
            if (needy < 0 )
                needy=0;
            if (needy >= srf->h )
                needy=srf->h-1;
            *abc = ((int32_t *)srf->pixels)[(int)needx + (int)needy*srf->w];

            needx = amnt+i;
            needy = amnt+j;

            if (needx+ww2 < 0 )
                needx=0;
            if (needx+ww2 >= srf->w )
                needx=srf->w-1;
            if (needy < 0 )
                needy=0;
            if (needy >= srf->h )
                needy=srf->h-1;
            *abc3 = ((int32_t *)srf->pixels)[(int)needx+ww2+ (int)needy*srf->w];

            needx = amnt+i;
            needy = amnt+j;

            needy+=hh2;
            if (needx < 0 )
                needx=0;
            if (needx >= srf->w )
                needx=srf->w-1;
            if (needy < 0 )
                needy=0;
            if (needy >= srf->h )
                needy=srf->h-1;
            *abc2 = ((int32_t *)srf->pixels)[needx+ needy*srf->w];

            needx = amnt+i;
            needy = amnt+j;

            needy+=hh2;
            if (needx+ww2 < 0 )
                needx=0;
            if (needx+ww2 >= srf->w )
                needx=srf->w-1;
            if (needy < 0 )
                needy=0;
            if (needy >= srf->h )
                needy=srf->h-1;
            *abc4 = ((int32_t *)srf->pixels)[(int)needx+ww2+ (int)(needy)*srf->w];
            abc++;
            abc2++;
            abc3++;
            abc4++;

        }

        }
ss++;
if (ss%100 == 0)
 printf("%d\n",SDL_GetTicks()-tt);

    SDL_UnlockSurface(am);
    SDL_UnlockSurface(srf);

    DrawImageToSurf(am,0,0,srf);
   // SDL_FreeSurface(am);


}
