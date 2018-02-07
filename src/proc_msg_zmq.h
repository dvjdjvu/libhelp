/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   proc_msg_zmq.h
 * Author: root
 *
 * Created on 6 февраля 2018 г., 14:23
 */

#ifndef PROC_MSG_ZMQ_H
#define PROC_MSG_ZMQ_H

#include <zmq.h>
#include "proc_msg.h"

class ProcMsgZMQ : public ProcMsg {
public:
    ProcMsgZMQ();
    ~ProcMsgZMQ();
    
    void handler();
    void handler_start(int);
    void handler_start();
    void handler_stop();
    void handler_wait();
    
    bool create(char *addr);
    bool connect(char *addr);
    void close();
    
    int send(long type, char *msg);
    int send(long type, char *msg, int msg_size);
    char *recv(int *msg_type, int *msg_size);
    
    virtual void handler_parser(long type, char *data, int size) {
        if (data == NULL) {
            //puts("data == NULL");
            return;
        }
        
        cout << "data " << data << endl;
        //cout << type << endl;
        printf("type = %x\n", type);
        cout << "size " << size << endl;
    }
    
    
private:
    void *context;
    void *responder;
    
    thread thread_handler;
    mutex  mutex_read;
};

#endif /* PROC_MSG_ZMQ_H */

