#include "System.h"



uint8_t     Renderer = RENDER_FLAT;

int GAME_Y=68;
int GAME_X=0;
int GAME_W=GAMESCREEN_W;
int GAME_H=GAMESCREEN_H;

//18 default cursors


MList *sublist = NULL;
int subid=0;


SDL_Surface *screen;
SDL_Surface *scrbuf=NULL;
SDL_Surface *fish, *tempbuf;

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
    memset(fishtable,0,sizeof(int)*GAMESCREEN_W*GAMESCREEN_H);

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
    screen=InitGraphicAndSound(640,480,32,fullscreen,fontsdir);


    Rend_InitSubList();

    tempbuf=SDL_CreateRGBSurface(SDL_SWSURFACE,640,480-68*2,32,0,0,0,255);
    fish=SDL_CreateRGBSurface(SDL_SWSURFACE,640,480-68*2,32,0,0,0,255);

    //cur=new(Cursor);//"g0gac011.zcr"));
    //LoadCursor("g0gac011.zcr",cur);

    Mouse_LoadCursors();

    SDL_ShowCursor(SDL_DISABLE);

    PanaX = getdirectvar(SLOT_VIEW_POS);

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
    if (scrbuf)
        DrawImageToSurf(scr,x,y+GAME_Y,screen);
}

void Rend_DrawImageToScr(SDL_Surface *scr,int x, int y)
{
    if (scrbuf)
        DrawImageToSurf(scr,x,y,screen);
}

void Rend_LoadGamescr(char *path)
{
    if (scrbuf)
        SDL_FreeSurface(scrbuf);

    scrbuf=IMG_Load(path);

    if (!scrbuf)
        printf("ERROR:  IMG_Load(%s): %s\n\n",path, IMG_GetError());
    else
    {
        ConvertImage(&scrbuf);
    }

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
    return (MouseX() >= GAME_X          &&\
            MouseX() <= GAME_X + GAME_W &&\
            MouseY() >= GAME_Y          &&\
            MouseY() <= GAME_Y + GAME_H );
}

int Rend_GetMouseGameX()
{
    int tmpl;
    int tmp;
    switch(Renderer)
    {
    case RENDER_FLAT:
        return MouseX() - GAME_X;
        break;

    case RENDER_PANA:
    {
        tmp = MouseY() - GAME_Y;

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
    }
    default:
        return MouseX() - GAME_X;
    }
}

int Rend_GetMouseGameY()
{
    int tmpl;
    int tmp;
    switch(Renderer)
    {
    case RENDER_FLAT:
        return MouseY() - GAME_Y;
        break;

    case RENDER_PANA:
    {
        tmp = MouseY() - GAME_Y;

        if (tmp >= 0 && tmp < GAMESCREEN_H)
            return fishtable[MouseX()][tmp].y;
        else
            return tmp;

        break;
    }
    default:
        return MouseY() - GAME_Y;
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
    SDL_FillRect(screen,0,0);
    DrawImage(scrbuf,0,GAME_Y);
}

void Rend_DrawPanorama()
{
    SDL_LockSurface(screen);
    SDL_LockSurface(scrbuf);
    for(int y = 0; y < GAMESCREEN_H; y++)
    {

        int *nww = ((int *)screen->pixels) + (y+GAME_Y)*screen->w; // only for 32 bit color

        for(int x = 0; x < GAMESCREEN_W; x++)
        {
            // int *nww = (int *)screen->pixels;
            int *old = (int *)scrbuf->pixels;    // only for 32 bit color

            int newx = fishtable[x][y].x + *PanaX;

            if (newx < 0)
                newx += scrbuf->w;
            else if (newx > scrbuf->w)
                newx -= scrbuf->w;

            *nww = old[newx + fishtable[x][y].y * scrbuf->w];
            nww++; //more faster than mul %)
        }
    }
    SDL_UnlockSurface(screen);
    SDL_UnlockSurface(scrbuf);
}

void Rend_PanaRender()
{
    if (Rend_MouseInGamescr())
    {
        if (ReversePana == false)
        {
            if (MouseX() > GAMESCREEN_W - GAMESCREEN_P)
                *PanaX +=GetgVarInt(53)/20;
            if (MouseX() < GAMESCREEN_P)
                *PanaX -=GetgVarInt(53)/20;
        }
        else
        {
            if (MouseX() > GAMESCREEN_W - GAMESCREEN_P)
                *PanaX -=GetgVarInt(53)/20;
            if (MouseX() < GAMESCREEN_P)
                *PanaX +=GetgVarInt(53)/20;
        }
    }




    if (*PanaX >= scrbuf->w)
        *PanaX %= scrbuf->w;
    if (*PanaX < 0)
        *PanaX += scrbuf->w;

    SDL_FillRect(screen,0,0);

    Rend_DrawPanorama();
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
        Rend_PanaRender();


    Ctrl_DrawSlots();

    Rend_ProcessSubs();

    menu_UpdateMenuBar();

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

void Rend_DeleteSubRect(struct_SubRect *erect)
{
    SDL_FreeSurface(erect->img);
    delete erect;
}

void Rend_ClearSubs()
{
    StartMList(sublist);
    while(!eofMList(sublist))
    {
        struct_SubRect *subrec = (struct_SubRect *)DataMList(sublist);
        Rend_DeleteSubRect(subrec);
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
            Rend_DeleteSubRect(subrec);
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
