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

#include "proc_msg_zmq.h"

ProcMsgZMQ::ProcMsgZMQ() {

}

ProcMsgZMQ::~ProcMsgZMQ() {

}

void ProcMsgZMQ::handler() {
    int size = 0;
    int type = 0;
    char *data = NULL;
    while (1) {
        size = 0;
        type = 0;
        data = this->recv(&type, &size);
        this->handler_parser(type, data, size);
        usleep(200);
    }
}

void ProcMsgZMQ::handler_start(int) {
    this->handler_start();
}

void ProcMsgZMQ::handler_start() {
    this->thread_handler = thread(&ProcMsgZMQ::handler, this);
}

void ProcMsgZMQ::handler_stop() {
    this->thread_handler.detach();
}

void ProcMsgZMQ::handler_wait() {
    this->thread_handler.join();
}

bool ProcMsgZMQ::create(char *addr) { // "tcp://127.0.0.1:5555"
    do {
        this->context = zmq_ctx_new();
        if (this->context == NULL) {
            break;
        }

        this->responder = zmq_socket(this->context, ZMQ_PULL);
        if (this->responder == NULL) {
            break;
        }

        if (zmq_bind(this->responder, addr) != 0) {
            break;
        }

        this->conn_on = 1;
        
        return true;
    } while (0);

    this->conn_on = 0;
    
    return false;
}

bool ProcMsgZMQ::connect(char *addr) { // "tcp://127.0.0.1:5555"
    do {
        this->context = zmq_ctx_new();
        if (this->context == NULL) {
            break;
        }

        this->responder = zmq_socket(this->context, ZMQ_PUSH);
        if (this->responder == NULL) {
            break;
        }

        if (zmq_connect(this->responder, addr) != 0) {
            break;
        }

        this->conn_on = 1;
        
        return true;
    } while (0);

    this->conn_on = 0;
    
    return false;
}

void ProcMsgZMQ::close() {
    zmq_close(this->responder);
    zmq_ctx_destroy(this->context);
}

int ProcMsgZMQ::send(long type, char *msg) {
    return this->send(type, msg, strlen(msg));
}

int ProcMsgZMQ::send(long type, char *msg, int msg_size) {
    proc_msg_s _pmsg;

    _pmsg.type = type;
    memset(_pmsg.text, 0, PROC_MSG_MESSAGE_SIZE); // Заполняем строку нулями.
    if (msg_size <= PROC_MSG_MESSAGE_SIZE) {
        strncpy(_pmsg.text, msg, msg_size);
    } else {
        strncpy(_pmsg.text, msg, PROC_MSG_MESSAGE_SIZE);
    }
    
    return zmq_send(this->responder, &_pmsg, sizeof (_pmsg), 0);
}

char *ProcMsgZMQ::recv(int *msg_type, int *msg_size) {
    int ret = zmq_recv(this->responder, &this->pmsg, sizeof (this->pmsg), 0);
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

/*
int main() {

    ProcMsgZMQ Pmsg_s;
    ProcMsgZMQ Pmsg_c;

    cout << "create  " << Pmsg_s.create((char *) "tcp://127.0.0.1:5555") << endl;
    puts("handler_start");
    Pmsg_s.handler_start();
    
    
    cout << "connect " << Pmsg_c.connect((char *) "tcp://127.0.0.1:5556") << endl;

    sleep(3);

    cout << "send: " << Pmsg_c.send(PROC_MSG_DATA_CLIENT_STR, (char *) "Hello world22", strlen((char *) "Hello world22")) << endl;
    
    cout << "send: " << Pmsg_c.send(PROC_MSG_DATA_CLIENT_STR, (char *) "Hello world", strlen((char *) "Hello world")) << endl;
    
    puts("sleep 5");
    sleep(5);

    cout << "send: " << Pmsg_c.send(PROC_MSG_DATA_CLIENT_STR, (char *) "Hello world22", strlen((char *) "Hello world22")) << endl;
    
    Pmsg_s.handler_wait();

    return 0;
}
*/