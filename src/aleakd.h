//
// Created by ebeuque on 18/02/2021.
//

#ifndef LIB_ALEAKD_H
#define LIB_ALEAKD_H

void alkead_allow_print();
void alkead_init(int idx);
void alkead_set_current_thread(int idx, const char* name);
void alkead_set_current_thread_name(const char* name);
void alkead_reset(int idx);
void alkead_start(int idx);
void alkead_stop(int idx);
void alkead_print_leaks(int idx);

#endif // LIB_ALEAKD_H
