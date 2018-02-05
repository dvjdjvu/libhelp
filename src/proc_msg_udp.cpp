/* 
 * File:   proc_msg_udp.cpp
 * Author: root
 * 
 * Created on 31 января 2018 г., 18:00
 */

#include "proc_msg_udp.h"

ProcMsgUDP::ProcMsgUDP() {
}

ProcMsgUDP::~ProcMsgUDP() {

}

void ProcMsgUDP::handler() {

    int size = 0;
    int type = 0;
    char *data = NULL;
    while (1) {
        size = 0;
        type = 0;
        data = this->recv(&type, &size);
        this->handler_parser(type, data, size);
        //usleep(200);
    }
}

void ProcMsgUDP::handler_start(long _msgtype_get) {
    this->msgtype_get = _msgtype_get;
    this->thread_handler = thread(&ProcMsgUDP::handler, this);
}

void ProcMsgUDP::handler_stop() {
    this->thread_handler.detach();
}

void ProcMsgUDP::handler_wait() {
    this->thread_handler.join();
}

bool ProcMsgUDP::connect(char *ip, int port) {

    this->type = PROC_MSG_TYPE_CLIENT;

    // Создаем сокет.
    this->sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->sd == -1) {
        return false;
    }

    memset((char *) &this->master_isa, 0, sizeof (this->master_isa));

    this->master_isa.sin_family = AF_INET;
    this->master_isa.sin_port = htons(port);
    if (inet_aton(ip, &this->master_isa.sin_addr) == 0) {
        return false;
    }

    return true;
}

bool ProcMsgUDP::create(char *ip, int port) {

    this->type = PROC_MSG_TYPE_SERVER;

    // Создаем сокет.
    this->sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->sd == -1) {
        return false;
    }

    memset((char *) &this->master_isa, 0, sizeof (this->master_isa));

    this->master_isa.sin_family = AF_INET;
    this->master_isa.sin_port = htons(port);
    if (inet_aton(ip, &this->master_isa.sin_addr) == 0) {
        return false;
    }

    //bind socket to port
    if (bind(this->sd, (struct sockaddr*) &this->master_isa, sizeof (this->master_isa)) == -1) {
        return false;
    }

    return true;
}

int ProcMsgUDP::send(long type, char *msg) {
    int ret = this->send(type, msg, strlen(msg));
    
    printf("ret = %d \n", ret);
    
    return ret;
    
    //return this->send(type, msg, strlen(msg));
}

int ProcMsgUDP::send(long type, char *msg, int msg_size) {
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

    if (this->type == PROC_MSG_TYPE_SERVER) {
        return sendto(this->sd, &_pmsg, sizeof (_pmsg), 0, (struct sockaddr *) &this->slave_isa, sizeof (this->slave_isa));
    } else {
        return sendto(this->sd, &_pmsg, sizeof (_pmsg), 0, (struct sockaddr *) &this->master_isa, sizeof (this->master_isa));
    }

}

char *ProcMsgUDP::recv(int *msg_type, int *msg_size) {
    int ret;

    socklen_t addr_len = sizeof (this->slave_isa);

    if (this->type == PROC_MSG_TYPE_SERVER) {
        ret = recvfrom(this->sd, &this->pmsg, sizeof (this->pmsg), 0, (struct sockaddr *) &this->slave_isa, (socklen_t *) & addr_len);
    } else {
        ret = recvfrom(this->sd, &this->pmsg, sizeof (this->pmsg), 0, (struct sockaddr *) &this->master_isa, (socklen_t *) & addr_len);
    }


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

void ProcMsgUDP::close() {
    ::close(this->sd);
}

/*
int main() {

    ProcMsgUDP Pmsg_s;
    ProcMsgUDP Pmsg_c;

    cout << "create  " << Pmsg_s.create((char *) "127.0.0.1", 9090) << endl;
    cout << "connect " << Pmsg_c.connect((char *) "127.0.0.1", 9090) << endl;


    sleep(2);
    puts("handler_start");
    Pmsg_s.handler_start(PROC_MSG_DATA_CLIENT_JSON);
    sleep(3);

    cout << "send: " << Pmsg_c.send(PROC_MSG_DATA_CLIENT_STR, (char *) "Hello world", strlen((char *) "Hello world")) << endl;
    perror("send");

    puts("sleep 5");
    sleep(5);

    Pmsg_s.handler_wait();

    return 0;
}
*/
