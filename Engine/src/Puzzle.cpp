
#include "System.h"


pzllst *CreatePzlLst()
{
    pzllst *tmp     = new (pzllst);
    tmp->_list      = CreateMList();
    tmp->stksize    = 0;
    tmp->exec_times = 0;
    return tmp;
}


void Parse_Puzzle_Results_Action(char *instr, MList *lst)
{
    char *str;
    char buf[255];
    bool HaveDts=false;

    func_node *nod;
    char *params;

    str = instr;

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
                HaveDts=true;
            end_s=i;
            break;
        }
    }

    if (strCMP(buf,"set_screen")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_set_screen;
        return;
    }

    if (strCMP(buf,"debug")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_debug;
        return;
    }

    if (strCMP(buf,"assign")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_assign;
        return;
    }
    if (strCMP(buf,"timer")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        char buff[255];
        int tmp1=0;

        sscanf(str+end_s+1,"%d",&tmp1);

        sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

        nod->param=(char *)malloc(strlen(buff)+1);
        strcpy(nod->param,buff);

        nod->func=action_timer;
        return;
    }
    if (strCMP(buf,"set_partial_screen")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_set_partial_screen;
        return;
    }

    if (strCMP(buf,"change_location")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_change_location;
        return;
    }

    if (strCMP(buf,"dissolve")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        //params=GetParams(str+end_s);
        nod->param=NULL;
        //strcpy(nod->param,params);

        nod->func=action_dissolve; //make save prev W R VI
        return;
    }

    if (strCMP(buf,"disable_control")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_disable_control;
        return;
    }
    if (strCMP(buf,"enable_control")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_enable_control;
        return;
    }
    if (strCMP(buf,"add")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_add;
        return;
    }
    if (strCMP(buf,"random")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        char buff[255];
        int tmp1=0;

        sscanf(str+end_s+1,"%d",&tmp1);

        sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

        nod->param=(char *)malloc(strlen(buff)+1);
        strcpy(nod->param,buff);

        nod->func=action_random;
        return;
    }
    if (strCMP(buf,"animplay")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        char buff[255];
        int tmp1=0;

        sscanf(str+end_s+1,"%d",&tmp1);

        //SetgVarInt(tmp1,0); //hack ?

        sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

        nod->param=(char *)malloc(strlen(buff)+1);
        strcpy(nod->param,buff);

        nod->func=action_animplay;
        return;
    }
    if (strCMP(buf,"universe_music")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        char buff[255];
        int tmp1=0;

        sscanf(str+end_s+1,"%d",&tmp1);

        sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

        nod->param=(char *)malloc(strlen(buff)+1);
        strcpy(nod->param,buff);

        nod->func=action_universe_music;
        return;
    }
    if (strCMP(buf,"music")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        char buff[255];
        int tmp1=0;

        sscanf(str+end_s+1,"%d",&tmp1);

        sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

        nod->param=(char *)malloc(strlen(buff)+1);
        strcpy(nod->param,buff);

        nod->func=action_universe_music;
        return;
    }

    if (strCMP(buf,"kill")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_kill;
        return;
    }

    if (strCMP(buf,"stop")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_stop;
        return;
    }

    if (strCMP(buf,"inventory")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_inventory;
        return;
    }

    if (strCMP(buf,"crossfade")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_crossfade;
        return;
    }

    if (strCMP(buf,"streamvideo")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_streamvideo;
        return;
    }

    if (strCMP(buf,"animpreload")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        char buff[255];
//        int tmp1=0;

        sprintf(buff,"%d %s",atoi(str+end_s+1),GetParams(str+end_s+1));

        nod->param=(char *)malloc(strlen(buff)+1);
        strcpy(nod->param,buff);

        nod->func=action_animpreload;
        return;
    }
    if (strCMP(buf,"playpreload")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        char buff[255];

        if (HaveDts)
            sprintf(buff,"%d %s",atoi(str+end_s+1),GetParams(str+end_s+1));
        else
            sprintf(buff,"%d %s",0,GetParams(str+end_s));


        //sprintf(buff,"%d %s",atoi(str+end_s+1),GetParams(str+end_s+1));

        nod->param=(char *)malloc(strlen(buff)+1);
        strcpy(nod->param,buff);

        nod->func=action_playpreload;
        return;
    }
    if (strCMP(buf,"syncsound")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        params=GetParams(str+end_s);
        nod->param=(char *)malloc(strlen(params)+1);
        strcpy(nod->param,params);

        nod->func=action_syncsound;
        return;
    }

    if (strCMP(buf,"ttytext")==0)
    {
        nod=new(func_node);
        AddToMList(lst,nod);

        char buff[255];
        int tmp1=0;

        sscanf(str+end_s+1,"%d",&tmp1);

        sprintf(buff,"%d %s",tmp1,GetParams(str+end_s+1));

        nod->param=(char *)malloc(strlen(buff)+1);
        strcpy(nod->param,buff);

        nod->func=action_ttytext;
        return;
    }
}

int Parse_Puzzle_Flags(puzzlenode *pzl,FILE *fl)
{
    int  good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
        str=PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"once_per_inst")==0)
        {
            pzl->flags |= FLAG_ONCE_PER_I;
        }
        else if (strCMP(str,"do_me_now")==0)
        {
            pzl->flags |= FLAG_DO_ME_NOW;
        }
        else if (strCMP(str,"disabled")==0)
        {
            pzl->flags |= FLAG_DISABLED;
        }
    }

    return good;
}

int Parse_Puzzle_Criteria(puzzlenode *pzl,FILE *fl)
{
    int  good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    MList *crit_nodes_lst=CreateMList();

    AddToMList(pzl->CritList,crit_nodes_lst);

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
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
            for (ij=0; ij<strlen(str); ij++)
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


            for (ij++; ij<strlen(str); ij++)
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

int Parse_Puzzle_Results(puzzlenode *pzl,FILE *fl)
{
    int  good = 0;
    char buf[FILE_LN_BUF];
    char *str;

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
        str=PrepareString(buf);

        if (str[0] == '}')
        {
            good = 1;
            break;
        }
        else if (strCMP(str,"action:") == 0)
        {
            Parse_Puzzle_Results_Action(str+7,pzl->ResList);
        }

    }
    return good;
}

int Parse_Puzzle(pzllst *lst,FILE *fl,char *ctstr)
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
    pzl->flags    = 0;

    while (!feof(fl))
    {
        fgets(buf,FILE_LN_BUF,fl);
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

    if ((pzl->flags & FLAG_ONCE_PER_I ))
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

bool examine_criterias(puzzlenode *nod)
{
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
        if (fun->func(fun->param, nod->owner) == ACTION_BREAK)
            {
                return ACTION_BREAK;
                break;
            }

        NextMList(nod->ResList);
    }
}

int Puzzle_try_exec(puzzlenode *pzlnod) //, pzllst *owner)
{
    if (pzlnod->flags & FLAG_DISABLED)
        return ACTION_NORMAL;

    if (GetgVarInt(pzlnod->slot) != 1)
    {
        if (pzlnod->owner->exec_times == 0)
            if (! pzlnod->flags & FLAG_DO_ME_NOW)
                return ACTION_NORMAL;
        if (examine_criterias(pzlnod))
        {

#ifdef TRACE
            printf("Puzzle: %d (%s) \n",pzlnod->slot,ReturnListName(pzlnod->owner));
#endif

            SetgVarInt(pzlnod->slot, 1);

            return execute_puzzle_node(pzlnod);
        }
    }
}

