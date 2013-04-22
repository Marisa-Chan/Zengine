#include "Graph.h"
#include "System.h"
#include "Sound.h"
#include <dirent.h>

const int FiveBitToEightBitLookupTable [32] =
{
    0, 8, 16, 24, 32, 41, 49, 57, 65, 74, 82, 90, 98, 106, 115, 123,
    131, 139, 148, 156, 164, 172, 180, 189, 197, 205, 213, 222, 230, 238, 246, 255
};

const int FiveBitToEightBitLookupTable_SDL [32] =
{
    0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248
};

static SDL_Surface *screen;

static uint16_t width ,\
height,\
bpp;

static float mgamma=1.0;

#define SFTYPE  SDL_SWSURFACE
//#define SFTYPE  SDL_HWSURFACE


MList *FontList = NULL;




void LoadFonts(char *fontsdir)
{
    FontList = CreateMList();

    DIR *dr=opendir(fontsdir);

    if (dr == NULL)
    {
        printf( "Unable to open folder %s\n",fontsdir);
        exit(1);
    }


    char buf[255];

    dirent *de=readdir(dr);
    while (de)
    {
        if (strcmp(de->d_name,"..")!=0 && strcmp(de->d_name,".")!=0)
        {
            sprintf(buf,"%s/%s",fontsdir,de->d_name);
            TTF_Font *fnt = TTF_OpenFont(buf,10);
            if (fnt != NULL)
            {
                struct_graph_font *tmpfnt = new(struct_graph_font);
                strncpy(tmpfnt->Name,TTF_FontFaceFamilyName(fnt),63);
                strncpy(tmpfnt->path,buf,255);
                AddToMList(FontList,tmpfnt);
                TTF_CloseFont(fnt);
            }

        }
        de=readdir(dr);
    }
    closedir(dr);
}

TTF_Font *GetFontByName(char *name,int size)
{
    struct_graph_font *fnt = NULL;

    StartMList(FontList);
    while (!eofMList(FontList))
    {
        fnt = (struct_graph_font *)DataMList(FontList);
        if(strCMP(fnt->Name,name)==0)
            break;

        NextMList(FontList);
    }

    if (fnt == NULL)
        return NULL;

    return TTF_OpenFont(fnt->path,size);
}

SDL_Surface *SwitchFullscreen()
{
    int32_t flags = screen->flags;

    screen = SDL_SetVideoMode(0,0,0,flags ^ SDL_FULLSCREEN);
    if (screen == NULL)
        screen = SDL_SetVideoMode(0,0,0,flags);
    else
        flags ^= SDL_FULLSCREEN;

    return screen;
}

SDL_Surface *InitGraphicAndSound(uint16_t wi, uint16_t he, uint16_t b,bool ful, char *fontsdir)
{

    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        exit(1);
    }

    width=wi;
    height=he;
    bpp=b;

    if (ful)
        screen=SDL_SetVideoMode(wi, he, b, SFTYPE | SDL_FULLSCREEN);
    else
        screen=SDL_SetVideoMode(wi, he, b, SFTYPE);

    SDL_WM_SetCaption(TITLE,NULL);

    InitMusic();
    TTF_Init();

    LoadFonts(fontsdir);

    return screen;
}

void setGamma(float val)
{
    if (val > 0.4 && val < 2.1)
    {
        mgamma = val;
        SDL_SetGamma(mgamma,mgamma,mgamma);
    }
}

float getGamma()
{
    return mgamma;
}

void ConvertImage(SDL_Surface **tmp)
{
    SDL_Surface *tmp2=SDL_ConvertSurface(*tmp,screen->format,SFTYPE);
    SDL_FreeSurface(*tmp);
    *tmp=tmp2;
}

SDL_Surface *CreateSurface(uint16_t w,uint16_t h)
{
    return SDL_CreateRGBSurface(SFTYPE,w,h,bpp,0,0,0,0);;
}

SDL_Surface *LoadConvertImg(const char *file)
{
    SDL_Surface *tmpbuf=IMG_Load(file);
    if (!tmpbuf)
        printf("\nERROR:  IMG_Load(%s) %s\n\n",file, IMG_GetError());
    else
        ConvertImage(&tmpbuf);

    return tmpbuf;
}

SDL_Surface *LoadConvertImg(const char *file,uint32_t key)
{
    SDL_Surface *tmpbuf=IMG_Load(file);
    if (!tmpbuf)
        printf("\nERROR:  IMG_Load(%s) %s\n\n",file, IMG_GetError());
    else
        ConvertImage(&tmpbuf);

    SDL_SetColorKey(tmpbuf,SDL_SRCCOLORKEY ,key);

    return tmpbuf;
}

anim_surf *LoadAnimImage(const char *file, int32_t mask)
{

#ifdef LOADTRACE
    printf("fallback-mechanism\n");
#endif

    char buf[64];
    const char *bufp;
    strcpy(buf,file);
    int len=strlen(buf);

    bufp=GetFilePath(buf);
    if (bufp==NULL)
        return NULL;

    SDL_Surface *tmp=IMG_Load(bufp);
    //ConvertImage(&tmp);

    buf[len-1]='m';
    buf[len-2]='n';
    buf[len-3]='a';

    bufp=GetExactFilePath(buf);
    if (bufp==NULL)
        return NULL;

    anim_surf *atmp=new(anim_surf);

    FILE *ff=fopen(bufp,"rb");
    fread(&atmp->info,1,sizeof(atmp->info),ff);
    fclose(ff);

    //atmp->info.time&=0x7f;

    typedef SDL_Surface* PSDL_Surface;

    atmp->info.time /= 10;
    atmp->img=new PSDL_Surface[atmp->info.frames];//frames * sizeof(SDL_Surface *));

    for (uint8_t i=0; i< atmp->info.frames; i++)
    {
        atmp->img[i] = CreateSurface(atmp->info.w,atmp->info.h);
        if (mask != 0 && mask != -1)
            SDL_SetColorKey(atmp->img[i],SDL_SRCCOLORKEY ,mask);
        SDL_Rect rtmp;
        rtmp.x=0;
        rtmp.y=i*atmp->info.h;
        rtmp.w= atmp->info.w;
        rtmp.h= atmp->info.h;
        SDL_BlitSurface(tmp,&rtmp,atmp->img[i],0);
    }

    SDL_FreeSurface(tmp);

    return atmp;
}

void DrawAnimImage(anim_surf *anim, int x, int y, int frame)
{
    if (!anim)
        return;

    int32_t frames = anim->info.frames;
    if (frame >= frames)
    {
//#ifdef TRACE
//        printf("Error, required frame(%d) of animation is out of range (%d) \n",frame,anim->info.frames);
//#endif
        return;
    }

    DrawImage(anim->img[frame],x,y);
}

void DrawAnimImageToSurf(anim_surf *anim, int x, int y, int frame,SDL_Surface *surf)
{
    if (!anim)
        return;
    int32_t frames = anim->info.frames;
    if (frame >= frames)
    {
//#ifdef TRACE
//        printf("Error, required frame(%d) of animation is out of range (%d) \n",frame,anim->info.frames);
//#endif
        return;
    }

    DrawImageToSurf(anim->img[frame],x,y,surf);
}

void FreeAnimImage(anim_surf *anim)
{
    if (!anim)
        return;

    if (anim)
    {
        int32_t frames = anim->info.frames;
        for (int i=0; i<frames; i++)
            if (anim->img[i])
                SDL_FreeSurface(anim->img[i]);

        delete [] anim->img;
        delete anim;
    }
}


void DrawImage(SDL_Surface *surf, int16_t x, int16_t y)
{
    if (!surf)
        return;

    SDL_Rect tmp;
    tmp.x=x;//ceil(x*sc_fac);
    tmp.y=y;//ceil(y*sc_fac);
    tmp.w=0;
    tmp.h=0;
    SDL_BlitSurface(surf,0,screen,&tmp);
}

void DrawImageToSurf(SDL_Surface *surf, int16_t x, int16_t y,SDL_Surface *dest)
{
    if (!surf)
        return;

    SDL_Rect tmp;
    tmp.x=x;//ceil(x*sc_fac);
    tmp.y=y;//ceil(y*sc_fac);
    tmp.w=0;
    tmp.h=0;
    //SDL_StretchSurfaceBlit(surf,0,dest,0);
    SDL_BlitSurface(surf,0,dest,&tmp);
}

void SetColorKey(SDL_Surface *surf,int8_t r, int8_t g, int8_t b)
{
    SDL_SetColorKey(surf,SDL_SRCCOLORKEY,Rend_MapScreenRGB(r,g,b));
}

void ClearColorKey(SDL_Surface *surf)
{
    SDL_SetColorKey(surf,0,0);
}




scaler *CreateScaler(SDL_Surface *src, uint16_t w, uint16_t h)
{
    scaler *tmp = new(scaler);
    tmp->surf = src;

    tmp->w = w;
    tmp->h = h;
    tmp->offsets = NULL;

    if (w == src->w && h == src->h)
        return tmp;

    tmp->offsets = (int32_t *)malloc(tmp->w * tmp->h * sizeof(int32_t));

    float xfac = tmp->surf->w / (float)tmp->w;
    float yfac = tmp->surf->h / (float)tmp->h;

    int32_t pos = 0;

    int32_t *tmpofs = tmp->offsets;

    for (int16_t yy = 0; yy < tmp->h; yy++)
        for (int16_t xx = 0; xx < tmp->w; xx++)
        {
            int32_t newx = xx*xfac;
            int32_t newy = yy*yfac;
            int32_t posofs = newx + newy * tmp->surf->w;

            *tmpofs = posofs-pos;

            tmpofs++;

            pos = posofs;
        }

    return tmp;
}

void DeleteScaler(scaler *scal)
{
    if (scal->offsets != NULL)
        free(scal->offsets);

    delete scal;
}

void DrawScaler(scaler *scal,int16_t x, int16_t y, SDL_Surface *dst)
{
    if (!scal)
        return;
    if ((scal->surf->format->BytesPerPixel != dst->format->BytesPerPixel) ||
            x >= dst->w || y >= dst->h || x<= -scal->w || y<= -scal->h)
        return;

    if (scal->offsets == NULL)
        DrawImageToSurf(scal->surf,x,y,dst);
    else
    {
        if (x >= 0 && y >= 0 &&
                x + scal->w  <=  dst->w &&
                y + scal->h  <=  dst->h)
        {
            int32_t oneline = dst->w - scal->w;

            SDL_LockSurface(scal->surf);
            SDL_LockSurface(dst);

            if (GAME_BPP == 32)
            {
                int32_t *ofs = (int32_t *)dst->pixels;

                ofs += y * dst->w + x;

                int32_t *ifs = (int32_t *)scal->surf->pixels;
                int32_t *dlt = scal->offsets;

                for (int16_t yy = 0; yy < scal->h; yy++)
                {
                    for (int16_t xx = 0; xx < scal->w; xx++)
                    {
                        ifs += *dlt;
                        *ofs = *ifs;
                        ofs++;
                        dlt++;
                    }

                    ofs += oneline;
                }
            }
            else if (GAME_BPP == 16)
            {
                int16_t *ofs = (int16_t *)dst->pixels;

                ofs += y * dst->w + x;

                int16_t *ifs = (int16_t *)scal->surf->pixels;
                int32_t *dlt = scal->offsets;

                for (int16_t yy = 0; yy < scal->h; yy++)
                {
                    for (int16_t xx = 0; xx < scal->w; xx++)
                    {
                        ifs += *dlt;
                        *ofs = *ifs;
                        ofs++;
                        dlt++;
                    }

                    ofs += oneline;
                }
            }
            else
            {
                printf("Produce your scaler code there \"%s\":%d\n",__FILE__, __LINE__);
            }
            SDL_UnlockSurface(scal->surf);
            SDL_UnlockSurface(dst);
        }
        else
        {
            int32_t oneline = dst->w - scal->w;

            SDL_LockSurface(scal->surf);
            SDL_LockSurface(dst);

            if (GAME_BPP == 32)
            {
                int32_t *ofs = (int32_t *)dst->pixels;
                int32_t *maxofs = ofs, *minofs = ofs;

                maxofs += dst->w * dst->h;

                ofs += y * dst->w + x;

                int16_t lx = 0 ,rx = scal->w;

                if (x < 0)
                    lx = -x;

                if (x + scal->w >= dst->w)
                    rx = dst->w - x;

                int32_t *ifs = (int32_t *)scal->surf->pixels;
                int32_t *dlt = scal->offsets;

                for (int16_t yy = 0; yy < scal->h; yy++)
                {
                    for (int16_t xx = 0; xx < scal->w; xx++)
                    {
                        ifs += *dlt;
                        if (ofs >= minofs && ofs < maxofs &&
                                xx >= lx && xx < rx)
                            *ofs = *ifs;
                        ofs++;
                        dlt++;
                    }

                    ofs += oneline;
                }
            }
            else if (GAME_BPP == 16)
            {
                int16_t *ofs = (int16_t *)dst->pixels;
                int16_t *maxofs = ofs, *minofs = ofs;

                maxofs += dst->w * dst->h;

                ofs += y * dst->w + x;

                int16_t lx = 0 ,rx = scal->w;

                if (x < 0)
                    lx = -x;

                if (x + scal->w >= dst->w)
                    rx = dst->w - x;

                int16_t *ifs = (int16_t *)scal->surf->pixels;
                int32_t *dlt = scal->offsets;

                for (int16_t yy = 0; yy < scal->h; yy++)
                {
                    for (int16_t xx = 0; xx < scal->w; xx++)
                    {
                        ifs += *dlt;
                        if (ofs >= minofs && ofs < maxofs &&
                                xx >= lx && xx < rx)
                            *ofs = *ifs;
                        ofs++;
                        dlt++;
                    }

                    ofs += oneline;
                }
            }
            else
            {
                printf("Produce your scaler code there \"%s\":%d\n",__FILE__, __LINE__);
            }
            SDL_UnlockSurface(scal->surf);
            SDL_UnlockSurface(dst);
        }
    }
}

void DrawScalerToScreen(scaler *scal,int16_t x, int16_t y)
{
    DrawScaler(scal,x,y,screen);
}

uint32_t time=0;
int32_t frames=0;
int32_t fps=1;

int32_t GetFps()
{
    return fps;
}

void FpsCounter()
{
    if (millisec() > time)
    {
        fps = frames;
        if (fps == 0)
            fps = 1;
        frames = 0;
        time = millisec() + 1000;
    }
    frames++;
}



