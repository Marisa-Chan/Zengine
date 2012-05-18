
#include "System.h"


pzllst *CreatePzlLst()
{
    pzllst *tmp     = new (pzllst);
    tmp->_list      = CreateMList();
    tmp->stksize    = 0;
    tmp->exec_times = 0;
    return tmp;
}

char *copy_params(const char *params)
{
    char *tmp = (char *)malloc(strlen(params)+1);
    strcpy(tmp,params);
    return tmp;
}

void Parse_Puzzle_Results_Action(char *instr, MList *lst)
{
    char *str;
    char buf[255];
    int  slot;

    func_node *nod;
    const char *params=" ";

    str = instr;

    slot = 0;

    memset(buf,0,255);

    int end_s=strlen(str);
    // printf("%s\n",str);

    for (int i=0; i<end_s; i++)
    {
        if (str[i]!='(' && str[i]!=0x20 && str[i]!=0x09 && str[i]!='#' && str[i]!=0x00 && str[i]!=':')
            buf[i]=str[i];
        else
        {
            if (str[i]==':')
                slot = atoi(str + i + 1);

            params = GetParams(str + i);

            end_s=i;
            break;
        }
    }

    if (strCMP(buf,"set_screen")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_set_screen;
        return;
    }

    if (strCMP(buf,"debug")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_debug;
        return;
    }

    if (strCMP(buf,"assign")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_assign;
        return;
    }
    if (strCMP(buf,"timer")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_timer;
        return;
    }
    if (strCMP(buf,"set_partial_screen")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_set_partial_screen;
        return;
    }

    if (strCMP(buf,"change_location")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_change_location;
        return;
    }

    if (strCMP(buf,"dissolve")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_dissolve;
        return;
    }

    if (strCMP(buf,"disable_control")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_disable_control;
        return;
    }
    if (strCMP(buf,"enable_control")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_enable_control;
        return;
    }
    if (strCMP(buf,"add")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_add;
        return;
    }
    if (strCMP(buf,"random")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_random;
        return;
    }
    if (strCMP(buf,"animplay")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_animplay;
        return;
    }
    if (strCMP(buf,"universe_music")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_universe_music;
        return;
    }
    if (strCMP(buf,"music")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_music;
        return;
    }

    if (strCMP(buf,"kill")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_kill;
        return;
    }

    if (strCMP(buf,"stop")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_stop;
        return;
    }

    if (strCMP(buf,"inventory")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func = action_inventory;
        return;
    }

    if (strCMP(buf,"crossfade")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_crossfade;
        return;
    }

    if (strCMP(buf,"streamvideo")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_streamvideo;
        return;
    }

    if (strCMP(buf,"animpreload")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_animpreload;
        return;
    }
    if (strCMP(buf,"playpreload")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_playpreload;
        return;
    }
    if (strCMP(buf,"syncsound")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_syncsound;
        return;
    }

    if (strCMP(buf,"menu_bar_enable")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_menu_bar_enable;
        return;
    }

    if (strCMP(buf,"delay_render")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_delay_render;
        return;
    }

    if (strCMP(buf,"ttytext")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_ttytext;
        return;
    }

    if (strCMP(buf,"attenuate")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_attenuate;
        return;
    }

    if (strCMP(buf,"pan_track")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_pan_track;
        return;
    }

    if (strCMP(buf,"animunload")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_animunload;
        return;
    }

    if (strCMP(buf,"flush_mouse_events")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_flush_mouse_events;
        return;
    }

    if (strCMP(buf,"save_game")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_save_game;
        return;
    }

    if (strCMP(buf,"restore_game")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_restore_game;
        return;
    }

    if (strCMP(buf,"quit")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_quit;
        return;
    }

    if (strCMP(buf,"rotate_to")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_rotate_to;
        return;
    }

    if (strCMP(buf,"distort")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_distort;
        return;
    }

    if (strCMP(buf,"preferences")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_preferences;
        return;
    }

    if (strCMP(buf,"region")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_region;
        return;
    }
    if (strCMP(buf,"display_message")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_display_message;
        return;
    }
    if (strCMP(buf,"set_venus")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_set_venus;
        return;
    }
    if (strCMP(buf,"disable_venus")==0)
    {
        nod = new(func_node);
        AddToMList(lst,nod);

        nod->param = copy_params(params);
        nod->slot  = slot;

        nod->func  = action_disable_venus;
        return;
    }

}

int Parse_Puzzle_Flags(puzzlenode *pzl,mfile *fl)
{
    int  good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str=PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"once_per_inst")==0)
        {
            ScrSys_SetFlag(pzl->slot, ScrSys_GetFlag(pzl->slot) | FLAG_ONCE_PER_I);
        }
        else if (strCMP(str,"do_me_now")==0)
        {
            ScrSys_SetFlag(pzl->slot, ScrSys_GetFlag(pzl->slot) | FLAG_DO_ME_NOW);
        }
        else if (strCMP(str,"disabled")==0)
        {
            ScrSys_SetFlag(pzl->slot, ScrSys_GetFlag(pzl->slot) | FLAG_DISABLED);
        }
    }

    return good;
}

int Parse_Puzzle_Criteria(puzzlenode *pzl,mfile *fl)
{
    int  good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    MList *crit_nodes_lst=CreateMList();

    AddToMList(pzl->CritList,crit_nodes_lst);

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str=PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (str[0] == '[')
        {
            crit_node *nod=new(crit_node);
            AddToMList(crit_nodes_lst,nod);

            sscanf(&str[1],"%d",&nod->slot1);

            int ij;
            int32_t t_len = strlen(str);
            for (ij=0; ij<t_len; ij++)
            {
                if (str[ij]=='!')
                {
                    nod->oper=CRIT_OP_NOT;
                    break;
                }
                else if (str[ij]=='>')
                {
                    nod->oper=CRIT_OP_GRE;
                    break;
                }
                else if (str[ij]=='<')
                {
                    nod->oper=CRIT_OP_LEA;
                    break;
                }
                else if (str[ij]=='=')
                {
                    nod->oper=CRIT_OP_EQU;
                    break;
                }
            }


            for (ij++; ij<t_len; ij++)
            {
                if (str[ij]=='[')
                {
                    sscanf(&str[ij+1],"%d",&nod->slot2);
                    nod->var2=true;
                    break;
                }
                else if (str[ij]!=0x20 && str[ij]!=0x09)
                {
                    sscanf(&str[ij],"%d",&nod->slot2);
                    nod->var2=false;
                    break;
                }
            }
        }
        else
        {
            printf("Warning!!! %s\n",str);
            good = 0;
        }

    }

    return good;
}

int Parse_Puzzle_Results(puzzlenode *pzl,mfile *fl)
{
    int  good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str=PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strlen(str)>0)
        {
            char *str2 = strchr(str,':'); //action: background: event: other
            if (str2 != NULL)
                Parse_Puzzle_Results_Action(str2+1,pzl->ResList);
#ifdef TRACE
            else
                printf("Unknown result action: %s\n",str);
#endif
        }

    }
    return good;
}

int Parse_Puzzle(pzllst *lst,mfile *fl,char *ctstr)
{
    int  good = 0;

    char buf[FILE_LN_BUF];
    char *str;

    uint32_t    slot;
    sscanf(ctstr,"puzzle:%d",&slot); //read slot number;

#ifdef FULLTRACE
    printf("puzzle:%d Creating object\n",slot);
#endif

    puzzlenode *pzl=new(puzzlenode);

    pzl->owner    = lst;
    pzl->slot     = slot;
    pzl->CritList = CreateMList();
    pzl->ResList  = CreateMList();

    ScrSys_SetFlag(pzl->slot,0);

    while (!mfeof(fl))
    {
        mfgets(buf,FILE_LN_BUF,fl);
        str=PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"criteria")==0) //PARSE CRITERIA
        {
#ifdef FULLTRACE
            printf("Creating criteria\n");
#endif
            Parse_Puzzle_Criteria(pzl,fl);
        }
        else if (strCMP(str,"results")==0) //RESULTS
        {
#ifdef FULLTRACE
            printf("Creating results\n");
#endif
            Parse_Puzzle_Results(pzl,fl);
        }
        else if (strCMP(str,"flags")==0)  // FLAGS
        {
#ifdef FULLTRACE
            printf("Reading flags\n");
#endif
            Parse_Puzzle_Flags(pzl,fl);
        }

    }

    if ((ScrSys_GetFlag(pzl->slot) & FLAG_ONCE_PER_I ))
        SetgVarInt(slot,0);

    if (good == 1)  //All ok? then, adds this puzzle to list
        AddToMList(lst->_list,pzl);

    return good;
}


void DeletePuzzleList(pzllst *lst)
{
    StartMList(lst->_list);
    while (!eofMList(lst->_list))
    {
        puzzlenode *nod=(puzzlenode *)DataMList(lst->_list);


#ifdef FULLTRACE
        printf("Deleting Puzzle #%d\n",nod->slot);
#endif

        StartMList(nod->CritList);
        while (!eofMList(nod->CritList))
        {

            MList *criteries=(MList *)DataMList(nod->CritList);

            StartMList(criteries);
            while (!eofMList(criteries))
            {
                crit_node *critnd=(crit_node *)DataMList(criteries);
                delete critnd;
                NextMList(criteries);
            }
            DeleteMList(criteries);

            NextMList(nod->CritList);
        }
        DeleteMList(nod->CritList);


        StartMList(nod->ResList);
        while (!eofMList(nod->ResList))
        {
            func_node *fun=(func_node *)DataMList(nod->ResList);
            if (fun->param != NULL)
                free(fun->param);
            delete fun;
            NextMList(nod->ResList);
        }
        DeleteMList(nod->ResList);




        delete nod;

        NextMList(lst->_list);
    }

    DeleteMList(lst->_list);
    delete lst;
}

void FlushPuzzleList(pzllst *lst)
{
    StartMList(lst->_list);
    while (!eofMList(lst->_list))
    {
        puzzlenode *nod=(puzzlenode *)DataMList(lst->_list);


#ifdef FULLTRACE
        printf("Deleting Puzzle #%d\n",nod->slot);
#endif

        StartMList(nod->CritList);
        while (!eofMList(nod->CritList))
        {

            MList *criteries=(MList *)DataMList(nod->CritList);

            StartMList(criteries);
            while (!eofMList(criteries))
            {
                crit_node *critnd=(crit_node *)DataMList(criteries);
                delete critnd;
                NextMList(criteries);
            }
            DeleteMList(criteries);

            NextMList(nod->CritList);
        }
        DeleteMList(nod->CritList);


        StartMList(nod->ResList);
        while (!eofMList(nod->ResList))
        {
            func_node *fun=(func_node *)DataMList(nod->ResList);
            if (fun->param != NULL)
                free(fun->param);
            delete fun;
            NextMList(nod->ResList);
        }
        DeleteMList(nod->ResList);


        delete nod;

        NextMList(lst->_list);
    }

    lst->exec_times = 0;
    lst->stksize    = 0;

    FlushMList(lst->_list);
}

bool ProcessCriteries(MList *lst)
{
    bool tmp=true;

    StartMList(lst);
    while (!eofMList(lst))
    {
        crit_node *critnd=(crit_node *)DataMList(lst);
#ifdef FULLTRACE
        printf("        [%d] %d [%d] %d\n",critnd->slot1,critnd->oper,critnd->slot2,critnd->var2);
#endif
        int tmp1=GetgVarInt(critnd->slot1);
        int tmp2;

        if (critnd->var2)
            tmp2=GetgVarInt(critnd->slot2);
        else
            tmp2=critnd->slot2;

        switch (critnd->oper)
        {
        case CRIT_OP_EQU:
            tmp &= (tmp1 == tmp2);
            break;
        case CRIT_OP_GRE:
            tmp &= (tmp1 > tmp2);
            break;
        case CRIT_OP_LEA:
            tmp &= (tmp1 < tmp2);
            break;
        case CRIT_OP_NOT:
            tmp &= (tmp1 != tmp2);
            break;
        }

        if (!tmp) break;

        NextMList(lst);
    }
    return tmp;
}

bool examine_criterias(puzzlenode *nod)
{
    if (nod->CritList->count == 0)
        return true;

    StartMList(nod->CritList);

    while (!eofMList(nod->CritList))
    {
        MList *criteries=(MList *)DataMList(nod->CritList);

        if (ProcessCriteries(criteries))
            return true;

        NextMList(nod->CritList);
    }
    return false;
}

int execute_puzzle_node(puzzlenode *nod)
{
    StartMList(nod->ResList);
    while (!eofMList(nod->ResList))
    {
        func_node *fun=(func_node *)DataMList(nod->ResList);
        if (fun->func(fun->param, fun->slot , nod->owner) == ACTION_BREAK)
            {
                return ACTION_BREAK;
                break;
            }

        NextMList(nod->ResList);
    }
    return ACTION_NORMAL;
}

int Puzzle_try_exec(puzzlenode *pzlnod) //, pzllst *owner)
{
    if (ScrSys_GetFlag(pzlnod->slot) & FLAG_DISABLED)
        return ACTION_NORMAL;

    if (GetgVarInt(pzlnod->slot) != 1)
    {
        if (pzlnod->owner->exec_times == 0 )
            if (!(ScrSys_GetFlag(pzlnod->slot) & FLAG_DO_ME_NOW))
                return ACTION_NORMAL;
        if (examine_criterias(pzlnod))
        {

#ifdef TRACE
            printf("Puzzle: %d (%s) \n",pzlnod->slot,ScrSys_ReturnListName(pzlnod->owner));
#endif

            SetgVarInt(pzlnod->slot, 1);

            return execute_puzzle_node(pzlnod);
        }
    }

    return ACTION_NORMAL;
}


