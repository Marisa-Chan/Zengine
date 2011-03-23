#include "System.h"
#include "menu.h"


#define menu_ITEM  0
#define menu_MAGIC 1
#define menu_MAIN  2

#define menu_MAIN_EL_W  135
#define menu_MAIN_EL_H  32

#define menu_MAIN_IMAGE_SAVE   0
#define menu_MAIN_IMAGE_REST   1
#define menu_MAIN_IMAGE_PREF   2
#define menu_MAIN_IMAGE_EXIT   3

#define menu_MAIN_CENTER       320

#define menu_MAGIC_SPACE       28
#define menu_MAGIC_ITEM_W      47

#define menu_ITEM_SPACE        28

uint16_t menu_bar_flag = 0xFFFF;

bool   menu_Scrolled [3] = {false,false,false};
int16_t menu_ScrollPos[3] = {0,0,0};

SDL_Surface *menubar[4][2];
SDL_Surface *menuback[3][2];
SDL_Surface *menupicto[256];


int menu_MAIN_X = (640-580)/2;

int8_t menu_mousefocus = -1;




void menu_SetMenuBarVal(uint16_t val)
{
    menu_bar_flag = val;
}

uint16_t menu_GetMenuBarVal()
{
    return menu_bar_flag;
}

void menu_LoadGraphics()
{
    char buf[255];
    for (int i=1;i<4;i++)
    {
        sprintf(buf,"gmzau%2.2x1.tga",i);
        menuback[i-1][0] = LoadConvertImg(GetFilePath(buf));
        sprintf(buf,"gmzau%2.2x1.tga",i+0x10);
        menuback[i-1][1] = LoadConvertImg(GetFilePath(buf));
    }
    for (int i=0;i<4;i++)
    {
        sprintf(buf,"gmzmu%2.2x1.tga",i);
        menubar[i][0] = LoadConvertImg(GetFilePath(buf));
        sprintf(buf,"gmznu%2.2x1.tga",i);
        menubar[i][1] = LoadConvertImg(GetFilePath(buf));
    }

    for (int i=0; i<256; i++)
        menupicto[i] = NULL;
}

void menu_UpdateMenuBar()
{
    char buf[255];

    if (MouseY() <= 40)
        {

            switch (menu_mousefocus)
            {
            case menu_ITEM:
                if (menu_bar_flag & menu_BAR_ITEM)
                {
                if (!menu_Scrolled[menu_ITEM])
                    menu_ScrollPos [menu_ITEM]+=12;

                if (menu_ScrollPos[menu_ITEM] >= 0)
                    {
                        menu_Scrolled[menu_ITEM] = true;
                        menu_ScrollPos [menu_ITEM] = 0;
                    }

                DrawImage(menuback[menu_ITEM][0],menu_ScrollPos[menu_ITEM],0);

                int item_count = GetgVarInt(SLOT_TOTAL_INV_AVAIL);
                if (item_count == 0)
                    item_count = 20;
                printf("%d\n",item_count);
                for (int i=0; i<item_count; i++)
                {
                    if (GetgVarInt(SLOT_START_SLOT + i) != 0)
                    {
                        int itemnum = GetgVarInt(SLOT_START_SLOT + i);

                        if (menupicto[itemnum] == NULL)
                            {
                                sprintf(buf,"gmzwu%2.2x1.tga",itemnum);
                                menupicto[itemnum] = LoadConvertImg(GetFilePath(buf),0);
                            }
                        int itemspace = (600-menu_ITEM_SPACE) / item_count;
                        DrawImage(menupicto[itemnum],menu_ScrollPos[menu_ITEM] + itemspace * i,0);
                    }
                }


                }
                break;

            case menu_MAGIC:
                if (menu_bar_flag & menu_BAR_MAGIC)
                {
                if (!menu_Scrolled[menu_MAGIC])
                    menu_ScrollPos [menu_MAGIC]+=12;

                if (menu_ScrollPos[menu_MAGIC] >= menuback[menu_MAGIC][0]->w)
                    {
                        menu_Scrolled[menu_MAGIC] = true;
                        menu_ScrollPos [menu_MAGIC] = menuback[menu_MAGIC][0]->w;
                    }

                DrawImage(menuback[menu_MAGIC][0],640-menu_ScrollPos[menu_MAGIC],0);

                for (int i=0; i<12; i++)
                {
                    if (GetgVarInt(SLOT_SPELL_1 + i) != 0)
                    {
                        int itemnum = 0xE0 + i;
                        if (menupicto[itemnum] == NULL)
                            {
                                sprintf(buf,"gmzwu%2.2x1.tga",itemnum);
                                menupicto[itemnum] = LoadConvertImg(GetFilePath(buf),0);
                            }

                        DrawImage(menupicto[itemnum],menu_ScrollPos[menu_MAGIC] + menu_MAGIC_SPACE + menu_MAGIC_ITEM_W * i,0);
                    }
                }

                }
                break;

            case menu_MAIN:
                if (!menu_Scrolled[menu_MAIN])
                    menu_ScrollPos [menu_MAIN]+=2;

                if (menu_ScrollPos[menu_MAIN] >= 0)
                    {
                        menu_Scrolled[menu_MAIN] = true;
                        menu_ScrollPos [menu_MAIN] = 0;
                    }

                DrawImage(menuback[menu_MAIN][0],menu_MAIN_X,menu_ScrollPos[menu_MAIN]);

                //EXIT
                if (menu_bar_flag & menu_BAR_EXIT)
                {
                    if (MouseInRect(menu_MAIN_CENTER+menu_MAIN_EL_W,
                                    menu_ScrollPos[menu_MAIN],
                                    menu_MAIN_EL_W,
                                    menu_MAIN_EL_H))
                    {
                        DrawImage(menubar[menu_MAIN_IMAGE_EXIT][1],menu_MAIN_CENTER+menu_MAIN_EL_W,
                                                                   menu_ScrollPos[menu_MAIN]);

                        if (MouseUp(SDL_BUTTON_LEFT))
                            __END();

                    }
                    else
                        DrawImage(menubar[menu_MAIN_IMAGE_EXIT][0],menu_MAIN_CENTER+menu_MAIN_EL_W,
                                                                   menu_ScrollPos[menu_MAIN]);
                }

                //SETTINGS
                if (menu_bar_flag & menu_BAR_SETTINGS)
                {
                    if (MouseInRect(menu_MAIN_CENTER,
                                    menu_ScrollPos[menu_MAIN],
                                    menu_MAIN_EL_W,
                                    menu_MAIN_EL_H))
                    {
                        DrawImage(menubar[menu_MAIN_IMAGE_PREF][1],menu_MAIN_CENTER,
                                                                   menu_ScrollPos[menu_MAIN]);

                        if (MouseUp(SDL_BUTTON_LEFT))
                            SetNeedLocate('g','j','p','e',0);

                    }
                    else
                        DrawImage(menubar[menu_MAIN_IMAGE_PREF][0],menu_MAIN_CENTER,
                                                                   menu_ScrollPos[menu_MAIN]);
                }

                //LOAD
                if (menu_bar_flag & menu_BAR_RESTORE)
                {
                    if (MouseInRect(menu_MAIN_CENTER-menu_MAIN_EL_W,
                                    menu_ScrollPos[menu_MAIN],
                                    menu_MAIN_EL_W,
                                    menu_MAIN_EL_H))
                    {
                        DrawImage(menubar[menu_MAIN_IMAGE_REST][1],menu_MAIN_CENTER-menu_MAIN_EL_W,
                                                                   menu_ScrollPos[menu_MAIN]);

                        if (MouseUp(SDL_BUTTON_LEFT))
                            SetNeedLocate('g','j','r','e',0);

                    }
                    else
                        DrawImage(menubar[menu_MAIN_IMAGE_REST][0],menu_MAIN_CENTER-menu_MAIN_EL_W,
                                                                   menu_ScrollPos[menu_MAIN]);
                }

                //SAVE
                if (menu_bar_flag & menu_BAR_SAVE)
                {
                    if (MouseInRect(menu_MAIN_CENTER-menu_MAIN_EL_W*2,
                                    menu_ScrollPos[menu_MAIN],
                                    menu_MAIN_EL_W,
                                    menu_MAIN_EL_H))
                    {
                        DrawImage(menubar[menu_MAIN_IMAGE_SAVE][1],menu_MAIN_CENTER-menu_MAIN_EL_W*2,
                                                                   menu_ScrollPos[menu_MAIN]);

                        if (MouseUp(SDL_BUTTON_LEFT))
                            SetNeedLocate('g','j','s','e',0);

                    }
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
                    DrawImage(menuback[menu_MAGIC][1], 640-menuback[menu_MAGIC][1]->w, 0);

               if (MouseInRect(menu_MAIN_X,0,\
                            menuback[menu_MAIN][1]->w,\
                            4))
                {
                    menu_mousefocus = menu_MAIN;
                    menu_Scrolled[menu_MAIN]  = false;
                    menu_ScrollPos[menu_MAIN] = menuback[menu_MAIN][1]->h - menuback[menu_MAIN][0]->h;
                }

               if (MouseInRect(640-28,0,28,32))
                {
                    menu_mousefocus = menu_MAGIC;
                    menu_Scrolled[menu_MAGIC]  = false;
                    menu_ScrollPos[menu_MAGIC] = 28;
                }
               if (MouseInRect(0,0,28,32))
                {
                    menu_mousefocus = menu_ITEM;
                    menu_Scrolled[menu_ITEM]  = false;
                    menu_ScrollPos[menu_ITEM] = menuback[menu_ITEM][1]->w - menuback[menu_ITEM][0]->w;
                }
            }




        }
    else
        {
            menu_mousefocus = -1;
        }
}
