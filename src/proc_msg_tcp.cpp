/* 
 * File:   proc_msg_tcp.cpp
 * Author: root
 * 
 * Created on 5 февраля 2018 г., 15:15
 * 
 * g++ proc_msg.cpp proc_msg_tcp.cpp -pthread -std=c++11
 */

#include "proc_msg_tcp.h"

ProcMsgTCP::ProcMsgTCP() {
}

ProcMsgTCP::~ProcMsgTCP() {

}

void ProcMsgTCP::handler() {
    int size = 0;
    int type = 0;
    char *data = NULL;
    while (1) {

        if (this->type == PROC_MSG_TYPE_SERVER) {
            this->client_sd = accept(this->sd, NULL, NULL);

            if (this->client_sd < 0) {
                continue;
            }
            puts("new connect");
            while (1) {
                size = 0;
                type = 0;
                data = this->recv(&type, &size);
                puts("recv");
                if (data == NULL) {
                    puts("data == NULL");
                    break;
                }

                this->handler_parser(type, data, size);
            }

            ::close(this->client_sd);
        } else {
            while (1) {
                size = 0;
                type = 0;
                data = this->recv(&type, &size);
                if (data == NULL) {
                    puts("data == NULL");
                    break;
                }

                this->handler_parser(type, data, size);
            }
        }
    }
}

void ProcMsgTCP::handler_start(long _msgtype_get) {
    this->msgtype_get = _msgtype_get;
    this->thread_handler = thread(&ProcMsgTCP::handler, this);
}

void ProcMsgTCP::handler_stop() {
    this->thread_handler.detach();
}

void ProcMsgTCP::handler_wait() {
    this->thread_handler.join();
}

bool ProcMsgTCP::connect(char *ip, int port) {

    this->type = PROC_MSG_TYPE_CLIENT;

    // Создаем сокет.
    this->sd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sd == -1) {
        return false;
    }

    memset((char *) &this->master_isa, 0, sizeof (this->master_isa));

    this->master_isa.sin_family = AF_INET;
    this->master_isa.sin_port = htons(port);
    if (inet_aton(ip, &this->master_isa.sin_addr) == 0) {
        return false;
    }

    if (::connect(this->sd, (struct sockaddr *) &this->master_isa, sizeof (this->master_isa)) < 0) {
        return false;
    }

    return true;
}

bool ProcMsgTCP::create(char *ip, int port) {

    this->type = PROC_MSG_TYPE_SERVER;

    // Создаем сокет.
    this->sd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sd == -1) {
        return false;
    }

    memset((char *) &this->master_isa, 0, sizeof (this->master_isa));

    this->master_isa.sin_family = AF_INET;
    this->master_isa.sin_port = htons(port);
    if (inet_aton(ip, &this->master_isa.sin_addr) == 0) {
        return false;
    }

    // Bind socket to address
    if (bind(this->sd, (struct sockaddr *) &this->master_isa, sizeof (this->master_isa)) == -1) {
        return false;
    }

    // Start listing on the socket
    if (listen(this->sd, 1) == -1) {
        return false;
    }

    return true;
}

int ProcMsgTCP::send(long type, char *msg) {
    int ret = this->send(type, msg, strlen(msg));

    printf("ret = %d \n", ret);

    return ret;

    //return this->send(type, msg, strlen(msg));
}

int ProcMsgTCP::send(long type, char *msg, int msg_size) {
    proc_msg_s _pmsg;

    _pmsg.type = type;
    memset(_pmsg.text, 0, PROC_MSG_MESSAGE_SIZE); // Заполняем строку нулями.
    if (msg_size <= PROC_MSG_MESSAGE_SIZE) {
        strncpy(_pmsg.text, msg, msg_size);
    } else {
        strncpy(_pmsg.text, msg, PROC_MSG_MESSAGE_SIZE);
    }

    //cout << "msg " << msg << endl;
    //cout << type << endl;
    //printf("type = %x\n", type);
    //cout << "msg_size " << msg_size << endl;

    //if (this->type == PROC_MSG_TYPE_SERVER) {
    return ::send(this->sd, &_pmsg, sizeof (_pmsg), 0);
    //} else {
    //    return send(this->sd, &_pmsg, sizeof (_pmsg), 0);
    //}

}

char *ProcMsgTCP::recv(int *msg_type, int *msg_size) {
    int ret;

    socklen_t addr_len = sizeof (this->slave_isa);

    if (this->type == PROC_MSG_TYPE_SERVER) {
        ret = ::recv(this->client_sd, &this->pmsg, sizeof (this->pmsg), 0);
    } else {
        ret = ::recv(this->sd, &this->pmsg, sizeof (this->pmsg), 0);
    }

    printf("ret = %d\n", ret);
    this->perror("recv");
    if (ret <= 0) {
        return NULL;
    }

    if (time(NULL) - this->pmsg.time_send > 1) {
        return NULL;
    }

    *msg_size = ret;
    *msg_type = this->pmsg.type;

    return &this->pmsg.text[0];
}

void ProcMsgTCP::close() {
    ::close(this->sd);
}

/*
int main() {

    ProcMsgTCP Pmsg_s;
    ProcMsgTCP Pmsg_c;

    cout << "create  " << Pmsg_s.create((char *) "127.0.0.1", 9090) << endl;
    puts("handler_start");
    Pmsg_s.handler_start(PROC_MSG_DATA_CLIENT_JSON);
    
    
    cout << "connect " << Pmsg_c.connect((char *) "127.0.0.1", 9090) << endl;


    sleep(2);
    sleep(3);

    cout << "send: " << Pmsg_c.send(PROC_MSG_DATA_CLIENT_STR, (char *) "Hello world", strlen((char *) "Hello world")) << endl;
    perror("send");

    puts("sleep 5");
    sleep(5);

    cout << "send: " << Pmsg_c.send(PROC_MSG_DATA_CLIENT_STR, (char *) "Hello world22", strlen((char *) "Hello world22")) << endl;
    
    Pmsg_s.handler_wait();

    return 0;
}
 */
