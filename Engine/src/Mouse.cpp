
#include "System.h"

#define CURSOR_STATES      2

#define CURSOR_UP_STATE    0
#define CURSOR_DW_STATE    1


char *(CurNames[])= {"active","arrow","backward","downarrow","forward","handpt","handpu","hdown","hleft",\
                     "hright","hup","idle","leftarrow","rightarrow","suggest_surround","suggest_tilt","turnaround","zuparrow"
                    };

char *(CurFiles[])= {"g0gbc011.zcr","g0gac001.zcr","g0gac021.zcr","g0gac031.zcr","g0gac041.zcr","g0gac051.zcr","g0gac061.zcr","g0gac071.zcr","g0gac081.zcr",\
                     "g0gac091.zcr","g0gac101.zcr","g0gac011.zcr","g0gac111.zcr","g0gac121.zcr","g0gac131.zcr","g0gac141.zcr","g0gac151.zcr","g0gac161.zcr"
                    };

char *(CurFilesDowned[])= {"g0gdc011.zcr","g0gac001.zcr","g0gcc021.zcr","g0gcc031.zcr","g0gcc041.zcr","g0gcc051.zcr","g0gcc061.zcr","g0gcc071.zcr","g0gcc081.zcr",\
                           "g0gcc091.zcr","g0gcc101.zcr","g0gcc011.zcr","g0gcc111.zcr","g0gcc121.zcr","g0gcc131.zcr","g0gcc141.zcr","g0gcc151.zcr","g0gcc161.zcr"
                          };

Cursor *CurDefault[NUM_CURSORS][CURSOR_STATES]; //0 - normal; 1 - downed

Cursor *cur;
int8_t  cursor_index=0;

int current_obj_cur=0; //Number of loaded custom cursor
Cursor *objcur[2][CURSOR_STATES] = {NULL, NULL, NULL, NULL};

static bool DrawCursor = true;

void Mouse_LoadCursors()
{
    for (int i=0; i<18; i++)
    {
        CurDefault[i][CURSOR_UP_STATE]=new(Cursor);
        Mouse_LoadCursor(CurFiles[i],CurDefault[i][CURSOR_UP_STATE]);
        CurDefault[i][CURSOR_DW_STATE]=new(Cursor);
        Mouse_LoadCursor(CurFilesDowned[i],CurDefault[i][CURSOR_DW_STATE]);
    }

    cur = CurDefault[CURSOR_IDLE][0];
}

void Mouse_SetCursor(int indx)
{
    int8_t stt = CURSOR_UP_STATE;

    if (MouseDown(SDL_BUTTON_LEFT) || MouseDown(SDL_BUTTON_RIGHT))
        stt = CURSOR_DW_STATE;

    if (indx == CURSOR_OBJ_0)
        cur = objcur[0][stt];
    else if (indx == CURSOR_OBJ_1)
        cur = objcur[1][stt];
    else
        cur = CurDefault[indx][stt];

    cursor_index = indx;
}

Cursor *Mouse_GetCursor(int indx)
{
    int8_t stt = CURSOR_UP_STATE;

    if (MouseDown(SDL_BUTTON_LEFT) || MouseDown(SDL_BUTTON_RIGHT))
        stt = CURSOR_DW_STATE;

    if (indx == CURSOR_OBJ_0)
        return objcur[0][stt];
    else if (indx == CURSOR_OBJ_1)
        return objcur[1][stt];
    else
        return CurDefault[indx][stt];
}

Cursor *Mouse_GetCurrentCursor()
{
    return cur;
}

bool Mouse_IsCurrentCur(int indx)
{
    return indx == cursor_index;
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

    cur->img = LoadConvertImg(tmp2,Rend_MapScreenRGB(0,0,0));

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
        if (DrawCursor)
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

void Mouse_ShowCursor()
{
    DrawCursor = true;
}

void Mouse_HideCursor()
{
    DrawCursor = false;
}

void Mouse_LoadObjCursor(int num)
{
    if (objcur[0][CURSOR_UP_STATE]!=NULL)
        Mouse_DeleteCursor(objcur[0][CURSOR_UP_STATE]);
    if (objcur[0][CURSOR_DW_STATE]!=NULL)
        Mouse_DeleteCursor(objcur[0][CURSOR_DW_STATE]);

    if (objcur[1][CURSOR_UP_STATE]!=NULL)
        Mouse_DeleteCursor(objcur[1][CURSOR_UP_STATE]);
    if (objcur[1][CURSOR_DW_STATE]!=NULL)
        Mouse_DeleteCursor(objcur[1][CURSOR_DW_STATE]);

    objcur[0][CURSOR_UP_STATE]=new(Cursor);
    objcur[0][CURSOR_DW_STATE]=new(Cursor);
    objcur[1][CURSOR_UP_STATE]=new(Cursor);
    objcur[1][CURSOR_DW_STATE]=new(Cursor);

    current_obj_cur=num;

    char buf[16];
    sprintf(buf,"g0b%cc%2.2x1.tga",'a',current_obj_cur);
    Mouse_LoadCursor(buf,objcur[0][CURSOR_UP_STATE]);
    sprintf(buf,"g0b%cc%2.2x1.tga",'b',current_obj_cur);
    Mouse_LoadCursor(buf,objcur[1][CURSOR_UP_STATE]);
    sprintf(buf,"g0b%cc%2.2x1.tga",'c',current_obj_cur);
    Mouse_LoadCursor(buf,objcur[0][CURSOR_DW_STATE]);
    sprintf(buf,"g0b%cc%2.2x1.tga",'d',current_obj_cur);
    Mouse_LoadCursor(buf,objcur[1][CURSOR_DW_STATE]);
}
