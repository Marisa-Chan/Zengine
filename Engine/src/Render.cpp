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





struct_effect *Effects[EFFECTS_MAX_CNT];


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

int32_t pana_Zero = 0;

void Rend_pana_SetAngle(float angle)
{
    pana_angle = angle;
}

void Rend_pana_SetLinscale(float linscale)
{
    pana_linscale = linscale;
}

void Rend_pana_SetZeroPoint(int32_t point)
{
    pana_Zero = point;
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

    Effects_Init();
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
    {
        if (Renderer == RENDER_FLAT)
            DrawImageToSurf(scr,x,y,tempbuf);
        else if (Renderer == RENDER_PANA)
            DrawImageToSurf(scr,x,y,tempbuf);
        else if (Renderer == RENDER_TILT)
            DrawImageToSurf(scr,x,y+GAMESCREEN_H_2-*view_X,tempbuf);
    }
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

int8_t Rend_LoadGamescr(const char *file)
{
    int8_t good = 1;
    if (scrbuf)
        SDL_FreeSurface(scrbuf);

    scrbuf=loader_LoadFile(file,Rend_GetRenderer() == RENDER_PANA);

    SDL_FillRect(tempbuf,0,0);

    if (!scrbuf)
        printf("ERROR:  IMG_Load(%s): %s\n\n",file, SDL_GetError());

    if (!scrbuf) // no errors if no screen
    {
        good = 0;
        scrbuf=CreateSurface(GAMESCREEN_W,GAMESCREEN_H);
    }

    if (Renderer != RENDER_TILT)
        pana_PanaWidth = scrbuf->w;
    else
        pana_PanaWidth = scrbuf->h;

    return good;
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
    Rend_tilt_SetLinscale(0.65);
    Rend_tilt_SetAngle(60.0);
    Rend_pana_SetLinscale(0.55);
    Rend_pana_SetAngle(60.0);
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
        for (int32_t ai=0; ai<GAMESCREEN_H*GAMESCREEN_W; ai++)
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
        for (int32_t ai=0; ai<GAMESCREEN_H*GAMESCREEN_W; ai++)
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
    int32_t tt = *view_X;

    if (KeyDown(SDLK_LEFT))
    {
        int32_t speed = GetgVarInt(SLOT_KBD_ROTATE_SPEED) / 10;
        *view_X -= (pana_ReversePana == false ? speed: -speed);
    }
    else if (KeyDown(SDLK_RIGHT))
    {
        int32_t speed = GetgVarInt(SLOT_KBD_ROTATE_SPEED) / 10;
        *view_X += (pana_ReversePana == false ? speed: -speed);
    }

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


    if (tt < pana_Zero)
    {
        if (*view_X >= pana_Zero)
            SetgVarInt(SLOT_ROUNDS,GetgVarInt(SLOT_ROUNDS) + 1);
    }
    else if (tt > pana_Zero)
    {
        if (*view_X <= pana_Zero)
            SetgVarInt(SLOT_ROUNDS,GetgVarInt(SLOT_ROUNDS) - 1);
    }
    else if (tt == pana_Zero)
    {
        if (*view_X < pana_Zero)
            SetgVarInt(SLOT_ROUNDS,GetgVarInt(SLOT_ROUNDS) - 1);
        else if (*view_X > pana_Zero)
            SetgVarInt(SLOT_ROUNDS,GetgVarInt(SLOT_ROUNDS) + 1);
    }

    if (*view_X >= pana_PanaWidth)
        *view_X %= pana_PanaWidth;
    if (*view_X < 0)
        *view_X += pana_PanaWidth;
}

void Rend_MouseInteractOfRender()
{
    if (GetgVarInt(SLOT_PANAROTATE_SPEED) == 0)
        SetgVarInt(SLOT_PANAROTATE_SPEED, 700);
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
        Rend_DrawTilt_pre();

    //draw dynamic controls
    Ctrl_DrawControls();

    //effect-processor
    Effects_Process();


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
    tmp->timer = -1;

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

        if (subrec->timer >= 0)
        {
            subrec->timer -= GetDTime();
            if (subrec->timer < 0)
                subrec->todelete = true;
        }

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

void Rend_DelaySubDelete(struct_SubRect *sub, int32_t time)
{
    if (time > 0)
        sub->timer = time;
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

SDL_Surface *Rend_GetLocationScreenImage()
{
    return scrbuf;
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
int32_t tilt_gap = GAMESCREEN_H_2;

void Rend_tilt_SetAngle(float angle)
{
    tilt_angle = angle;
}

void Rend_tilt_SetLinscale(float linscale)
{
    tilt_linscale = fabs(linscale);
    if (tilt_linscale > 1.0 || tilt_linscale == 0)
        tilt_linscale = 1.0;
    tilt_gap = (float(GAMESCREEN_H_2) * tilt_linscale);
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

void Rend_DrawTilt_pre()
{
    DrawImageToSurf(scrbuf,0,GAMESCREEN_H_2-*view_X,tempbuf);
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
        for (int32_t ai=0; ai<GAMESCREEN_H*GAMESCREEN_W; ai++)
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
        for (int32_t ai=0; ai<GAMESCREEN_H*GAMESCREEN_W; ai++)
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
    if (KeyDown(SDLK_UP))
    {
        int32_t speed = GetgVarInt(SLOT_KBD_ROTATE_SPEED) / 10;
        *view_X -= (pana_ReversePana == false ? speed: -speed);
    }
    else if (KeyDown(SDLK_DOWN))
    {
        int32_t speed = GetgVarInt(SLOT_KBD_ROTATE_SPEED) / 10;
        *view_X += (pana_ReversePana == false ? speed: -speed);
    }

    if (Rend_MouseInGamescr())
    {
        if (MouseY() > GAMESCREEN_Y + GAMESCREEN_H - GAMESCREEN_P)
        {
            int32_t mspeed = GetgVarInt(SLOT_PANAROTATE_SPEED) >> 4;
            int32_t param  = (((MouseY() - GAMESCREEN_Y - GAMESCREEN_H + GAMESCREEN_P) << 7 ) / GAMESCREEN_P * mspeed) >> 7;

            *view_X += (pana_ReversePana == false ? param: -param);
        }
        if (MouseY() < GAMESCREEN_Y + GAMESCREEN_P)
        {
            int32_t mspeed = GetgVarInt(SLOT_PANAROTATE_SPEED) >> 4;
            int32_t param  = (((GAMESCREEN_Y + GAMESCREEN_P - MouseY()) << 7) / GAMESCREEN_P * mspeed) >> 7;

            *view_X -= (pana_ReversePana == false ? param: -param);
        }
    }

    if (*view_X >= (pana_PanaWidth - tilt_gap))
        *view_X = pana_PanaWidth - tilt_gap;
    if (*view_X <= tilt_gap)
        *view_X = tilt_gap;
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





int32_t Effects_GetColor(uint32_t x, uint32_t y)
{
    int32_t color = 0;

    int32_t xx = x;
    int32_t yy = y;

    SDL_LockSurface(scrbuf);
    if (GAME_BPP == 32)
    {

        if (xx < scrbuf->w && yy < scrbuf->h)
        {
            int32_t *pixels = (int32_t *)scrbuf->pixels;

            color = pixels[scrbuf->w*yy + xx];
        }
    }
    else if (GAME_BPP == 16)
    {
        if (xx < scrbuf->w && yy < scrbuf->h)
        {
            int16_t *pixels = (int16_t *)scrbuf->pixels;

            color = pixels[scrbuf->w*yy + xx];
        }
    }
    else
    {
        printf("Write your code for %d bit mode in %s at %d line.\n",GAME_BPP,__FILE__,__LINE__);
        //exit()
    }
    SDL_UnlockSurface(scrbuf);

    return color;
}

int8_t *Effects_Map_Useart(int32_t color, int32_t color_dlta, int32_t x, int32_t y, int32_t w, int32_t h)
{
    int8_t *tmp = (int8_t *)malloc(w*h);
    memset(tmp,0,w*h);

    SDL_LockSurface(scrbuf);
    if (GAME_BPP == 32)
    {
        int32_t * scrn = (int32_t *)scrbuf->pixels;
        for (int32_t j=y; j<y+h; j++)
            for (int32_t i=x; i<x+w; i++)
            {
                if (i>=0 && i<scrbuf->w && j>=0 && j<scrbuf->h)
                    //if ((color - color_dlta) < scrn[i+j*scrbuf->w] && (color + color_dlta) < scrn[i+j*scrbuf->w])
                    if ((color - color_dlta) <= scrn[i+j*scrbuf->w])
                        tmp[(i-x)+(j-y)*w] = 1;
            }
    }
    else if (GAME_BPP == 16)
    {
        int16_t * scrn = (int16_t *)scrbuf->pixels;
        for (int32_t j=y; j<y+h; j++)
            for (int32_t i=x; i<x+w; i++)
            {
                if (i>=0 && i<scrbuf->w && j>=0 && j<scrbuf->h)
                    if ((color - color_dlta) < scrn[i+j*scrbuf->w] && (color + color_dlta) > scrn[i+j*scrbuf->w])
                        tmp[(i-x)+(j-y)*w] = 1;
            }
    }
    else
    {
        printf("Write your code for %d bit mode in %s at %d line.\n",GAME_BPP,__FILE__,__LINE__);
    }
    SDL_UnlockSurface(scrbuf);

    return tmp;
}

int32_t Rend_EF_Light_Setup(char *string, int32_t x, int32_t y, int32_t w, int32_t h, int32_t delay,int32_t steps)
{
    int32_t eftmp = Effects_AddEffect(EFFECT_LIGH);

    if (eftmp == -1)
        return -1;

    struct_effect *ef = Effects_GetEf(eftmp);

    if (ef == NULL)
    {
        Effects_Delete(eftmp);
        return -1;
    }

    if (strCMP(string,"useart") == 0)
    {
        int32_t xx,yy,dlt;
        sscanf(string,"useart[%d,%d,%d]",&xx,&yy,&dlt);

        int32_t color = Effects_GetColor(xx,yy);

        if (GAME_BPP == 32)
        {
            dlt = FiveBitToEightBitLookupTable_SDL[(dlt & 0x1F)];
            dlt = SDL_MapRGB(scrbuf->format,dlt,dlt,dlt);
        }
        else if (GAME_BPP == 16)
            dlt = ((dlt & 0x1F) << 10) | ((dlt & 0x1F) << 5) | (dlt & 0x1F);
        else
            printf("Write your code for %d bit mode in %s at %d line.\n",GAME_BPP,__FILE__,__LINE__);

        ef->effect.ef1.map = Effects_Map_Useart(color,dlt,x,y,w,h);
    }
    else
    {
        //WRITE CODE FOR IMAGES.... but not want %)
    }

    ef->delay = delay;
    ef->time = 0;

    ef->x = x;
    ef->w = w;
    ef->y = y;
    ef->h = h;
    ef->effect.ef1.sign = 1;

    ef->effect.ef1.maxstp = steps;
    ef->effect.ef1.stp = 0;

    ef->effect.ef1.surface = CreateSurface(w,h);

    return eftmp;
}

void Rend_EF_Light_Draw(struct_effect *ef)
{
    if (!ef->effect.ef1.surface)
        return;

    ef->time -= GetDTime();

    if (ef->time<0)
    {
        ef->time = ef->delay;
        ef->effect.ef1.stp += ef->effect.ef1.sign;
        if (ef->effect.ef1.stp == ef->effect.ef1.maxstp*ef->effect.ef1.sign)
            ef->effect.ef1.sign = -ef->effect.ef1.sign;
    }

    int32_t x = ef->x;
    int32_t y = ef->y;
    int32_t w = ef->w;
    int32_t h = ef->h;

    if (Rend_GetScreenPart(&x,&y,w,h,ef->effect.ef1.surface) == 1)
    {
        SDL_Surface *srf = ef->effect.ef1.surface;

        SDL_LockSurface(srf);

        if (GAME_BPP == 32)
        {
            int32_t *px = (int32_t *)srf->pixels;
            int32_t stp = ef->effect.ef1.stp;

            int32_t color = SDL_MapRGB(srf->format,8,8,8);

            for (int32_t j = 0; j< srf->h; j++)
                for (int32_t i = 0; i< srf->w; i++)
                    if (ef->effect.ef1.map[i+j*w] == 1)
                    {

                        if (stp < 0)
                        {
                            int32_t pixel = px[i+j*w];
                            int32_t min = (pixel >> 16) & 0xFF;
                            if (((pixel >> 8) & 0xFF) < min)
                                min = (pixel >> 8) & 0xFF;
                            if ((pixel & 0xFF) < min)
                                min = pixel & 0xFF;

                            int32_t minstp = min >> 3; //  min / 8
                            if (minstp > -stp)
                            {
                                px[i+j*w] += stp*color;
                            }
                            else
                            {
                                px[i+j*w] -= minstp*color;
                            }

                        }
                        else
                        {
                            int32_t pixel = px[i+j*w];
                            int32_t max = (pixel >> 16) & 0xFF;
                            if (((pixel >> 8) & 0xFF) > max)
                                max = (pixel >> 8) & 0xFF;
                            if ((pixel & 0xFF) > max)
                                max = pixel  & 0xFF;

                            int32_t maxstp = (0xFF-max) >> 3; //  (255-max) / 8
                            if (maxstp > stp)
                            {
                                px[i+j*w] += stp*color;
                            }
                            else
                            {
                                px[i+j*w] += maxstp*color;
                            }

                        }
                    }
        }
        else
        {
            printf("Write your code for %d bit mode in %s at %d line.\n",GAME_BPP,__FILE__,__LINE__);
        }

        SDL_UnlockSurface(srf);

        DrawImageToSurf(srf,x,y,tempbuf);
    }


}

int32_t Rend_EF_Wave_Setup(int32_t delay, int32_t frames, int32_t s_x, int32_t s_y, float apml, float waveln, float spd)
{

//    Effects |= REG_EF_WAVE;

    int32_t eftmp = Effects_AddEffect(EFFECT_WAVE);

    if (eftmp == -1)
        return -1;

    struct_effect *ef = Effects_GetEf(eftmp);

    if (ef == NULL)
    {
        Effects_Delete(eftmp);
        return -1;
    }


    if (ef->effect.ef0.ampls)
    {
        for(int32_t i=0; i<ef->effect.ef0.frame_cnt; i++)
            free(ef->effect.ef0.ampls[i]);
        free(ef->effect.ef0.ampls);
    }

    if (!ef->effect.ef0.surface)
        ef->effect.ef0.surface = CreateSurface(GAMESCREEN_W,GAMESCREEN_H);

    ef->effect.ef0.frame = -1;
    ef->effect.ef0.frame_cnt = frames;

    ef->delay = delay;
    ef->time  = 0;

    ef->effect.ef0.ampls = (int8_t **)malloc(frames * sizeof(int8_t *));

    int32_t frmsize = GAMESCREEN_H_2 * GAMESCREEN_W_2;

    float phase = 0;

    int32_t w_4 = GAMESCREEN_W / 4;
    int32_t h_4 = GAMESCREEN_H / 4;

    for(int32_t i=0; i<ef->effect.ef0.frame_cnt; i++)
    {
        ef->effect.ef0.ampls[i] = (int8_t *)malloc(frmsize*sizeof(int8_t));

        for(int y=0; y<GAMESCREEN_H_2; y++)
            for(int x=0; x<GAMESCREEN_W_2; x++)
            {
                int32_t dx = (x - w_4);
                int32_t dy = (y - h_4);

                ef->effect.ef0.ampls[i][x+y*GAMESCREEN_W_2] = apml * fastSin ( fastSqrt(dx * dx / (float)s_x + dy * dy / (float)s_y)/ (-waveln * 3.1415926) + phase);;
            }
        phase += spd;
    }

    return eftmp;
}

void Rend_EF_Wave_Draw(struct_effect *ef)
{
    if (!ef->effect.ef0.surface)
        return;

    ef->time -= GetDTime();

    if (ef->time<0)
    {
        ef->time = ef->delay;
        ef->effect.ef0.frame++;
        if (ef->effect.ef0.frame >= ef->effect.ef0.frame_cnt)
            ef->effect.ef0.frame = 0;
    }

    SDL_LockSurface(ef->effect.ef0.surface);
    SDL_LockSurface(tempbuf);

    for(int y=0; y<GAMESCREEN_H_2; y++)
    {
        int32_t *abc  = ((int32_t *)ef->effect.ef0.surface->pixels) + y*GAMESCREEN_W;
        int32_t *abc2 = ((int32_t *)ef->effect.ef0.surface->pixels) + (y+GAMESCREEN_H_2)*GAMESCREEN_W;
        int32_t *abc3 = ((int32_t *)ef->effect.ef0.surface->pixels) + y*GAMESCREEN_W + GAMESCREEN_W_2;
        int32_t *abc4 = ((int32_t *)ef->effect.ef0.surface->pixels) + (y+GAMESCREEN_H_2)*GAMESCREEN_W + GAMESCREEN_W_2;
        for(int x=0; x<GAMESCREEN_W_2; x++)
        {
            int8_t amnt = ef->effect.ef0.ampls[ef->effect.ef0.frame][x+y*GAMESCREEN_W_2];
            int32_t n_x = x+amnt;
            int32_t n_y = y+amnt;

            if (n_x < 0 )
                n_x=0;
            if (n_x >= GAMESCREEN_W )
                n_x=GAMESCREEN_W-1;
            if (n_y < 0 )
                n_y=0;
            if (n_y >= GAMESCREEN_H )
                n_y=GAMESCREEN_H-1;
            *abc = ((int32_t *)tempbuf->pixels)[n_x + n_y*GAMESCREEN_W];

            n_x = x+amnt + GAMESCREEN_W_2;
            n_y = y+amnt;

            if (n_x < 0 )
                n_x=0;
            if (n_x >= GAMESCREEN_W )
                n_x=GAMESCREEN_W-1;
            if (n_y < 0 )
                n_y=0;
            if (n_y >= GAMESCREEN_H )
                n_y=GAMESCREEN_H-1;
            *abc3 = ((int32_t *)tempbuf->pixels)[n_x + n_y*GAMESCREEN_W];

            n_x = x+amnt;
            n_y = y+amnt + GAMESCREEN_H_2;

            if (n_x < 0 )
                n_x=0;
            if (n_x >= GAMESCREEN_W )
                n_x=GAMESCREEN_W-1;
            if (n_y < 0 )
                n_y=0;
            if (n_y >= GAMESCREEN_H )
                n_y=GAMESCREEN_H-1;
            *abc2 = ((int32_t *)tempbuf->pixels)[n_x + n_y*GAMESCREEN_W];

            n_x = x+amnt + GAMESCREEN_W_2;
            n_y = y+amnt + GAMESCREEN_H_2;

            if (n_x < 0 )
                n_x=0;
            if (n_x >= GAMESCREEN_W )
                n_x=GAMESCREEN_W-1;
            if (n_y < 0 )
                n_y=0;
            if (n_y >= GAMESCREEN_H )
                n_y=GAMESCREEN_H-1;
            *abc4 = ((int32_t *)tempbuf->pixels)[n_x + n_y*GAMESCREEN_W];

            abc++;
            abc2++;
            abc3++;
            abc4++;
        }
    }

    SDL_UnlockSurface(ef->effect.ef0.surface);
    SDL_UnlockSurface(tempbuf);

    DrawImageToSurf(ef->effect.ef0.surface,0,0,tempbuf);
}


//Function used to get region for apply post-process effects

int8_t Rend_GetScreenPart(int32_t *x, int32_t *y, int32_t w, int32_t h, SDL_Surface *dst)
{
    if (dst != NULL)
    {
        SDL_Rect rct;
        rct.x = *x;
        rct.y = *y;
        rct.w = w;
        rct.h = h;

        SDL_BlitSurface(scrbuf,&rct,dst,NULL);
    }

    if (Renderer == RENDER_FLAT)
    {
        if ((*x + w )< 0    ||
                (*x)>= scrbuf->w ||
                (*y + h )< 0    ||
                (*y)>= scrbuf->h )
            return 0;
        else
            return 1;
    }
    else if (Renderer == RENDER_PANA)
    {
        if ((*y + h )< 0    ||
                (*y)>= scrbuf->h )
            return 0;
        else
        {
            int32_t xx = *x % pana_PanaWidth;

            if (xx<0)
                xx = *x+pana_PanaWidth;

            if (xx+w >(*view_X-GAMESCREEN_W_2) && xx <(*view_X+GAMESCREEN_W_2))
            {
                *x -= *view_X-GAMESCREEN_W_2;
                return 1;
            }

            if (*view_X > (pana_PanaWidth - GAMESCREEN_W_2))
            {
                if (*x < (GAMESCREEN_W_2 -(pana_PanaWidth-*view_X)) )
                {
                    *x += GAMESCREEN_W_2+(pana_PanaWidth-*view_X);
                    return 1;
                }
            }
            else if (*view_X < GAMESCREEN_W_2)
            {
                if (*x+w > pana_PanaWidth-(GAMESCREEN_W_2-*view_X))
                {
                    *x -= pana_PanaWidth-(GAMESCREEN_W_2-*view_X);
                    return 1;
                }
            }
        }

    }
    else if (Renderer == RENDER_TILT)
    {
        printf("I HATE this shit! Write your code for tilt render in %s at %d line.\n",__FILE__,__LINE__);
    }

    return 0;
}




void Effects_Init()
{
    for (int32_t i=0; i<EFFECTS_MAX_CNT; i++)
        Effects[i] = NULL;
}



void Effects_Process()
{
    for (int32_t i=0; i<EFFECTS_MAX_CNT; i++)
        if (Effects[i] != NULL)
        {
            if (Effects[i]->type == EFFECT_WAVE)
                Rend_EF_Wave_Draw(Effects[i]);

            if (Effects[i]->type == EFFECT_LIGH)
                Rend_EF_Light_Draw(Effects[i]);

            if (Effects[i]->type == EFFECT_9)
                Rend_EF_9_Draw(Effects[i]);
        }
}

int32_t Effects_FindFree()
{
    int32_t tmp = -1;

    for (int32_t i=0; i<EFFECTS_MAX_CNT; i++)
        if (Effects[i] == NULL)
        {
            tmp = i;
            break;
        }
    return tmp;
}

int32_t Effects_AddEffect(int32_t type)
{
    int32_t s = Effects_FindFree();

    if (s == -1)
        return -1;

    Effects[s] = new(struct_effect);
    Effects[s]->type = type;
    Effects[s]->delay = 100;
    Effects[s]->time  = 0;
    Effects[s]->x = 0;
    Effects[s]->y = 0;
    Effects[s]->w = 0;
    Effects[s]->h = 0;

    if (type == EFFECT_WAVE)
    {
        Effects[s]->effect.ef0.ampls = NULL;
        Effects[s]->effect.ef0.frame = 0;
        Effects[s]->effect.ef0.frame_cnt = 0;
        Effects[s]->effect.ef0.surface = NULL;
    }
    else if (type == EFFECT_LIGH)
    {
        Effects[s]->effect.ef1.map = NULL;
        Effects[s]->effect.ef1.surface = NULL;
        Effects[s]->effect.ef1.maxstp = 0;
        Effects[s]->effect.ef1.sign = 1;
        Effects[s]->effect.ef1.stp = 0;
    }
    else if (type == EFFECT_9)
    {
        Effects[s]->effect.ef9.cloud = NULL;
        Effects[s]->effect.ef9.cloud_mod = NULL;
        Effects[s]->effect.ef9.mapping = NULL;
        Effects[s]->effect.ef9.mask = NULL;
    }
    return s;
}

void Effects_Delete(uint32_t index)
{
    if (index < EFFECTS_MAX_CNT)
        if (Effects[index] != NULL)
        {
            switch(Effects[index]->type)
            {
            case EFFECT_WAVE:
                if (Effects[index]->effect.ef0.ampls)
                {
                    for(int32_t i=0; i<Effects[index]->effect.ef0.frame_cnt; i++)
                        free(Effects[index]->effect.ef0.ampls[i]);
                    free(Effects[index]->effect.ef0.ampls);
                }
                if (Effects[index]->effect.ef0.surface)
                    SDL_FreeSurface(Effects[index]->effect.ef0.surface);
                break;
            case EFFECT_LIGH:
                if (Effects[index]->effect.ef1.map)
                    free(Effects[index]->effect.ef1.map);

                if (Effects[index]->effect.ef1.surface)
                    SDL_FreeSurface(Effects[index]->effect.ef1.surface);
                break;
            case EFFECT_9:
                if (Effects[index]->effect.ef9.cloud)
                    SDL_FreeSurface(Effects[index]->effect.ef9.cloud);
                if (Effects[index]->effect.ef9.mapping)
                    SDL_FreeSurface(Effects[index]->effect.ef9.mapping);
                if (Effects[index]->effect.ef9.mask)
                    SDL_FreeSurface(Effects[index]->effect.ef9.mask);
                if (Effects[index]->effect.ef9.cloud_mod)
                    free(Effects[index]->effect.ef9.cloud_mod);
                break;
            }
            delete Effects[index];
            Effects[index] = NULL;
        }
}

struct_effect * Effects_GetEf(uint32_t index)
{
    if (index < EFFECTS_MAX_CNT)
        return Effects[index];
    else
        return NULL;
}


int Rend_deleteRegion(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_REGION)
        return NODE_RET_NO;


    if (nod->nodes.node_region != -1)
        Effects_Delete(nod->nodes.node_region);

    if (nod->slot > 0)
    {
        SetgVarInt(nod->slot,2);
        setGNode(nod->slot,NULL);
    }

    delete nod;

    return NODE_RET_DELETE;
}





int32_t Rend_EF_9_Setup(char *mask, char *clouds, int32_t delay,int32_t x, int32_t y, int32_t w, int32_t h)
{

    int32_t eftmp = Effects_AddEffect(EFFECT_9);

    if (eftmp == -1)
        return -1;

    struct_effect *ef = Effects_GetEf(eftmp);

    if (ef == NULL)
    {
        Effects_Delete(eftmp);
        return -1;
    }

    ef->effect.ef9.cloud = loader_LoadFile(clouds,0);
    ef->effect.ef9.mask = loader_LoadFile(mask,0);

    if (ef->effect.ef9.cloud == NULL || ef->effect.ef9.mask == NULL)
    {
        Effects_Delete(eftmp);
        return -1;
    }

    ef->effect.ef9.cloud_mod = (int8_t *)calloc(ef->effect.ef9.cloud->w,ef->effect.ef9.cloud->h);
    ef->effect.ef9.mapping = CreateSurface(w,h);

    ef->x = x;
    ef->y = y;
    ef->w = w;
    ef->h = h;
    ef->delay = delay;

    return eftmp;
}

void Rend_EF_9_Draw(struct_effect *ef)
{
    if (!ef->effect.ef9.mapping)
        return;

    ef->time -= GetDTime();

    if (ef->time<0)
    {
        ef->time = ef->delay;

        SDL_LockSurface(ef->effect.ef9.cloud);

        if (GAME_BPP == 32)
        {
            SDL_Surface *clouds = ef->effect.ef9.cloud;

            int8_t *mp = ef->effect.ef9.cloud_mod;

            uint32_t *aa=(uint32_t *)clouds->pixels;


            for (int32_t yy=0; yy< clouds->h; yy++)
            {
                for(int32_t xx=0; xx<clouds->w; xx++)
                {
                    if (mp[xx] == 0)
                    {
                        if ((aa[xx] & 0xFF) <= 0x10)
                            mp[xx] = 1;
                        else
                            aa[xx] -= 0x0080808;
                    }
                    else
                    {
                        if ((aa[xx] & 0xFF) >= 0xEF)
                            mp[xx] = 0;
                        else
                            aa[xx] += 0x0080808;
                    }

                }

                int32_t shift_num = GetgVarInt(SLOT_EF9_SPEED);

                for(int32_t sn = 0; sn < shift_num; sn++)
                {
                    uint32_t tmp1 = aa[0];
                    int8_t   tmp2 = mp[0];

                    for(int32_t xx=0; xx<clouds->w-1; xx++)
                    {
                        aa[xx] = aa[xx+1];
                        mp[xx] = mp[xx+1];
                    }
                    aa[clouds->w-1] = tmp1;
                    mp[clouds->w-1] = tmp2;
                }

                aa += clouds->w;
                mp += clouds->w;
            }
        }
        else
        {
            printf("Write your code for %d bit mode in %s at %d line.\n",GAME_BPP,__FILE__,__LINE__);
        }

        SDL_UnlockSurface(ef->effect.ef9.cloud);


    }

    int32_t x = ef->x;
    int32_t y = ef->y;
    int32_t w = ef->w;
    int32_t h = ef->h;

    if (Rend_GetScreenPart(&x,&y,w,h,ef->effect.ef9.mapping) == 1)
    {
        SDL_Surface *srf = ef->effect.ef9.mapping;
        SDL_Surface *mask = ef->effect.ef9.mask;
        SDL_Surface *cloud = ef->effect.ef9.cloud;

        int32_t minw = srf->w;
        if (minw > mask->w)
            minw = mask->w;
        if (minw > cloud->w)
            minw = cloud->w;

        int32_t minh = srf->h;
        if (minh > mask->h)
            minh = mask->h;
        if (minh > cloud->h)
            minh = cloud->h;

        SDL_LockSurface(srf);
        SDL_LockSurface(mask);
        SDL_LockSurface(cloud);


        if (GAME_BPP == 32)
        {
            uint32_t *srfpx = (uint32_t *)srf->pixels;
            uint32_t *mskpx = (uint32_t *)mask->pixels;
            uint32_t *cldpx = (uint32_t *)cloud->pixels;

            for (int32_t y=0; y<minh; y++)
            {
                for (int32_t x=0; x<minw; x++)
                    if (mskpx[x] != 0)
                    {
                        uint32_t m_r, m_g, m_b;
                        //COLOR_RGBA32(mskpx[x],m_r,m_g,m_b);

                        m_r = GetgVarInt(SLOT_EF9_R);
                        m_g = GetgVarInt(SLOT_EF9_G);
                        m_b = GetgVarInt(SLOT_EF9_B);

                        if (m_r >= 32)
                            m_r = 31;

                        if (m_g >= 32)
                            m_g = 31;

                        if (m_b >= 32)
                            m_b = 31;

                        m_r = FiveBitToEightBitLookupTable[m_r];
                        m_g = FiveBitToEightBitLookupTable[m_g];
                        m_b = FiveBitToEightBitLookupTable[m_b];

                        uint32_t c_r, c_g, c_b;
                        COLOR_RGBA32(cldpx[x],c_r,c_g,c_b);

                        uint32_t s_r, s_g, s_b;
                        COLOR_RGBA32(srfpx[x],s_r,s_g,s_b);

                        m_r = (m_r * c_r) / 0xFF;
                        m_g = (m_g * c_g) / 0xFF;
                        m_b = (m_b * c_b) / 0xFF;

                        s_r = ((s_r + m_r) > 0xFF) ? 0xFF : (s_r + m_r);
                        s_g = ((s_g + m_g) > 0xFF) ? 0xFF : (s_g + m_g);
                        s_b = ((s_b + m_b) > 0xFF) ? 0xFF : (s_b + m_b);

                        srfpx[x] = s_r | (s_g << 8) | (s_b << 16);

                    }
                srfpx+=srf->w;
                mskpx+=mask->w;
                cldpx+=cloud->w;
            }

        }
        else
        {
            printf("Write your code for %d bit mode in %s at %d line.\n",GAME_BPP,__FILE__,__LINE__);
        }

        SDL_UnlockSurface(srf);
        SDL_UnlockSurface(mask);
        SDL_UnlockSurface(cloud);

        DrawImageToSurf(srf,x,y,tempbuf);
    }


}
