#include "System.h"
#include "SDL_ttf.h"

struct_subtitles *sub_LoadSubtitles(char *filename)
{

    char buf[FILE_LN_BUF];
    char buf2[FILE_LN_BUF];
    char *str1; // without left spaces, paramname
    char *str2; // params

    char *subs[500];
    int subscount=0;

    for (int i=0; i<500;i++)
        subs[i] = NULL;

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
                tmp->SubRect = Rend_CreateSubRect(x,y+48,x2-x,y2-y);
            }
        else if (strCMP(str1,"TextFile") == 0)
        {
            char *fil2 = GetFilePath(str2);
            if (fil2 == NULL)
                {
                    delete tmp;
                    return NULL;
                }
            FILE *f2 = fopen(fil2,"rb");
            int subscount = 0;
            while (!feof(f2))
            {
                fgets(buf2,FILE_LN_BUF,f2);
                subs[subscount] = (char *)malloc(strlen(buf2)+1);
                strcpy(subs[subscount],buf2);
                subscount++;
            }
            fclose(f2);
            tmp->subs = (struct_one_subtitle *)calloc(subscount,sizeof(struct_one_subtitle));
            tmp->subscount = subscount;
        }
        else//it's must be sub info
        {
            int st;
            int en;
            int sb;
            sscanf(str2,"(%d,%d)=%d",&st,&en,&sb);
            if (subscount = 0 || sb >= tmp->subscount)
                {
                    printf("Error in subs %s\n",filename);
                    exit(-1);
                }
            tmp->subs[sb].start = st*1.5;
            tmp->subs[sb].stop  = en*1.5;
            tmp->subs[sb].text  = subs[sb];
            subs[sb] = NULL;

        }

    }
    fclose(f);

    for (int i=0; i<500;i++)
        if (subs[i] != NULL)
            delete subs[i];

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

    if (j!=-1 && j!=sub->currentsub)
        {
            char *sss;
            sss = sub->subs[j].text;
            if (sss != NULL)
            {
            for (int i=0; i<=strlen(sss);i++)
                if (sss[i] == '>')
                    {
                        sss +=i+1;
                        break;
                    }

            TTF_Font *temp_font = TTF_OpenFont("/media/166/games/ZGI/pacs/ZORKNORM.TTF", 12 );
            if(!temp_font) {
            printf("TTF_OpenFont: %s\n", TTF_GetError());
            exit(1);
            }

            SDL_FillRect(sub->SubRect->img,NULL,0);

            SDL_Color clr={255,255,255};

            SDL_Surface *aaa= TTF_RenderUTF8_Solid(temp_font,sss,clr);
            SDL_FreeSurface(aaa);
            DrawImageToSurf(aaa,0,0,sub->SubRect->img);
            TTF_CloseFont(temp_font);
            }
            sub->currentsub = j;
        }

    if (GetBeat())
        sub->subtime++;
}
