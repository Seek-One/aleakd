//
// Created by ebeuque on 18/02/2021.
//

#ifndef LIB_ALEAKD_H
#define LIB_ALEAKD_H

#include <stdlib.h>

void aleakd_enable_print_action(int bEnable); // Default 1

// All thread
void aleakd_all_threads_start();
void aleakd_all_threads_stop();
void aleakd_all_threads_print_leaks(int bDetail);
int aleakd_all_threads_count();

// Current thread
void aleakd_current_thread_set_name(const char* name);
void aleakd_current_thread_start();
void aleakd_current_thread_stop();
void aleakd_current_thread_print_leaks(int bDetail);

// Thread access
void aleakd_init(int idx);
void aleakd_set_current_thread(int idx, const char* name);
void aleakd_reset(int idx);
void aleakd_start(int idx);
void aleakd_stop(int idx);
void aleakd_thread_print_leaks(int idx, int bDetail);
int aleakd_thread_alloc_count(int idx);
size_t aleakd_thread_memory_usage(int idx);
size_t aleakd_thread_memory_usage_max(int idx);

// Debug
void aleakd_set_break_alloc_num(unsigned long num);

#endif // LIB_ALEAKD_H
