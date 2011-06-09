#include "System.h"
#include "SDL_ttf.h"
#include <string.h>

struct_textfile *sub_LoadTextFile(char *file)
{
    FILE *f = fopen(file,"rb");

    if (f == NULL)
        return NULL;

    fseek(f,0,SEEK_END);
    int sz = ftell(f);
    fseek(f,0,SEEK_SET);

    struct_textfile *tmp = new(struct_textfile);

    tmp->buffer = (char *)calloc(sz+1,1);

    fread(tmp->buffer,sz,1,f);
    fclose(f);


    int linescount=0;

    for (int i=0; i<sz; i++)
        if (tmp->buffer[i] == 0x0A)
            linescount ++;


    tmp->count = linescount;

    tmp->params = (char **)malloc(linescount * sizeof(char *));
    tmp->subs   = (char **)malloc(linescount * sizeof(char *));

    linescount=0;
    int i=0;
    bool txtline = false;
    while (i<sz)
    {
        if (tmp->buffer[i] != 0x0 && tmp->buffer[i] != 0xA && tmp->buffer[i] != 0xD)
            txtline = true;
        else
            txtline = false;

        if (!txtline)
        {
            tmp->buffer[i] = 0x0;
            i++;
        }
        else
        {
            if (linescount<tmp->count)
            {


                tmp->params[linescount] = strchr(tmp->buffer+i,'<') + 1;

                tmp->subs[linescount] = strchr(tmp->params[linescount],'>');

                *tmp->subs[linescount] = 0x0;

                tmp->subs[linescount]++;

                i = tmp->subs[linescount] - tmp->buffer;

                for(;;)
                {
                    if (i<sz)
                    {
                        if (tmp->buffer[i] == 0xA || tmp->buffer[i] ==0xD)
                            break;
                    }
                    else
                        break;

                    i++;
                }
            }
            txtline = false;

            linescount++;
        }
    }

    return tmp;
}

struct_subtitles *sub_LoadSubtitles(char *filename)
{

    char buf[FILE_LN_BUF];
    char buf2[FILE_LN_BUF];
    char *str1; // without left spaces, paramname
    char *str2; // params

    int subscount=0;

    struct_subtitles *tmp;

    char *fil = GetFilePath(filename);
    if (fil == NULL)
        return NULL;

    tmp = new(struct_subtitles);

    tmp->subtime = 0;
    tmp->currentsub = -1;
    tmp->SubRect = NULL;
    tmp->subscount = 0;

    FILE *f = fopen(fil,"rb");
    while (!feof(f))
    {
        fgets(buf,FILE_LN_BUF,f);
        str1 = TrimLeft(buf);

        for (int i=0; i<strlen(str1); i++)
            if (str1[i] == ':')
            {
                str1[i] = 0x0;
                str2 = str1+i+1;
                break;
            }

        for (int i=strlen(str2)-1; i>=0; i--)
            if (str2[i] == '~' || str2[i]==0x0A || str2[i]==0x0D)
                str2[i] = 0x0;

        if (strCMP(str1,"Initialization") == 0)
        {
            ;
        }
        else if (strCMP(str1,"Rectangle") == 0)
        {
            int x;
            int y;
            int x2;
            int y2;
            sscanf(str2,"%d %d %d %d",&x,&y,&x2,&y2);
            tmp->SubRect = Rend_CreateSubRect(x,y+68,x2-x,y2-y);
        }
        else if (strCMP(str1,"TextFile") == 0)
        {
            char *fil2 = GetFilePath(str2);
            if (fil2 == NULL)
            {
                delete tmp;
                return NULL;
            }
           /* FILE *f2 = fopen(fil2,"rb");
            while (!feof(f2))
            {
                fgets(buf2,FILE_LN_BUF,f2);
                TrimRight(buf2);
                subs[subscount] = (char *)malloc(strlen(buf2)+1);
                strcpy(subs[subscount],buf2);
                subscount++;
            }
            fclose(f2);*/
            tmp->txt = sub_LoadTextFile(fil2);//
            tmp->subs = (struct_one_subtitle *)calloc(tmp->txt->count,sizeof(struct_one_subtitle));
            subscount = tmp->txt->count;
        }
        else//it's must be sub info
        {
            int st;
            int en;
            int sb;
            sscanf(str2,"(%d,%d)=%d",&st,&en,&sb);
            if (subscount == 0 || sb > subscount)
            {
                printf("Error in subs %s\n",filename);
                exit(-1);
            }
            tmp->subs[tmp->subscount].start = st;
            tmp->subs[tmp->subscount].stop  = en;
            tmp->subs[tmp->subscount].sub  = sb;

            tmp->subscount++;

        }

    }
    fclose(f);

    return tmp;
}

int sub_ProcessSub(struct_subtitles *sub)
{
    int j=-1;
    for (int i=0; i<sub->subscount; i++)
        if (sub->subtime >= sub->subs[i].start &&
                sub->subtime <= sub->subs[i].stop)
        {
            j=i;
            break;
        }

    if (j == -1 && sub->currentsub!= -1)
    {
        SDL_FillRect(sub->SubRect->img,NULL,0);
        sub->currentsub = -1;
    }

    if (j!=-1 && j!=sub->currentsub)
    {
        char *sss;
        sss = sub->txt->subs[sub->subs[j].sub];
        if (sss != NULL)
        {
            for (int i=0; i<=strlen(sss); i++)
                if (sss[i] == '>')
                {
                    sss +=i+1;
                    break;
                }

            TTF_Font *temp_font = GetFontByName("ZorkDeath", 18 );
            if(!temp_font)
            {
                printf("TTF_OpenFont: %s\n", TTF_GetError());
                exit(1);
            }

            SDL_FillRect(sub->SubRect->img,NULL,0);

            SDL_Color clr= {255,255,255};

            SDL_Surface *aaa= TTF_RenderUTF8_Solid(temp_font,sss,clr);

            DrawImageToSurf(aaa,0,0,sub->SubRect->img);
            SDL_FreeSurface(aaa);
            TTF_CloseFont(temp_font);
        }
        sub->currentsub = j;
    }

    if (GetNBeat(3))
        sub->subtime++;
}

void sub_DeleteSub(struct_subtitles *sub)
{
    sub->SubRect->todelete = true;

    delete sub->txt->buffer;
    delete sub->txt;
    delete [] sub->subs;
    delete sub;
}
