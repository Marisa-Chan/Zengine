#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED

#define TXT_STYLE_VAR_TRUE  1
#define TXT_STYLE_VAR_FALSE 0

#define TXT_JUSTIFY_CENTER 0
#define TXT_JUSTIFY_LEFT   1
#define TXT_JUSTIFY_RIGHT  2

#define TXT_RET_NOTHING    0x0
#define TXT_RET_FNTCHG     0x1
#define TXT_RET_FNTSTL     0x2
#define TXT_RET_NEWLN      0x4
#define TXT_RET_HASSTBOX   0x8


#define TXT_CFG_FONTNAME_MAX_LEN   64
//buffers size used for parsing text
#define TXT_CFG_BUF_MAX_LEN        512
//used for drawing text
#define TXT_CFG_TEXTURES_LINES     256
#define TXT_CFG_TEXTURES_PER_LINE  6

struct struct_txt_style{
    char    fontname[TXT_CFG_FONTNAME_MAX_LEN];
    int8_t  justify;  //0 - center, 1-left, 2-right
    int16_t  size;
    uint8_t  red;     //0-255
    uint8_t  green;   //0-255
    uint8_t  blue;    //0-255
    int8_t   newline;
    int8_t   escapement;
    int8_t   italic;   //0 - OFF, 1 - ON
    int8_t   bold;     //0 - OFF, 1 - ON
    int8_t   underline;//0 - OFF, 1 - ON
    int8_t   strikeout;//0 - OFF, 1 - ON
    int8_t   skipcolor;//0 - OFF, 1 - ON
    int32_t  statebox;
    //char image ??
};

void txt_init_txt_struct(struct_txt_style *style);
struct_txt_style *txt_NewTxt();
int8_t txt_parse_txt_params(struct_txt_style *style, const char *string, int32_t len);
void txt_DrawTxtWithJustify(char *txt, TTF_Font *fnt, SDL_Color clr, SDL_Surface *dst, int dy, int justify);
int32_t txt_get_linelen_for_width();
void txt_readfontstyle(struct_txt_style *style, char *strin);
void txt_set_font_style(TTF_Font *font, struct_txt_style *fnt_stl);
int32_t txt_DrawTxt(char *txt, struct_txt_style *fnt_stl, SDL_Surface *dst);
void txt_DrawTxtInOneLine(const char *text,SDL_Surface *dst);

struct struct_ttytext
{
    int32_t            x,y;
    int32_t            w,h;
    struct_txt_style   style;
    TTF_Font           *fnt;
    char               *txtbuf;
    int32_t            txtpos;
    int32_t            txtsize;
    int32_t            delay;
    int32_t            nexttime;
    SDL_Surface        *img;
    int32_t            dx,dy;
};

struct_action_res *txt_CreateTTYtext();
int txt_DeleteTTYtext(struct_action_res *nod);
int txt_ProcessTTYtext(struct_action_res *nod);

#endif // TEXT_H_INCLUDED
