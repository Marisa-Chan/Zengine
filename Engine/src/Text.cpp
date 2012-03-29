#include "System.h"

void txt_init_txt_struct(struct_txt_style *style)
{
    style->blue = 255;
    style->green = 255;
    style->red   = 255;
    strcpy(style->fontname,"Arial");
    style->bold       = TXT_STYLE_VAR_FALSE;
    style->escapement = 0;
    style->italic     = TXT_STYLE_VAR_FALSE;
    style->justify    = TXT_JUSTIFY_LEFT;
    style->newline    = 0;
    style->size       = 12;
    style->skipcolor  = TXT_STYLE_VAR_FALSE;
    style->strikeout  = TXT_STYLE_VAR_FALSE;
    style->underline  = TXT_STYLE_VAR_FALSE;
    style->statebox   = 0;
}

struct_txt_style *txt_NewTxt()
{
    struct_txt_style *txt = new(struct_txt_style);
    txt_init_txt_struct(txt);
    return txt;
}

int8_t txt_parse_txt_params(struct_txt_style *style, const char *strin, int32_t len)
{
    char buf[TXT_CFG_BUF_MAX_LEN];
    memcpy(buf,strin,len);
    buf[len] = 0x0;

    int8_t retval = TXT_RET_NOTHING;

    char *token;

    const char *find = " ";

    //font with "item what i want"
    char *fontitem = strcasestr(buf,"font");
    if (fontitem != NULL)
    {
        fontitem += 5; //to next item
        if (fontitem[0] == '"')
        {
            fontitem++;

            int32_t len=0;
            while (fontitem[len]!='"' && fontitem[len]>=' ')
            {
                style->fontname[len] = fontitem[len];
                len++;
            }
            style->fontname[len] = 0;
        }
        else
        {
            int32_t len=0;
            while (fontitem[len]>' ')
            {
                style->fontname[len] = fontitem[len];
                len++;
            }
            style->fontname[len] = 0;
        }
        retval |= TXT_RET_FNTCHG;
    }

    token = strtok(buf,find);

    bool gooood;

    while (token != NULL)
    {
        gooood = true;

//        if ( strCMP(token,"font") == 0 )
//        {
//            token = strtok(NULL,find);
//            if (token == NULL)
//                gooood = false;
//            else
//            {
//                if (strCMP(style->fontname,token) != 0)
//                {
//                    strcpy(style->fontname,token);
//                    retval |= TXT_RET_FNTCHG;
//                }
//
//            }
//
//        }
//        else
        if( strCMP(token,"blue") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
            {
                int32_t tmp = atoi(token);
                if (style->blue != tmp)
                {
                    style->blue = tmp;
                    retval |= TXT_RET_FNTSTL;
                }
            }

        }
        else if( strCMP(token,"red") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
            {
                int32_t tmp = atoi(token);
                if (style->red != tmp)
                {
                    style->red = tmp;
                    retval |= TXT_RET_FNTSTL;
                }
            }
        }
        else if( strCMP(token,"green") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
            {
                int32_t tmp = atoi(token);
                if (style->green != tmp)
                {
                    style->green = tmp;
                    retval |= TXT_RET_FNTSTL;
                }
            }
        }
        else if( strCMP(token,"newline") == 0 )
        {
            if ((retval & TXT_RET_NEWLN) == 0)
                style->newline = 0;

            style->newline++;
            retval |= TXT_RET_NEWLN;
        }
        else if( strCMP(token,"point") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
            {
                int32_t tmp=atoi(token);
                if (style->size != tmp)
                {
                    style->size = tmp;
                    retval |= TXT_RET_FNTCHG;
                }

            }

        }
        else if( strCMP(token,"escapement") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
                style->escapement = atoi(token);
        }
        else if( strCMP(token,"italic") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
            {
                if (style->italic != TXT_STYLE_VAR_TRUE)
                {
                    style->italic = TXT_STYLE_VAR_TRUE;
                    retval |= TXT_RET_FNTSTL;
                }
            }
            else if( strCMP(token,"off") == 0 )
            {
                if (style->italic != TXT_STYLE_VAR_FALSE)
                {
                    style->italic = TXT_STYLE_VAR_FALSE;
                    retval |= TXT_RET_FNTSTL;
                }
            }
            else
                gooood = false;
        }
        else if( strCMP(token,"underline") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
            {
                if (style->underline != TXT_STYLE_VAR_TRUE)
                {
                    style->underline = TXT_STYLE_VAR_TRUE;
                    retval |= TXT_RET_FNTSTL;
                }
            }
            else if( strCMP(token,"off") == 0 )
            {
                if (style->underline != TXT_STYLE_VAR_FALSE)
                {
                    style->underline = TXT_STYLE_VAR_FALSE;
                    retval |= TXT_RET_FNTSTL;
                }
            }
            else
                gooood = false;
        }
        else if( strCMP(token,"strikeout") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
            {
                if (style->strikeout != TXT_STYLE_VAR_TRUE)
                {
                    style->strikeout = TXT_STYLE_VAR_TRUE;
                    retval |= TXT_RET_FNTSTL;
                }
            }
            else if( strCMP(token,"off") == 0 )
            {
                if (style->strikeout != TXT_STYLE_VAR_FALSE)
                {
                    style->strikeout = TXT_STYLE_VAR_FALSE;
                    retval |= TXT_RET_FNTSTL;
                }
            }
            else
                gooood = false;
        }
        else if( strCMP(token,"bold") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
            {
                if (style->bold != TXT_STYLE_VAR_TRUE)
                {
                    style->bold = TXT_STYLE_VAR_TRUE;
                    retval |= TXT_RET_FNTSTL;
                }
            }
            else if( strCMP(token,"off") == 0 )
            {
                if (style->bold != TXT_STYLE_VAR_FALSE)
                {
                    style->bold = TXT_STYLE_VAR_FALSE;
                    retval |= TXT_RET_FNTSTL;
                }
            }
            else
                gooood = false;
        }
        else if( strCMP(token,"skipcolor") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"on") == 0 )
                style->skipcolor = TXT_STYLE_VAR_TRUE;
            else if( strCMP(token,"off") == 0 )
                style->skipcolor = TXT_STYLE_VAR_FALSE;
            else
                gooood = false;
        }
        else if( strCMP(token,"image") == 0 )
        {
            //token = strtok(NULL,find);
        }
        else if( strCMP(token,"statebox") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else
            {
                style->statebox = atoi(token);
                retval |= TXT_RET_HASSTBOX;
            }
        }
        else if( strCMP(token,"justify") == 0 )
        {
            token = strtok(NULL,find);
            if (token == NULL)
                gooood = false;
            else if( strCMP(token,"center") == 0 )
                style->justify = TXT_JUSTIFY_CENTER;
            else if( strCMP(token,"left") == 0 )
                style->justify = TXT_JUSTIFY_LEFT;
            else if( strCMP(token,"right") == 0 )
                style->justify = TXT_JUSTIFY_RIGHT;
            else
                gooood = false;
        }

        if (gooood)
            token = strtok(NULL,find);

    }
    return retval;
}

void txt_DrawTxtWithJustify(char *txt, TTF_Font *fnt, SDL_Color clr, SDL_Surface *dst, int lineY, int justify)
{
    SDL_Surface *aaa= TTF_RenderUTF8_Solid(fnt,txt,clr);

    if (justify == TXT_JUSTIFY_LEFT)

        DrawImageToSurf(aaa,0,lineY - aaa->h,dst);

    else if (justify == TXT_JUSTIFY_CENTER)

        DrawImageToSurf(aaa, (dst->w - aaa->w)/2,  lineY - aaa->h,  dst);

    else if (justify == TXT_JUSTIFY_RIGHT)

        DrawImageToSurf(aaa, dst->w - aaa->w,  lineY - aaa->h,  dst);

    SDL_FreeSurface(aaa);
}


void txt_readfontstyle(struct_txt_style *style, char *strin)
{
    int32_t strt=-1;
    int32_t endt=-1;

    int32_t t_len = strlen(strin);

    for (int32_t i=0; i< t_len; i++)
    {
        if (strin[i] == '<')
            strt = i;
        else if (strin[i] == '>')
        {
            endt = i;
            if (strt != -1)
                if ((endt-strt-1) > 0)
                    txt_parse_txt_params(style,strin+strt+1,endt-strt-1);
        }

    }
}

void txt_set_font_style(TTF_Font *font, struct_txt_style *fnt_stl)
{
    int32_t temp_stl=0;

    if (fnt_stl->bold == TXT_STYLE_VAR_TRUE)
        temp_stl |= TTF_STYLE_BOLD;

    if (fnt_stl->italic == TXT_STYLE_VAR_TRUE)
        temp_stl |= TTF_STYLE_ITALIC;

    if (fnt_stl->underline == TXT_STYLE_VAR_TRUE)
        temp_stl |= TTF_STYLE_UNDERLINE;

#if (SDL_TTF_NUM_VERSION >= 20010)
    if (fnt_stl->strikeout == TXT_STYLE_VAR_TRUE)
        temp_stl |= TTF_STYLE_STRIKETHROUGH;
#endif

    TTF_SetFontStyle(font,temp_stl);
}

int32_t txt_DrawTxt(char *txt, struct_txt_style *fnt_stl, SDL_Surface *dst)
{
    TTF_Font *temp_font = GetFontByName(fnt_stl->fontname, fnt_stl->size);
    if(!temp_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(1);
    }

    SDL_FillRect(dst,NULL,0);

    SDL_Color clr= {fnt_stl->red,fnt_stl->green,fnt_stl->blue,255};

    txt_set_font_style(temp_font,fnt_stl);

    int32_t w,h;

    TTF_SizeUTF8(temp_font,txt,&w,&h);

    txt_DrawTxtWithJustify(txt,temp_font,clr,dst,fnt_stl->size,fnt_stl->justify);

    TTF_CloseFont(temp_font);

    return w;
}

SDL_Surface *txt_RenderUTF8(TTF_Font *fnt, char *text, struct_txt_style *style)
{
    txt_set_font_style(fnt,style);
    SDL_Color clr= {style->red,style->green,style->blue,255};
    return TTF_RenderUTF8_Solid(fnt,text,clr);
}

void txt_DrawTxtInOneLine(const char *text,SDL_Surface *dst)
{
    struct_txt_style style,style2;
    txt_init_txt_struct(&style);
    int32_t strt=-1;
    int32_t endt=-1;
    int32_t i=0;
    int32_t dx=0,dy=0;
    int32_t txt_w,txt_h;
    int32_t txtpos=0;
    char buf[TXT_CFG_BUF_MAX_LEN];
    char buf2[TXT_CFG_BUF_MAX_LEN];
    memset(buf,0,TXT_CFG_BUF_MAX_LEN);
    memset(buf2,0,TXT_CFG_BUF_MAX_LEN);

    SDL_Surface *TxtSurfaces[TXT_CFG_TEXTURES_LINES][TXT_CFG_TEXTURES_PER_LINE];
    int32_t currentline=0, currentlineitm=0;

    int8_t TxtJustify[TXT_CFG_TEXTURES_LINES];
    int8_t TxtPoint[TXT_CFG_TEXTURES_LINES];

    memset(TxtSurfaces,0,sizeof(TxtSurfaces[0][0])*TXT_CFG_TEXTURES_LINES*TXT_CFG_TEXTURES_PER_LINE);

    int32_t stringlen=strlen(text);

    TTF_Font *font=NULL;

    font = GetFontByName(style.fontname,style.size);
    txt_set_font_style(font,&style);

    int32_t prevbufspace=0, prevtxtspace=0;

    while (i<stringlen)
    {
        TxtJustify[currentline] = style.justify;
        TxtPoint[currentline] = style.size;
        if (text[i]== '<')
        {
            int32_t ret = 0;

            strt = i;
            while (i<stringlen && text[i]!='>')
                i++;
            endt = i;
            if (strt != -1)
                if ((endt-strt-1) > 0)
                {
                    style2 = style;
                    ret = txt_parse_txt_params(&style,text+strt+1,endt-strt-1);
                }

            if (ret & (TXT_RET_FNTCHG | TXT_RET_FNTSTL | TXT_RET_NEWLN))
            {
                if (strlen(buf)>0)
                {

                    TTF_SizeUTF8(font,buf,&txt_w,&txt_h);

                    TxtSurfaces[currentline][currentlineitm] = txt_RenderUTF8(font,buf,&style2);

                    currentlineitm++;

                    memset(buf,0,TXT_CFG_BUF_MAX_LEN);
                    prevbufspace=0;
                    txtpos=0;
                    dx+=txt_w;

                }
                if (ret & TXT_RET_FNTCHG)
                {
                    TTF_CloseFont(font);
                    font = GetFontByName(style.fontname,style.size);
                    txt_set_font_style(font,&style);
                }
                if (ret & TXT_RET_FNTSTL)
                    txt_set_font_style(font,&style);

                if (ret & TXT_RET_NEWLN)
                {
                    currentline++;
                    currentlineitm=0;
                    dx=0;
                }
            }

            if (ret & TXT_RET_HASSTBOX)
            {
                char buf3[MINIBUFSZ];
                sprintf(buf3,"%d",GetgVarInt(style.statebox));
                strcat(buf,buf3);
                txtpos+=strlen(buf3);
            }

        }
        else
        {

            buf[txtpos++]=text[i];

            if (text[i] == ' ')
            {
                prevbufspace = txtpos-1;
                prevtxtspace = i;
            }


            if (font != NULL)
            {
                TTF_SizeUTF8(font,buf,&txt_w,&txt_h);
                if (txt_w+dx > dst->w)
                {
                    if (prevbufspace == 0)
                    {
                        prevtxtspace = i;
                        prevbufspace = txtpos-1;
                    }
                    memcpy(buf2,buf,prevbufspace+1);
                    buf2[prevbufspace+1] = 0x0;

                    if (strlen(buf2)>0)
                        TxtSurfaces[currentline][currentlineitm] = txt_RenderUTF8(font,buf2,&style);

                    memset(buf,0,TXT_CFG_BUF_MAX_LEN);
                    i = prevtxtspace;
                    prevbufspace=0;
                    txtpos=0;
                    currentline++;
                    currentlineitm=0;
                    dx=0;
                }
            }
        }
        i++;
    }

    if (strlen(buf)>0)
        TxtSurfaces[currentline][currentlineitm] = txt_RenderUTF8(font,buf,&style);

    dy=0;
    for (i=0; i<=currentline; i++)
    {
        int32_t j=0;
        int32_t width=0;
        while(TxtSurfaces[i][j] != NULL)
        {
            width+=TxtSurfaces[i][j]->w;
            j++;
        }
        dx=0;
        for (int32_t jj=0; jj<j; jj++)
        {
            if (     TxtJustify[i] == TXT_JUSTIFY_LEFT)
                DrawImageToSurf(TxtSurfaces[i][jj],dx, dy+TxtPoint[i] - TxtSurfaces[i][jj]->h,dst);

            else if (TxtJustify[i] == TXT_JUSTIFY_CENTER)
                DrawImageToSurf(TxtSurfaces[i][jj],((dst->w - width)>>1)+dx,  dy+TxtPoint[i] - TxtSurfaces[i][jj]->h,dst);

            else if (TxtJustify[i] == TXT_JUSTIFY_RIGHT)
                DrawImageToSurf(TxtSurfaces[i][jj],dst->w-width+dx, dy+TxtPoint[i] - TxtSurfaces[i][jj]->h,dst);

            dx+=TxtSurfaces[i][jj]->w;
        }

        dy+=TxtPoint[i];
    }

    for (i=0; i<TXT_CFG_TEXTURES_LINES; i++)
        for (int32_t j=0; j<TXT_CFG_TEXTURES_PER_LINE; j++)
            if (TxtSurfaces[i][j]!= NULL)
                SDL_FreeSurface(TxtSurfaces[i][j]);

}




struct_action_res *txt_CreateTTYtext()
{
    struct_action_res *tmp;
    tmp = ScrSys_CreateActRes(NODE_TYPE_TTYTEXT);

    tmp->nodes.tty_text = new (struct_ttytext);
    tmp->nodes.tty_text->delay = 0;
    txt_init_txt_struct(&tmp->nodes.tty_text->style);
    tmp->nodes.tty_text->fnt = NULL;
    tmp->nodes.tty_text->x = 0;
    tmp->nodes.tty_text->y = 0;
    tmp->nodes.tty_text->w = 0;
    tmp->nodes.tty_text->h = 0;
    tmp->nodes.tty_text->txtbuf = NULL;
    tmp->nodes.tty_text->txtpos = 0;
    tmp->nodes.tty_text->img = NULL;
    tmp->nodes.tty_text->nexttime =0;
    tmp->nodes.tty_text->dx = 0;
    tmp->nodes.tty_text->dy = 0;
    tmp->nodes.tty_text->txtsize = 0;

    return tmp;
}

int txt_DeleteTTYtext(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_TTYTEXT)
        return NODE_RET_NO;

    if (nod->nodes.tty_text->img != NULL)
        SDL_FreeSurface(nod->nodes.tty_text->img);

    if (nod->nodes.tty_text->fnt != NULL)
        TTF_CloseFont(nod->nodes.tty_text->fnt);

    if (nod->slot > 0)
    {
        SetgVarInt(nod->slot,2);
        setGNode(nod->slot,NULL);
    }

    delete nod;

    return NODE_RET_DELETE;
}

void ttynewline(struct_ttytext *tty)
{
    tty->dy+=tty->style.size;
    tty->dx=0;
}

void ttyscroll(struct_ttytext *tty)
{
    int32_t scroll=0;
    while (tty->dy-scroll > tty->h-tty->style.size)
        scroll+=tty->style.size;
    SDL_Surface *tmp = CreateSurface(tty->w,tty->h);
    DrawImageToSurf(tty->img,0,-scroll,tmp);
    SDL_FreeSurface(tty->img);
    tty->img=tmp;
    tty->dy-=scroll;
}

void outchartotty(uint16_t chr,struct_ttytext *tty)
{
    SDL_Color clr = {tty->style.red,tty->style.green,tty->style.blue,255};

    SDL_Surface *tmp_surf = TTF_RenderGlyph_Solid(tty->fnt,chr,clr);

    int32_t minx,maxx,miny,maxy,advice;
    TTF_GlyphMetrics(tty->fnt,chr,&minx,&maxx,&miny,&maxy,&advice);

    if (tty->dx+advice > tty->w)
        ttynewline(tty);

    if (tty->dy + tty->style.size + tty->style.size/4 > tty->h)
        ttyscroll(tty);

    DrawImageToSurf(tmp_surf,tty->dx,tty->dy+tty->style.size - maxy,tty->img);

    tty->dx += advice;

    SDL_FreeSurface(tmp_surf);
}

int32_t getglyphwidth(TTF_Font *fnt,uint16_t chr)
{
    int32_t minx,maxx,miny,maxy,advice;
    TTF_GlyphMetrics(fnt,chr,&minx,&maxx,&miny,&maxy,&advice);
    return advice;
}

int txt_ProcessTTYtext(struct_action_res *nod)
{
    if (nod->node_type != NODE_TYPE_TTYTEXT)
        return NODE_RET_NO;

    struct_ttytext *tty = nod->nodes.tty_text;

    tty->nexttime -= GetDTime();

    if (tty->nexttime < 0)
    {
        if (tty->txtpos < tty->txtsize)
        {
            if (tty->txtbuf[tty->txtpos] == '<')
            {
                int32_t strt=tty->txtpos, endt=0,ret=0;
                while(tty->txtbuf[tty->txtpos] != '>' && tty->txtpos < tty->txtsize)
                    tty->txtpos++;
                endt = tty->txtpos;
                if (strt != -1)
                    if ((endt-strt-1) > 0)
                        ret = txt_parse_txt_params(&tty->style,tty->txtbuf+strt+1,endt-strt-1);

                if (ret & (TXT_RET_FNTCHG | TXT_RET_FNTSTL | TXT_RET_NEWLN))
                {
                    if (ret & TXT_RET_FNTCHG)
                    {
                        TTF_CloseFont(tty->fnt);
                        tty->fnt = GetFontByName(tty->style.fontname,tty->style.size);
                        txt_set_font_style(tty->fnt,&tty->style);
                    }
                    if (ret & TXT_RET_FNTSTL)
                        txt_set_font_style(tty->fnt,&tty->style);

                    if (ret & TXT_RET_NEWLN)
                        ttynewline(tty);
                }

                if (ret & TXT_RET_HASSTBOX)
                {
                    char buf[MINIBUFSZ];
                    sprintf(buf,"%d",GetgVarInt(tty->style.statebox));

                    int32_t t_len = strlen(buf);
                    for (int8_t j=0; j<t_len; j++)
                        outchartotty(buf[j],tty);
                }

                tty->txtpos++;
            }
            else
            {
                int8_t charsz=GetUtf8CharSize(tty->txtbuf[tty->txtpos]);

                uint16_t chr = ReadUtf8Char(&tty->txtbuf[tty->txtpos]);



                if (chr == ' ')
                {
                    int32_t i=tty->txtpos + charsz;
                    int32_t width=getglyphwidth(tty->fnt,chr);

                    while (i < tty->txtsize && tty->txtbuf[i] != ' ' && tty->txtbuf[i] != '<')
                    {

                        int8_t chsz   = GetUtf8CharSize(tty->txtbuf[i]);
                        uint16_t uchr = ReadUtf8Char(&tty->txtbuf[i]);

                        width+=getglyphwidth(tty->fnt,uchr);;

                        i+=chsz;
                    }
                    if (tty->dx+width > tty->w)
                        ttynewline(tty);
                    else
                        outchartotty(chr,tty);


                }
                else
                    outchartotty(chr,tty);

                tty->txtpos+=charsz;
            }
            tty->nexttime = tty->delay;
            Rend_DrawImageToGamescr(tty->img,tty->x,tty->y);
        }
        else
        {
            txt_DeleteTTYtext(nod);
            return NODE_RET_DELETE;
        }


    }

    return NODE_RET_OK;
}



