//
// Created by ebeuque on 18/02/2021.
//

#ifndef LIB_ALEAKD_H
#define LIB_ALEAKD_H

void aleakd_allow_print();
void aleakd_init(int idx);
void aleakd_set_current_thread(int idx, const char* name);
void aleakd_set_current_thread_name(const char* name);
void aleakd_reset(int idx);
void aleakd_start(int idx);
void aleakd_stop(int idx);
void aleakd_print_leaks(int idx);

#endif // LIB_ALEAKD_H
