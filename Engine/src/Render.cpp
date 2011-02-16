
#include "System.h"



uint8_t     Renderer = RENDER_FLAT;

int GAME_Y=68;

//18 default cursors

char *(CurNames[])= {"active","arrow","backward","downarrow","forward","handpt","handpu","hdown","hleft",\
                     "hright","hup","idle","leftarrow","rightarrow","suggest_surround","suggest_tilt","turnaround","zuparrow"
                    };

char *(CurFiles[])= {"g0gbc011.zcr","g0gac001.zcr","g0gac021.zcr","g0gac031.zcr","g0gac041.zcr","g0gac051.zcr","g0gac061.zcr","g0gac071.zcr","g0gac081.zcr",\
                     "g0gac091.zcr","g0gac101.zcr","g0gac011.zcr","g0gac111.zcr","g0gac121.zcr","g0gac131.zcr","g0gac141.zcr","g0gac151.zcr","g0gac161.zcr"
                    };

Cursor *CurDefault[NUM_CURSORS];





SDL_Surface *screen;
SDL_Surface *scrbuf=NULL;
SDL_Surface *fish, *tempbuf;

int fishtable[640][480-68*2];

Cursor *cur;

int current_obj_cur=0;
Cursor *objcur[2];




void SetFishTable(double angl, double k)
{
    memset(fishtable,0,sizeof(int)*640*(480-68*2));

    int yy=480-68*2;
    int ww=640;

    double half_w = (double)ww / 2.0;
    double half_h = (double)yy / 2.0;

    double angle  = (angl * 3.14159265 / 180.0 );
    double hhdtan = half_h / tan(angle);
    double tandhh = tan(angle)/half_h;

    for (int x=0; x<ww; x++)
    {
        double poX = (double)x - half_w;

        double nX  = k * hhdtan * atan(poX*tandhh);
        double nn  = cos(atan(poX*tandhh));
        double nhw = half_h * nn * hhdtan * tandhh*2.0;

        int relx   = floor(nX + half_w);
        double yk  = nhw / (double)yy;

        for (int y=0; y<yy; y++)
        {
            double et1=(double)y*yk;
            double et2=((double)yy-nhw)/2.0 + et1;

            fishtable[x][y] = relx + floor(et2) * ww;
        }
    }



    /*double mmx=angle/half_w;
    double mmx2=angle/half_h;


    for(int y = 0; y < yy; y ++)
    for(int x = 0; x < ww; x ++){
        // Calculate new position of the pixel
        int newX = sin( ((double)x - half_w) *  mmx)*half_w * k + half_w;
        int newY = cos( ((double)x - half_w)* k *mmx)*((double)y - half_h)+half_h ;
        //else
        //    newY = (1.0+sin( ((double)x - half_w) *  mmx))*((double)y - half_h)+half_h ;

        if (newX>=0 && newX<ww && newY>=0 && newY<yy)
            fishtable[x][y] = newX+newY*ww;
            //fishtable[newX][newY] = x+y*ww;
    }*/


}


