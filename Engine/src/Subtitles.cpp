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

    tmp->params = (char **)calloc(linescount,sizeof(char *));
    tmp->subs   = (char **)calloc(linescount,sizeof(char *));


    linescount=0;
    int i=0;
    bool txtline = false;
    while (i<sz)
    {
        if (tmp->buffer[i] != 0x0 && tmp->buffer[i] != 0xA)
            txtline = true;
        else
            txtline = false;

        if (!txtline || tmp->buffer[i] == 0xD)
        {
            tmp->buffer[i] = 0x0;
            i++;
        }
        else
        {
            if (linescount<tmp->count)
            {
                tmp->params[linescount] = strchr(tmp->buffer+i,'<');

                tmp->subs[linescount] = NULL;

                if (tmp->params[linescount] != NULL)
                {
                    tmp->params[linescount]++;

                    tmp->subs[linescount] = strchr(tmp->params[linescount],'>');

                    if (tmp->subs[linescount] != NULL)
                    {

                        *tmp->subs[linescount] = 0x0;

                        tmp->subs[linescount]++;

                        i = tmp->subs[linescount] - tmp->buffer;


                    }
                }

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

void sub_DeleteTextFile(struct_textfile *txt)
{
    free(txt->buffer);
    free(txt->subs);
    free(txt->params);
    delete txt;
}


struct_font_style sub_parse_parameters(char *string)
{
    char buff[512];
    strcpy(buff,string);

    struct_font_style style;
    style.blue  = 255;
    style.green = 255;
    style.red   = 255;
    strcpy(style.fontname,"Arial");
    style.bold       = SUB_STYLE_OFF;
    style.escapement = 0;
    style.italic     = SUB_STYLE_OFF;
    style.justify    = SUB_JUSTIFY_CENTER;
    style.newline    = 0;
    style.size       = 12;
    style.skipcolor  = SUB_STYLE_OFF;
    style.strikeout  = SUB_STYLE_OFF;
    style.underline  = SUB_STYLE_OFF;

    char *token;

    char *find = " ";

    token = strtok(buff,find);

    bool gooood;

    while (token != NULL)
    {
        gooood = true;

        if ( strCMP(token,"font") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
                strcpy(style.fontname,token);
        }
        else if( strCMP(token,"blue") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
                style.blue = atoi(token);
        }
        else if( strCMP(token,"red") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
                style.red = atoi(token);
        }
        else if( strCMP(token,"green") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
                style.green = atoi(token);
        }
        else if( strCMP(token,"newline") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
                style.newline = SUB_STYLE_ON;
            else if( strCMP(token,"off") == 0 )
                style.newline = SUB_STYLE_OFF;
            else
                gooood = false;
        }
        else if( strCMP(token,"point") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
                style.size = atoi(token);
        }
        else if( strCMP(token,"escapement") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
                style.escapement = atoi(token);
        }
        else if( strCMP(token,"italic") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
                style.italic = SUB_STYLE_ON;
            else if( strCMP(token,"off") == 0 )
                style.italic = SUB_STYLE_OFF;
            else
                gooood = false;
        }
        else if( strCMP(token,"underline") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
                style.underline = SUB_STYLE_ON;
            else if( strCMP(token,"off") == 0 )
                style.underline = SUB_STYLE_OFF;
            else
                gooood = false;
        }
        else if( strCMP(token,"strikeout") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
                style.strikeout = SUB_STYLE_ON;
            else if( strCMP(token,"off") == 0 )
                style.strikeout = SUB_STYLE_OFF;
            else
                gooood = false;
        }
        else if( strCMP(token,"bold") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
                style.bold = SUB_STYLE_ON;
            else if( strCMP(token,"off") == 0 )
                style.bold = SUB_STYLE_OFF;
            else
                gooood = false;
        }
        else if( strCMP(token,"skipcolor") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
                style.skipcolor = SUB_STYLE_ON;
            else if( strCMP(token,"off") == 0 )
                style.skipcolor = SUB_STYLE_OFF;
            else
                gooood = false;
        }
        else if( strCMP(token,"image") == 0 )
        {
            //token = strtok(NULL,find);
        }
        else if( strCMP(token,"statebox") == 0 )
        {
            //token = strtok(NULL,find);
        }
        else if( strCMP(token,"justify") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"center") == 0 )
                style.justify = SUB_JUSTIFY_CENTER;
            else if( strCMP(token,"left") == 0 )
                style.justify = SUB_JUSTIFY_LEFT;
            else if( strCMP(token,"right") == 0 )
                style.justify = SUB_JUSTIFY_RIGHT;
            else
                gooood = false;
        }

        if (gooood)
            token = strtok(NULL,find);

    }

    return style;

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
                                              x2-x,
                                              y2-y);
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


void sub_DrawSubWithJustify(char *txt, TTF_Font *fnt, SDL_Color clr, SDL_Surface *dst, int dy, int justify)
{
    SDL_Surface *aaa= TTF_RenderUTF8_Solid(fnt,txt,clr);


    if (justify == SUB_JUSTIFY_LEFT)

        DrawImageToSurf(aaa,0,dy,dst);

    else if (justify == SUB_JUSTIFY_CENTER)

        DrawImageToSurf(aaa, (dst->w - aaa->w)/2,  dy,  dst);

    else if (justify == SUB_JUSTIFY_RIGHT)

        DrawImageToSurf(aaa, dst->w - aaa->w,  dy,  dst);


    SDL_FreeSurface(aaa);
}

int sub_DrawTextToRect(char *txt, struct_font_style *fnt_stl, SDL_Surface *dst)
{

    TTF_Font *temp_font = GetFontByName(fnt_stl->fontname, fnt_stl->size);
    if(!temp_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(1);
    }

    int temp_stl=0;

    if (fnt_stl->bold == SUB_STYLE_ON)
        temp_stl |= TTF_STYLE_BOLD;

    if (fnt_stl->italic == SUB_STYLE_ON)
        temp_stl |= TTF_STYLE_ITALIC;

    if (fnt_stl->underline == SUB_STYLE_ON)
        temp_stl |= TTF_STYLE_UNDERLINE;

#if (SDL_TTF_NUM_VERSION >= 20010)
    if (fnt_stl->strikeout == SUB_STYLE_ON)
        temp_stl |= TTF_STYLE_STRIKETHROUGH;
#endif

    TTF_SetFontStyle(temp_font,temp_stl);

    SDL_FillRect(dst,NULL,0);

    SDL_Color clr= {fnt_stl->red,fnt_stl->green,fnt_stl->blue};


    int w,h,ww;

    TTF_SizeUTF8(temp_font,txt,&w,&h);

    ww=w;

    if (w < dst->w)
        sub_DrawSubWithJustify(txt,temp_font,clr,dst,0,fnt_stl->justify);
    else
    {

        int dy=0;
        int di=0;

        for (;;)
        {
            if (txt[0] == 0x0 || di > 10)
                break;

            int k=0;
            char tmpstr[255];

            TTF_SizeUTF8(temp_font,txt,&w,&h);
            if (w >= dst->w)
            {
                for (int i=0; i<strlen(txt); i++)
                    if (txt[i] == ' ' || txt[i] == '\t')
                    {
                        strncpy(tmpstr,txt,i);
                        tmpstr[i] = 0;

                        TTF_SizeUTF8(temp_font,tmpstr,&w,&h);

                        if (w > dst->w)
                            break;

                        k = i;
                    }

                strncpy(tmpstr,txt,k);
                tmpstr[k] = 0;

                sub_DrawSubWithJustify(tmpstr,temp_font,clr,dst,dy,fnt_stl->justify);

                dy+=fnt_stl->size;
                txt +=k;

            }
            else
            {
                sub_DrawSubWithJustify(txt,temp_font,clr,dst,dy,fnt_stl->justify);
                break;
            }

            di++;
        }



    }
    return ww;

    TTF_CloseFont(temp_font);
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

            if (sub->txt->params[sub->subs[j].sub])
            {
                struct_font_style fnt_style = sub_parse_parameters(sub->txt->params[sub->subs[j].sub]);

                sub_DrawTextToRect(sss,&fnt_style,sub->SubRect->img);
            }
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
