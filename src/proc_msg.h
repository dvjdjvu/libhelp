/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   proc_msg.h
 * Author: root
 *
 * Created on 2 февраля 2018 г., 12:46
 */

#ifndef PROC_MSG_H
#define PROC_MSG_H

#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <thread>
#include <mutex>

#include <iostream>

using namespace std;

#define PROC_MSG_TYPE_SERVER 0x0010
#define PROC_MSG_TYPE_CLIENT 0x0011

#define PROC_MSG_MESSAGE_SIZE 1024

#define PROC_MSG_DATA_SERVER_JSON     0x1000
#define PROC_MSG_DATA_SERVER_STR      0x1001
#define PROC_MSG_DATA_SERVER_ARRAY    0x1002

#define PROC_MSG_DATA_CLIENT_JSON     0x2000
#define PROC_MSG_DATA_CLIENT_STR      0x2001
#define PROC_MSG_DATA_CLIENT_ARRAY    0x2002

class ProcMsg {
public:

    int conn_on = 0;
    
    ProcMsg();
    ~ProcMsg();

    struct proc_msg_s {
        long type; // Тип передаваемых данных.
        time_t time_send;
        char text[PROC_MSG_MESSAGE_SIZE]; // Данные. 
    };

    proc_msg_s pmsg;

    /**
     * @brief Тип запрашиваемых сообщений.
     */
    long msgtype_get = 0;
    
    /**
     * @brief Печать ошибки @ref errno
     */
    void perror();

    /**
     * @brief Печать ошибки @ref errno
     */
    void perror(const char *s);
};

#endif /* PROC_PROC_MSG_H */

