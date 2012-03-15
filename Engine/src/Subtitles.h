#ifndef SUBTITLES_H_INCLUDED
#define SUBTITLES_H_INCLUDED

#include "Render.h"


#define SUB_CORRECT_VERT   (-16)
#define SUB_CORRECT_HORIZ  0


struct struct_one_subtitle{
    int start;
    int stop;
    int sub;
};

struct struct_textfile{
    int count;
    char *buffer; //for all subs
    char **subs;   //for access to subs
};

struct struct_subtitles{
    struct_SubRect *SubRect;
    int subscount; //number of subs;

    struct_one_subtitle *subs;
    struct_textfile *txt;    //array

    int currentsub;
};


struct_subtitles *sub_LoadSubtitles(char *filename);
void sub_ProcessSub(struct_subtitles *sub, int subtime);
void sub_DeleteSub(struct_subtitles *sub);

#endif // SUBTITLES_H_INCLUDED
