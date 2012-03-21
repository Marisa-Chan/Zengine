#include "System.h"
#include "menu.h"


#define menu_ITEM  0
#define menu_MAGIC 1
#define menu_MAIN  2



#define menu_MAIN_IMAGE_SAVE   0
#define menu_MAIN_IMAGE_REST   1
#define menu_MAIN_IMAGE_PREF   2
#define menu_MAIN_IMAGE_EXIT   3

#define menu_MAIN_EL_W     135
#define menu_MAIN_EL_H     32

#define menu_MAGIC_SPACE       28
#define menu_MAGIC_ITEM_W      47

#define menu_ITEM_SPACE        28

#define menu_SCROLL_TIME_INV   1.0
#define menu_SCROLL_TIME       0.5

#define menu_zgi_inv_hot_w     28
#define menu_zgi_inv_h         32
#define menu_zgi_inv_w         600

#define menu_znemesis_butanim  200
#define menu_znemesis_but_frames  6
#define menu_znemesis_but_max_frm 4
#define menu_znemesis_but_clk_frm 5

uint16_t menu_bar_flag = 0xFFFF;

bool   menu_Scrolled [3] = {false,false,false};
float menu_ScrollPos[3] = {0,0,0};

SDL_Surface *menubar[4][2];
SDL_Surface *menuback[3][2];
SDL_Surface *menupicto[256][2];

int8_t menu_mousefocus = -1;

bool inmenu=false;

int16_t mouse_on_item=-1;


SDL_Surface *menubar_znem;
SDL_Surface *menubut_znem[4][menu_znemesis_but_frames];
float        scrollpos_znem;
bool         scrolled_znem;

int16_t      butframe_znem[4];
int16_t      lastbut_znem = -1;

int32_t      znem_but_anim = menu_znemesis_butanim;


void menu_SetMenuBarVal(uint16_t val)
{
    menu_bar_flag = val;
}

uint16_t menu_GetMenuBarVal()
{
    return menu_bar_flag;
}

void menu_LoadGraphics_zgi()
{
    char buf[255];
    for (int i=1; i<4; i++)
    {
        sprintf(buf,"gmzau%2.2x1.tga",i);
        menuback[i-1][0] = loader_LoadFile(buf,0);
        sprintf(buf,"gmzau%2.2x1.tga",i+0x10);
        menuback[i-1][1] = loader_LoadFile(buf,0);
    }
    for (int i=0; i<4; i++)
    {
        sprintf(buf,"gmzmu%2.2x1.tga",i);
        menubar[i][0] = loader_LoadFile(buf,0);
        sprintf(buf,"gmznu%2.2x1.tga",i);
        menubar[i][1] = loader_LoadFile(buf,0);
    }

    for (int i=0; i<256; i++)
    {
        menupicto[i][0] = NULL;
        menupicto[i][1] = NULL;
    }

}

void menu_LoadGraphics_znemesis()
{
    char buf[255];

    for (int j=1; j<=4; j++)
        for (int i=0; i<menu_znemesis_but_frames; i++)
        {
            sprintf(buf,"butfrm%d%d.tga",j,i);
            menubut_znem[j-1][i] = loader_LoadFile(buf,0);
        }

    menubar_znem = loader_LoadFile("bar.tga",0);
}

void menu_UpdateMenuBar_zgi()
{
    mouse_on_item = -1;

    if (MouseY() <= menu_HOT_Y)
    {
        inmenu = true;

        switch (menu_mousefocus)
        {
        case menu_ITEM:
            if (menu_bar_flag & menu_BAR_ITEM)
            {
                SetgVarInt(SLOT_MENU_STATE,1);

                if (!menu_Scrolled[menu_ITEM])
                {
                    float scrl = (menu_zgi_inv_w / float(GetFps())) /menu_SCROLL_TIME_INV;

                    if (scrl == 0)
                        scrl = 1.0;

                    menu_ScrollPos [menu_ITEM] += scrl;
                }

                if (menu_ScrollPos[menu_ITEM] >= 0)
                {
                    menu_Scrolled[menu_ITEM] = true;
                    menu_ScrollPos [menu_ITEM] = 0;
                }

                int item_count = GetgVarInt(SLOT_TOTAL_INV_AVAIL);
                if (item_count == 0)
                    item_count = 20;



                for (int i=0; i<item_count; i++)
                {
                    int itemspace = (menu_zgi_inv_w-menu_ITEM_SPACE) / item_count;

                    if (MouseInRect(menu_ScrollPos[menu_ITEM] + itemspace * i,0,menu_zgi_inv_hot_w,menu_zgi_inv_h))
                    {
                        mouse_on_item = i;

                        if (MouseUp(SDL_BUTTON_LEFT))
                        {
                            int32_t mouse_item = GetgVarInt(SLOT_INVENTORY_MOUSE);
                            if (mouse_item >= 0  && mouse_item < 0xE0)
                            {
                                inv_drop(mouse_item);
                                inv_add(GetgVarInt(SLOT_START_SLOT + i));
                                SetgVarInt(SLOT_START_SLOT + i,mouse_item);
                            }
                        }
                    }
                }


            }
            break;

        case menu_MAGIC:
            if (menu_bar_flag & menu_BAR_MAGIC)
            {
                SetgVarInt(SLOT_MENU_STATE,3);

                if (!menu_Scrolled[menu_MAGIC])
                {
                    float scrl = (menu_zgi_inv_w / float(GetFps())) /menu_SCROLL_TIME_INV;

                    if (scrl == 0)
                        scrl = 1.0;

                    menu_ScrollPos [menu_MAGIC] += scrl;
                }

                if (menu_ScrollPos[menu_MAGIC] >= menuback[menu_MAGIC][0]->w)
                {
                    menu_Scrolled[menu_MAGIC] = true;
                    menu_ScrollPos [menu_MAGIC] = menuback[menu_MAGIC][0]->w;
                }

                for (int i=0; i<12; i++)
                {

                    int itemnum;
                    if (GetgVarInt(SLOT_REVERSED_SPELLBOOK) == 1)
                        itemnum = 0xEE + i;
                    else
                        itemnum = 0xE0 + i;


                    if (MouseInRect(GAME_W + menu_MAGIC_SPACE + menu_MAGIC_ITEM_W * i-menu_ScrollPos[menu_MAGIC],0,menu_zgi_inv_hot_w,menu_zgi_inv_h))
                    {
                        mouse_on_item = i;
                        if (MouseUp(SDL_BUTTON_LEFT))
                            if (GetgVarInt(SLOT_INVENTORY_MOUSE) == 0 || GetgVarInt(SLOT_INVENTORY_MOUSE) >= 0xE0)
                                if (GetgVarInt(SLOT_SPELL_1 + i) != 0)
                                    SetgVarInt(SLOT_USER_CHOSE_THIS_SPELL,itemnum);
                    }
                }

            }
            break;

        case menu_MAIN:

            SetgVarInt(SLOT_MENU_STATE,2);

            if (!menu_Scrolled[menu_MAIN])
            {
                float scrl = (menu_MAIN_EL_H / float(GetFps())) /menu_SCROLL_TIME;

                if (scrl == 0)
                    scrl = 1.0;

                menu_ScrollPos [menu_MAIN] += scrl;

            }

            if (menu_ScrollPos[menu_MAIN] >= 0)
            {
                menu_Scrolled[menu_MAIN] = true;
                menu_ScrollPos [menu_MAIN] = 0;
            }

            //EXIT
            if (menu_bar_flag & menu_BAR_EXIT)
                if (MouseInRect(menu_MAIN_CENTER+menu_MAIN_EL_W,
                                menu_ScrollPos[menu_MAIN],
                                menu_MAIN_EL_W,
                                menu_MAIN_EL_H))
                {
                    mouse_on_item = menu_MAIN_IMAGE_EXIT;
                    if (MouseUp(SDL_BUTTON_LEFT))
                        ifquit();
                }


            //SETTINGS
            if (menu_bar_flag & menu_BAR_SETTINGS)
                if (MouseInRect(menu_MAIN_CENTER,
                                menu_ScrollPos[menu_MAIN],
                                menu_MAIN_EL_W,
                                menu_MAIN_EL_H))
                {
                    mouse_on_item = menu_MAIN_IMAGE_PREF;
                    if (MouseUp(SDL_BUTTON_LEFT))
                        SetNeedLocate(PrefWorld,PrefRoom,PrefNode,PrefView,0);
                }



            //LOAD
            if (menu_bar_flag & menu_BAR_RESTORE)
                if (MouseInRect(menu_MAIN_CENTER-menu_MAIN_EL_W,
                                menu_ScrollPos[menu_MAIN],
                                menu_MAIN_EL_W,
                                menu_MAIN_EL_H))
                {
                    mouse_on_item = menu_MAIN_IMAGE_REST;
                    if (MouseUp(SDL_BUTTON_LEFT))
                        SetNeedLocate(LoadWorld,LoadRoom,LoadNode,LoadView,0);
                }


            //SAVE
            if (menu_bar_flag & menu_BAR_SAVE)
                if (MouseInRect(menu_MAIN_CENTER-menu_MAIN_EL_W*2,
                                menu_ScrollPos[menu_MAIN],
                                menu_MAIN_EL_W,
                                menu_MAIN_EL_H))
                {
                    mouse_on_item = menu_MAIN_IMAGE_SAVE;
                    if (MouseUp(SDL_BUTTON_LEFT))
                        SetNeedLocate(SaveWorld,SaveRoom,SaveNode,SaveView,0);
                }


            break;

        default:

            if (MouseInRect(menu_MAIN_X,0,\
                            menuback[menu_MAIN][1]->w,\
                            8))
            {
                menu_mousefocus = menu_MAIN;
                menu_Scrolled[menu_MAIN]  = false;
                menu_ScrollPos[menu_MAIN] = menuback[menu_MAIN][1]->h - menuback[menu_MAIN][0]->h;
            }

            if (menu_bar_flag & menu_BAR_MAGIC)
                if (MouseInRect(GAME_W-menu_zgi_inv_hot_w,0,menu_zgi_inv_hot_w,menu_zgi_inv_h))
                {
                    menu_mousefocus = menu_MAGIC;
                    menu_Scrolled[menu_MAGIC]  = false;
                    menu_ScrollPos[menu_MAGIC] = menu_zgi_inv_hot_w;
                }

            if (menu_bar_flag & menu_BAR_ITEM)
                if (MouseInRect(0,0,menu_zgi_inv_hot_w,menu_zgi_inv_h))
                {
                    menu_mousefocus = menu_ITEM;
                    menu_Scrolled[menu_ITEM]  = false;
                    menu_ScrollPos[menu_ITEM] = menu_zgi_inv_hot_w - menu_zgi_inv_w;
                }
        }
    }
    else
    {
        inmenu = false;
        SetDirectgVarInt(SLOT_MENU_STATE,0);
        menu_mousefocus = -1;
    }
}


#define znem_but1_w  120
#define znem_but1_x  0
#define znem_but2_w  144
#define znem_but2_x  120
#define znem_but3_w  128
#define znem_but3_x  264
#define znem_but4_w  120
#define znem_but4_x  392

void menu_UpdateMenuBar_znemesis()
{
    mouse_on_item = -1;

    if (MouseY() <= menu_HOT_Y)
    {
        inmenu = true;

            SetgVarInt(SLOT_MENU_STATE,2);

            if (!scrolled_znem)
            {
                float scrl = (menubar_znem->h / float(GetFps())) /menu_SCROLL_TIME;

                if (scrl == 0)
                    scrl = 1.0;

                scrollpos_znem += scrl;
            }

            if (scrollpos_znem >= 0)
            {
                scrolled_znem = true;
                scrollpos_znem = 0;
            }

            //EXIT
            if (menu_bar_flag & menu_BAR_EXIT)
                if (MouseInRect(menu_MAIN_X+znem_but4_x,
                                menu_ScrollPos[menu_MAIN],
                                znem_but4_w,
                                menu_MAIN_EL_H))
                {
                    mouse_on_item = menu_MAIN_IMAGE_EXIT;
                    if (MouseUp(SDL_BUTTON_LEFT))
                    {
                        butframe_znem[menu_MAIN_IMAGE_EXIT] = menu_znemesis_but_clk_frm;
                        ifquit();
                    }

                }


            //SETTINGS
            if (menu_bar_flag & menu_BAR_SETTINGS)
                if (MouseInRect(menu_MAIN_X+znem_but3_x,
                                menu_ScrollPos[menu_MAIN],
                                znem_but3_w,
                                menu_MAIN_EL_H))
                {
                    mouse_on_item = menu_MAIN_IMAGE_PREF;
                    if (MouseUp(SDL_BUTTON_LEFT))
                    {
                        butframe_znem[menu_MAIN_IMAGE_PREF] = menu_znemesis_but_clk_frm;
                        SetNeedLocate(PrefWorld,PrefRoom,PrefNode,PrefView,0);
                    }

                }



            //LOAD
            if (menu_bar_flag & menu_BAR_RESTORE)
                if (MouseInRect(menu_MAIN_X+znem_but2_x,
                                menu_ScrollPos[menu_MAIN],
                                znem_but2_w,
                                menu_MAIN_EL_H))
                {
                    mouse_on_item = menu_MAIN_IMAGE_REST;
                    if (MouseUp(SDL_BUTTON_LEFT))
                    {
                        butframe_znem[menu_MAIN_IMAGE_REST] = menu_znemesis_but_clk_frm;
                        SetNeedLocate(LoadWorld,LoadRoom,LoadNode,LoadView,0);
                    }

                }


            //SAVE
            if (menu_bar_flag & menu_BAR_SAVE)
                if (MouseInRect(menu_MAIN_X+znem_but1_x,
                                menu_ScrollPos[menu_MAIN],
                                znem_but1_w,
                                menu_MAIN_EL_H))
                {
                    mouse_on_item = menu_MAIN_IMAGE_SAVE;
                    if (MouseUp(SDL_BUTTON_LEFT))
                    {
                        butframe_znem[menu_MAIN_IMAGE_SAVE] = menu_znemesis_but_clk_frm;
                        SetNeedLocate(SaveWorld,SaveRoom,SaveNode,SaveView,0);
                    }

                }

            if (lastbut_znem != mouse_on_item && mouse_on_item != -1)
            {
                butframe_znem[mouse_on_item] = 0;
                znem_but_anim = 0;
            }
            else if (mouse_on_item != -1)
            {
                if (butframe_znem[mouse_on_item] < menu_znemesis_but_max_frm)
                {
                    znem_but_anim -= GetDTime();

                    if (znem_but_anim < 0)
                    {
                        znem_but_anim = menu_znemesis_butanim;
                        butframe_znem[mouse_on_item]++;
                    }
                }
            }

            lastbut_znem = mouse_on_item;

    }
    else
    {
        inmenu = false;
        SetDirectgVarInt(SLOT_MENU_STATE,0);
        menu_mousefocus = -1;
        scrolled_znem  = false;
        lastbut_znem = -1;
        mouse_on_item = -1;

        if (scrollpos_znem > -menubar_znem->h)
        {
            float scrl = (menubar_znem->h / float(GetFps())) /menu_SCROLL_TIME;

            if (scrl == 0)
                scrl = 1.0;

            scrollpos_znem -= scrl;
        }
        else
            scrollpos_znem = -menubar_znem->h;
    }
}




void menu_DrawMenuBar_zgi()
{
    char buf[255];

    if (inmenu)
    {

        switch (menu_mousefocus)
        {
        case menu_ITEM:
            if (menu_bar_flag & menu_BAR_ITEM)
            {
                DrawImage(menuback[menu_ITEM][0],menu_ScrollPos[menu_ITEM],0);

                int item_count = GetgVarInt(SLOT_TOTAL_INV_AVAIL);
                if (item_count == 0)
                    item_count = 20;


                for (int i=0; i<item_count; i++)
                {
                    int itemspace = (menu_zgi_inv_w-menu_ITEM_SPACE) / item_count;

                    bool inrect = false;

                    if (mouse_on_item == i)
                        inrect = true;


                    if (GetgVarInt(SLOT_START_SLOT + i) != 0)
                    {
                        int itemnum = GetgVarInt(SLOT_START_SLOT + i);

                        if (menupicto[itemnum][0] == NULL)
                        {
                            sprintf(buf,"gmzwu%2.2x1.tga",itemnum);
                            menupicto[itemnum][0] = loader_LoadFile(buf,0,0);
                        }
                        if (menupicto[itemnum][1] == NULL)
                        {
                            sprintf(buf,"gmzxu%2.2x1.tga",itemnum);
                            menupicto[itemnum][1] = loader_LoadFile(buf,0,0);
                        }
                        if (inrect)
                            DrawImage(menupicto[itemnum][1],menu_ScrollPos[menu_ITEM] + itemspace * i,0);
                        else
                            DrawImage(menupicto[itemnum][0],menu_ScrollPos[menu_ITEM] + itemspace * i,0);
                    }
                }


            }
            break;

        case menu_MAGIC:
            if (menu_bar_flag & menu_BAR_MAGIC)
            {
                DrawImage(menuback[menu_MAGIC][0],GAME_W-menu_ScrollPos[menu_MAGIC],0);

                for (int i=0; i<12; i++)
                {

                    int itemnum;
                    if (GetgVarInt(SLOT_REVERSED_SPELLBOOK) == 1)
                        itemnum = 0xEE + i;
                    else
                        itemnum = 0xE0 + i;

                    bool inrect = false;

                    if (mouse_on_item == i)
                        inrect = true;

                    if (GetgVarInt(SLOT_SPELL_1 + i) != 0)
                    {

                        if (menupicto[itemnum][0] == NULL)
                        {
                            sprintf(buf,"gmzwu%2.2x1.tga",itemnum);
                            menupicto[itemnum][0] = loader_LoadFile(buf,0,0);
                        }
                        if (menupicto[itemnum][1] == NULL)
                        {
                            sprintf(buf,"gmzxu%2.2x1.tga",itemnum);
                            menupicto[itemnum][1] = loader_LoadFile(buf,0,0);
                        }
                        if (inrect)
                            DrawImage(menupicto[itemnum][1],GAME_W + menu_MAGIC_SPACE + menu_MAGIC_ITEM_W * i-menu_ScrollPos[menu_MAGIC],0);
                        else
                            DrawImage(menupicto[itemnum][0],GAME_W + menu_MAGIC_SPACE + menu_MAGIC_ITEM_W * i-menu_ScrollPos[menu_MAGIC],0);

                    }
                }

            }
            break;

        case menu_MAIN:


            DrawImage(menuback[menu_MAIN][0],menu_MAIN_X,menu_ScrollPos[menu_MAIN]);

            //EXIT
            if (menu_bar_flag & menu_BAR_EXIT)
            {
                if (mouse_on_item == menu_MAIN_IMAGE_EXIT)
                    DrawImage(menubar[menu_MAIN_IMAGE_EXIT][1],menu_MAIN_CENTER+menu_MAIN_EL_W,
                              menu_ScrollPos[menu_MAIN]);
                else
                    DrawImage(menubar[menu_MAIN_IMAGE_EXIT][0],menu_MAIN_CENTER+menu_MAIN_EL_W,
                              menu_ScrollPos[menu_MAIN]);
            }

            //SETTINGS
            if (menu_bar_flag & menu_BAR_SETTINGS)
            {
                if (mouse_on_item == menu_MAIN_IMAGE_PREF)
                    DrawImage(menubar[menu_MAIN_IMAGE_PREF][1],menu_MAIN_CENTER,
                              menu_ScrollPos[menu_MAIN]);
                else
                    DrawImage(menubar[menu_MAIN_IMAGE_PREF][0],menu_MAIN_CENTER,
                              menu_ScrollPos[menu_MAIN]);
            }

            //LOAD
            if (menu_bar_flag & menu_BAR_RESTORE)
            {
                if (mouse_on_item == menu_MAIN_IMAGE_REST)
                    DrawImage(menubar[menu_MAIN_IMAGE_REST][1],menu_MAIN_CENTER-menu_MAIN_EL_W,
                              menu_ScrollPos[menu_MAIN]);
                else
                    DrawImage(menubar[menu_MAIN_IMAGE_REST][0],menu_MAIN_CENTER-menu_MAIN_EL_W,
                              menu_ScrollPos[menu_MAIN]);
            }

            //SAVE
            if (menu_bar_flag & menu_BAR_SAVE)
            {
                if (mouse_on_item == menu_MAIN_IMAGE_SAVE)
                    DrawImage(menubar[menu_MAIN_IMAGE_SAVE][1],menu_MAIN_CENTER-menu_MAIN_EL_W*2,
                              menu_ScrollPos[menu_MAIN]);
                else
                    DrawImage(menubar[menu_MAIN_IMAGE_SAVE][0],menu_MAIN_CENTER-menu_MAIN_EL_W*2,
                              menu_ScrollPos[menu_MAIN]);
            }

            break;

        default:
            DrawImage(menuback[menu_MAIN][1],menu_MAIN_X,0);

            if (menu_bar_flag & menu_BAR_ITEM)
                DrawImage(menuback[menu_ITEM][1],0,0);

            if (menu_bar_flag & menu_BAR_MAGIC)
                DrawImage(menuback[menu_MAGIC][1], GAME_W-menu_zgi_inv_hot_w, 0);

        }
    }

}


void menu_DrawMenuBar_znemesis()
{
    if (inmenu)
    {
            DrawImage(menubar_znem,menu_MAIN_X,scrollpos_znem);

            //EXIT
            if (menu_bar_flag & menu_BAR_EXIT)
            {
                if (mouse_on_item == menu_MAIN_IMAGE_EXIT)
                    DrawImage(menubut_znem[menu_MAIN_IMAGE_EXIT][butframe_znem[menu_MAIN_IMAGE_EXIT]],menu_MAIN_X+znem_but4_x,
                              scrollpos_znem);
            }

            //SETTINGS
            if (menu_bar_flag & menu_BAR_SETTINGS)
            {
                if (mouse_on_item == menu_MAIN_IMAGE_PREF)
                    DrawImage(menubut_znem[menu_MAIN_IMAGE_PREF][butframe_znem[menu_MAIN_IMAGE_PREF]],menu_MAIN_X+znem_but3_x,
                              scrollpos_znem);
            }

            //LOAD
            if (menu_bar_flag & menu_BAR_RESTORE)
            {
                if (mouse_on_item == menu_MAIN_IMAGE_REST)
                    DrawImage(menubut_znem[menu_MAIN_IMAGE_REST][butframe_znem[menu_MAIN_IMAGE_REST]],menu_MAIN_X+znem_but2_x,
                              scrollpos_znem);
            }

            //SAVE
            if (menu_bar_flag & menu_BAR_SAVE)
            {
                if (mouse_on_item == menu_MAIN_IMAGE_SAVE)
                    DrawImage(menubut_znem[menu_MAIN_IMAGE_SAVE][butframe_znem[menu_MAIN_IMAGE_SAVE]],menu_MAIN_X+znem_but1_x,
                              scrollpos_znem);
            }

    }
    else if (scrollpos_znem > -menubar_znem->h)
        DrawImage(menubar_znem,menu_MAIN_X,scrollpos_znem);
}
