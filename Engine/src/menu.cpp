#include "System.h"
#include "menu.h"


#define menu_ITEM  0
#define menu_MAGIC 1
#define menu_MAIN  2

uint16_t menu_bar = 0xFFFF;

bool   menu_Scrolled [3] = {false,false,false};
int16_t menu_ScrollPos[3] = {0,0,0};

SDL_Surface *menubar[4][2];
SDL_Surface *menuback[3][2];

int menu_MAIN_X = (640-580)/2;

int8_t menu_mousefocus = -1;

void menu_SetMenuBarVal(uint16_t val)
{
    menu_bar = val;
}

uint16_t menu_GetMenuBarVal()
{
    return menu_bar;
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
}

void menu_UpdateMenuBar()
{

    if (MouseY() <= 40)
        {

            switch (menu_mousefocus)
            {
            case menu_ITEM:
                if (!menu_Scrolled[menu_ITEM])
                    menu_ScrollPos [menu_ITEM]+=12;

                if (menu_ScrollPos[menu_ITEM] >= 0)
                    {
                        menu_Scrolled[menu_ITEM] = true;
                        menu_ScrollPos [menu_ITEM] = 0;
                    }

                DrawImage(menuback[menu_ITEM][0],menu_ScrollPos[menu_ITEM],0);

                break;

            case menu_MAGIC:
                if (!menu_Scrolled[menu_MAGIC])
                    menu_ScrollPos [menu_MAGIC]+=12;

                if (menu_ScrollPos[menu_MAGIC] >= menuback[menu_MAGIC][0]->w)
                    {
                        menu_Scrolled[menu_MAGIC] = true;
                        menu_ScrollPos [menu_MAGIC] = menuback[menu_MAGIC][0]->w;
                    }

                DrawImage(menuback[menu_MAGIC][0],640-menu_ScrollPos[menu_MAGIC],0);

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

                break;

            default:
               DrawImage(menuback[menu_MAIN][1],menu_MAIN_X,0);
               DrawImage(menuback[menu_ITEM][1],0,0);
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
