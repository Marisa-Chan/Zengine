#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "System.h"
#include "Game.h"
#include "Graph.h"

using namespace std;

int main(int argc, char **argv)
{
    InitFileManage();

    char buf[1024];
    //char *pa="/media/1000/Temp/pack_FILES";
    char *pa=argv[1];

    sprintf(buf,"%s/%s",pa,"ZASSETS1/GLOBAL");
    ListDir(buf);

    sprintf(buf,"%s/%s",pa,"ZASSETS1/PORTFOOZ");
    ListDir(buf);

    sprintf(buf,"%s/%s",pa,"ZASSETS1/MONAST1");
    ListDir(buf);

    sprintf(buf,"%s/%s",pa,"ZASSETS1/JAIL");
    ListDir(buf);

    sprintf(buf,"%s/%s",pa,"ZGI_MX");
    ListDir(buf);

    sprintf(buf,"%s/%s",pa,"CURSOR");
    ListDir(buf);

    sprintf(buf,"%s/%s",pa,"SCRIPTS");
    ListDir(buf);

    sprintf(buf,"%s/%s",pa,"ZASSETS2/UNDERG2");
    ListDir(buf);

    AddReplacer("TGA","PNG");
    AddReplacer("RAW","WAV");
    AddReplacer("IFP","WAV");





    InitGraphics();
    InitScriptsEngine();

    InitGameLoop();

    int bl=0;

    InitMTime(15);
    bool done = false;
    while (!done )
    {
#ifdef TRACE
            printf("\n\nLoop #%d\n\n",bl);
#endif
  //      printf("\n\nLoop #%d\n\n",bl);
        //Update game timer
        ProcMTime();
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
            {
                SetHit(event.key.keysym.sym);
                break;
            }
            }
        }
        //check for keydown (continous)
        UpdateKeyboard();


        if (KeyHit(SDLK_ESCAPE))
            done=true;

        GameLoop();

        //DrawAnimImage
        //printf ("\n\nloop #%d\n",i);

        //SDL_Flip;


        bl++;
    }



    return 0;
}
