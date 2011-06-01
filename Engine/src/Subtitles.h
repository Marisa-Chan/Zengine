#ifndef SUBTITLES_H_INCLUDED
#define SUBTITLES_H_INCLUDED

#include "Render.h"

struct struct_one_subtitle{
    int start;
    int stop;
    char *text;
};

struct struct_subtitles{
    struct_SubRect *SubRect;
    int subscount; //number of subs;

    struct_one_subtitle *subs;    //array

    int subtime; //total time
    int currentsub;
};


struct_subtitles *sub_LoadSubtitles(char *filename);
int sub_ProcessSub(struct_subtitles *sub);

#endif // SUBTITLES_H_INCLUDED
