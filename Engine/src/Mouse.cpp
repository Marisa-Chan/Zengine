
#include "System.h"

#define CURSOR_STATES      2

#define CURSOR_UP_STATE    0
#define CURSOR_DW_STATE    1


const char *(CurNames[])= {"active","arrow","backward","downarrow","forward","handpt","handpu","hdown","hleft",\
                     "hright","hup","idle","leftarrow","rightarrow","suggest_surround","suggest_tilt","turnaround","zuparrow"
                    };

const char *(CurFiles_zgi[])= {"g0gbc011.zcr","g0gac001.zcr","g0gac021.zcr","g0gac031.zcr","g0gac041.zcr","g0gac051.zcr","g0gac061.zcr","g0gac071.zcr","g0gac081.zcr",\
                     "g0gac091.zcr","g0gac101.zcr","g0gac011.zcr","g0gac111.zcr","g0gac121.zcr","g0gac131.zcr","g0gac141.zcr","g0gac151.zcr","g0gac161.zcr"
                    };
const char *(CurFiles_znemesis[])= {"00act","arrow","back","down","forw","handpt","handpu","hdown","hleft",\
                     "hright","hup","00idle","left","right","ssurr","stilt","turn","up"
                    };

Cursor *CurDefault[NUM_CURSORS][CURSOR_STATES]; //0 - normal; 1 - downed

Cursor *cur;
int8_t  cursor_index=0;

int current_obj_cur=0; //Number of loaded custom cursor
Cursor *objcur[2][CURSOR_STATES] = {{NULL, NULL}, {NULL, NULL}};

static bool DrawCursor = true;

void Mouse_LoadCursors_zgi()
{
    for (int i=0; i<18; i++)
    {
        CurDefault[i][CURSOR_UP_STATE]=new(Cursor);
        loader_LoadMouseCursor(CurFiles_zgi[i],CurDefault[i][CURSOR_UP_STATE]);
        CurDefault[i][CURSOR_DW_STATE]=new(Cursor);
        char buf[MINIBUFSZ];
        strcpy(buf,CurFiles_zgi[i]);
        buf[3]+=2;
        loader_LoadMouseCursor(buf,CurDefault[i][CURSOR_DW_STATE]);
    }

    cur = CurDefault[CURSOR_IDLE][0];
}

void Mouse_LoadCursors_znemesis()
{
    for (int i=0; i<18; i++)
    {
        char buf[MINIBUFSZ];
        sprintf(buf,"%sa.zcr",CurFiles_znemesis[i]);

        CurDefault[i][CURSOR_UP_STATE]=new(Cursor);
        loader_LoadMouseCursor(buf,CurDefault[i][CURSOR_UP_STATE]);
        CurDefault[i][CURSOR_DW_STATE]=new(Cursor);


        sprintf(buf,"%sb.zcr",CurFiles_znemesis[i]);
        loader_LoadMouseCursor(buf,CurDefault[i][CURSOR_DW_STATE]);
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


void Mouse_LoadCursor(const char *file, Cursor *cur)
{
    char tmp[64];
    const char *tmp2;
    strcpy(tmp,file);
    int len=strlen(tmp);

    cur->img = loader_LoadFile(tmp,0,Rend_MapScreenRGB(0,0,0));

    if (cur->img == NULL)
        return;

    tmp[len-3]='p';
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

const char *Mouse_GetName(int indx)
{
    return CurNames[indx];
}

int Mouse_GetCursorIndex(char *name)
{
    for (int i=0; i< NUM_CURSORS; i++)
        if (strcasecmp(name,CurNames[i]) == 0)
                return i;

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

int16_t math_get_points_rotation_angle(int16_t x, int16_t y, int16_t x2, int16_t y2)
{
        // get angle in range (-90, 90) using arctangens
        int16_t dist_x = x2-x;
        int16_t dist_y = y2-y;
        int16_t angle = atan(dist_y / (float)abs(dist_x)) * 57;

        /* *(mul)57 ~ 180/3.1415 */

        // do quarter modifications
        int16_t quarter_index = (int16_t)(dist_x < 0) | ((int16_t)(dist_y > 0) << 1);

        switch(quarter_index)
        {
            case 0:
                angle = -angle;
                break;
            case 1:
                angle = angle + 180;
                break;
            case 2:
                angle = 360 - angle;
                break;
            case 3:
                angle = 180 + angle;
                break;
        }

        return angle;
}

int16_t Mouse_GetAngle(int16_t x, int16_t y, int16_t x2, int16_t y2) //not exact but near and fast
{
    if (x == x2 && y == y2)
        return -1;
    else if (x2 == x)
    {
        if (y > y2)
            return 90;
        else
            return 270;
    }
    else if (y2 == y)
    {
        if (x < x2)
            return 0;
        else
            return 180;
    }
    else
        return math_get_points_rotation_angle(x,y,x2,y2);

    /*if (x2 >= x-5 && x2 <= x+5)
    {
        if (y > y2)
            return 90;
        else
            return 270;
    }
    else if (y2 >= y-5 && y2 <= y+5)
    {
        if (x < x2)
            return 0;
        else
            return 180;
    }
    else
    {
        if (y > y2)
        {
            if (x < x2)
                return 45;
            else
                return 135;
        }
        else
        {
            if (x < x2)
                return 315;
            else
                return 225;
        }
    }*/
}

void Mouse_LoadObjCursor_zgi(int num)
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
    sprintf(buf,"g0b%cc%2.2x1.zcr",'a',current_obj_cur);
    loader_LoadMouseCursor(buf,objcur[0][CURSOR_UP_STATE]);
    sprintf(buf,"g0b%cc%2.2x1.zcr",'b',current_obj_cur);
    loader_LoadMouseCursor(buf,objcur[1][CURSOR_UP_STATE]);
    sprintf(buf,"g0b%cc%2.2x1.zcr",'c',current_obj_cur);
    loader_LoadMouseCursor(buf,objcur[0][CURSOR_DW_STATE]);
    sprintf(buf,"g0b%cc%2.2x1.zcr",'d',current_obj_cur);
    loader_LoadMouseCursor(buf,objcur[1][CURSOR_DW_STATE]);
}

void Mouse_LoadObjCursor_znemesis(int num)
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
    sprintf(buf,"%2.2didle%c.zcr",current_obj_cur,'a');
    loader_LoadMouseCursor(buf,objcur[0][CURSOR_UP_STATE]);
    sprintf(buf,"%2.2didle%c.zcr",current_obj_cur,'b');
    loader_LoadMouseCursor(buf,objcur[0][CURSOR_DW_STATE]);
    sprintf(buf,"%2.2dact%c.zcr",current_obj_cur,'a');
    loader_LoadMouseCursor(buf,objcur[1][CURSOR_UP_STATE]);
    sprintf(buf,"%2.2dact%c.zcr",current_obj_cur,'b');
    loader_LoadMouseCursor(buf,objcur[1][CURSOR_DW_STATE]);
}
