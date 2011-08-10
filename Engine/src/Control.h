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
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};


struct pushnode
{
    bool flat; //true - flat, false - warp
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
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
    int32_t *eligible_objects;
    int32_t eligable_cnt;
    int16_t cursor;
    SDL_Surface *srf;
};

struct inputnode
{
    Rect rectangle;
    Rect hotspot;
    SDL_Surface *rect;
    int32_t next_tab;

    char text[SAVE_NAME_MAX_LEN+1];
    int16_t textwidth;
    bool textchanged;

    anim_surf *cursor;
    int32_t  frame;
    bool readonly;
    bool enterkey;

    bool focused;
    struct_txt_style string_init;
    struct_txt_style string_chooser_init;
};

struct saveloadnode
{
    bool forsaving;
    int32_t  inputslot[MAX_SAVES];
    ctrlnode *input_nodes[MAX_SAVES];
    char     Names[MAX_SAVES][SAVE_NAME_MAX_LEN+1];
};

struct ctrlnode
{
    int32_t slot;
    int8_t  type;
    union node
    {
        slotnode     *slot;
        pushnode     *push;
        inputnode    *inp;
        saveloadnode *svld;

        void  *unknown;
    } node;
    void   (*func)(ctrlnode *);
};


ctrlnode *Ctrl_CreateNode(int type);

int Parse_Control(MList *controlst,FILE *fl,char *ctstr);
void ProcessControls(MList *ctrlst);

bool Ctrl_Eligeblity(int obj, slotnode *slut);
void Ctrl_DrawControls();

void DeleteControlList(MList *lst);
void FlushControlList(MList *lst);


void control_slot(ctrlnode *ct);
void control_push(ctrlnode *ct);
void control_input(ctrlnode *ct);
void control_save(ctrlnode *ct);

ctrlnode *GetControlByID(int32_t id);

#endif // CONTROL_H_INCLUDED
