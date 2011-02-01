#include <stdlib.h>

#include "mylist.h"




//Creates single linked-list object
MList *CreateMList()
{
    MList *tmp;
    tmp = new (MList);

    tmp->CurNode    = NULL;
    tmp->Head       = NULL;
    tmp->Tail       = NULL;
    tmp->count      = 0;
    tmp->indx       = 0;
    tmp->stkpos     = 0;
    return tmp;
}

//Adds item to linked-list
MList_node * AddToMList(MList *lst, void *item)
{
    MList_node *tmp = new (MList_node);
    tmp->data = item;
    tmp->next = NULL;
    tmp->prev = lst->Tail;
    tmp->idx = lst->indx;
    if (lst->count == 0)
    {
        lst->Head=tmp;
        lst->CurNode=tmp;
        lst->Tail=tmp;

    }
    else
    {
        lst->Tail->next=tmp;
        lst->Tail=tmp;
    }

    lst->count++;
    lst->indx++;

    return tmp;
}

//Go to the first linked-list item
void StartMList(MList *lst)
{
    lst->CurNode = lst->Head;
}

void LastMList(MList *lst)
{
    lst->CurNode = lst->Tail;
}

//Go to next linked-list item
//returns false if next item not exist
bool NextSMList(MList *lst)
{
    if (lst->CurNode->next)
    {
        lst->CurNode=lst->CurNode->next;
        return true;
    }
    else
        return false;
}

//Go to next linked-list item
//returns false if next item not exist
bool PrevSMList(MList *lst)
{
    if (lst->CurNode->prev)
    {
        lst->CurNode=lst->CurNode->prev;
        return true;
    }
    else
        return false;
}

//Go to next linked-list item without checking of item exist's
void NextMList(MList *lst)
{
    if (lst->CurNode)
    lst->CurNode=lst->CurNode->next;
}

//Go to prev linked-list item without checking of item exist's
void PrevMList(MList *lst)
{
    if (lst->CurNode)
    lst->CurNode=lst->CurNode->prev;
}

//Go to element index
bool ToIndxMList(MList *lst, unsigned int indx)
{
    if (lst->indx > indx)
    {
        lst->CurNode = lst->Head;
        while(lst->CurNode->next != NULL &&\
              lst->CurNode->idx  != indx &&\
              lst->CurNode->idx   < indx )
            lst->CurNode = lst->CurNode->next;

        if (lst->CurNode->idx == indx)
            return true;
        else
            return false;
    }
    else
        return false;
}

//Get data of element
void *DataMList(MList *lst)
{
    return lst->CurNode->data;
}

//Delete list object and delete all nodes assigned to list
void DeleteMList(MList *lst)
{
    if (lst->count > 0)
    {
    MList_node *nxt = lst->Head->next;
    lst->CurNode = lst->Head;
    while (lst->CurNode)
        {
            nxt = lst->CurNode->next;
            delete lst->CurNode;
            lst->CurNode = nxt;
        }
    }
    delete lst;
}

void DeleteCurrent(MList *lst)
{
    if (lst->CurNode->next)
        lst->CurNode->next->prev = lst->CurNode->prev;

    if (lst->CurNode->prev)
        lst->CurNode->prev->next = lst->CurNode->next;

    if (lst->CurNode == lst->Tail)
        lst->Tail = lst->CurNode->prev;

    if (lst->CurNode == lst->Head)
        lst->Head = lst->CurNode->next;

    MList_node *nod;

    if (lst->CurNode->prev)
        nod = lst->CurNode->prev;
    else
        nod = lst->Head;

    delete lst->CurNode;

    lst->stkpos  = 0; //Clean Stack!
    lst->CurNode = nod;
    lst->count--;

    if (lst->count==0)
        {
        lst->CurNode=NULL;
        lst->Head=NULL;
        lst->Tail=NULL;
        }
}

bool pushMList(MList *lst)
{
    if (lst->stkpos >= MLIST_STACK)
        return false;

    lst->Stack[lst->stkpos] = lst->CurNode;
    lst->stkpos++;

    return true;
}

bool popMList(MList *lst)
{
    if (lst->stkpos <= 0)
        return false;

    lst->stkpos--;
    lst->CurNode = lst->Stack[lst->stkpos];

    return true;
}

//Return true on EOF of list
bool eofMList(MList *lst)
{
    return (lst->CurNode == NULL);
}
