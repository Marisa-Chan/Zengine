
#include "System.h"
#include "anims.h"


MList  *preload =NULL;


MList *anim_getpreloadLst()
{
    return preload;
}

void anim_InitPreloadList()
{
    if (preload)
        DeleteMList(preload);

    preload = CreateMList();
}

