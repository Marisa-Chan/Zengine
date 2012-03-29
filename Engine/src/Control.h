#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED


#define CTRL_UNKN     0
#define CTRL_PUSH     1
#define CTRL_INPUT    2
#define CTRL_SLOT     3
#define CTRL_SAVE     4
#define CTRL_LEVER    5
#define CTRL_SAFE     6
#define CTRL_FIST     7
#define CTRL_HOTMV    8
#define CTRL_PAINT    9
#define CTRL_TITLER   10
#define CTRL_PANA     100
#define CTRL_FLAT     101
#define CTRL_TILT     102


struct Rect
{
    int32_t x;
    int32_t y;
    union
    {
    int32_t w;
    int32_t x2;
    };
    union
    {
    int32_t h;
    int32_t y2;
    };
};

void InitRect(Rect *rct);

#define CTRL_PUSH_EV_UP   0
#define CTRL_PUSH_EV_DWN  1
#define CTRL_PUSH_EV_DBL  2


struct pushnode
{
    bool flat; //true - flat, false - warp
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    int16_t count_to;
    int16_t cursor;
    int8_t event; // 0 - up, 1 - down, 2 - double
};

struct slotnode
{
    bool flat; //true - flat, false - warp
    Rect rectangle;
    //int warp;
    char distance_id[MINIBUFSZ];
    Rect hotspot;
    //int do_skip;
    //int8_t event;
    int32_t *eligible_objects;
    int32_t eligable_cnt;
    int16_t cursor;
    int32_t loaded_img;
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

struct safenode
{
    int16_t  num_states;
    int16_t  cur_state;
    animnode *anm;
    int32_t  center_x;
    int32_t  center_y;
    Rect     rectangle;
    int16_t  radius_inner;
    int32_t  radius_inner_sq;
    int16_t  radius_outer;
    int32_t  radius_outer_sq;
    int16_t  zero_pointer;
    int16_t  start_pointer;
    int32_t  cur_frame;
    int32_t  to_frame;
    int32_t  frame_time;
};

struct titlernode
{
    int16_t      num_strings;
    char *       strings[CTRL_TITLER_MAX_STRINGS];
    Rect         rectangle;
    SDL_Surface  *surface;
    int16_t      current_string;
    int16_t      next_string;
};

struct paintnode
{
    uint8_t     *brush;
    int16_t     b_w;
    int16_t     b_h;
    SDL_Surface *paint;
    Rect     rectangle;
    int32_t  last_x;
    int32_t  last_y;
    int32_t  *eligible_objects;
    int32_t  eligable_cnt;
    int16_t  cursor;
};

struct fistnode
{
    uint32_t fiststatus;
    uint8_t  fistnum;
    int16_t  cursor;
    int8_t   order;
    struct   fists_up
    {
        int32_t num_box;
        Rect boxes[CTRL_FIST_MAX_BOXES];
    } fists_up[CTRL_FIST_MAX_FISTS];
    struct   fists_dwn
    {
        int32_t num_box;
        Rect boxes[CTRL_FIST_MAX_BOXES];
    } fists_dwn[CTRL_FIST_MAX_FISTS];

    int32_t num_entries;
    struct   entries
    {
        uint32_t strt;
        uint32_t send;
        int32_t anm_str;
        int32_t anm_end;
        int32_t sound;
    } entries[CTRL_FIST_MAX_ENTRS];

    animnode *anm;
    Rect     anm_rect;
    int32_t  soundkey;
    int32_t  frame_cur;
    int32_t  frame_end;
    int32_t  frame_time;
    int32_t  animation_id;
};

struct hotmvnode
{
    int32_t  num_frames;
    int32_t  frame_time;
    int32_t  cur_frame;
    int32_t  rend_frame;
    int32_t  cycle;
    int32_t  num_cycles;
    Rect    *frame_list;
    animnode *anm;
    Rect     rect;
};

struct ctrlnode
{
    int32_t slot;
    int8_t  type;
    int32_t venus;
    union node
    {
        slotnode     *slot;
        pushnode     *push;
        inputnode    *inp;
        saveloadnode *svld;
        levernode    *lev;
        safenode     *safe;
        fistnode     *fist;
        hotmvnode    *hotmv;
        paintnode    *paint;
        titlernode   *titler;

        void  *unknown;
    } node;
    void   (*func)(ctrlnode *);
};


ctrlnode *Ctrl_CreateNode(int type);

void ctrl_setvenus(ctrlnode *nod);

int Parse_Control(MList *controlst,mfile *fl,char *ctstr);
void ProcessControls(MList *ctrlst);

bool Ctrl_Eligeblity(int obj, slotnode *slut);
bool Ctrl_Eligeblity(int obj, int32_t *slots, int32_t count);

void Ctrl_DrawControls();

void DeleteControlList(MList *lst);
void FlushControlList(MList *lst);


void control_slot(ctrlnode *ct);
void control_push(ctrlnode *ct);
void control_input(ctrlnode *ct);
void control_save(ctrlnode *ct);
void control_lever(ctrlnode *ct);

void control_safe(ctrlnode *ct);
void control_safe_draw(ctrlnode *ct);

void control_fist(ctrlnode *ct);
void control_fist_draw(ctrlnode *ct);

void control_titler(ctrlnode *ct);
void control_titler_draw(ctrlnode *ct);

void control_paint(ctrlnode *ct);

void control_hotmv(ctrlnode *ct);
void control_hotmv_draw(ctrlnode *ct);

ctrlnode *GetControlByID(int32_t id);

#endif // CONTROL_H_INCLUDED
