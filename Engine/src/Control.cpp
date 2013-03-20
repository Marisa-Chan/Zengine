#include "System.h"
#include <errno.h>
#include <math.h>

int FocusInput=0;
bool pushChangeMouse=false;

pushnode * CreatePushNode()
{
    pushnode *tmp = new(pushnode);
    tmp->cursor = CURSOR_IDLE;
    tmp->flat = false;
    tmp->x = 0;
    tmp->y = 0;
    tmp->w = 0;
    tmp->h = 0;
    tmp->count_to = 2;
    tmp->event = CTRL_PUSH_EV_UP;
    return tmp;
}

inputnode * CreateInputNode()
{
    inputnode *tmp = new(inputnode);
    tmp->rect = NULL;
    InitRect(&tmp->rectangle);
    InitRect(&tmp->hotspot);
    tmp->cursor = NULL;
    tmp->frame = 0;
    tmp->period = 0;
    tmp->readonly = false;
    tmp->textwidth = 0;
    tmp->textchanged = true;
    tmp->enterkey  = false;
    tmp->focused   = false;
    memset(tmp->text,0,SAVE_NAME_MAX_LEN+1);
    txt_init_txt_struct(&tmp->string_init);
    txt_init_txt_struct(&tmp->string_chooser_init);
    return tmp;
}

slotnode * CreateSlotNode()
{
    slotnode *tmp = new(slotnode);
    tmp->cursor = CURSOR_IDLE;
    tmp->eligable_cnt = 0;
    tmp->eligible_objects = NULL;
    tmp->srf = NULL;
    tmp->loaded_img = -1;
    InitRect(&tmp->rectangle);
    InitRect(&tmp->hotspot);
    tmp->flat = false;
    memset((void *)&tmp->distance_id,0,MINIBUFSZ);
    return tmp;
}

fistnode * CreateFistNode()
{
    fistnode *tmp = new(fistnode);
    tmp->cursor   = CURSOR_IDLE;
    tmp->anm      = NULL;
    tmp->soundkey = 0;
    tmp->fiststatus = 0;
    tmp->order    = 0;
    tmp->fistnum  = 0;

    tmp->frame_cur = -1;
    tmp->frame_end = -1;
    tmp->frame_time = 0;
    tmp->animation_id = 0;

    for (int32_t i=0; i<CTRL_FIST_MAX_FISTS; i++)
    {
        tmp->fists_up[i].num_box = 0;
        tmp->fists_dwn[i].num_box = 0;
    }


    tmp->num_entries = 0;
    for (int32_t i=0; i<CTRL_FIST_MAX_ENTRS; i++)
    {
        tmp->entries[i].anm_end = 0;
        tmp->entries[i].anm_str = 0;
        tmp->entries[i].sound   = 0;
        tmp->entries[i].send    = 0;
        tmp->entries[i].strt    = 0;
    }

    InitRect(&tmp->anm_rect);
    return tmp;
}

hotmvnode * CreateHotMovieNode()
{
    hotmvnode *tmp = new(hotmvnode);
    tmp->anm = NULL;
    tmp->cycle = 0;
    tmp->cur_frame = -1;
    tmp->rend_frame = -1;
    tmp->frame_list = NULL;
    tmp->frame_time = 0;
    tmp->num_cycles = 0;
    tmp->num_frames = 0;
    InitRect(&tmp->rect);
    return tmp;
}

levernode * CreateLeverNode()
{
    levernode *tmp = new(levernode);
    tmp->cursor = CURSOR_IDLE;
    InitRect(&tmp->AnimCoords);
    tmp->anm = NULL;
    tmp->curfrm = 0;
    tmp->delta_x = 0;
    tmp->delta_y = 0;
    tmp->frames = 0;
    tmp->startpos = 0;
    tmp->mirrored = 0;
    tmp->rendfrm = -1;
    tmp->last_mouse_x = 0;
    tmp->last_mouse_y = 0;
    tmp->mouse_angle = 0;
    tmp->mouse_count = 0;
    tmp->mouse_captured = false;
    tmp->autoout = false;
    tmp->autoseq = -1;
    tmp->autoseq_frm = 0;
    tmp->autoseq_time = 0;
    for (int16_t i=0; i< CTRL_LEVER_MAX_FRAMES; i++)
    {
        tmp->hotspots[i].x = 0;
        tmp->hotspots[i].y = 0;
        tmp->hotspots[i].angles = 0;
        for (int16_t j=0; j< CTRL_LEVER_MAX_DIRECTS; j++)
        {
            tmp->hotspots[i].directions[j].angle = 0;
            tmp->hotspots[i].directions[j].toframe = -1;
        }

        tmp->hasout[i] = 0;
    }
    return tmp;
}

saveloadnode * CreateSaveNode()
{
    saveloadnode *tmp = new(saveloadnode);

    tmp->forsaving = false;

    for (int32_t i=0; i<MAX_SAVES; i++)
    {
        tmp->inputslot[i] = -1;
        tmp->input_nodes[i] = NULL;
        memset(tmp->Names[i],0,SAVE_NAME_MAX_LEN+1);

    }

    return tmp;
}

safenode * CreateSafeNode()
{
    safenode *tmp = new(safenode);

    tmp->anm = NULL;
    tmp->center_y  = 0;
    tmp->center_x  = 0;
    tmp->cur_state = 0;
    tmp->num_states = 0;
    tmp->radius_inner = 0;
    tmp->radius_outer = 0;
    tmp->radius_inner_sq = 0;
    tmp->radius_outer_sq = 0;
    tmp->cur_frame = -1;
    tmp->to_frame = 0;
    tmp->frame_time = 0;
    InitRect(&tmp->rectangle);
    return tmp;
}

paintnode * CreatePaintNode()
{
    paintnode *tmp = new(paintnode);

    tmp->eligable_cnt = 0;
    tmp->eligible_objects  = NULL;
    tmp->brush = NULL;
    tmp->paint = NULL;
    tmp->last_x = 0;
    tmp->last_y = 0;
    tmp->cursor = CURSOR_IDLE;
    InitRect(&tmp->rectangle);
    return tmp;
}

titlernode * CreateTitlerNode()
{
    titlernode *tmp = new(titlernode);

    tmp->num_strings = 0;
    tmp->surface  = NULL;
    InitRect(&tmp->rectangle);

    for (int32_t i=0; i<CTRL_TITLER_MAX_STRINGS; i++)
        tmp->strings[i] = NULL;

    return tmp;
}

ctrlnode *Ctrl_CreateNode(int type)
{
    ctrlnode *tmp;
    tmp = new(ctrlnode);
    tmp->node.unknown = NULL;
    tmp->slot = 0;
    tmp->venus = -1;
    tmp->type = CTRL_UNKN;
    tmp->func = NULL;

    switch(type)
    {
    case CTRL_PUSH:
        tmp->type = CTRL_PUSH;
        tmp->node.push = CreatePushNode();
        tmp->func = control_push;
        break;

    case CTRL_INPUT:
        tmp->type = CTRL_INPUT;
        tmp->node.inp = CreateInputNode();
        tmp->func = control_input;
        break;

    case CTRL_SLOT:
        tmp->type = CTRL_SLOT;
        tmp->node.slot = CreateSlotNode();
        tmp->func = control_slot;
        break;

    case CTRL_SAVE:
        tmp->type = CTRL_SAVE;
        tmp->node.svld = CreateSaveNode();
        tmp->func = control_save;
        break;
    case CTRL_LEVER:
        tmp->type = CTRL_LEVER;
        tmp->node.lev = CreateLeverNode();
        tmp->func = control_lever;
        break;
    case CTRL_SAFE:
        tmp->type = CTRL_SAFE;
        tmp->node.safe = CreateSafeNode();
        tmp->func = control_safe;
        break;
    case CTRL_FIST:
        tmp->type = CTRL_FIST;
        tmp->node.fist = CreateFistNode();
        tmp->func = control_fist;
        break;
    case CTRL_HOTMV:
        tmp->type = CTRL_HOTMV;
        tmp->node.hotmv = CreateHotMovieNode();
        tmp->func = control_hotmv;
        break;
    case CTRL_PAINT:
        tmp->type = CTRL_PAINT;
        tmp->node.paint = CreatePaintNode();
        tmp->func = control_paint;
        break;
    case CTRL_TITLER:
        tmp->type = CTRL_TITLER;
        tmp->node.titler = CreateTitlerNode();
        tmp->func = control_titler;
        break;
    };
    return tmp;
}

void InitRect(Rect *rct)
{
    rct->h = 0;
    rct->w = 0;
    rct->x = 0;
    rct->y = 0;
}

bool Ctrl_Eligeblity(int obj, slotnode *slot)
{
    bool eli = false;

    for (int i=0; i< slot->eligable_cnt; i++)
        if (obj == slot->eligible_objects[i])
        {
            eli = true;
            break;
        }
    return eli;
}

bool Ctrl_Eligeblity(int obj, int32_t *slots, int32_t count)
{
    bool eli = false;

    for (int i=0; i< count; i++)
        if (obj == slots[i])
        {
            eli = true;
            break;
        }
    return eli;
}

void control_slot_draw(ctrlnode *nod)
{
    slotnode *slut = nod->node.slot;

    int tmp1 = GetgVarInt(nod->slot);

    if (tmp1 != 0)
    {
        if (slut->loaded_img != tmp1)
            if (slut->srf != NULL)
            {
                SDL_FreeSurface(slut->srf);
                slut->srf=NULL;
                slut->loaded_img = -1;
            }

        if (slut->srf==NULL && slut->loaded_img != tmp1)
        {
            char bff[16];
#ifdef GAME_ZGI
            sprintf(bff,CTRL_SLOT_FILE_NAME,slut->distance_id,tmp1);
#endif
#ifdef GAME_NEMESIS
            sprintf(bff,CTRL_SLOT_FILE_NAME,tmp1,slut->distance_id);
#endif

            slut->srf=loader_LoadFile(bff,0,Rend_MapScreenRGB(0,0,0));

            slut->loaded_img = tmp1;
        }

        if (slut->srf)
        {
            int32_t drawx = slut->rectangle.x;
            int32_t drawy = slut->rectangle.y;

            if ((slut->rectangle.w - slut->rectangle.x) > slut->srf->w)
                drawx = slut->rectangle.x + ((slut->rectangle.w - slut->rectangle.x) - slut->srf->w) / 2 ;

            if ((slut->rectangle.h - slut->rectangle.y) > slut->srf->h)
                drawy = slut->rectangle.y + ((slut->rectangle.h - slut->rectangle.y) - slut->srf->h) / 2 ;

            Rend_DrawImageUpGamescr(slut->srf,    drawx + GAMESCREEN_FLAT_X,  drawy);
        }

    }
    else
    {
        if (slut->srf != NULL)
        {
            SDL_FreeSurface(slut->srf);
            slut->srf=NULL;
            slut->loaded_img = -1;
        }

    }

}

void control_input_draw(ctrlnode *ct)
{
    inputnode *inp = ct->node.inp;

    if (strlen(inp->text))
    {
        if (inp->textchanged)
        {

            SDL_FillRect(inp->rect,NULL,0);

            if (!inp->readonly || !inp->focused)
                inp->textwidth = txt_DrawTxt(inp->text,&inp->string_init,inp->rect);
            else
                inp->textwidth = txt_DrawTxt(inp->text,&inp->string_chooser_init,inp->rect);

            inp->textchanged = false;
        }
        Rend_DrawImageUpGamescr(inp->rect,inp->rectangle.x + GAMESCREEN_FLAT_X,inp->rectangle.y);
    }
    else
        inp->textwidth = 0;


    if (FocusInput == ct->slot)
    {
        if (!inp->readonly)
        {
            if (inp->cursor != NULL)
            {

                Rend_DrawImageUpGamescr(inp->cursor,inp->rectangle.x + GAMESCREEN_FLAT_X + inp->textwidth,inp->rectangle.y,inp->frame);

                inp->period -= GetDTime();

                if (inp->period <= 0)
                {
                    inp->frame++;
                    inp->period = inp->cursor->info.time;
                }

                int32_t max_frames = inp->cursor->info.frames;

                if (inp->frame >= max_frames)
                    inp->frame = 0;
            }
        }
    }
}

void control_lever_draw(ctrlnode *ct)
{
    levernode *lev = ct->node.lev;
    if (lev->curfrm == lev->rendfrm)
        return;

    if ((lev->rendfrm > lev->curfrm) && lev->mirrored)
        anim_RenderAnimFrame(lev->anm, lev->AnimCoords.x, lev->AnimCoords.y, lev->AnimCoords.w, lev->AnimCoords.h , lev->frames*2 -1 - lev->curfrm);
    else
        anim_RenderAnimFrame(lev->anm, lev->AnimCoords.x, lev->AnimCoords.y, lev->AnimCoords.w, lev->AnimCoords.h , lev->curfrm);

    lev->rendfrm = lev->curfrm;
}

void Ctrl_DrawControls()
{
    MList *ctrl = Getctrl();
    StartMList(ctrl);

    while (!eofMList(ctrl))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(ctrl);

        if (!(ScrSys_GetFlag(nod->slot) & FLAG_DISABLED))
        {
            if (nod->type == CTRL_SLOT)
                control_slot_draw(nod);
            else if (nod->type == CTRL_INPUT)
                control_input_draw(nod);
            else if (nod->type == CTRL_LEVER)
                control_lever_draw(nod);
            else if (nod->type == CTRL_SAFE)
                control_safe_draw(nod);
            else if (nod->type == CTRL_HOTMV)
                control_hotmv_draw(nod);
            else if (nod->type == CTRL_FIST)
                control_fist_draw(nod);
            else if (nod->type == CTRL_TITLER)
                control_titler_draw(nod);
        }
        NextMList(ctrl);
    }
}


void control_input(ctrlnode *ct)
{
    inputnode *inp = ct->node.inp;
    bool mousein = false;

    if ( inp->rectangle.x <= Rend_GetMouseGameX() &&\
         inp->rectangle.w >= Rend_GetMouseGameX() &&\
         inp->rectangle.y <= Rend_GetMouseGameY() &&\
         inp->rectangle.h >= Rend_GetMouseGameY() )
        mousein = true;
    if ( inp->hotspot.x <= Rend_GetMouseGameX() &&\
         inp->hotspot.w >= Rend_GetMouseGameX() &&\
         inp->hotspot.y <= Rend_GetMouseGameY() &&\
         inp->hotspot.h >= Rend_GetMouseGameY() )
        mousein = true;

    if (FocusInput == ct->slot)
    {
        if (!inp->readonly)
        {
            if (KeyAnyHit())
            {
                SDLKey key = GetLastKey();
                int tmplen = strlen(inp->text);

                if ((key>=SDLK_0 && key<=SDLK_9) ||
                        (key>=SDLK_a && key<=SDLK_z) ||
                        (key==SDLK_SPACE))
                {
                    if (tmplen < SAVE_NAME_MAX_LEN)
                    {
                        inp->text[tmplen] = key;
                        inp->textchanged  = true;
                    }
                }
                else if(key == SDLK_BACKSPACE)
                {
                    if (tmplen > 0)
                    {
                        inp->text[tmplen-1] = 0x0;
                        inp->textchanged  = true;
                    }
                }
                else if(key == SDLK_RETURN)
                {
                    // if (tmplen > 0)
                    inp->enterkey = true;
                }
                else if(key == SDLK_TAB)
                {
                    if (inp->next_tab > 0)
                        FocusInput = inp->next_tab;

                    FlushKeybKey(SDLK_TAB);
                }
                else if(key == SDLK_ESCAPE)
                {
                    inp->text[0] = 0;
                    inp->textchanged = true;
                }

            }
        }
        else
        {
            if (KeyHit(SDLK_TAB))
            {
                if (inp->next_tab > 0)
                {
                    int32_t cycle = inp->next_tab;
                    ctrlnode *c_tmp = ct;
                    while(cycle > 0 && cycle != ct->slot && c_tmp != NULL)
                    {
                        c_tmp = GetControlByID(cycle);

                        if (c_tmp)
                            if (c_tmp->type == CTRL_INPUT)
                            {
                                if (strlen(c_tmp->node.inp->text) > 0)
                                {
                                    FocusInput = cycle;
                                    break;
                                }
                                else
                                    cycle = c_tmp->node.inp->next_tab;
                            }
                    }
                }

                FlushKeybKey(SDLK_TAB);
            }
            else if (KeyHit(SDLK_RETURN))
            {
                inp->enterkey = true;
            }
        }
    }

    if (inp->readonly)
    {
        if (FocusInput == ct->slot && !inp->focused)
        {
            inp->textchanged = true;
            inp->focused = true;
        }
        else if (FocusInput != ct->slot && inp->focused)
        {
            inp->textchanged = true;
            inp->focused = false;
        }
    }

    if (mousein)
    {
        if (!inp->readonly)
        {
            if (Mouse_IsCurrentCur(CURSOR_IDLE))
                Mouse_SetCursor(CURSOR_ACTIVE);

            if (MouseUp(SDL_BUTTON_LEFT))
            {
                ctrl_setvenus(ct);
                FocusInput = ct->slot;
                FlushMouseBtn(SDL_BUTTON_LEFT);
            }
        }
        else if (inp->text[0]>=' ')
        {
            if (Mouse_IsCurrentCur(CURSOR_IDLE))
                Mouse_SetCursor(CURSOR_ACTIVE);

            if (MouseMove())
                FocusInput = ct->slot;

            if (MouseUp(SDL_BUTTON_LEFT))
            {
                ctrl_setvenus(ct);
                FocusInput = 0;
                inp->enterkey = true;
            }

        }


    }
}

void control_slot(ctrlnode *ct)
{
    slotnode *slut = ct->node.slot;
    bool mousein = false;

    if ( slut->hotspot.x <= Rend_GetMouseGameX() &&\
            slut->hotspot.w >= Rend_GetMouseGameX() &&\
            slut->hotspot.y <= Rend_GetMouseGameY() &&\
            slut->hotspot.h >= Rend_GetMouseGameY() )
        mousein = true;

    if (mousein)
    {

        //  if (GetgVarInt(ct->slot)!=0)
        if (Mouse_IsCurrentCur(CURSOR_IDLE))
            Mouse_SetCursor(slut->cursor);

        if (MouseUp(SDL_BUTTON_LEFT))
        {

            ctrl_setvenus(ct);

            int32_t item = GetgVarInt(ct->slot);
            int32_t mouse_item = GetgVarInt(SLOT_INVENTORY_MOUSE);
            if ( item != 0 )
            {
                if ( mouse_item != 0)
                {
                    if (Ctrl_Eligeblity(mouse_item, slut))
                    {
                        inv_drop(mouse_item);
                        inv_add(item);
                        SetgVarInt(ct->slot,mouse_item);
                    }
                }
                else
                {
                    inv_add(item);
                    SetgVarInt(ct->slot,0);
                }
            }
            else if ( mouse_item == 0 )
            {
                if ( Ctrl_Eligeblity(0, slut))
                {
                    inv_drop(0);
                    SetgVarInt(ct->slot,0);
                }
            }
            else if ( Ctrl_Eligeblity(mouse_item, slut))
            {
                SetgVarInt(ct->slot,mouse_item);
                inv_drop(mouse_item);
            }

#ifdef TRACE
            printf("Pushed\n");
            printf("Slot #%d to 1\n",ct->slot);
#endif
            FlushMouseBtn(SDL_BUTTON_LEFT);

        }

    }
}

void control_paint(ctrlnode *ct)
{
    bool mousein=false;
    paintnode *paint = ct->node.paint;

    if (!Rend_MouseInGamescr())
        return;

    int32_t mX = Rend_GetMouseGameX();
    int32_t mY = Rend_GetMouseGameY();

    if (mX >= paint->rectangle.x                      &&\
            mX <  paint->rectangle.x + paint->rectangle.w &&\
            mY >= paint->rectangle.y                      &&\
            mY <  paint->rectangle.y + paint->rectangle.h)
        mousein = true;

    if (mousein)
    {
        if (Ctrl_Eligeblity(GetgVarInt(SLOT_INVENTORY_MOUSE),paint->eligible_objects,paint->eligable_cnt))
        {
            if (Mouse_IsCurrentCur(CURSOR_IDLE))
                Mouse_SetCursor(paint->cursor);

            if (MouseDown(SDL_BUTTON_LEFT))
            {

                ctrl_setvenus(ct);

                if (mX != paint->last_x || mY != paint->last_y)
                {
                    SDL_Surface *scrn = Rend_GetLocationScreenImage();
                    SDL_LockSurface(scrn);
                    SDL_LockSurface(paint->paint);

                    int32_t cen_x = 0;// paint->b_w / 2;
                    int32_t cen_y = 0;//paint->b_h / 2;

                    int32_t d_x = mX - paint->rectangle.x;
                    int32_t d_y = mY - paint->rectangle.y;

                    if (GAME_BPP == 32)
                    {
                        uint32_t *px = (uint32_t *)scrn->pixels;
                        uint32_t *fr = (uint32_t *)paint->paint->pixels;

                        for (int32_t y=0; y<paint->b_h; y++)
                            for (int32_t x=0; x<paint->b_w; x++)
                                if (paint->brush[x+y*paint->b_w] != 0)
                                    if ((d_x - cen_x)+x >= 0              &&\
                                            (d_x - cen_x)+x < paint->paint->w &&\
                                            (d_y - cen_y)+y >= 0              &&\
                                            (d_y - cen_y)+y < paint->paint->h )
                                    {
                                        int32_t rel_x = (mX - cen_x) + x;
                                        int32_t rel_y = (mY - cen_y) + y;
                                        int32_t fr_x  = (d_x - cen_x)+x;
                                        int32_t fr_y  = (d_y - cen_y)+y;
                                        px[rel_x + rel_y*scrn->w] = fr[fr_x + fr_y*paint->paint->w];
                                    }



                    }
                    else if (GAME_BPP == 16)
                    {
                        uint16_t *px = (uint16_t *)scrn->pixels;
                        uint16_t *fr = (uint16_t *)paint->paint->pixels;

                        for (int32_t y=0; y<paint->b_h; y++)
                            for (int32_t x=0; x<paint->b_w; x++)
                                if (paint->brush[x+y*paint->b_w] != 0)
                                    if ((d_x - cen_x)+x >= 0              &&\
                                            (d_x - cen_x)+x < paint->paint->w &&\
                                            (d_y - cen_y)+y >= 0              &&\
                                            (d_y - cen_y)+y < paint->paint->h )
                                    {
                                        int32_t rel_x = (mX - cen_x) + x;
                                        int32_t rel_y = (mY - cen_y) + y;
                                        int32_t fr_x  = (d_x - cen_x)+x;
                                        int32_t fr_y  = (d_y - cen_y)+y;
                                        px[rel_x + rel_y*scrn->w] = fr[fr_x + fr_y*paint->paint->w];
                                    }
                    }
                    else
                    {
                        printf("Write your code for %d bit mode in %s at %d line.\n",GAME_BPP,__FILE__,__LINE__);
                    }
                    SDL_UnlockSurface(scrn);
                    SDL_UnlockSurface(paint->paint);

                    paint->last_x = mX;
                    paint->last_y = mY;
                }
            }
        }
    }
}

void control_fist(ctrlnode *ct)
{
    bool mousein=false;
    fistnode *fist = ct->node.fist;
    int32_t n_fist = -1;


    if (!Rend_MouseInGamescr())
        return;

    int32_t mX = Rend_GetMouseGameX();
    int32_t mY = Rend_GetMouseGameY();

    if (fist->order != 0)
    {
        for(int32_t i=0; i<fist->fistnum; i++)
        {
            if (((fist->fiststatus >> i) & 1) == 1)
            {
                for(int32_t j=0; j<fist->fists_dwn[i].num_box; j++)
                    if (fist->fists_dwn[i].boxes[j].x  <= mX &&\
                            fist->fists_dwn[i].boxes[j].x2 >= mX &&\
                            fist->fists_dwn[i].boxes[j].y  <= mY &&\
                            fist->fists_dwn[i].boxes[j].y2 >= mY
                       )
                    {
                        mousein = true;
                        n_fist = i;
                        break;
                    }
            }
            else
            {
                for(int32_t j=0; j<fist->fists_up[i].num_box; j++)
                    if (fist->fists_up[i].boxes[j].x  <= mX &&\
                            fist->fists_up[i].boxes[j].x2 >= mX &&\
                            fist->fists_up[i].boxes[j].y  <= mY &&\
                            fist->fists_up[i].boxes[j].y2 >= mY
                       )
                    {
                        mousein = true;
                        n_fist = i;
                        break;
                    }
            }

            if (mousein)
                break;

        }
    }
    else
    {
        for(int32_t i=fist->fistnum-1; i>=0; i--)
        {
            if (((fist->fiststatus >> i) & 1) == 1)
            {
                for(int32_t j=0; j<fist->fists_dwn[i].num_box; j++)
                    if (fist->fists_dwn[i].boxes[j].x  <= mX &&\
                            fist->fists_dwn[i].boxes[j].x2 >= mX &&\
                            fist->fists_dwn[i].boxes[j].y  <= mY &&\
                            fist->fists_dwn[i].boxes[j].y2 >= mY
                       )
                    {
                        mousein = true;
                        n_fist = i;
                        break;
                    }
            }
            else
            {
                for(int32_t j=0; j<fist->fists_up[i].num_box; j++)
                    if (fist->fists_up[i].boxes[j].x  <= mX &&\
                            fist->fists_up[i].boxes[j].x2 >= mX &&\
                            fist->fists_up[i].boxes[j].y  <= mY &&\
                            fist->fists_up[i].boxes[j].y2 >= mY
                       )
                    {
                        mousein = true;
                        n_fist = i;
                        break;
                    }
            }

            if (mousein)
                break;

        }
    }


    if (mousein)
    {
        if (Mouse_IsCurrentCur(CURSOR_IDLE))
            Mouse_SetCursor(CURSOR_ACTIVE);

        if (MouseUp(SDL_BUTTON_LEFT))
        {

            ctrl_setvenus(ct);

            uint32_t old_status = fist->fiststatus;
            fist->fiststatus ^= (1 << n_fist);

            for(int32_t i=0; i<fist->num_entries; i++)
                if (fist->entries[i].strt == old_status &&\
                        fist->entries[i].send == fist->fiststatus)
                {
                    //printf("fist_seq %d \n",i);
                    fist->frame_cur = fist->entries[i].anm_str;
                    fist->frame_end = fist->entries[i].anm_end;
                    fist->frame_time = 0;

                    //printf("%d %d\n",fist->soundkey,fist->entries[i].sound);
                    SetgVarInt(fist->animation_id,1);
                    SetgVarInt(fist->soundkey, fist->entries[i].sound);
                    break;
                }

            SetgVarInt(ct->slot,fist->fiststatus);

            FlushMouseBtn(SDL_BUTTON_LEFT);
        }
    }




}

void control_fist_draw(ctrlnode *ct)
{


    fistnode *fist = ct->node.fist;

    if (fist->frame_cur >= 0 && fist->frame_end >= 0)
        if (fist->frame_cur <= fist->frame_end)
        {
            fist->frame_time -= GetDTime();

            if (fist->frame_time <= 0)
            {
                fist->frame_time = fist->anm->framerate;

                anim_RenderAnimFrame(fist->anm,fist->anm_rect.x,fist->anm_rect.y,
                                     fist->anm->rel_w,fist->anm->rel_h,
                                     fist->frame_cur);

                fist->frame_cur++;
                if (fist->frame_cur > fist->frame_end)
                    SetgVarInt(fist->animation_id, 2);

            }

        }

    /* if (fist->order != 0)
     {
         for(int32_t i=0;i<fist->fistnum;i++)
         {
             if (((fist->fiststatus >> i) & 1) == 1)
             {
                 for(int32_t j=0;j<fist->fists_dwn[i].num_box;j++)
                     rectangleRGBA(Rend_GetGameScreen(),
                                   fist->fists_dwn[i].boxes[j].x,
                                   fist->fists_dwn[i].boxes[j].y,
                                   fist->fists_dwn[i].boxes[j].x2,
                                   fist->fists_dwn[i].boxes[j].y2,
                                   255,0,0,255);
             }
             else
             {
                 for(int32_t j=0;j<fist->fists_up[i].num_box;j++)
                     rectangleRGBA(Rend_GetGameScreen(),
                                   fist->fists_up[i].boxes[j].x,
                                   fist->fists_up[i].boxes[j].y,
                                   fist->fists_up[i].boxes[j].x2,
                                   fist->fists_up[i].boxes[j].y2,
                                   255,0,0,255);
             }
         }
     }
     else
     {
         for(int32_t i=fist->fistnum-1;i>=0;i--)
         {
             if (((fist->fiststatus >> i) & 1) == 1)
             {
                 for(int32_t j=0;j<fist->fists_dwn[i].num_box;j++)
                     rectangleRGBA(Rend_GetGameScreen(),
                                   fist->fists_dwn[i].boxes[j].x,
                                   fist->fists_dwn[i].boxes[j].y,
                                   fist->fists_dwn[i].boxes[j].x2,
                                   fist->fists_dwn[i].boxes[j].y2,
                                   255,0,0,255);
             }
             else
             {
                 for(int32_t j=0;j<fist->fists_up[i].num_box;j++)
                     rectangleRGBA(Rend_GetGameScreen(),
                                   fist->fists_up[i].boxes[j].x,
                                   fist->fists_up[i].boxes[j].y,
                                   fist->fists_up[i].boxes[j].x2,
                                   fist->fists_up[i].boxes[j].y2,
                                   255,0,0,255);
             }
         }
     }*/

}

void control_hotmv(ctrlnode *ct)
{
    bool mousein=false;

    hotmvnode *hotm = ct->node.hotmv;

    if (hotm->cycle < hotm->num_cycles)
    {
        hotm->frame_time-=GetDTime();
        if (hotm->frame_time <= 0)
        {
            hotm->frame_time = hotm->anm->framerate;
            hotm->cur_frame++;
            if (hotm->cur_frame >= hotm->num_frames)
            {
                hotm->cycle++;
                hotm->cur_frame = 0;
                if (hotm->cycle == hotm->num_cycles)
                {
                    SetgVarInt(ct->slot,2);
#ifdef TRACE
                    printf("Max Cycles reached HotMov %d(Slot)\n",ct->slot);
#endif
                }
            }
        }
    }

    if (hotm->cycle < hotm->num_cycles)
    {
        if (!Rend_MouseInGamescr())
            return;


        int32_t mX = Rend_GetMouseGameX();
        int32_t mY = Rend_GetMouseGameY();
        int32_t curfr = hotm->cur_frame;

        if (hotm->rect.x + hotm->frame_list[curfr].x    <= mX &&\
                hotm->rect.x + hotm->frame_list[curfr].x2   >= mX &&\
                hotm->rect.y + hotm->frame_list[curfr].y    <= mY &&\
                hotm->rect.y + hotm->frame_list[curfr].y2   >= mY )
            mousein = true;

        if (mousein)
        {
            if (Mouse_IsCurrentCur(CURSOR_IDLE))
                Mouse_SetCursor(CURSOR_ACTIVE);

            if (MouseUp(SDL_BUTTON_LEFT))
            {
                ctrl_setvenus(ct);

                FlushMouseBtn(SDL_BUTTON_LEFT);

                SetgVarInt(ct->slot,1);

#ifdef TRACE
                printf("Pushed_HotMov %d(Slot)\n",ct->slot);
#endif
            }

        }
    }
}

void control_titler(ctrlnode *ct)
{
    titlernode *titler = ct->node.titler;
    if (titler->current_string != titler->next_string && titler->next_string >= 0 && titler->next_string < CTRL_TITLER_MAX_STRINGS)
    {
        titler->current_string = titler->next_string;
        SDL_FillRect(titler->surface,NULL,SDL_MapRGBA(titler->surface->format,0,0,0,255));
        if (titler->strings[titler->current_string] != NULL && titler->surface != NULL)
            txt_DrawTxtInOneLine(titler->strings[titler->current_string],titler->surface);
    }
}

void control_titler_draw(ctrlnode *ct)
{
    titlernode *titler = ct->node.titler;

    if (titler->surface)
        Rend_DrawImageUpGamescr(titler->surface, titler->rectangle.x + GAMESCREEN_FLAT_X,  titler->rectangle.y);
}

void control_hotmv_draw(ctrlnode *ct)
{
    hotmvnode *hotm = ct->node.hotmv;
    if (hotm->cur_frame == hotm->rend_frame)
        return;

    hotm->rend_frame = hotm->cur_frame;

    if (hotm->cycle < hotm->num_cycles)
        anim_RenderAnimFrame(hotm->anm, hotm->rect.x, hotm->rect.y, hotm->rect.w, hotm->rect.h , hotm->cur_frame);

}

void control_safe(ctrlnode *ct)
{
    bool mousein=false;

    if (!Rend_MouseInGamescr())
        return;
    safenode *safe = ct->node.safe;

    int32_t mX = Rend_GetMouseGameX();
    int32_t mY = Rend_GetMouseGameY();

    if ( safe->rectangle.x                     <= mX &&\
            safe->rectangle.x+safe->rectangle.w   >= mX &&\
            safe->rectangle.y                     <= mY &&\
            safe->rectangle.y+safe->rectangle.h   >= mY )
    {
        int32_t mR = (mX - safe->center_x) * (mX - safe->center_x) + (mY - safe->center_y) * (mY - safe->center_y);

        if (mR < safe->radius_outer_sq &&\
                mR > safe->radius_inner_sq)
            mousein = true;
    }

    if (mousein)
    {
        if (Mouse_IsCurrentCur(CURSOR_IDLE))
            Mouse_SetCursor(CURSOR_ACTIVE);

        if (MouseUp(SDL_BUTTON_LEFT))
        {
            FlushMouseBtn(SDL_BUTTON_LEFT);

            ctrl_setvenus(ct);

            float raddeg = 57.29578;//180/3.1415926
            float dd = atan2(mX - safe->center_x,mY - safe->center_y)*raddeg;

            int32_t dp_state = 360 / safe->num_states;

            int32_t m_state = (safe->num_states - ((((int32_t)dd + 540) % 360) / dp_state)) % safe->num_states;

            int32_t v3 = (m_state + safe->cur_state - safe->zero_pointer + safe->num_states - 1 ) % safe->num_states;

            int32_t dbl = safe->num_states * 2;

            int32_t v11 = (dbl + v3) % safe->num_states;

            int32_t v8 = (v11 + safe->num_states - safe->start_pointer) % safe->num_states;


            safe->cur_state = v11;
            safe->to_frame = v8;

            SetgVarInt(ct->slot,v11);
#ifdef TRACE
            printf("Safe:%d(Slot) = %d\n",ct->slot,GetgVarInt(ct->slot));
#endif

        }

    }



}

void control_safe_draw(ctrlnode *ct)
{
    safenode *safe = ct->node.safe;
    if (safe->cur_frame == safe->to_frame)
        return;

    safe->frame_time -= GetDTime();

    if (safe->frame_time <= 0)
    {
        if (safe->cur_frame < safe->to_frame)
        {
            safe->cur_frame++;
            anim_RenderAnimFrame(safe->anm, safe->rectangle.x, safe->rectangle.y, safe->rectangle.w, safe->rectangle.h , safe->cur_frame);
        }
        else
        {
            safe->cur_frame--;
            anim_RenderAnimFrame(safe->anm, safe->rectangle.x, safe->rectangle.y, safe->rectangle.w, safe->rectangle.h , safe->num_states*2 - safe->cur_frame);
        }

        safe->frame_time = safe->anm->framerate;
    }
}

void control_push(ctrlnode *ct)
{
    bool mousein=false;

#ifdef FULLTRACE
    printf("Push_toggle\n");
#endif

    if (!Rend_MouseInGamescr())
        return;
    pushnode *psh = ct->node.push;

    if ( psh->x          <= Rend_GetMouseGameX() &&\
            psh->x+psh->w   >= Rend_GetMouseGameX() &&\
            psh->y          <= Rend_GetMouseGameY() &&\
            psh->y+psh->h   >= Rend_GetMouseGameY() )
        mousein = true;

    if (mousein)
    {
        if (!pushChangeMouse)
            if (Mouse_IsCurrentCur(CURSOR_IDLE))
            {
                Mouse_SetCursor(psh->cursor);
                pushChangeMouse = true;
            }

        //if (MouseHit(SDL_BUTTON_LEFT))
        //    psh->downed = true;

        int8_t pushed = 0;

        switch(psh->event)
        {
        case CTRL_PUSH_EV_UP:
            if (MouseUp(SDL_BUTTON_LEFT))
                pushed = 1;
            break;
        case  CTRL_PUSH_EV_DWN:
            if (MouseHit(SDL_BUTTON_LEFT))
                pushed = 1;
            break;
        case  CTRL_PUSH_EV_DBL:
            if (MouseDblClk())
                pushed = 1;
            break;
        default:

            if (MouseUp(SDL_BUTTON_LEFT))
                pushed = 1;
            break;
        };

        if (pushed == 1)
        {
            ctrl_setvenus(ct);
#ifdef TRACE
            printf("Pushed #%d\n",ct->slot);
#endif
            int32_t val = GetgVarInt(ct->slot);
            val++;
            val %= psh->count_to;
            SetgVarInt(ct->slot,val);

            FlushMouseBtn(SDL_BUTTON_LEFT);
        }

    }

}


void control_save(ctrlnode *ct)
{
    saveloadnode *sv = ct->node.svld;
    for (int i=0; i<MAX_SAVES; i++)
        if (sv->inputslot[i] != -1)
            if (sv->input_nodes[i]->node.inp->enterkey)
            {
                if(sv->forsaving)
                {
                    if (strlen(sv->input_nodes[i]->node.inp->text) > 0)
                    {
                        char fln[32];

                        bool tosave = true;

                        sprintf(fln,CTRL_SAVE_SAVES,i+1);
                        if (FileExist(fln))
                        {
                            if (game_question_message(GetSystemString(SYSTEM_STR_SAVEEXIST)))
                                tosave = true;
                            else
                                tosave = false;
                        }


                        if (tosave)
                        {
                            FILE *f = fopen(CTRL_SAVE_FILE,"wb");

                            for (int j=0; j<MAX_SAVES; j++)
                                if (j!=i)
                                    fprintf(f,"%s\r\n",sv->Names[j]);
                                else
                                    fprintf(f,"%s\r\n",sv->input_nodes[i]->node.inp->text);

                            fclose(f);

                            ScrSys_SaveGame(fln);
                            game_delay_message(1500,GetSystemString(SYSTEM_STR_SAVED));
                            SetNeedLocate('0','0','0','0',0);
                            break;
                        }
                    }
                    else
                    {
                        game_timed_message(2000,GetSystemString(SYSTEM_STR_SAVEEMPTY));
                    }
                }
                else
                {
                    char fln[32];

                    sprintf(fln,CTRL_SAVE_SAVES,i+1);

                    ScrSys_LoadGame(fln);

                    break;
                }

                sv->input_nodes[i]->node.inp->enterkey = false;
            }
}

void control_lever(ctrlnode *ct)
{

    levernode *lev = ct->node.lev;
    if (lev->curfrm < CTRL_LEVER_MAX_FRAMES)
    {
        if (!lev->mouse_captured)
        {

            if (lev->hotspots[lev->curfrm].x                <= Rend_GetMouseGameX() &&\
                    lev->hotspots[lev->curfrm].x + lev->delta_x >= Rend_GetMouseGameX() &&\
                    lev->hotspots[lev->curfrm].y                <= Rend_GetMouseGameY() &&\
                    lev->hotspots[lev->curfrm].y + lev->delta_y >= Rend_GetMouseGameY())
            {
                Mouse_SetCursor(lev->cursor);

                if (MouseDown(SDL_BUTTON_LEFT))
                {
                    ctrl_setvenus(ct);

                    lev->mouse_captured = true;
                    lev->mouse_count = CTRL_LEVER_ANGL_TIME;
                    lev->mouse_angle = -1;
                    lev->last_mouse_x = MouseX();
                    lev->last_mouse_y = MouseY();
                    lev->autoout = false;
                }
            }

            //if (!lev->mouse_captured) /* if still not pressed*/
            if (lev->autoout)
            {
                if (lev->autoseq_frm < lev->hasout[lev->autoseq])
                {
                    lev->autoseq_time -= GetDTime();

                    if (lev->autoseq_time < 0)
                    {
                        lev->curfrm = lev->outproc[lev->autoseq][lev->autoseq_frm];
                        SetgVarInt(ct->slot, lev->curfrm);
                        lev->autoseq_frm++;
                        lev->autoseq_time = CTRL_LEVER_AUTO_DELAY;
                    }
                }
                else
                    lev->autoout = false;
            }
        }
        else
        {
            if (!MouseDown(SDL_BUTTON_LEFT))
            {
                lev->mouse_captured = false;

                if (!lev->autoout) /* not initiated */
                {
                    if (lev->hasout[lev->curfrm] > 0) /* if has animation */
                    {
                        lev->autoseq = lev->curfrm;
                        lev->autoseq_frm = 0;
                        lev->autoseq_time = CTRL_LEVER_AUTO_DELAY;
                        lev->autoout = true;
                    }
                }
            }
            else
            {
                Mouse_SetCursor(lev->cursor);

                if (lev->mouse_angle != -1)
                    for (int16_t j=0; j<CTRL_LEVER_MAX_DIRECTS; j++)
                        if (lev->hotspots[lev->curfrm].directions[j].toframe != -1)
                        {
                            int16_t angl = lev->hotspots[lev->curfrm].directions[j].angle;

                            if (angl + CTRL_LEVER_ANGL_DELTA  >  lev->mouse_angle   &&
                                    angl - CTRL_LEVER_ANGL_DELTA  <  lev->mouse_angle   )
                            {
                                lev->curfrm = lev->hotspots[lev->curfrm].directions[j].toframe;
                                SetgVarInt(ct->slot, lev->curfrm);

                                lev->mouse_angle = -1;

                                break;
                            }
                        }

            }
        }

    }

    if (lev->mouse_count <= 0)
    {
        lev->mouse_count = CTRL_LEVER_ANGL_TIME;
        lev->mouse_angle = Mouse_GetAngle(lev->last_mouse_x,
                                          lev->last_mouse_y,
                                          MouseX(),
                                          MouseY());

        lev->last_mouse_x = MouseX();
        lev->last_mouse_y = MouseY();
    }


    lev->mouse_count -=GetDTime();
}


int Parse_Control_Flat()
{
    Rend_SetRenderer (RENDER_FLAT);
    return 1;
}

int Parse_Control_Lever(MList *controlst, mfile *fl, uint32_t slot)
{
    char buf[FILE_LN_BUF];
    char *str;

    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_LEVER);
    levernode *lev = ctnode->node.lev;

    AddToMList(controlst,ctnode);

    ctnode->slot      = slot;

    char filename[MINIBUFSZ];

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            break;
        }
        else if (strCMP(str,"descfile")==0)
        {
            str   = GetParams(str);
            strcpy(filename,str);
        }
        else if (strCMP(str,"cursor")==0)
        {
            str   = GetParams(str);
            lev->cursor = Mouse_GetCursorIndex(str);
        }
        else if (strCMP(str,"venus_id") == 0)
        {
            str = GetParams(str);
            ctnode->venus = atoi(str);
        }
    }

    const char *descfile = GetFilePath(filename);

    if (!descfile)
        return 0; //FAIL

    FILE *file2 = fopen(descfile,"rb");

    while (!feof(file2))
    {
        fgets(buf,FILE_LN_BUF,file2);
        str = PrepareString(buf);

        if (strCMP(str,"animation_id")==0)
        {
            //sscanf(str,"animation_id:%d",);
        }
        else if (strCMP(str,"filename")==0)
        {
            char minbuf[MINIBUFSZ];
            sscanf(str,"filename:%s",minbuf);
            int16_t ln = strlen(minbuf);
            if (minbuf[ln-1] == '~')
                minbuf[ln-1] = 0;
            lev->anm = anim_CreateAnim();
            anim_LoadAnim(lev->anm,minbuf,0,0,0,0);

        }
        else if (strCMP(str,"skipcolor")==0)
        {

        }
        else if (strCMP(str,"anim_coords")==0)
        {
            int32_t t1,t2,t3,t4;
            sscanf(str,"anim_coords:%d %d %d %d~",&t1,&t2,&t3,&t4);
            lev->AnimCoords.x = t1;
            lev->AnimCoords.y = t2;
            lev->AnimCoords.w = t3-t1+1;
            lev->AnimCoords.h = t4-t2+1;
        }
        else if (strCMP(str,"mirrored")==0)
        {
            int32_t t1;
            sscanf(str,"mirrored:%d",&t1);
            if (t1 == 1)
                lev->mirrored = true;
            else
                lev->mirrored = false;
        }
        else if (strCMP(str,"frames")==0)
        {
            int32_t t1;
            sscanf(str,"frames:%d",&t1);
            lev->frames = t1;
        }
        else if (strCMP(str,"elsewhere")==0)
        {

        }
        else if (strCMP(str,"out_of_control")==0)
        {

        }
        else if (strCMP(str,"start_pos")==0)
        {
            int32_t t1;
            sscanf(str,"start_pos:%d",&t1);
            lev->startpos = t1;
            lev->curfrm = lev->startpos;
        }
        else if (strCMP(str,"hotspot_deltas") == 0)
        {
            int32_t t1,t2;
            sscanf(str,"hotspot_deltas:%d %d",&t1, &t2);
            lev->delta_x = t1;
            lev->delta_y = t2;
        }
        else
        {
            int32_t t1,t2,t3;
            if (sscanf(str,"%d:%d %d", &t1, &t2, &t3) == 3)
            {
                if (t1 < CTRL_LEVER_MAX_FRAMES)
                {
                    lev->hotspots[t1].x = t2;
                    lev->hotspots[t1].y = t3;
                    char *token;
                    const char *find = " ";
                    char tmpbuf[FILE_LN_BUF];
                    strcpy(tmpbuf,str);
                    token = strtok(tmpbuf,find);
                    while (token != NULL)
                    {
                        if( strCMP(token,"d") == 0 )
                        {
                            int32_t t4,t5;
                            sscanf(token,"d=%d,%d",&t4,&t5);
                            if (lev->hotspots[t1].angles < CTRL_LEVER_MAX_DIRECTS)
                            {
                                int16_t angles = lev->hotspots[t1].angles;
                                lev->hotspots[t1].directions[angles].toframe = t4;
                                lev->hotspots[t1].directions[angles].angle   = t5;
                                lev->hotspots[t1].angles++;
                            }
                        }
                        token = strtok(NULL,find);
                    }
                    lev->hasout[t1] = 0;
                    int32_t len = strlen(str);
                    for (int32_t g=0; g<len; g++)
                        if (tolower(str[g]) == 'p')
                        {
                            int32_t tr1,tr2;
                            int8_t num = sscanf(str+g+1,"(%d to %d)", &tr1, &tr2);
                            if (num == 2)
                            {
                                lev->outproc[t1][ lev->hasout[t1] ] = tr2;
                                lev->hasout[t1]++;
                            }
                        }
                }
            }
        }

    }//while (!feof(file2))

    //SetgVarInt(ctnode->slot, lev->curfrm);
    lev->curfrm = GetgVarInt(ctnode->slot);

    fclose(file2);

    /*    if (lev->anm)
        {
            if (lev->AnimCoords.w > lev->anm->rel_w)
                lev->AnimCoords.w = round((double)lev->AnimCoords.w / (double)lev->anm->rel_w) * lev->anm->rel_w;
            else
                lev->AnimCoords.w = lev->anm->rel_w / round((double)lev->anm->rel_w/(double)lev->AnimCoords.w);

            if (lev->AnimCoords.h > lev->anm->rel_h)
                lev->AnimCoords.h = round((double)lev->AnimCoords.h / (double)lev->anm->rel_h) * lev->anm->rel_h;
            else
                lev->AnimCoords.h = lev->anm->rel_h / round((double)lev->anm->rel_h/(double)lev->AnimCoords.h);

            if (lev->AnimCoords.w == 0)
                lev->AnimCoords.w = lev->anm->rel_w;
            if (lev->AnimCoords.h == 0)
                lev->AnimCoords.h = lev->anm->rel_h;
        }
    */
    return 1;
}


int Parse_Control_HotMov(MList *controlst, mfile *fl, uint32_t slot)
{
    char buf[FILE_LN_BUF];
    char *str;

    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_HOTMV);
    hotmvnode *hotm = ctnode->node.hotmv;

    AddToMList(controlst,ctnode);

    ctnode->slot      = slot;
    SetDirectgVarInt(slot,0);

    char filename[MINIBUFSZ];

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            break;
        }
        else if (strCMP(str,"hs_frame_list")==0)
        {
            str   = GetParams(str);
            strcpy(filename,str);
        }
        else if (strCMP(str,"num_frames")==0)
        {
            str   = GetParams(str);
            hotm->num_frames = atoi(str)+1;
            hotm->frame_list = (Rect *)calloc(hotm->num_frames,sizeof(Rect));
        }
        else if (strCMP(str,"num_cycles")==0)
        {
            str   = GetParams(str);
            hotm->num_cycles = atoi(str);
        }
        else if (strCMP(str,"animation")==0)
        {
            str   = GetParams(str);
            char file[MINIBUFSZ];
            sscanf(str,"%s",file);
            hotm->anm = anim_CreateAnim();
            anim_LoadAnim(hotm->anm,file,0,0,0,0);
        }
        else if (strCMP(str,"rectangle")==0)
        {
            str   = GetParams(str);
            int32_t t1,t2,t3,t4;
            sscanf(str,"%d %d %d %d",&t1,&t2,&t3,&t4);
            hotm->rect.x = t1;
            hotm->rect.y = t2;
            hotm->rect.w = t3-t1+1;
            hotm->rect.h = t4-t2+1;
        }
        else if (strCMP(str,"venus_id") == 0)
        {
            str = GetParams(str);
            ctnode->venus = atoi(str);
        }
    }

    const char *descfile = GetFilePath(filename);

    if (!descfile)
        return 0; //FAIL

    FILE *file2 = fopen(descfile,"rb");

    while (!feof(file2))
    {
        fgets(buf,FILE_LN_BUF,file2);
        int32_t t1,t2,t3,t4,tt;
        sscanf(buf,"%d:%d %d %d %d~",&tt,&t1,&t2,&t3,&t4);
        if (tt>=0 && tt<hotm->num_frames)
        {
            hotm->frame_list[tt].x = t1;
            hotm->frame_list[tt].y = t2;
            hotm->frame_list[tt].x2 = t3;
            hotm->frame_list[tt].y2 = t4;
        }
    }


    fclose(file2);

    return 1;
}

int Parse_Control_Panorama(mfile *fl)
{
    char  buf[FILE_LN_BUF];
    char *str;
    int  good = 0;

    double angle = 27.0;
    double     k = 0.55;
    int      tmp = 0;

    Rend_SetRenderer (RENDER_PANA);

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"angle")==0)
        {
            str   = GetParams(str);
            angle = atof(str);
            Rend_pana_SetAngle(angle);
        }
        else if (strCMP(str,"linscale")==0)
        {
            str   = GetParams(str);
            k = atof(str);
            Rend_pana_SetLinscale(k);
        }
        else if (strCMP(str,"reversepana")==0)
        {
            str   = GetParams(str);
            tmp = atoi(str);
            if (tmp == 1)
                Rend_SetReversePana(true);
        }
        else if (strCMP(str,"zeropoint")==0)
        {
            str   = GetParams(str);
            tmp = atoi(str);
            Rend_pana_SetZeroPoint(tmp);
        }

    }

    //  printf("%f\n",angle);
    //  printf("%f\n",k);

    Rend_pana_SetTable();
    return good;
}

int Parse_Control_Tilt(mfile *fl)
{
    char  buf[FILE_LN_BUF];
    char *str;
    int  good = 0;

    double angle = 27.0;
    double     k = 0.55;
    int      tmp = 0;

    Rend_SetRenderer (RENDER_TILT);

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"angle")==0)
        {
            str   = GetParams(str);
            angle = atof(str);
            Rend_tilt_SetAngle(angle);
        }
        else if (strCMP(str,"linscale")==0)
        {
            str   = GetParams(str);
            k = atof(str);
            Rend_tilt_SetLinscale(k);
        }
        else if (strCMP(str,"reversepana")==0)
        {
            str   = GetParams(str);
            tmp = atoi(str);
            if (tmp == 1)
                Rend_SetReversePana(true);
        }
    }

    //  printf("%f\n",angle);
    //  printf("%f\n",k);

    Rend_tilt_SetTable();
    return good;
}

int Parse_Control_Save(MList *controlst, mfile *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_SAVE);
    saveloadnode *sv = ctnode->node.svld;

    AddToMList(controlst,ctnode);

    ctnode->slot      = slot;
    SetDirectgVarInt(slot,0);

    FILE *f = fopen(CTRL_SAVE_FILE,"rb");
    if (f == NULL)
    {
        f = fopen(CTRL_SAVE_FILE,"wb");
        for(int i=0; i< MAX_SAVES; i++)
            fprintf(f,"\r\n");
        fseek(f,0,SEEK_SET);
    }

    for (int i=0; i<MAX_SAVES; i++)
    {
        fgets(buf,FILE_LN_BUF,f);
        memset(sv->Names[i],0,SAVE_NAME_MAX_LEN+1);
        str = TrimRight(buf);
        if (strlen(str)>0)
        {
            char fln[32];
            sprintf(fln,CTRL_SAVE_SAVES,i+1);
            FILE *d = fopen(fln,"rb");
            if (d!= NULL)
            {
                strcpy(sv->Names[i],str);
                fclose(d);
            }
        }
    }

    fclose(f);



    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"savebox")==0)
        {
            str=GetParams(str);
            int ctrlslot, saveslot;
            sscanf(str,"%d %d",&saveslot,&ctrlslot);

            saveslot--;

            ctrlnode *nd = GetControlByID(ctrlslot);
            if (nd != NULL)
                if (nd->type == CTRL_INPUT)
                {
                    strcpy(nd->node.inp->text,sv->Names[saveslot]);
                    nd->node.inp->textchanged = true;
                    sv->inputslot[saveslot] = ctrlslot;
                    sv->input_nodes[saveslot] = nd;
                }


        }
        else if (strCMP(str,"control_type")==0)
        {
            str=GetParams(str);
            if (strCMP(str,"save") == 0)
                sv->forsaving = true;
            else if (strCMP(str,"restore") == 0)
                sv->forsaving = false;
            else
                printf("Unknown control_type: %s\n",str);

        }
        else
        {
            printf("Unknown parameter for save control: %s\n",str);
        }

    }//while (!feof(fl))

    for (int i=0; i< MAX_SAVES; i++)
        if (sv->inputslot[i] != -1)
        {
            ctrlnode *nd = sv->input_nodes[i];
            if (nd != NULL)
                if (nd->type == CTRL_INPUT)
                    nd->node.inp->readonly = !sv->forsaving;
        }
    return good;
}

int Parse_Control_Titler(MList *controlst, mfile *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_TITLER);
    titlernode *titler = ctnode->node.titler;

    AddToMList(controlst,ctnode);
    ctnode->slot      = slot;

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"rectangle")==0)
        {
            str=GetParams(str);
            sscanf(str,"%d %d %d %d",\
                   &titler->rectangle.x,\
                   &titler->rectangle.y,\
                   &titler->rectangle.w,\
                   &titler->rectangle.h);

            titler->surface = CreateSurface(titler->rectangle.w-titler->rectangle.x+1,titler->rectangle.h-titler->rectangle.y+1);
            SetColorKey(titler->surface,0,0,0);
        }
        else if (strCMP(str,"string_resource_file")==0)
        {
            str=GetParams(str);
            const char *tmp = GetFilePath(str);
            if (tmp != NULL)
            {
                titler->num_strings = 0;
                mfile *fl2 = mfopen (tmp);

                m_wide_to_utf8(fl2);

                char bf[FILE_LN_BUF];

                while (!mfeof(fl2) && titler->num_strings<CTRL_TITLER_MAX_STRINGS)
                {
                    mfgets(bf,FILE_LN_BUF,fl2);
                    char *str2 = PrepareString(bf);
                    int32_t bflen = strlen(str2);
                    if (bf > 0)
                    {
                        titler->strings[titler->num_strings] = (char *)calloc(bflen+1,1);
                        strcpy(titler->strings[titler->num_strings],str2);
                    }

                    titler->num_strings++;
                }

                mfclose(fl2);

            }
        }
    }//while (!feof(fl))
    return good;
}

int Parse_Control_Input(MList *controlst, mfile *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_INPUT);
    inputnode *inp = ctnode->node.inp;

    AddToMList(controlst,ctnode);

    ctnode->slot      = slot;
    SetDirectgVarInt(slot,0);

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"rectangle")==0)
        {
            str=GetParams(str);
            sscanf(str,"%d %d %d %d",\
                   &inp->rectangle.x,\
                   &inp->rectangle.y,\
                   &inp->rectangle.w,\
                   &inp->rectangle.h);

            inp->rect = CreateSurface(inp->rectangle.w-inp->rectangle.x,inp->rectangle.h-inp->rectangle.y);
            SetColorKey(inp->rect,0,0,0);
        }
        else if (strCMP(str,"aux_hotspot")==0)
        {
            str=GetParams(str);
            sscanf(str,"%d %d %d %d",\
                   &inp->hotspot.x,\
                   &inp->hotspot.y,\
                   &inp->hotspot.w,\
                   &inp->hotspot.h);
        }
        else if (strCMP(str,"cursor_animation_frames")==0)
        {
            // str=GetParams(str);
            //  inp->frame = atoi(str);
        }//if (str[0] == '}')
        else if (strCMP(str,"next_tabstop")==0)
        {
            str = GetParams(str);
            inp->next_tab = atoi(str);
        }
        else if (strCMP(str,"cursor_animation")==0)
        {
            str=GetParams(str);
            char file[16];

            sscanf(str,"%s",file);
            if (strcasestr(file,"rlf")!=NULL)
                inp->cursor = loader_LoadRlf(file,0,0);
            else
                inp->cursor = LoadAnimImage(file,0);
        }
        else if (strCMP(str,"focus")==0)
        {
            str=GetParams(str);
            if (atoi(str) == 1)
                FocusInput = ctnode->slot;
        }
        else if (strCMP(str,"string_init")==0)
        {
            str=GetParams(str);
            char *tmp = GetSystemString(atoi(str));
            if (tmp != NULL)
                txt_readfontstyle(&inp->string_init,tmp);
        }
        else if (strCMP(str,"chooser_init_string")==0)
        {
            str=GetParams(str);
            char *tmp = GetSystemString(atoi(str));
            if (tmp != NULL)
                txt_readfontstyle(&inp->string_chooser_init,tmp);
        }
        else if (strCMP(str,"venus_id") == 0)
        {
            str = GetParams(str);
            ctnode->venus = atoi(str);
        }

    }//while (!feof(fl))
    return good;
}


int Parse_Control_Paint(MList *controlst, mfile *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;


    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_PAINT);
    paintnode *paint = ctnode->node.paint;

    AddToMList(controlst,ctnode);
    ctnode->slot      = slot;

    char filename[MINIBUFSZ];

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"rectangle")==0)
        {
            str=GetParams(str);
            sscanf(str,"%d %d %d %d",\
                   &paint->rectangle.x,\
                   &paint->rectangle.y,\
                   &paint->rectangle.w,\
                   &paint->rectangle.h);
            //paint->rectangle.w -= (paint->rectangle.x-1);
            // paint->rectangle.h -= (paint->rectangle.y-1);
        }
        else if (strCMP(str,"brush_file")==0)
        {
            str=GetParams(str);
            SDL_Surface *tmp = loader_LoadFile(str,0);
            if (tmp)
            {
                paint->brush = (uint8_t *)malloc(tmp->w*tmp->h * sizeof(uint8_t));
                paint->b_w = tmp->w;
                paint->b_h = tmp->h;
                SDL_LockSurface(tmp);
                if (GAME_BPP == 32)
                {
                    uint32_t *a = (uint32_t *)tmp->pixels;

                    for(int32_t j=0; j<paint->b_h; j++)
                        for(int32_t i=0; i<paint->b_w; i++)
                            if (a[i+j*tmp->w] < 0x7F7F7F)
                                paint->brush[i+j*tmp->w] = 0;
                            else
                                paint->brush[i+j*tmp->w] = 1;
                }
                else
                {
                    printf("Write your code for %d bit mode in %s at %d line.\n",GAME_BPP,__FILE__,__LINE__);
                }
                SDL_UnlockSurface(tmp);
                SDL_FreeSurface(tmp);
            }
        }
        else if (strCMP(str,"cursor")==0)
        {
            str=GetParams(str);
            paint->cursor = Mouse_GetCursorIndex(str);
        }
        else if (strCMP(str,"paint_file")==0)
        {
            str=GetParams(str);
            strcpy(filename,str);
        }
        else if (strCMP(str,"venus_id") == 0)
        {
            str = GetParams(str);
            ctnode->venus = atoi(str);
        }
        else if (strCMP(str,"eligible_objects")==0)
        {
            str=GetParams(str);
            int tmpobj=0;
            int strl=strlen(str);
            for (int i=0; i < strl; i++)
                if (str[i] == ' ')
                    tmpobj++;

            tmpobj++;

            paint->eligable_cnt = tmpobj;
            paint->eligible_objects = (int32_t *)malloc (tmpobj * sizeof(int32_t));
            int i=0;
            tmpobj=0;

            for (;;)
            {
                if (i>=strl)
                    break;
                if (str[i] != ' ')
                {
                    paint->eligible_objects[tmpobj] = atoi(str + i);
                    tmpobj++;

                    while (i<strl && str[i] != ' ')
                        i++;
                }
                i++;
            }//for (;;)
        }//if (str[0] == '}')
    }//while (!feof(fl))


    SDL_Surface *tmp = loader_LoadFile(filename,0);

    if (tmp)
    {
        paint->paint = CreateSurface(paint->rectangle.w,paint->rectangle.h);

        SDL_Rect tr;
        tr.x = paint->rectangle.x;
        tr.y = paint->rectangle.y;
        tr.w = paint->rectangle.w;
        tr.h = paint->rectangle.h;

        SDL_BlitSurface(tmp,&tr,paint->paint,NULL);
        SDL_FreeSurface(tmp);
    }
    else
        return 0;

    return good;
}

int Parse_Control_Slot(MList *controlst, mfile *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;


    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_SLOT);
    slotnode *slut = ctnode->node.slot;

    AddToMList(controlst,ctnode);
    ctnode->slot      = slot;
    slut->srf         = NULL;

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"rectangle")==0)
        {
            str=GetParams(str);
            sscanf(str,"%d %d %d %d",\
                   &slut->rectangle.x,\
                   &slut->rectangle.y,\
                   &slut->rectangle.w,\
                   &slut->rectangle.h);
        }
        else if (strCMP(str,"hotspot")==0)
        {
            str=GetParams(str);
            sscanf(str,"%d %d %d %d",\
                   &slut->hotspot.x,\
                   &slut->hotspot.y,\
                   &slut->hotspot.w,\
                   &slut->hotspot.h);
        }
        else if (strCMP(str,"cursor")==0)
        {
            str=GetParams(str);
            slut->cursor = Mouse_GetCursorIndex(str);
        }
        else if (strCMP(str,"distance_id")==0)
        {
            str=GetParams(str);
            strcpy(slut->distance_id,str);
        }
        else if (strCMP(str,"venus_id") == 0)
        {
            str = GetParams(str);
            ctnode->venus = atoi(str);
        }
        else if (strCMP(str,"eligible_objects")==0)
        {
            str=GetParams(str);
            int tmpobj=0;
            int strl=strlen(str);
            for (int i=0; i < strl; i++)
                if (str[i] == ' ')
                    tmpobj++;

            tmpobj++;

            slut->eligable_cnt = tmpobj;
            slut->eligible_objects = (int32_t *)malloc (tmpobj * sizeof(int32_t));
            int i=0;
            tmpobj=0;

            for (;;)
            {
                if (i>=strl)
                    break;
                if (str[i] != ' ')
                {
                    slut->eligible_objects[tmpobj] = atoi(str + i);
                    tmpobj++;

                    while (i<strl && str[i] != ' ')
                        i++;
                }
                i++;
            }//for (;;)
        }//if (str[0] == '}')
    }//while (!feof(fl))
    return good;
}

int Parse_Control_PushTgl(MList *controlst, mfile *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;

//    SetgVarInt(slot,0);



    ctrlnode *ctnode  = Ctrl_CreateNode(CTRL_PUSH);
    pushnode *psh     = ctnode->node.push;
    ctnode->slot      = slot;

    psh->cursor       = CURSOR_IDLE;

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"flat_hotspot") == 0)
        {
            psh->flat=true;
            str = GetParams(str);
            sscanf(str,"%d %d %d %d",&psh->x,&psh->y,&psh->w,&psh->h);
#ifdef FULLTRACE
            printf("    Flat %d %d %d %d %d\n",psh->x,psh->y,psh->w,psh->h,psh->flat);
#endif
        }
        else if (strCMP(str,"warp_hotspot") == 0)
        {
            psh->flat=true;
            str = GetParams(str);
            sscanf(str,"%d %d %d %d",&psh->x,&psh->y,&psh->w,&psh->h);
#ifdef FULLTRACE
            printf("    Warp %d %d %d %d %d\n",psh->x,psh->y,psh->w,psh->h,psh->flat);
#endif
        }
        else if (strCMP(str,"cursor") == 0)
        {
            str = GetParams(str); //cursor
            psh->cursor = Mouse_GetCursorIndex(str);
        }
        else if (strCMP(str,"animation") == 0)
        {

        }
        else if (strCMP(str,"mouse_event") == 0)
        {
            str = GetParams(str);
            if (strCMP(str,"up") == 0 )
                psh->event = CTRL_PUSH_EV_UP;
            else if (strCMP(str,"down") == 0 )
                psh->event = CTRL_PUSH_EV_DWN;
            else if (strCMP(str,"double") == 0 )
                psh->event = CTRL_PUSH_EV_DBL;
        }
        else if (strCMP(str,"sound") == 0)
        {

        }
        else if (strCMP(str,"count_to") == 0)
        {
            str = GetParams(str);
            psh->count_to = atoi(str)+1;
        }
        else if (strCMP(str,"venus_id") == 0)
        {
            str = GetParams(str);
            ctnode->venus = atoi(str);
        }
    }

    if (good == 1)
        AddToMList(controlst,ctnode);

    return good;
}

int Parse_Control_Fist(MList *controlst, mfile *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;


    ctrlnode *ctnode  = Ctrl_CreateNode(CTRL_FIST);
    fistnode *fist    = ctnode->node.fist;
    ctnode->slot      = slot;

    char filename[MINIBUFSZ];

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"sound_key") == 0)
        {
            str = GetParams(str);
            fist->soundkey = atoi(str);
        }
        else if (strCMP(str,"cursor") == 0)
        {
            str = GetParams(str);
            fist->cursor = Mouse_GetCursorIndex(str);
        }
        else if (strCMP(str,"descfile") == 0)
        {
            str = GetParams(str);
            strcpy(filename,str);
        }
        else if (strCMP(str,"animation_id") == 0)
        {
            str = GetParams(str);
            fist->animation_id = atoi(str);
        }
        else if (strCMP(str,"venus_id") == 0)
        {
            str = GetParams(str);
            ctnode->venus = atoi(str);
        }
    }

    const char *pth = GetFilePath(filename);

    if (pth != NULL)
    {
        FILE *fil = fopen(pth,"rb");

        while(!feof(fil))
        {
            fgets(buf,FILE_LN_BUF,fil);
            str = PrepareString(buf);
            int32_t ln = strlen(str);
            if (str[ln-1] == '~')
                str[ln-1] = 0;

            if (strCMP(str,"animation_id")==0)
            {
                //sscanf(str,"animation_id:%d",);
            }
            else if (strCMP(str,"animation")==0)
            {
                char minbuf[MINIBUFSZ];
                sscanf(str,"animation:%s",minbuf);
                fist->anm = anim_CreateAnim();
                anim_LoadAnim(fist->anm,minbuf,0,0,0,0);
            }
            else if (strCMP(str,"anim_rect")==0)
            {
                int32_t t1,t2,t3,t4;
                sscanf(str,"anim_rect:%d %d %d %d",&t1,&t2,&t3,&t4);
                fist->anm_rect.x = t1;
                fist->anm_rect.y = t2;
                fist->anm_rect.w = t3-t1+1;
                fist->anm_rect.h = t4-t2+1;
            }
            else if (strCMP(str,"num_fingers")==0)
            {
                int32_t t1;
                sscanf(str,"num_fingers:%d",&t1);
                if (t1 >= 0 && t1 <= CTRL_FIST_MAX_FISTS)
                    fist->fistnum = t1;
            }
            else if (strCMP(str,"entries")==0)
            {
                int32_t t1;
                sscanf(str,"entries:%d",&t1);
                if (t1 >= 0 && t1 <= CTRL_FIST_MAX_ENTRS)
                    fist->num_entries = t1;
            }
            else if (strCMP(str,"eval_order_ascending")==0)
            {
                int32_t t1;
                sscanf(str,"eval_order_ascending:%d",&t1);
                fist->order = t1;
            }
            else if (strCMP(str,"up_hs_num_")==0)
            {
                int32_t t1,t2;
                sscanf(str,"up_hs_num_%d:%d",&t1,&t2);
                if (t1 >= 0 && t1 < fist->fistnum)
                    if (t2 >= 0 && t2 <= CTRL_FIST_MAX_BOXES)
                        fist->fists_up[t1].num_box = t2;
            }
            else if (strCMP(str,"up_hs_")==0)
            {
                int32_t t1,t2,t3,t4,t5,t6;
                sscanf(str,"up_hs_%d_%d:%d %d %d %d",&t1,&t2,&t3,&t4,&t5,&t6);
                if (t1 >= 0 && t1 < fist->fistnum)
                    if (t2 >= 0 && t2 < fist->fists_up[t1].num_box)
                    {
                        fist->fists_up[t1].boxes[t2].x  = t3;
                        fist->fists_up[t1].boxes[t2].y  = t4;
                        fist->fists_up[t1].boxes[t2].x2 = t5;
                        fist->fists_up[t1].boxes[t2].y2 = t6;
                    }

            }
            else if (strCMP(str,"down_hs_num_")==0)
            {
                int32_t t1,t2;
                sscanf(str,"down_hs_num_%d:%d",&t1,&t2);
                if (t1 >= 0 && t1 < fist->fistnum)
                    if (t2 >= 0 && t2 <= CTRL_FIST_MAX_BOXES)
                        fist->fists_dwn[t1].num_box = t2;
            }
            else if (strCMP(str,"down_hs_")==0)
            {
                int32_t t1,t2,t3,t4,t5,t6;
                sscanf(str,"down_hs_%d_%d:%d %d %d %d",&t1,&t2,&t3,&t4,&t5,&t6);
                if (t1 >= 0 && t1 < fist->fistnum)
                    if (t2 >= 0 && t2 < fist->fists_dwn[t1].num_box)
                    {
                        fist->fists_dwn[t1].boxes[t2].x  = t3;
                        fist->fists_dwn[t1].boxes[t2].y  = t4;
                        fist->fists_dwn[t1].boxes[t2].x2 = t5;
                        fist->fists_dwn[t1].boxes[t2].y2 = t6;
                    }
            }
            else
            {
                int32_t t1,t2,t3,t4;
                char s1[MINIBUFSZ];
                char s2[MINIBUFSZ];
                if (sscanf(str,"%d:%s %s %d %d (%d)",&t1,s1,s2,&t2,&t3,&t4)== 6)
                {
                    if (t1 >= 0 && t1 < fist->num_entries)
                    {
                        int32_t n1=0;
                        int32_t len = strlen(s1);
                        for(int32_t i=0; i<len; i++)
                            if (s1[i] != '0')
                                n1 |= (1 << i);

                        int32_t n2=0;
                        int32_t tmp_len = strlen(s2);
                        for(int32_t i=0; i<tmp_len; i++)
                            if (s2[i] != '0')
                                n2 |= (1 << i);

                        fist->entries[t1].strt = n1;
                        fist->entries[t1].send = n2;
                        fist->entries[t1].anm_str = t2;
                        fist->entries[t1].anm_end = t3;
                        fist->entries[t1].sound = t4;
                    }
                }

            }
        }
        fclose(fil);
    }
    else
        good = 0;

    if (good == 1)
        AddToMList(controlst,ctnode);

    return good;
}

int Parse_Control_Safe(MList *controlst, mfile *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;


    ctrlnode *ctnode  = Ctrl_CreateNode(CTRL_SAFE);
    safenode *safe     = ctnode->node.safe;
    ctnode->slot      = slot;

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str = PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"animation") == 0)
        {
            str = GetParams(str);

            safe->anm = anim_CreateAnim();
            anim_LoadAnim(safe->anm,str,0,0,0,0);
        }
        else if (strCMP(str,"rectangle") == 0)
        {
            str = GetParams(str);
            sscanf(str,"%d %d %d %d",&safe->rectangle.x,&safe->rectangle.y,&safe->rectangle.w,&safe->rectangle.h);
            safe->rectangle.w -= (safe->rectangle.x-1);
            safe->rectangle.h -= (safe->rectangle.y-1);
        }
        else if (strCMP(str,"center") == 0)
        {
            str = GetParams(str);
            sscanf(str,"%d %d",&safe->center_x,&safe->center_y);
        }
        else if (strCMP(str,"num_states") == 0)
        {
            str = GetParams(str);
            safe->num_states = atoi(str);
        }
        else if (strCMP(str,"dial_inner_radius") == 0)
        {
            str = GetParams(str);
            safe->radius_inner = atoi(str);
            safe->radius_inner_sq = safe->radius_inner * safe->radius_inner;
        }
        else if (strCMP(str,"radius") == 0)
        {
            str = GetParams(str);
            safe->radius_outer = atoi(str);
            safe->radius_outer_sq = safe->radius_outer * safe->radius_outer;
        }
        else if (strCMP(str,"zero_radians_offset") == 0)
        {
            str = GetParams(str);
            safe->zero_pointer = atoi(str);
        }
        else if (strCMP(str,"pointer_offset") == 0)
        {
            str = GetParams(str);
            safe->start_pointer = atoi(str);
            safe->cur_state = safe->start_pointer;
        }
        else if (strCMP(str,"cursor") == 0)
        {
            //not needed
        }
        else if (strCMP(str,"mirrored") == 0)
        {
            //not needed
        }
        else if (strCMP(str,"venus_id") == 0)
        {
            str = GetParams(str);
            ctnode->venus = atoi(str);
        }
    }

    if (good == 1)
        AddToMList(controlst,ctnode);

    return good;
}

int Parse_Control(MList *controlst,mfile *fl,char *ctstr)
{
    int  good = 0;

    uint32_t    slot;
    char      ctrltp[100];
    memset(ctrltp,0,100);

    sscanf(ctstr,"control:%d %s",&slot,ctrltp); //read slot number;


#ifdef FULLTRACE
    printf("Creating control:%d %s Creating object\n",slot,ctrltp);
#endif


    if (strCMP(ctrltp,"flat")==0)
    {
#ifdef FULLTRACE
        printf("    Flat Renderer\n");
#endif

        Parse_Control_Flat();
    }
    else if (strCMP(ctrltp,"pana")==0)
    {
#ifdef FULLTRACE
        printf("    Panorama Renderer\n");
#endif

        Parse_Control_Panorama(fl);
    }
    else if (strCMP(ctrltp,"tilt")==0)
    {
#ifdef FULLTRACE
        printf("    Tilt Renderer\n");
#endif

        Parse_Control_Tilt(fl);
    }
    else if (strCMP(ctrltp,"push_toggle")==0)
    {
        Parse_Control_PushTgl(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"input")==0)
    {
        Parse_Control_Input(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"save")==0)
    {
        Parse_Control_Save(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"slot")==0)
    {
        Parse_Control_Slot(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"lever")==0)
    {
        Parse_Control_Lever(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"safe")==0)
    {
        Parse_Control_Safe(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"fist")==0)
    {
        Parse_Control_Fist(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"hotmovie")==0)
    {
        Parse_Control_HotMov(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"paint")==0)
    {
        Parse_Control_Paint(controlst,fl,slot);
    }
    else if (strCMP(ctrltp,"titler")==0)
    {
        Parse_Control_Titler(controlst,fl,slot);
    }

    return good;
}



void ProcessControls(MList *ctrlst)
{

    pushChangeMouse = false;

    LastMList(ctrlst);

    while (!eofMList(ctrlst))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(ctrlst);

#ifdef FULLTRACE
        printf("Control, slot:%d \n",nod->slot);
#endif

        if (!(ScrSys_GetFlag(nod->slot) & FLAG_DISABLED))  //(nod->enable)
            if (nod->func != NULL)
                nod->func(nod);

        PrevMList(ctrlst);
    }
}


void ctrl_Delete_PushNode(ctrlnode *nod)
{
    delete nod->node.push;
}

void ctrl_Delete_SlotNode(ctrlnode *nod)
{
    if (nod->node.slot->srf)
        SDL_FreeSurface(nod->node.slot->srf);
    if (nod->node.slot->eligible_objects)
        free(nod->node.slot->eligible_objects);
    delete nod->node.slot;
}

void ctrl_Delete_InputNode(ctrlnode *nod)
{
    if (nod->node.inp->cursor)
        FreeAnimImage(nod->node.inp->cursor);
    if (nod->node.inp->rect)
        SDL_FreeSurface(nod->node.inp->rect);
    delete nod->node.inp;
}

void ctrl_Delete_SaveNode(ctrlnode *nod)
{
    delete nod->node.svld;
}

void ctrl_Delete_LeverNode(ctrlnode *nod)
{
    if (nod->node.lev->anm != NULL)
        anim_DeleteAnim(nod->node.lev->anm);
    delete nod->node.lev;
}

void ctrl_Delete_SafeNode(ctrlnode *nod)
{
    if (nod->node.safe->anm != NULL)
        anim_DeleteAnim(nod->node.safe->anm);
    delete nod->node.safe;
}

void ctrl_Delete_FistNode(ctrlnode *nod)
{
    if (nod->node.fist->anm != NULL)
        anim_DeleteAnim(nod->node.fist->anm);
    delete nod->node.fist;
}

void ctrl_Delete_HotmovNode(ctrlnode *nod)
{
    if (nod->node.hotmv->anm != NULL)
        anim_DeleteAnim(nod->node.hotmv->anm);
    if (nod->node.hotmv->frame_list != NULL)
        free(nod->node.hotmv->frame_list);
    delete nod->node.hotmv;
}

void ctrl_Delete_PaintNode(ctrlnode *nod)
{
    if (nod->node.paint->brush != NULL)
        free(nod->node.paint->brush);
    if (nod->node.paint->eligible_objects != NULL)
        free(nod->node.paint->eligible_objects);
    if (nod->node.paint->paint != NULL)
        SDL_FreeSurface(nod->node.paint->paint);
    delete nod->node.paint;
}

void ctrl_Delete_TitlerNode(ctrlnode *nod)
{
    if (nod->node.titler->surface != NULL)
        SDL_FreeSurface(nod->node.titler->surface);
    for (int32_t i=0; i<CTRL_TITLER_MAX_STRINGS; i++)
        if (nod->node.titler->strings[i] != NULL)
            free(nod->node.titler->strings[i]);
    delete nod->node.titler;
}

void DeleteSelControl(ctrlnode *nod)
{
    switch (nod->type)
    {
    case CTRL_PUSH:
        ctrl_Delete_PushNode(nod);
        break;
    case CTRL_SLOT:
        ctrl_Delete_SlotNode(nod);
        break;
    case CTRL_INPUT:
        ctrl_Delete_InputNode(nod);
        break;
    case CTRL_SAVE:
        ctrl_Delete_SaveNode(nod);
        break;
    case CTRL_LEVER:
        ctrl_Delete_LeverNode(nod);
        break;
    case CTRL_SAFE:
        ctrl_Delete_SafeNode(nod);
        break;
    case CTRL_HOTMV:
        ctrl_Delete_HotmovNode(nod);
        break;
    case CTRL_FIST:
        ctrl_Delete_FistNode(nod);
        break;
    case CTRL_PAINT:
        ctrl_Delete_PaintNode(nod);
        break;
    case CTRL_TITLER:
        ctrl_Delete_TitlerNode(nod);
        break;
    }

    delete nod;
}

void DeleteControlList(MList *lst)
{

    StartMList(lst);
    while (!eofMList(lst))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(lst);

        DeleteSelControl(nod);

        NextMList(lst);
    }

    DeleteMList(lst);
}

void FlushControlList(MList *lst)
{
    StartMList(lst);
    while (!eofMList(lst))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(lst);

        DeleteSelControl(nod);

        NextMList(lst);
    }

    FlushMList(lst);
}

ctrlnode *GetControlByID(int32_t id)
{
    MList *lst = Getctrl();
    pushMList(lst);
    StartMList(lst);
    while (!eofMList(lst))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(lst);

        if (nod->slot == id)
        {
            popMList(lst);
            return nod;
        }

        NextMList(lst);
    }

    popMList(lst);
    return NULL;
}

void ctrl_setvenus(ctrlnode *nod)
{
    if (nod->venus >= 0)
    {
        if (GetgVarInt(nod->venus) > 0)
            SetgVarInt(SLOT_VENUS,nod->venus);
    }
}


