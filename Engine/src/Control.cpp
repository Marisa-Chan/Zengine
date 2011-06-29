
#include "System.h"

int FocusInput=0;

ctrlnode *Ctrl_CreateNode(int type)
{
    ctrlnode *tmp;
    tmp = new(ctrlnode);
    tmp->node.unknown = NULL;
    tmp->slot = 0;
    tmp->type = CTRL_UNKN;
    tmp->func = NULL;

    switch(type)
    {
        case CTRL_PUSH:
            tmp->type = CTRL_PUSH;
            tmp->node.push = new(pushnode);
            tmp->func = control_push;
        break;

        case CTRL_INPUT:
            tmp->type = CTRL_INPUT;
            tmp->node.inp = new(inputnode);
            tmp->func = control_input;
            tmp->node.inp->cursor = NULL;
        break;

        case CTRL_SLOT:
            tmp->type = CTRL_SLOT;
            tmp->node.slot = new(slotnode);
            tmp->func = control_slot;
            tmp->node.slot->srf = NULL;
        break;

    };
    return tmp;
}


bool Ctrl_Eligeblity(int obj, slotnode *slut)
{
    bool eli = false;

    for (int i=0; i< slut->eligable_cnt; i++)
        if (obj == slut->eligible_objects[i])
        {
            eli = true;
            break;
        }
    return eli;
}


void Ctrl_DrawSlots()
{
    MList *ctrl = Getctrl();
    StartMList(ctrl);

    while (!eofMList(ctrl))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(ctrl);
        if (nod->type == CTRL_SLOT)
        {
            slotnode *slut = nod->node.slot;

            //rectangleRGBA(screen,slut->hotspot.x,slut->hotspot.y+GAME_Y,slut->hotspot.w,slut->hotspot.h+GAME_Y,255,0,0,255);


            int tmp1 = GetgVarInt(nod->slot);
            //printf("%d %d\n",nod->slot,tmp1);
            bool in = Ctrl_Eligeblity(tmp1,slut);

            if (in && tmp1!=GetgVarInt(9))
            {
                if (slut->srf==NULL)
                {
                    char bff[16];
                    sprintf(bff,"G0ZYU%2.2x1.tga",tmp1);
                    slut->srf=IMG_Load(GetFilePath(bff));
                    ConvertImage(&slut->srf);
                    SDL_SetColorKey(slut->srf,SDL_SRCCOLORKEY ,SDL_MapRGB(slut->srf->format,0,0,0));
                }

                Rend_DrawImageUpGamescr(slut->srf,    slut->rectangle.x,  slut->rectangle.y);
            }
            else
            {
                if (slut->srf != NULL)
                {
                    SDL_FreeSurface(slut->srf);
                    slut->srf=NULL;
                }

            }

        }
        NextMList(ctrl);
    }
}


void control_input(ctrlnode *ct)
{
    inputnode *inp = ct->node.inp;
    bool mousein = false;

    if ( inp->hotspot.x <= Rend_GetMouseGameX() &&\
         inp->hotspot.w >= Rend_GetMouseGameX() &&\
         inp->hotspot.y <= Rend_GetMouseGameY() &&\
         inp->hotspot.h >= Rend_GetMouseGameY() )
        mousein = true;

    if (mousein)
    {
       // if (Mouse_IsCurrentCur(CURSOR_IDLE))
                Mouse_SetCursor(CURSOR_ACTIVE);

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

        if (GetgVarInt(ct->slot)!=0)
            if (Mouse_IsCurrentCur(CURSOR_IDLE))
                Mouse_SetCursor(slut->cursor);

        if (MouseUp(SDL_BUTTON_LEFT))
        {
            if (GetgVarInt(SLOT_INVENTORY_MOUSE)==0)
            {
                SetgVarInt(SLOT_INVENTORY_MOUSE,GetgVarInt(ct->slot));
                SetgVarInt(ct->slot,0);

            }
            else if (Ctrl_Eligeblity(GetgVarInt(SLOT_INVENTORY_MOUSE),slut))
            {
                int te=GetgVarInt(ct->slot);
                SetgVarInt(ct->slot,GetgVarInt(SLOT_INVENTORY_MOUSE));
                SetgVarInt(SLOT_INVENTORY_MOUSE,te);
                SDL_FreeSurface(slut->srf);
                slut->srf=NULL;
            }
#ifdef TRACE
            printf("Pushed\n");
            printf("Slot #%d to 1\n",ct->slot);
#endif
            FlushMouseBtn(SDL_BUTTON_LEFT);

        }

    }
}

void control_push(ctrlnode *ct)
{
    bool mousein=false;

#ifdef FULLTRACE
    printf("Push_toggle\n");
#endif

    pushnode *psh = ct->node.push;

    if ( psh->x          <= Rend_GetMouseGameX() &&\
         psh->x+psh->w   >= Rend_GetMouseGameX() &&\
         psh->y          <= Rend_GetMouseGameY() &&\
         psh->y+psh->h   >= Rend_GetMouseGameY() )
            mousein = true;

    if (mousein)
    {
        if (Mouse_IsCurrentCur(CURSOR_IDLE))
            Mouse_SetCursor(psh->cursor);

        if (MouseUp(SDL_BUTTON_LEFT))
        {
#ifdef TRACE
            printf("Pushed #%d\n",ct->slot);
#endif
            SetgVarInt(ct->slot,1);

            FlushMouseBtn(SDL_BUTTON_LEFT);

        }

    }
}



int Parse_Control_Flat()
{
    Rend_SetRenderer (RENDER_FLAT);
}



int Parse_Control_Panorama(FILE *fl)
{
    char  buf[FILE_LN_BUF];
    char *str;
    int  good = 0;

    double angle = 27.0;
    double     k = 0.55;
    int      tmp = 0;

    Rend_SetRenderer (RENDER_PANA);

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
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
        }
        else if (strCMP(str,"linscale")==0)
        {
            str   = GetParams(str);
            k = atof(str);
        }
        else if (strCMP(str,"reversepana")==0)
        {
            str   = GetParams(str);
            tmp = atoi(str);
            printf("%d\n",tmp);
            if (tmp == 1)
                Rend_SetReversePana(true);
        }

    }

  //  printf("%f\n",angle);
  //  printf("%f\n",k);

    Rend_SetFishTable(angle,k);

}

int Parse_Control_Input(MList *controlst, FILE *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;


    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_INPUT);
    inputnode *inp = ctnode->node.inp;

    AddToMList(controlst,ctnode);

    ctnode->slot      = slot;

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
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
        else if (strCMP(str,"cursor_animation")==0)
        {
            str=GetParams(str);
            char file[16];
            sscanf(str,"%s",file);
            str = GetFilePath(file);
            if (str!=NULL)
                inp->cursor = LoadAnimImage(str,0x00);
        }
        else if (strCMP(str,"cursor_animation_frames")==0)
        {
            str=GetParams(str);
            inp->frames = atoi(str);
        }//if (str[0] == '}')
    }//while (!feof(fl))

}

int Parse_Control_Slot(MList *controlst, FILE *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;


    ctrlnode *ctnode = Ctrl_CreateNode(CTRL_SLOT);
    slotnode *slut = ctnode->node.slot;

    AddToMList(controlst,ctnode);
    ctnode->slot      = slot;
    slut->srf         = NULL;

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
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
            for (int i=0; i<NUM_CURSORS; i++)
                if (strcasecmp(str,Mouse_GetName(i)) == 0)
                {
                    slut->cursor = i;
                    break;
                }
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
            slut->eligible_objects = (int *)malloc (tmpobj * sizeof(int));
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

}

int Parse_Control_PushTgl(MList *controlst, FILE *fl, uint32_t slot)
{
    int good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    SetgVarInt(slot,0);



    ctrlnode *ctnode  = Ctrl_CreateNode(CTRL_PUSH);
    pushnode *psh     = ctnode->node.push;
    ctnode->slot      = slot;

    psh->cursor       = CURSOR_IDLE;

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
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
            sscanf(str,"%d, %d, %d, %d",&psh->x,&psh->y,&psh->w,&psh->h);
#ifdef FULLTRACE
            printf("    Flat %d %d %d %d %d\n",psh->x,psh->y,psh->w,psh->h,psh->flat);
#endif
        }
        else if (strCMP(str,"warp_hotspot") == 0)
        {
            psh->flat=true;
            str = GetParams(str);
            sscanf(str,"%d, %d, %d, %d",&psh->x,&psh->y,&psh->w,&psh->h);
#ifdef FULLTRACE
            printf("    Warp %d %d %d %d %d\n",psh->x,psh->y,psh->w,psh->h,psh->flat);
#endif
        }
        else if (strCMP(str,"cursor") == 0)
        {
            str = GetParams(str); //cursor
            psh->cursor = Mouse_GetCursorIndex(str);
        }
    }

    if (good == 1)
        AddToMList(controlst,ctnode);

    return good;
}

int Parse_Control(MList *controlst,FILE *fl,char *ctstr)
{
    int  good = 0;

    char buf[FILE_LN_BUF];
    char *str;

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
        printf("    Flat Rendere\n");
#endif

        Parse_Control_Flat();
    }
    else if (strCMP(ctrltp,"pana")==0)
    {
#ifdef FULLTRACE
        printf("    Panorama Rendere\n");
#endif

        Parse_Control_Panorama(fl);
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

    }
    else if (strCMP(ctrltp,"slot")==0)
    {
        Parse_Control_Slot(controlst,fl,slot);
    }

    return good;
}



void ProcessControls(MList *ctrlst)
{

    pushnode *psh;//=(pushnode *) nod->node;
    slotnode *slut;

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

void DeleteSelControl(ctrlnode *nod)
{
    switch (nod->type)
        {
        case CTRL_PUSH:
            delete nod->node.push;
        break;
        case CTRL_SLOT:
            if (nod->node.slot->srf)
                SDL_FreeSurface(nod->node.slot->srf);
            delete nod->node.slot;
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
    pushnode *psh;
    slotnode *slt;

    StartMList(lst);
    while (!eofMList(lst))
    {
        ctrlnode *nod=(ctrlnode *)DataMList(lst);


        DeleteSelControl(nod);

        NextMList(lst);
    }

    FlushMList(lst);
}
