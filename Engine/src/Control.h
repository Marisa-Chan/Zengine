#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

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
    uint16_t cursor;
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
    uint16_t cursor;
    SDL_Surface *srf;
};

struct ctrlnode
{
    uint32_t slot;
    uint8_t  type;
    union
    {
        slotnode *slot;
        pushnode *push;
    } node;
    void   (*func)(ctrlnode *);
};



int Parse_Control(MList *controlst,FILE *fl,char *ctstr);

bool Ctrl_Eligeblity(int obj, slotnode *slut);
void Ctrl_DrawSlots();


#endif // CONTROL_H_INCLUDED
