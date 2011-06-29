#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED


#define CTRL_UNKN     0
#define CTRL_PUSH     1
#define CTRL_INPUT    2
#define CTRL_SLOT     3
#define CTRL_SAVE     4
#define CTRL_PANA    10
#define CTRL_FLAT    11


struct Rect
{
    int x;
    int y;
    int w;
    int h;
};


struct pushnode
{
    bool flat; //true - flat, false - warp
    int x;
    int y;
    int w;
    int h;
    int16_t cursor;
};

struct slotnode
{
    bool flat; //true - flat, false - warp
    Rect rectangle;
    //int warp;
    //int distance_id;
    Rect hotspot;
    //int do_skip;
    int *eligible_objects;
    int eligable_cnt;
    int16_t cursor;
    SDL_Surface *srf;
};

struct inputnode
{
    Rect rectangle;
    Rect hotspot;
    int next_tab;
    char *text;
    anim_surf *cursor;
    int  frames;
};

struct ctrlnode
{
    int32_t slot;
    int8_t  type;
    union node
    {
        slotnode *slot;
        pushnode *push;
        inputnode *inp;

        void  *unknown;
    } node;
    void   (*func)(ctrlnode *);
};


ctrlnode *Ctrl_CreateNode(int type);

int Parse_Control(MList *controlst,FILE *fl,char *ctstr);

bool Ctrl_Eligeblity(int obj, slotnode *slut);
void Ctrl_DrawSlots();

void DeleteControlList(MList *lst);
void FlushControlList(MList *lst);


void control_slot(ctrlnode *ct);
void control_push(ctrlnode *ct);
void control_input(ctrlnode *ct);
#endif // CONTROL_H_INCLUDED
