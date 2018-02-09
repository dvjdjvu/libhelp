/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   proc_msg_zmq.cpp
 * Author: root
 * 
 * Created on 6 февраля 2018 г., 14:23
 * 
 * g++ proc_msg.cpp proc_msg_zmq.cpp -pthread -std=c++11 -lzmq
 */

#include "proc_msg.h"

bool pm::pm_create_zmq(char *addr) { // "tcp://127.0.0.1:5555"
    do {
        this->zmq_context = zmq_ctx_new();
        if (this->zmq_context == NULL) {
            break;
        }

        this->zmq_responder = zmq_socket(this->zmq_context, ZMQ_PULL);
        if (this->zmq_responder == NULL) {
            break;
        }

        if (zmq_bind(this->zmq_responder, addr) != 0) {
            break;
        }
        
        return true;
    } while (0);
    
    return false;
}

bool pm::pm_connect_zmq(char *addr) { // "tcp://127.0.0.1:5555"
    do {
        this->zmq_context = zmq_ctx_new();
        if (this->zmq_context == NULL) {
            break;
        }

        this->zmq_responder = zmq_socket(this->zmq_context, ZMQ_PUSH);
        if (this->zmq_responder == NULL) {
            break;
        }

        if (zmq_connect(this->zmq_responder, addr) != 0) {
            break;
        }
        
        return true;
    } while (0);
    
    return false;
}

void pm::pm_close_zmq() {
    zmq_close(this->zmq_responder);
    zmq_ctx_destroy(this->zmq_context);
}

int pm::pm_send_zmq(long type, char *msg) {
    return this->pm_send_zmq(type, msg, strlen(msg));
}

int pm::pm_send_zmq(long type, char *msg, int msg_size) {
    proc_msg_s _pmsg;

    _pmsg.type = type;
    memset(_pmsg.text, 0, PM_MESSAGE_SIZE); // Заполняем строку нулями.
    if (msg_size <= PM_MESSAGE_SIZE) {
        strncpy(_pmsg.text, msg, msg_size);
    } else {
        strncpy(_pmsg.text, msg, PM_MESSAGE_SIZE);
    }
    
    return zmq_send(this->zmq_responder, &_pmsg, sizeof (_pmsg), 0);
}

char *pm::pm_recv_zmq(int *msg_type, int *msg_size) {
    int ret = zmq_recv(this->zmq_responder, &this->pmsg, sizeof (this->pmsg), 0);
    if (ret < 0) {
        return NULL;
    }

    if (time(NULL) - this->pmsg.time_send > 1) {
        return NULL;
    }

    *msg_size = ret;
    *msg_type = this->pmsg.type;

    return &this->pmsg.text[0];
}
