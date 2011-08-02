


#ifndef ANIMS_H_INCLUDED
#define ANIMS_H_INCLUDED

struct animnode
{
    int32_t      x;
    int32_t      y;
    int32_t      w;
    int32_t      h;
    int32_t  start;
    int32_t    end;
    int32_t   loopcnt;
    int32_t   unk1; //???
    int32_t   unk2;
    int32_t   mask;
    int32_t   framerate;
    int32_t  CurFr;
    int32_t  nexttick;
    int32_t  loops;
    union anim
    {
        anim_surf *rlf;
        anim_avi  *avi;
    } anim;
    bool   vid;
    int32_t  playID;
    bool playing;
};

struct anim_preplay_node{
    int32_t pointingslot;
    int32_t playerid;
    int32_t      x;
    int32_t      y;
    int32_t      w;
    int32_t      h;
    int32_t  start;
    int32_t    end;
    int32_t   loop;
    animnode *point; //pointer to direct access
};


struct_action_res *anim_CreateAnimPlayNode();
struct_action_res *anim_CreateAnimPreNode();
struct_action_res *anim_CreateAnimPlayPreNode();

int anim_ProcessAnim(struct_action_res *nod);
void anim_LoadAnim(animnode *nod,char *filename,int u1, int u2, int32_t mask, int framerate);
void anim_DeleteAnim(animnode *nod);
int anim_PlayAnim(animnode *nod,int x, int y, int w, int h, int start, int end, int loop);


int anim_ProcessAnimPlayNode(struct_action_res *nod);
int anim_ProcessAnimPreNode(struct_action_res *nod);
int anim_ProcessAnimPrePlayNode(struct_action_res *nod);

int anim_DeleteAnimPlay(struct_action_res *nod);
int anim_DeleteAnimPreNod(struct_action_res *nod);
int anim_DeleteAnimPrePlayNode(struct_action_res *nod);


#endif // ANIMS_H_INCLUDED
