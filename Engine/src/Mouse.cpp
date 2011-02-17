
#include "System.h"



//////DEPRECATED
extern SDL_Surface *screen;
/////////////



char *(CurNames[])= {"active","arrow","backward","downarrow","forward","handpt","handpu","hdown","hleft",\
                     "hright","hup","idle","leftarrow","rightarrow","suggest_surround","suggest_tilt","turnaround","zuparrow"
                    };

char *(CurFiles[])= {"g0gbc011.zcr","g0gac001.zcr","g0gac021.zcr","g0gac031.zcr","g0gac041.zcr","g0gac051.zcr","g0gac061.zcr","g0gac071.zcr","g0gac081.zcr",\
                     "g0gac091.zcr","g0gac101.zcr","g0gac011.zcr","g0gac111.zcr","g0gac121.zcr","g0gac131.zcr","g0gac141.zcr","g0gac151.zcr","g0gac161.zcr"
                    };

Cursor *CurDefault[NUM_CURSORS];

Cursor *cur;

int current_obj_cur=0;
Cursor *objcur[2] = {NULL, NULL};


void Mouse_LoadCursors()
{
    for (int i=0; i<18; i++)
    {
        CurDefault[i]=new(Cursor);
        Mouse_LoadCursor(CurFiles[i],CurDefault[i]);
    }

    cur = CurDefault[CURSOR_IDLE];
}

void Mouse_SetCursor(int indx)
{
    if (indx == CURSOR_OBJ_0)
        cur = objcur[0];
    else if (indx == CURSOR_OBJ_1)
        cur = objcur[1];
    else
        cur = CurDefault[indx];
}

Cursor *Mouse_GetCursor(int indx)
{
    return CurDefault[indx];
}

Cursor *Mouse_GetCurrentCursor()
{
    return cur;
}

bool Mouse_IsCurrentCur(int indx)
{
    return cur == CurDefault[indx];
}


void Mouse_LoadCursor(char *file, Cursor *cur)
{
    char tmp[64];
    char *tmp2;
    strcpy(tmp,file);
    int len=strlen(tmp);
    tmp[len-1]='g';
    tmp[len-2]='n';
    tmp[len-3]='p';

    tmp2=GetExactFilePath(tmp);
    if (tmp2==NULL)
        return;

    cur->img = IMG_Load(tmp2);
    if (cur->img)
    {
        SDL_Surface *z=SDL_ConvertSurface(cur->img,screen->format,0);
        SDL_FreeSurface(cur->img);
        cur->img=z;
        SDL_SetColorKey(cur->img,SDL_SRCCOLORKEY ,SDL_MapRGB(cur->img->format,0,0,0));
    }
    tmp[len-2]='o';
    tmp[len-1]='i';
    tmp[len]='n';
    tmp[len+1]='t';
    tmp[len+2]=0x0;

    tmp2=GetExactFilePath(tmp);
    if (tmp2==NULL)
        return;

    FILE *f=fopen(tmp2,"rb");
    fread(&cur->ox,1,2,f);
    fread(&cur->oy,1,2,f);
    fclose(f);
}

void Mouse_DrawCursor(int x, int y)
{
    if (cur)
        Rend_DrawImageToScr(cur->img,x-cur->ox,y-cur->oy);
}

void Mouse_DeleteCursor(Cursor *cur)
{
    SDL_FreeSurface(cur->img);
    delete cur;
}

char *Mouse_GetName(int indx)
{
    return CurNames[indx];
}

int Mouse_GetCursorIndex(char *name)
{
    for (int i=0; i< NUM_CURSORS; i++)
    {
        if (strcasecmp(name,CurNames[i]) == 0)
            {
                return i;
                break;
            }
    }
    return CURSOR_IDLE;
}

int Mouse_GetCurrentObjCur()
{
    return current_obj_cur;
}


void Mouse_LoadObjCursor(int num)
{
    if (objcur[0]!=NULL)
        Mouse_DeleteCursor(objcur[0]);
    if (objcur[1]!=NULL)
        Mouse_DeleteCursor(objcur[1]);

    objcur[0]=new(Cursor);
    objcur[1]=new(Cursor);

    current_obj_cur=num;

    char buf[16];
    sprintf(buf,"g0bac%2.2x1.tga",current_obj_cur);
    Mouse_LoadCursor(buf,objcur[0]);
    sprintf(buf,"g0bbc%2.2x1.tga",current_obj_cur);
    Mouse_LoadCursor(buf,objcur[1]);
}
