#include "System.h"



uint8_t     Renderer = RENDER_FLAT;


MList *sublist = NULL;
int32_t subid=0;


SDL_Surface *screen;
SDL_Surface *scrbuf=NULL;
SDL_Surface *tempbuf=NULL;

int32_t RenderDelay = 0;


struct xy
{
    int32_t x;
    int32_t y;
};

xy render_table[GAMESCREEN_W][GAMESCREEN_H];

int32_t  *_X;

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

            render_table[x][y].x = relx;
            render_table[x][y].y = floor(et2+et1);
        }
    }

}

void Rend_InitGraphics(bool fullscreen, char *fontsdir)
{
    screen=InitGraphicAndSound(GAME_W,GAME_H,GAME_BPP,fullscreen,fontsdir);


    Rend_InitSubList();

    tempbuf=CreateSurface(GAMESCREEN_W,GAMESCREEN_H);

    //fish=SDL_CreateRGBSurface(SDL_SWSURFACE,640,480-68*2,32,0,0,0,255);

    //cur=new(Cursor);//"g0gac011.zcr"));
    //LoadCursor("g0gac011.zcr",cur);

    Mouse_LoadCursors();

    SDL_ShowCursor(SDL_DISABLE);

    _X = getdirectvar(SLOT_VIEW_POS);

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

    if (!scrbuf)
        printf("ERROR:  IMG_Load(%s): %s\n\n",path, IMG_GetError());

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
            if (MouseX() < GAMESCREEN_P)
                Mouse_SetCursor(CURSOR_LEFT);
            if (MouseX() > GAMESCREEN_W - GAMESCREEN_P)
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
    int tmpl;
    int tmp;
    switch(Renderer)
    {
    case RENDER_FLAT:
        return MouseX() - GAMESCREEN_X;
        break;
    case RENDER_PANA:
        tmp = MouseY() - GAMESCREEN_Y;

        if (tmp >= 0 && tmp < GAMESCREEN_H)
            tmpl = render_table[MouseX()][tmp].x;
        else
            tmpl = 0;

        tmpl += *_X;
        if (tmpl < 0)
            tmpl += pana_PanaWidth;
        else if (tmpl > pana_PanaWidth)
            tmpl -= pana_PanaWidth;

        return tmpl;
        break;

    case RENDER_TILT:
        tmp = MouseY() - GAMESCREEN_Y;

        if (tmp >= 0 && tmp < GAMESCREEN_H)
            tmpl = render_table[MouseX()][tmp].x;
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
    int tmpl;
    int tmp;
    switch(Renderer)
    {
    case RENDER_FLAT:
        return MouseY() - GAMESCREEN_Y;
        break;

    case RENDER_PANA:
        tmp = MouseY() - GAMESCREEN_Y;

        if (tmp >= 0 && tmp < GAMESCREEN_H)
            return render_table[MouseX()][tmp].y;
        else
            return tmp;

        break;

    case RENDER_TILT:
        tmp = MouseY() - GAMESCREEN_Y;

        if (tmp >= 0 && tmp < GAMESCREEN_H)
            tmpl = render_table[MouseX()][tmp].y;
        else
            tmpl = 0;

        tmpl+=*_X;

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
    DrawImageToSurf(scrbuf,0,0,tempbuf);
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

void Rend_DrawPanorama()
{
    //int asddsa= SDL_GetTicks();
    //hehe
    //float hhx = (float)(rand()%6+3)/10.0;
    /////
    SDL_LockSurface(tempbuf);
    SDL_LockSurface(scrbuf);
    if (GAME_BPP == 32)
    {
    int *nww = ((int *)tempbuf->pixels);
    int *old = (int *)scrbuf->pixels;    // only for 32 bit color
    for(int y = 0; y < GAMESCREEN_H; y++)
    {
         // only for 32 bit color

        for(int x = 0; x < GAMESCREEN_W; x++)
        {
            // int *nww = (int *)screen->pixels;

            int newx = render_table[x][y].x  /* *hhx */  + *_X;

            if (newx < 0)
                newx += scrbuf->w;
            else if (newx > scrbuf->w)
                newx -= scrbuf->w;

            *nww = old[newx + render_table[x][y].y * scrbuf->w];
            nww++; //more faster than mul %)
        }
    }
    }
    else if (GAME_BPP == 16)
    {
    short *nww = ((short *)tempbuf->pixels); // only for 16 bit color
    short *old = (short *)scrbuf->pixels;    // only for 16 bit color
    for(int y = 0; y < GAMESCREEN_H; y++)
    {
        for(int x = 0; x < GAMESCREEN_W; x++)
        {
            // int *nww = (int *)screen->pixels;

            int newx = render_table[x][y].x + *_X;

            if (newx < 0)
                newx += scrbuf->w;
            else if (newx > scrbuf->w)
                newx -= scrbuf->w;

            *nww = old[newx + render_table[x][y].y * scrbuf->w];
            nww++; //more faster than mul %)
        }
    }
    }
    else
    {
        printf("Write panorama code for %d bpp\n",GAME_BPP);
        exit(0);
    }
    SDL_UnlockSurface(tempbuf);
    SDL_UnlockSurface(scrbuf);
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

            *_X += (pana_ReversePana == false ? param: -param);
        }

        if (MouseX() < GAMESCREEN_X + GAMESCREEN_P)
        {
            int32_t mspeed = GetgVarInt(SLOT_PANAROTATE_SPEED) >> 4;
            int32_t param  = (((GAMESCREEN_X + GAMESCREEN_P - MouseX()) << 7) / GAMESCREEN_P * mspeed) >> 7;

            *_X -= (pana_ReversePana == false ? param: -param);
        }
    }

    if (*_X >= pana_PanaWidth)
        *_X %= pana_PanaWidth;
    if (*_X < 0)
        *_X += pana_PanaWidth;
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

    if (Renderer == RENDER_FLAT)
        Rend_FlatRender();
    else if (Renderer == RENDER_PANA)
        Rend_DrawPanorama();
    else if (Renderer == RENDER_TILT)
        Rend_DrawTilt();

    Ctrl_DrawControls();

    SDL_FillRect(screen,0,0);
    DrawImage(tempbuf,GAMESCREEN_X,GAMESCREEN_Y);


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
            DrawImage(subrec->img,subrec->x,subrec->y);

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

            render_table[x][y].y = rely;
            render_table[x][y].x = floor(et2+et1);
        }
    }

}

void Rend_DrawTilt()
{
    //float hhx = (float)(rand()%5+1)/10.0;
    SDL_LockSurface(tempbuf);
    SDL_LockSurface(scrbuf);
    if (GAME_BPP == 32)
    {
        int32_t maxIndx = scrbuf->w*scrbuf->h;
    int *nww = ((int *)tempbuf->pixels);
    int *old = (int *)scrbuf->pixels;    // only for 32 bit color
    for(int y = 0; y < GAMESCREEN_H; y++)
    {
         // only for 32 bit color

        for(int x = 0; x < GAMESCREEN_W; x++)
        {
            // int *nww = (int *)screen->pixels;

            int newy = render_table[x][y].y  /* *hhx */  + *_X;

            if (newy < 0)
                newy += scrbuf->h;
            else if (newy > scrbuf->h)
                newy -= scrbuf->h;

            int32_t index = render_table[x][y].x + newy * scrbuf->w;
            if (index > maxIndx)
                index %= maxIndx;
            *nww = old[index];
            nww++; //more faster than mul %)
        }
    }
    }
    else
    {
        printf("Write tilt code for %d bpp\n",GAME_BPP);
        exit(0);
    }
    SDL_UnlockSurface(tempbuf);
    SDL_UnlockSurface(scrbuf);
}


void Rend_tilt_MouseInteract()
{
if (Rend_MouseInGamescr())
    {
        if (tilt_Reverse == false)
        {
            if (MouseY() > GAMESCREEN_Y + GAMESCREEN_H - GAMESCREEN_P)
                *_X +=GetgVarInt(SLOT_PANAROTATE_SPEED)/100;
            if (MouseY() < GAMESCREEN_Y + GAMESCREEN_P)
                *_X -=GetgVarInt(SLOT_PANAROTATE_SPEED)/100;
        }
        else
        {
            if (MouseY() > GAMESCREEN_Y + GAMESCREEN_H - GAMESCREEN_P)
                *_X -=GetgVarInt(SLOT_PANAROTATE_SPEED)/100;
            if (MouseY() < GAMESCREEN_Y + GAMESCREEN_P)
                *_X +=GetgVarInt(SLOT_PANAROTATE_SPEED)/100;
        }
    }

    if (*_X >= (pana_PanaWidth - GAMESCREEN_H_2))
        *_X = pana_PanaWidth - GAMESCREEN_H_2;
    if (*_X <= GAMESCREEN_H_2)
        *_X = GAMESCREEN_H_2;
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
