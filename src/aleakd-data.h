#ifndef ALEAKD_DATA
#define ALEAKD_DATA

#include "../config.h"

#include "alloc-entry.h"
#include "thread-entry.h"

void aleakd_data_set_enable_print_action(int bEnable);
int aleakd_data_get_enable_print_action();

void aleakd_data_init_thread_list();
struct ThreadEntryList* aleakd_data_get_thread_list();
struct ThreadEntry* aleakd_data_get_thread(int idx);

struct AllocEntry* aleakd_data_get_alloc_list(int idx);

void aleakd_data_incr_alloc_number();
unsigned long aleakd_data_get_alloc_number();

void aleakd_data_set_break_alloc_num(unsigned long num);
unsigned long aleakd_data_get_break_alloc_num();

void aleakd_data_set_display_min_alloc_num(unsigned long num);
unsigned long aleakd_data_get_display_min_alloc_num();

#endif // ALEAKD_DATA
