


#ifndef ANIMS_H_INCLUDED
#define ANIMS_H_INCLUDED

struct animnode
{
    uint32_t slot;
    int      x;
    int      y;
    int      w;
    int      h;
    int  start;
    int    end;
    int   loopcnt;
    int   unk1; //???
    int   unk2;
    int   unk3;
    int   unk4;
    int  CurFr;
//    uint32_t nexttick;
    int  loops;
    void *anim;
    bool   vid;
};

struct animprenode{
    uint32_t  slot;
    char    *fil;
    int32_t   u1;
    int32_t   u2;
    int32_t   u3;
    int32_t   u4;
};


MList *anim_getpreloadLst();
MList *anim_getanimlst();

void anim_InitAnimLists();
void anim_ProcessAnims();
void anim_DeleteAnimNod(animnode *nod);
void anim_FlushAnims();
void anim_FlushPreload();

#endif // ANIMS_H_INCLUDED
