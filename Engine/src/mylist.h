#ifndef MYLIST_H_INCLUDED
#define MYLIST_H_INCLUDED

//node structure
struct MList_node
{
    MList_node  *next;      //pointer to next node
    MList_node  *prev;      //pointer to next node
    void        *data;      //pointer to data
    unsigned int idx;
};

//List structure
struct MList
{
    MList_node  *CurNode;   //pointer to current node
    MList_node  *Head;      //pointer to first node
    MList_node  *Tail;      //pointer to last node
    unsigned int count;     //count of elements
    unsigned int indx;     //count of elements
};

//Linked-list functions
MList *CreateMList();
MList_node *AddToMList(MList *lst, void *item);
void StartMList(MList *lst);
void LastMList(MList *lst);
bool NextSMList(MList *lst);
void NextMList(MList *lst);
void PrevMList(MList *lst);
bool ToIndxMList(MList *lst, unsigned int indx);
void *DataMList(MList *lst);
void DeleteMList(MList *lst);
void DeleteCurrent(MList *lst);
bool eofMList(MList *lst);

#endif // MYLIST_H_INCLUDED
