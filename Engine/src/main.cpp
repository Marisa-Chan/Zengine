#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "System.h"
#include "Game.h"
#include "Graph.h"

using namespace std;

bool done = false;

void END()
{
    done = true;
}

int main(int argc, char **argv)
{
    InitFileManage();

    char buf[512];
    char buf2[512];
    bool fullscreen = false;
    char *pa="./";
    for (int i=1; i<argc; i++)
    {
        if (strcasecmp(argv[i],"-f")==0)
        {
            fullscreen = true;
        }
        else
        {
            pa=argv[i];
        }
    }

    SetAppPath(pa);

    sprintf(buf,"%s/%s",pa,"Zork.dir");
    FILE *dirs=fopen(buf,"rb");

    if (!dirs)
        exit(1);

    while(!feof(dirs))
    {
        memset(buf,0,128);
        if (fgets(buf,128,dirs) == NULL)
            break;
        char *sstr=TrimRight(TrimLeft(buf));
        if (sstr!=NULL)
            if (strlen(sstr)>1)
            {
                sprintf(buf2,"%s/%s",pa,buf);

                ListDir(buf2);
            }

    }

    fclose(dirs);



    AddReplacer("TGA","PNG");
    AddReplacer("RAW","WAV");
    AddReplacer("SRC","WAV");
    AddReplacer("IFP","WAV");
    AddReplacer("AVI","MPG");




    InitVkKeys();

    sprintf(buf,"%s/%s",pa,"FONTS");
    Rend_InitGraphics(fullscreen,buf);

    sprintf(buf,"%s/%s",pa,SYS_STRINGS_FILE);
    ReadSystemStrings(buf);

    menu_LoadGraphics();

    InitScriptsEngine();


    InitGameLoop();

    InitMTime(35.0);


    while (!done )
    {
#ifdef TRACE
        //    printf("\n\nLoop #%d\n\n",bl);
#endif
        //      printf("\n\nLoop #%d\n\n",bl);
        //Update game timer
        ProcMTime();
        UpdateDTime();
        // message processing loop
        SDL_Event event;

        //Clear all hits
        FlushHits();
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keyhit's (one per press)
            case SDL_KEYDOWN:
                SetHit(event.key.keysym.sym);
                break;
            }
        }
        //check for keydown (continous)
        UpdateKeyboard();



        if ((KeyDown(SDLK_RALT) || KeyDown(SDLK_LALT)) && KeyHit(SDLK_RETURN))
            Rend_SwitchFullscreen();
        //done=true;



        GameLoop();

        //DrawAnimImage
        //printf ("\n\nloop #%d\n",i);

        //SDL_Flip;
    }

    SDL_Quit();


    return 0;
}
