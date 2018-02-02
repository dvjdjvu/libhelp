/* 
 * File:   proc_msg.cpp
 * Author: root
 * 
 * Created on 2 февраля 2018 г., 12:46
 */

#include "proc_msg.h"

ProcMsg::ProcMsg() {
}

ProcMsg::~ProcMsg() {
}

void ProcMsg::perror() {
    printf("%s\n", strerror(errno));
}

void ProcMsg::perror(const char *s) {
    printf("%s: %s\n", s, strerror(errno));
}

