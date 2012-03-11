#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED

int32_t inv_get_cnt();
void inv_set_cnt(int32_t cnt);

int32_t inv_get_item(int32_t index);
void inv_set_item(int32_t index, int32_t item);

void inv_drop(int item);
void inv_add(int item);
void inv_cycle();

#endif // INVENTORY_H_INCLUDED
