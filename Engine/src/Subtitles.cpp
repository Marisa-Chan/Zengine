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

    tmp->subs   = (char **)calloc(linescount,sizeof(char *));


    linescount=0;
    int i=0;
    char *curline=NULL;

    while (i<sz)
    {
        curline = tmp->buffer+i;

        while (i<sz)
        {
            if (tmp->buffer[i] == 0xA || tmp->buffer[i] == 0xD)
                break;
            i++;
        }

        if (tmp->buffer[i] == 0xD)
        {
            if (tmp->buffer[i+1] == 0xA)//windows
            {
                tmp->buffer[i] = 0;
                tmp->buffer[i+1] = 0;
                i+=2;
            }
            else //macOS
            {
                tmp->buffer[i] = 0;
                i++;
            }
        }
        else if (tmp->buffer[i] == 0xA) //unix
        {
            tmp->buffer[i] = 0;
            i++;
        }

        if (linescount<tmp->count)
            tmp->subs[linescount]   = curline;

        linescount++;
    }



    return tmp;
}

void sub_DeleteTextFile(struct_textfile *txt)
{
    free(txt->buffer);
    free(txt->subs);
    delete txt;
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
            tmp->SubRect = Rend_CreateSubRect(x + GAMESCREEN_X + SUB_CORRECT_HORIZ + GAMESCREEN_FLAT_X,
                                              y + GAMESCREEN_Y + SUB_CORRECT_VERT,
                                              x2-x+1,
                                              y2-y+1);
        }
        else if (strCMP(str1,"TextFile") == 0)
        {
            char *fil2 = GetFilePath(str2);
            if (fil2 == NULL)
            {
                delete tmp;
                return NULL;
            }

            tmp->txt = sub_LoadTextFile(fil2);
            tmp->subs = (struct_one_subtitle *)calloc(tmp->txt->count,sizeof(struct_one_subtitle));
            subscount = tmp->txt->count;
        }
        else//it's must be sub info
        {
            int st;
            int en;
            int sb;
            if ( sscanf(str2,"(%d,%d)=%d",&st,&en,&sb) == 3)
            {
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

    }
    fclose(f);

    return tmp;
}

int sub_ProcessSub(struct_subtitles *sub,int subtime)
{
    int j=-1;
    for (int i=0; i<sub->subscount; i++)
        if (subtime >= sub->subs[i].start &&
                subtime <= sub->subs[i].stop)
        {
            j=i;
            break;
        }

    if (j == -1 && sub->currentsub!= -1)
    {
        SDL_FillRect(sub->SubRect->img,NULL,SDL_MapRGBA(sub->SubRect->img->format,0,0,0,255));
        sub->currentsub = -1;
    }

    if (j!=-1 && j!=sub->currentsub)
    {
        char *sss;
        sss = sub->txt->subs[sub->subs[j].sub];
        if (sss != NULL)
            if (strlen(sss) > 0)
            {
                SDL_FillRect(sub->SubRect->img,NULL,SDL_MapRGBA(sub->SubRect->img->format,0,0,0,255));
                txt_DrawTxtInOneLine(sss,sub->SubRect->img);
            }


        sub->currentsub = j;
    }
}

void sub_DeleteSub(struct_subtitles *sub)
{
    Rend_DeleteSubRect(sub->SubRect);

    sub_DeleteTextFile(sub->txt);
    free(sub->subs);
    delete sub;
}
