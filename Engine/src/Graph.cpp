#include "Graph.h"
#include "System.h"
#include "Sound.h"
#include <dirent.h>

const int FiveBitToEightBitLookupTable [32] =
{
    0, 8, 16, 24, 32, 41, 49, 57, 65, 74, 82, 90, 98, 106, 115, 123,
    131, 139, 148, 156, 164, 172, 180, 189, 197, 205, 213, 222, 230, 238, 246, 255
};

static SDL_Surface *screen;

static uint16_t width ,\
height,\
bpp;

#define SFTYPE  SDL_SWSURFACE


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
#ifdef WIN32
        if (strcmp(de->d_name,"..")!=0 && strcmp(de->d_name,".")!=0)
#else
        if (de->d_type==DT_REG)
            if (strcmp(de->d_name,"..")!=0 && strcmp(de->d_name,".")!=0 && de->d_type == DT_REG)
#endif
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


void ConvertImage(SDL_Surface **tmp)
{
    SDL_Surface *tmp2=SDL_ConvertSurface(*tmp,screen->format,0);
    SDL_FreeSurface(*tmp);
    *tmp=tmp2;
}

SDL_Surface *CreateSurface(uint16_t w,uint16_t h)
{
    return SDL_CreateRGBSurface(SFTYPE,w,h,bpp,0,0,0,0);;
}

SDL_Surface *LoadConvertImg(char *file)
{
    SDL_Surface *tmpbuf=IMG_Load(file);
    if (!tmpbuf)
        printf("\nERROR:  IMG_Load(%s)\n\n",file, IMG_GetError());
    else
        ConvertImage(&tmpbuf);

    return tmpbuf;
}

SDL_Surface *LoadConvertImg(char *file,uint32_t key)
{
    SDL_Surface *tmpbuf=IMG_Load(file);
    if (!tmpbuf)
        printf("\nERROR:  IMG_Load(%s)\n\n",file, IMG_GetError());
    else
        ConvertImage(&tmpbuf);

    SDL_SetColorKey(tmpbuf,SDL_SRCCOLORKEY ,key);

    return tmpbuf;
}

anim_surf *LoadAnimImage(char *file, int mask)
{

    char buf[64];
    char *bufp;
    strcpy(buf,file);
    int len=strlen(buf);
    buf[len-1]='g';
    buf[len-2]='n';
    buf[len-3]='p';

    bufp=GetExactFilePath(buf);
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
    atmp->info.time=66;
    atmp->img=new(SDL_Surface *[atmp->info.frames]);//frames * sizeof(SDL_Surface *));

    for (uint8_t i=0; i< atmp->info.frames; i++)
    {
        atmp->img[i] = CreateSurface(atmp->info.w,atmp->info.h);
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

    if (frame >= anim->info.frames)
    {
        printf("Error, required frame of animation is out of range\n");
        return;
    }

    DrawImage(anim->img[frame],x,y);
}

void DrawAnimImageToSurf(anim_surf *anim, int x, int y, int frame,SDL_Surface *surf)
{
if (!anim)
    return;

    if (frame >= anim->info.frames)
    {
        printf("Error, required frame of animation is out of range\n");
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

        for (int i=0; i<anim->info.frames; i++)
            if (anim->img[i])
                SDL_FreeSurface(anim->img[i]);

        delete[] anim->img;
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
