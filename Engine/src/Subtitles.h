#ifndef SUBTITLES_H_INCLUDED
#define SUBTITLES_H_INCLUDED

#include "Render.h"

struct struct_one_subtitle{
    int start;
    int stop;
    int sub;
};

struct struct_font_style{
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
    //char image ??
    //int statebox O__o
};

struct struct_textfile{
    int count;
    char *buffer; //for all subs
    char **subs;   //for access to subs
    char **params; //for access to font params string
};

struct struct_subtitles{
    struct_SubRect *SubRect;
    int subscount; //number of subs;

    struct_one_subtitle *subs;
    struct_textfile *txt;    //array

    int subtime; //total time
    int currentsub;
};


struct_subtitles *sub_LoadSubtitles(char *filename);
int sub_ProcessSub(struct_subtitles *sub);
void sub_DeleteSub(struct_subtitles *sub);

#endif // SUBTITLES_H_INCLUDED
