#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED


#define CTRL_UNKN     0
#define CTRL_PUSH     1
#define CTRL_INPUT    2
#define CTRL_SLOT     3
#define CTRL_SAVE     4
#define CTRL_LEVER    5
#define CTRL_PANA    10
#define CTRL_FLAT    11
#define CTRL_TILT    12


struct Rect
{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};

void __InitRect(Rect *rct);


struct pushnode
{
    bool flat; //true - flat, false - warp
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    int16_t cursor;
    bool downed;
};

struct slotnode
{
    bool flat; //true - flat, false - warp
    Rect rectangle;
    //int warp;
    char distance_id[MINIBUFSZ];
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
    int32_t  period;
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

struct levernode
{
    int16_t cursor;
    //int32_t animation_id;
    bool mirrored;
    //uint32_t skipcolor;
    Rect AnimCoords;
    int16_t frames;
    int16_t startpos;
    animnode *anm;
    int16_t curfrm;
    int16_t rendfrm;
    struct hotspots
    {
        int16_t x;
        int16_t y;
        int16_t angles;
        struct directions
        {
            int16_t toframe;
            int16_t angle;
        } directions[CTRL_LEVER_MAX_DIRECTS];
    } hotspots[CTRL_LEVER_MAX_FRAMES];
    int16_t delta_x;
    int16_t delta_y;
    int16_t last_mouse_x;
    int16_t last_mouse_y;
    int16_t mouse_angle;
    int32_t mouse_count;
    bool   mouse_captured;
    int32_t hasout[CTRL_LEVER_MAX_FRAMES];  //seq's frame count
    int32_t outproc[CTRL_LEVER_MAX_FRAMES][CTRL_LEVER_MAX_FRAMES];  //seq's for every frames
    bool    autoout;      //seq initiated
    int32_t autoseq;      //what frame initiate this seq
    int32_t autoseq_frm;  //current seq frame
    int32_t autoseq_time; //time leave to next seq frame
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
        levernode    *lev;

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
void control_lever(ctrlnode *ct);

ctrlnode *GetControlByID(int32_t id);

#endif // CONTROL_H_INCLUDED
