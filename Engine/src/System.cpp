#include <SDL/SDL.h>
#include <inttypes.h>
#include <time.h>
#include <dirent.h>
#include <asm/stat.h>

#include "System.h"
#include "mylist.h"


//Returns count of millisecs(1/1000 of second) from system start
/*uint64_t millisec()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
*/




uint8_t KeyHits[512]; //Array with hitted keys (once per press)
bool AnyHit=false;    //it's indicate what any key was pressed
uint8_t *Keys;        //Array with pressed keys (while pressed)

int Mx, My;
uint8_t LMstate,Mstate;


//Reset state of key hits states
void FlushHits()
{
    AnyHit=false;
    memset(KeyHits,0,512);
}

//Sets hit state for key
void SetHit(SDLKey key)
{
    AnyHit=true;
    KeyHits[key]=1;
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

int MouseX()
{
    return Mx;
}

int MouseY()
{
    return My;
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
void InitMTime(int fps)
{
    mtime=0;
    btime=false;
    tofps=(1000-(DELAY<<1))/(fps);
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
bool GetTick()
{
    return btime;
}

//Return count of game timer ticks
uint64_t GetTickCount()
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

    if (buf[len-1] == '/' && buf[len-1] == '\\')
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
        if (de->d_type==DT_REG)
            if (strcmp(de->d_name,"..")!=0 && strcmp(de->d_name,".")!=0)
            {
                sprintf(buf2,"%s/%s",buf,de->d_name);

                FManNode *nod = new(FManNode);
                AddToMList(FMan,nod);

                nod->Path=(char *) malloc(strlen(buf2)+1);
                strcpy(nod->Path,buf2);
                nod->File=nod->Path+len+1;
            }
        de=readdir(dr);
    }
    closedir(dr);

}



char *GetExactFilePath(char *chr)
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
    return NULL;
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

