


#ifndef ANIMS_H_INCLUDED
#define ANIMS_H_INCLUDED

struct animnode
{
    int      x;
    int      y;
    int      w;
    int      h;
    int  start;
    int    end;
    int   loopcnt;
    int   unk1; //???
    int   unk2;
    int   mask;
    int   framerate;
    int  CurFr;
    int  nexttick;
    int  loops;
    union anim
    {
        anim_surf *rlf;
        anim_avi  *avi;
    } anim;
    bool   vid;
};

struct animprenode{
    char    *fil;
    int32_t   u1;
    int32_t   u2;
    int32_t   mask;
    int32_t   framerate;
};


MList *anim_getpreloadLst();
MList *anim_getanimlst();

struct_action_res *anim_CreateAnimNode();
struct_action_res *anim_CreateAnimPreNode();

void anim_InitAnimLists();
int anim_ProcessAnim(struct_action_res *nod);
void anim_DeleteAnimNod(struct_action_res *nod);
void anim_DeleteAnimPreNod(struct_action_res *nod);
void anim_FlushAnims();
void anim_FlushPreload();

#endif // ANIMS_H_INCLUDED
