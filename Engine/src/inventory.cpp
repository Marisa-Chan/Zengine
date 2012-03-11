#include "System.h"

int32_t inv_get_cnt()
{
    return GetgVarInt(SLOT_INV_STORAGE_S1);
}

void inv_set_cnt(int32_t cnt)
{
    SetgVarInt(SLOT_INV_STORAGE_S1,cnt);
}

int32_t inv_get_item(int32_t index)
{
    if (index < 49 && index >= 0)
        return GetgVarInt(SLOT_INV_STORAGE_0 + index);
    return -1;
}

void inv_set_item(int32_t index, int32_t item)
{
    if (index < 49 && index >= 0)
        SetgVarInt(SLOT_INV_STORAGE_0 + index,item);
}


void inv_drop(int item)
{

    int32_t items_cnt = inv_get_cnt();

    //if items in inventory > 0
    if ( items_cnt != 0)
    {
        int32_t index = 0;

        //finding needed item
        while (index < items_cnt)
        {
            if ( inv_get_item(index) == item )
                break;

            index++;
        }

        // if item in the inventory
        if ( items_cnt !=index )
        {
            //shift all items left with rewrite founded item
            for (int32_t v = index; v < items_cnt-1 ; v++)
                inv_set_item(v, inv_get_item(v+1));

            //del last item
            inv_set_item(items_cnt-1, 0);
            inv_set_cnt(inv_get_cnt() - 1);

            SetgVarInt(SLOT_INVENTORY_MOUSE, inv_get_item(0));
        }
    }
}

void inv_add_put(int item)
{
    int32_t cnt = inv_get_cnt();

    for ( int32_t i = cnt; i > 0; --i )
        inv_set_item(i, inv_get_item(i-1));

    inv_set_item(0,item);

    SetgVarInt(SLOT_INVENTORY_MOUSE, item);

    inv_set_cnt(inv_get_cnt() + 1);
}

void inv_add(int item)
{
    int32_t cnt = inv_get_cnt();

    if (cnt < 49)
    {


        int8_t flag = 1;

        if (cnt == 0)
        {
            inv_set_item(0,0);
            inv_set_cnt(1); //we needed empty item for cycle code
            cnt = 1;
        }

        for(int32_t cur = 0; cur < cnt; cur++)
            if (inv_get_item(cur) == item)
            {
                flag = 0;
                break;
            }


        if (flag)
        {
            for ( int32_t i = cnt; i > 0; --i )
                inv_set_item(i, inv_get_item(i-1));

            inv_set_item(0,item);

            SetgVarInt(SLOT_INVENTORY_MOUSE, item);

            inv_set_cnt(cnt + 1);
        }

    }
}

void inv_cycle()
{
    int32_t item_cnt = inv_get_cnt();
    int32_t cur_item = inv_get_item(0);
    if ( item_cnt > 1 )
    {
        for ( int32_t i = 0; i < item_cnt-1; i++)
            inv_set_item(i, inv_get_item(i+1));

        inv_set_item(item_cnt - 1, cur_item);

        SetgVarInt(SLOT_INVENTORY_MOUSE,inv_get_item(0));

    }
}
