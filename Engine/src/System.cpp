#include <SDL/SDL.h>
#include <inttypes.h>
#include <time.h>
#include <dirent.h>
//#include <asm/stat.h>

#include "System.h"
#include "mylist.h"

#include "VkKeys.h"


//Returns count of millisecs(1/1000 of second) from system start
/*uint64_t millisec()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
*/

#define KEYBUFLEN  14





uint8_t VkKeys[512];  //windows map vk keys

uint8_t KeyHits[512]; //Array with hitted keys (once per press)
bool AnyHit=false;    //it's indicate what any key was pressed
uint8_t *Keys;        //Array with pressed keys (while pressed)
SDLKey lastkey;
int keybbuf[KEYBUFLEN]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int Mx, My;
uint8_t LMstate,Mstate;


//Reset state of key hits states
void FlushHits()
{
    AnyHit=false;
    memset(KeyHits,0,512);
    lastkey=SDLK_FIRST;
}

//Sets hit state for key
void SetHit(SDLKey key)
{
    AnyHit=true;
    KeyHits[key]=1;
    lastkey=key;
    for (int i=0;i<KEYBUFLEN-1;i++)
        keybbuf[i]=keybbuf[i+1];
    keybbuf[KEYBUFLEN-1] = key;
}

int GetKeyBuffered(int indx)
{
    if (indx>KEYBUFLEN)
        return 0;
    else
        return keybbuf[KEYBUFLEN-indx-1];
}

bool CheckKeyboardMessage(char *msg, int len)
{
    if (len > KEYBUFLEN)
        return false;
    for (int i=0; i<len; i++)
        {
            int ki=GetWinKey((SDLKey)keybbuf[KEYBUFLEN-i-1]);
            if (msg[len-i-1] != ki   &&   msg[len-i-1] != '?')
                return false;
        }


    return true;
}

SDLKey GetLastKey()
{
    return lastkey;
}

//Returns hit state of the key
bool KeyHit(SDLKey key)
{
    if (KeyHits[key])
        return true;
    else
        return false;
}

//return true if any key was hitted(by key hit)
bool KeyAnyHit()
{
    return AnyHit;
}

//Update key pressed states
void UpdateKeyboard()
{
    Keys=SDL_GetKeyState(NULL);
    LMstate=Mstate;
    Mstate=SDL_GetMouseState(&Mx,&My);
}

//return true if key was pressed(continously)
bool KeyDown(SDLKey key)
{
    if (Keys[key])
        return true;
    else
        return false;
}

void InitVkKeys()
{
    memset(VkKeys,0,512*sizeof(uint8_t));
    VkKeys[SDLK_BACKSPACE]   = VK_BACK;
    VkKeys[SDLK_TAB]         = VK_TAB;
    VkKeys[SDLK_CLEAR]       = VK_CLEAR;
    VkKeys[SDLK_RETURN]      = VK_RETURN;
    VkKeys[SDLK_MENU]        = VK_MENU;
    VkKeys[SDLK_CAPSLOCK]    = VK_CAPITAL;
    VkKeys[SDLK_ESCAPE]      = VK_ESCAPE;
    VkKeys[SDLK_SPACE]       = VK_SPACE;
    VkKeys[SDLK_PAGEUP]      = VK_PRIOR;
    VkKeys[SDLK_PAGEDOWN]    = VK_NEXT;
    VkKeys[SDLK_END]         = VK_END;
    VkKeys[SDLK_HOME]        = VK_HOME;
    VkKeys[SDLK_LEFT]        = VK_LEFT;
    VkKeys[SDLK_UP]          = VK_UP;
    VkKeys[SDLK_RIGHT]       = VK_RIGHT;
    VkKeys[SDLK_DOWN]        = VK_DOWN;
    VkKeys[SDLK_PRINT]       = VK_PRINT;
    VkKeys[SDLK_INSERT]      = VK_INSERT;
    VkKeys[SDLK_DELETE]      = VK_DELETE;
    VkKeys[SDLK_HELP]        = VK_HELP;

    for (int i=0; i<= 9; i++)
        VkKeys[SDLK_0 + i]   = VK_0 + i;
    for (int i=0; i<= 25; i++)
        VkKeys[SDLK_a + i]   = VK_A + i;

    VkKeys[SDLK_KP0]         = VK_NUMPAD0;
    VkKeys[SDLK_KP1]         = VK_NUMPAD1;
    VkKeys[SDLK_KP2]         = VK_NUMPAD2;
    VkKeys[SDLK_KP3]         = VK_NUMPAD3;
    VkKeys[SDLK_KP4]         = VK_NUMPAD4;
    VkKeys[SDLK_KP5]         = VK_NUMPAD5;
    VkKeys[SDLK_KP6]         = VK_NUMPAD6;
    VkKeys[SDLK_KP7]         = VK_NUMPAD7;
    VkKeys[SDLK_KP8]         = VK_NUMPAD8;
    VkKeys[SDLK_KP9]         = VK_NUMPAD9;
    VkKeys[SDLK_KP_MULTIPLY] = VK_MULTIPLY;
    VkKeys[SDLK_KP_PLUS]     = VK_ADD;
    VkKeys[SDLK_KP_MINUS]    = VK_SUBTRACT;
    VkKeys[SDLK_KP_PERIOD]   = VK_DECIMAL;
    VkKeys[SDLK_KP_DIVIDE]   = VK_DIVIDE;

    for (int i=0; i< 15; i++)
        VkKeys[SDLK_F1 + i]  = VK_F1+i;

    VkKeys[SDLK_NUMLOCK]     = VK_NUMLOCK;
    VkKeys[SDLK_SCROLLOCK]   = VK_SCROLL;
    VkKeys[SDLK_LSHIFT]      = VK_LSHIFT;
    VkKeys[SDLK_RSHIFT]      = VK_RSHIFT;
    VkKeys[SDLK_LCTRL]       = VK_LCONTROL;
    VkKeys[SDLK_RCTRL]       = VK_RCONTROL;
    VkKeys[SDLK_MENU]        = VK_RMENU;
}

uint8_t GetWinKey(SDLKey key)
{
    return VkKeys[key];
}

int MouseX()
{
    return Mx;
}

int MouseY()
{
    return My;
}

bool MouseInRect(int x, int y, int w, int h)
{
    return (MouseX()>=x) && (MouseX()<=x+w) && (MouseY()>=y) && (MouseY()<=y+h);
}

bool MouseDown(int btn)
{
    if (Mstate & SDL_BUTTON(btn))
        return true;
    else
        return false;
}

bool MouseHit(int btn)
{
    if ((LMstate & SDL_BUTTON(btn)) == 0 && (Mstate & SDL_BUTTON(btn)) == SDL_BUTTON(btn))
        return true;
    else
        return false;
}

bool MouseUp(int btn)
{
    if ((Mstate & SDL_BUTTON(btn)) == 0 && (LMstate & SDL_BUTTON(btn)) == SDL_BUTTON(btn))
        return true;
    else
        return false;
}

void FlushMouseBtn(int btn)
{
    Mstate &= ~SDL_BUTTON(btn);
    LMstate &= ~SDL_BUTTON(btn);
}


uint64_t mtime=0;    //Game timer ticks [after ~23 milliards years will came overflow of this var, don't play so long]
bool    btime=false; //Indicates new Tick
uint64_t reltime=0;  //Realtime ticks for calculate game ticks
int     tofps=0;
#ifdef DINGOO
#define DELAY 2
#else
#define DELAY 10
#endif

//Resets game timer and set next realtime point to incriment game timer
void InitMTime(float fps)
{
    mtime=0;
    btime=false;
    tofps=ceil((1000.0-(float)(DELAY<<1))/(fps));
    reltime=millisec() + tofps;
}

//Process game timer.
void ProcMTime()
{
    if (reltime < millisec())   //New tick
    {
        mtime++;
        btime=true;
        reltime=millisec() + tofps;
    }
    else                        //No new tick
    {
        btime=false;
    }
    SDL_Delay(DELAY);
}

//Resturn true if new tick appeared
bool GetBeat()
{
    return btime;
}

bool Get2thBeat()
{
    return ((mtime & 0x1) == 0x1) & btime;
}

bool Get4thBeat()
{
    return ((mtime & 0x3) == 0x3) & btime;
}

bool GetNBeat(int n)
{
    return ((mtime % n) == 0) & btime;
}

//Return count of game timer ticks
uint64_t GetBeatCount()
{
    return mtime;
}






MList *FMan;
MList *FManRepl;

void InitFileManage()
{
    FMan = CreateMList();
    FManRepl = CreateMList();
}

void ListDir(char *dir)
{
    char buf[1024];
    char buf2[1024];
    strcpy(buf,dir);

    int len=strlen(buf);

    if (buf[len-1] == '/' || buf[len-1] == '\\')
        {
            buf[len-1]=0;
            len--;
        }


    DIR *dr=opendir(buf);

    if (dr == NULL)
        return;

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
                sprintf(buf2,"%s/%s",buf,de->d_name);

                FManNode *nod = new(FManNode);
                AddToMList(FMan,nod);

                nod->Path=(char *) malloc(strlen(buf2)+1);
                strcpy(nod->Path,buf2);
                nod->File=nod->Path+len+1;

                AddToBinTree(nod);
            }
        de=readdir(dr);
    }
    closedir(dr);

}



char *GetExactFilePath2(char *chr)
{
    StartMList(FMan);
    while(!eofMList(FMan))
    {
        FManNode* nod=(FManNode*)DataMList(FMan);
        if (strcasecmp(nod->File,chr)==0)
            {
                return nod->Path;
            }
        NextMList(FMan);
    }
    printf("file %s not found!\n",chr);
    return NULL;
}

char *GetFilePath2(char *chr)
{
    char buf[255];
    strcpy(buf,chr);

    StartMList(FManRepl);
    while(!eofMList(FManRepl))
    {
        FManRepNode *repnod=(FManRepNode *)DataMList(FManRepl);
        char *tmp=strcasestr(buf,repnod->ext);
        if (tmp!=NULL)
            {
                strcpy(tmp,repnod->ext2);
                break;
            }
        NextMList(FManRepl);
    }

    StartMList(FMan);
    while(!eofMList(FMan))
    {
        FManNode* nod=(FManNode*)DataMList(FMan);
        if (strcasecmp(nod->File,buf)==0)
            {
                return nod->Path;
            }
        NextMList(FMan);
    }
    printf("Can't find %s\n",chr);
    return NULL;
}

void AddReplacer(char *ext, char *ext2)
{
    FManRepNode *RepNode=new (FManRepNode);
    AddToMList(FManRepl,RepNode);
    RepNode->ext = (char *) malloc(strlen(ext)+1);
    strcpy(RepNode->ext,ext);
    RepNode->ext2 = (char *) malloc(strlen(ext2)+1);
    strcpy(RepNode->ext2,ext2);
}

bool FileExist(char * fil)
{
    if (FILE * file = fopen(fil, "r"))
    {
        fclose(file);
        return true;
    }
    return false;
}

char * TrimLeft(char *buf)
{
    int len = strlen(buf);

    char *str=buf;

    for (int i=0; i<len; i++)
        if (buf[i]!=0x20 && buf[i]!=0x09)
        {
            str=buf + i;
            break;
        }
    return str;
}

char * TrimRight(char *buf)
{
    int len = strlen(buf);

    char *str=buf;

    for (int i=len-1; i>=0; i--)
        if (buf[i]==0x20 || buf[i]==0x09 || buf[i]==0x0A || buf[i]==0x0D)
            buf[i] = 0x0;
        else
            break;
    return str;
}

char * PrepareString(char *buf)
{
    int len = strlen(buf);

    for (int i=len-1; i>-1; i--)
        if (buf[i]==0x0A || buf[i]==0x0D || buf[i]=='#' )
            buf[i]=0x00;

    char *str=buf;
    len = strlen(buf);

    for (int i=0; i<len; i++)
        if (buf[i]!=0x20 && buf[i]!=0x09)
        {
            str=buf + i;
            break;
        }

    len = strlen(str);

    for (int i=0; i<len; i++)
        str[i] = tolower(str[i]);
    return str;
}

char * GetParams(char *str)
{
    for (int i=strlen(str)-1; i>-1; i--)
    {
        if (str[i]==')')
            str[i]=0x0;
        else if (str[i]=='(')
        {
            return str+i+1;
        }
    }
}

int GetIntVal(char *chr)
{
    if (chr[0]=='[')
    {
        return GetgVarInt(atoi(chr+1));
    }
    else
        return atoi(chr);
}




BinTreeNd *root=NULL;
MList     *BinNodesList = NULL;


BinTreeNd *CreateBinTreeNd()
{
    BinTreeNd *tmp = new (BinTreeNd);
    tmp->one  = NULL;
    tmp->zero = NULL;
    tmp->nod  = NULL;

    if (BinNodesList == NULL)
        BinNodesList = CreateMList();

    AddToMList(BinNodesList,tmp);

    return tmp;
}

void AddToBinTree(FManNode *nod)
{
    char buffer[255];
    for (int i=0; i<strlen(nod->File);i++)
        buffer[i]=tolower(nod->File[i]);

    buffer[strlen(nod->File)] = 0x0;

    if (root == NULL)
        root = CreateBinTreeNd();

    BinTreeNd **treenod = &root;

    for (int j=0; j<strlen(buffer);j++)
        for (int i=0; i<8; i++)
            {
                int bit = ((buffer[j]) >> i) & 1;
                if (bit)
                    treenod = &((*treenod)->one);
                else
                    treenod = &((*treenod)->zero);

                if (*treenod == NULL)
                    *treenod = CreateBinTreeNd();
            }
    if ((*treenod)->nod == NULL) //we don't need to reSet nodes (ADDON and patches don't work without it)
        (*treenod)->nod = nod;
}

FManNode *FindInBinTree(char *chr)
{
    char buffer[255];
    for (int i=0; i<strlen(chr);i++)
        buffer[i]=tolower(chr[i]);

    buffer[strlen(chr)] = 0x0;

    BinTreeNd *treenod = root;

    for (int j=0; j<strlen(buffer);j++)
        for (int i=0; i<8; i++)
            {
                int bit = ((buffer[j]) >> i) & 1;
                if (bit)
                    treenod = treenod->one;
                else
                    treenod = treenod->zero;

                if (treenod == NULL)
                    return NULL;
            }

    return treenod->nod;
}

void DeleteBinTree()
{
    StartMList(BinNodesList);
    while(!eofMList(BinNodesList))
    {
        BinTreeNd *nd = (BinTreeNd *)DataMList(BinNodesList);
        delete nd;

        NextMList(BinNodesList);
    }
    DeleteMList(BinNodesList);

    BinNodesList = NULL;
    root         = NULL;
}

char *GetFilePath(char *chr)
{
    char buf[255];
    strcpy(buf,chr);

    StartMList(FManRepl);
    while(!eofMList(FManRepl))
    {
        FManRepNode *repnod=(FManRepNode *)DataMList(FManRepl);
        char *tmp=strcasestr(buf,repnod->ext);
        if (tmp!=NULL)
            {
                strcpy(tmp,repnod->ext2);
                break;
            }
        NextMList(FManRepl);
    }

    FManNode *nod = FindInBinTree(buf);
    if (nod != NULL)
        return nod->Path;
#ifdef TRACE
    printf("Can't find %s\n",chr);
#endif
    return NULL;
}

char *GetExactFilePath(char *chr)
{
    FManNode *nod = FindInBinTree(chr);
    if (nod != NULL)
        return nod->Path;
#ifdef TRACE
    printf("Can't find %s\n",chr);
#endif
    return NULL;
}
