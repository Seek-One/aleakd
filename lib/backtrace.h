//
// Created by ebeuque on 23/03/2021.
//

#ifndef ALEAKD_BACKTRACE_H
#define ALEAKD_BACKTRACE_H

#include "../config.h"

#include <execinfo.h>

void backtrace_init();
void backtrace_get_infos(void* addr, const char** pfname, void** pfbase, const char** psfname, void** psaddr);
int backtrace_check_addr(void* addr);
void backtrace_print();
void backtrace_print2();

#endif //ALEAKD_BACKTRACE_H
