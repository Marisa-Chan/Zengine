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

struct struct_txt_style{
    char    fontname[64];
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
int8_t txt_parse_txt_params(struct_txt_style *style, char *string, int32_t len);
void txt_DrawTxtWithJustify(char *txt, TTF_Font *fnt, SDL_Color clr, SDL_Surface *dst, int dy, int justify);
int32_t txt_get_linelen_for_width();
void txt_readfontstyle(struct_txt_style *style, char *strin);
void txt_set_font_style(TTF_Font *font, struct_txt_style *fnt_stl);
int32_t txt_DrawTxt(char *txt, struct_txt_style *fnt_stl, SDL_Surface *dst);
void txt_DrawTxtInOneLine(char *text,SDL_Surface *dst);

#endif // TEXT_H_INCLUDED
