#ifndef ALEAKD_DATA
#define ALEAKD_DATA

#include "../config.h"

#include "alloc-entry.h"
#include "thread-entry.h"

void aleakd_data_set_enable_print(int bEnable);
int aleakd_data_get_enable_print();

void aleakd_data_init_thread_list();
struct ThreadEntryList* aleakd_data_get_thread_list();
struct ThreadEntry* aleakd_data_get_thread(int idx);

struct AllocEntry* aleakd_data_get_alloc_list(int idx);

void aleakd_data_incr_alloc_number();
int aleakd_data_get_alloc_number();

#endif // ALEAKD_DATA
