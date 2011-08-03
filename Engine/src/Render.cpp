#include "System.h"



uint8_t     Renderer = RENDER_FLAT;


MList *sublist = NULL;
int subid=0;


SDL_Surface *screen;
SDL_Surface *scrbuf=NULL;
SDL_Surface *tempbuf=NULL;

int32_t RenderDelay = 0;


struct xy
{
    int x;
    int y;
};

xy fishtable[GAMESCREEN_W][GAMESCREEN_H];

int PanaWidth=1800;
int *PanaX;
bool ReversePana = false;





void Rend_SetDelay(int delay)
{
    RenderDelay = delay;
}

void Rend_SetFishTable(double angl, double k)
{
    memset(fishtable,0,sizeof(xy)*GAMESCREEN_W*GAMESCREEN_H);

    int yy=GAMESCREEN_H;
    int ww=GAMESCREEN_W;

    double half_w = (double)ww / 2.0;
    double half_h = (double)yy / 2.0;

    double angle  = (angl * 3.14159265 / 180.0 );
    double hhdtan = half_h / tan(angle);
    double tandhh = tan(angle)/half_h;

    for (int x=0; x<ww; x++)
    {
        double poX = (double)x - half_w +0.01; //0.01 - for zero tan/atan issue (vertical line on half of screen)

        double nX  = k * hhdtan * atan(poX*tandhh);
        double nn  = cos(atan(poX*tandhh));
        double nhw = half_h * nn * hhdtan * tandhh*2.0;

        int relx   = floor(nX);// + half_w);
        double yk  = nhw / (double)yy;

        for (int y=0; y<yy; y++)
        {
            double et1=(double)y*yk;
            double et2=((double)yy-nhw)/2.0 + et1;

            fishtable[x][y].x = relx;
            fishtable[x][y].y = floor(et2);
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

    PanaX = getdirectvar(SLOT_VIEW_POS);

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

    PanaWidth = scrbuf->w;
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
    {
        if (Rend_MouseInGamescr())
            if (MouseX() < GAMESCREEN_P)
                Mouse_SetCursor(CURSOR_LEFT);

        if (Rend_MouseInGamescr())
            if (MouseX() > GAMESCREEN_W - GAMESCREEN_P)
                Mouse_SetCursor(CURSOR_RIGH);
    }



    Mouse_DrawCursor(MouseX(),MouseY());
}

bool Rend_MouseInGamescr()
{
    return (MouseX() >= GAMESCREEN_X          &&\
            MouseX() <= GAMESCREEN_X + GAMESCREEN_W &&\
            MouseY() >= GAMESCREEN_Y          &&\
            MouseY() <= GAMESCREEN_Y + GAMESCREEN_H );
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
            tmpl = fishtable[MouseX()][tmp].x;
        else
            tmpl = 0;

        tmpl += *PanaX;
        if (tmpl < 0)
            tmpl += PanaWidth;
        else if (tmpl > PanaWidth)
            tmpl -= PanaWidth;

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
    {
        tmp = MouseY() - GAMESCREEN_Y;

        if (tmp >= 0 && tmp < GAMESCREEN_H)
            return fishtable[MouseX()][tmp].y;
        else
            return tmp;

        break;
    }
    default:
        return MouseY() - GAMESCREEN_Y;
    }
}

void Rend_SetRenderer(int meth)
{
    Renderer = meth;
    ReversePana = false;
}

void Rend_SetReversePana(bool pana)
{
    ReversePana = pana;
}

int Rend_GetRenderer()
{
    return Renderer;
}

void Rend_FlatRender()
{
    DrawImageToSurf(scrbuf,0,0,tempbuf);
}

void Rend_DrawPanorama()
{
    //hehe
    //float hhx = 5.0/10.0;
    /////
    SDL_LockSurface(tempbuf);
    SDL_LockSurface(scrbuf);
    if (GAME_BPP == 32)
    {
    for(int y = 0; y < GAMESCREEN_H; y++)
    {
        int *nww = ((int *)tempbuf->pixels) + (tempbuf->w * y); // only for 32 bit color

        for(int x = 0; x < GAMESCREEN_W; x++)
        {
            // int *nww = (int *)screen->pixels;
            int *old = (int *)scrbuf->pixels;    // only for 32 bit color

            int newx = fishtable[x][y].x  /* *hhx */   + *PanaX;

            if (newx < 0)
                newx += scrbuf->w;
            else if (newx > scrbuf->w)
                newx -= scrbuf->w;

            *nww = old[newx + fishtable[x][y].y * scrbuf->w];
            nww++; //more faster than mul %)
        }
    }
    }
    else if (GAME_BPP == 16)
    {
    for(int y = 0; y < GAMESCREEN_H; y++)
    {
        short *nww = ((short *)tempbuf->pixels) + (tempbuf->w * y); // only for 16 bit color

        for(int x = 0; x < GAMESCREEN_W; x++)
        {
            // int *nww = (int *)screen->pixels;
            short *old = (short *)scrbuf->pixels;    // only for 16 bit color

            int newx = fishtable[x][y].x + *PanaX;

            if (newx < 0)
                newx += scrbuf->w;
            else if (newx > scrbuf->w)
                newx -= scrbuf->w;

            *nww = old[newx + fishtable[x][y].y * scrbuf->w];
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
}

void Rend_PanaMouseInteract()
{
if (Rend_MouseInGamescr())
    {
        if (ReversePana == false)
        {
            if (MouseX() > GAMESCREEN_W - GAMESCREEN_P)
                *PanaX +=GetgVarInt(SLOT_PANAROTATE_SPEED)/20;
            if (MouseX() < GAMESCREEN_P)
                *PanaX -=GetgVarInt(SLOT_PANAROTATE_SPEED)/20;
        }
        else
        {
            if (MouseX() > GAMESCREEN_W - GAMESCREEN_P)
                *PanaX -=GetgVarInt(SLOT_PANAROTATE_SPEED)/20;
            if (MouseX() < GAMESCREEN_P)
                *PanaX +=GetgVarInt(SLOT_PANAROTATE_SPEED)/20;
        }
    }

    if (*PanaX >= scrbuf->w)
        *PanaX %= scrbuf->w;
    if (*PanaX < 0)
        *PanaX += scrbuf->w;
}

void Rend_MouseInteractOfRender()
{
    if (Renderer == RENDER_PANA)
        Rend_PanaMouseInteract();
}

int Rend_GetPanaWidth()
{
    return PanaWidth;
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
    else
        Rend_DrawPanorama();

    Ctrl_DrawControls();

    SDL_FillRect(screen,0,0);
    DrawImage(tempbuf,GAMESCREEN_X,GAMESCREEN_Y);


    Rend_ProcessSubs();

    menu_DrawMenuBar();

    Rend_ProcessCursor();

    //SDL_Flip(screen);
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
