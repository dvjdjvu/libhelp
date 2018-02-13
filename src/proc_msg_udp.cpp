/* 
 * File:   proc_msg_udp.cpp
 * Author: root
 * 
 * Created on 31 января 2018 г., 18:00
 */

#include "proc_msg.h"

bool pm::pm_connect_udp(char *ip, int port) {

    this->type = PM_TYPE_CLIENT;

    // Создаем сокет.
    this->sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->sd == -1) {
        return false;
    }

    memset((char *) &this->server_isa, 0, sizeof (this->server_isa));

    this->server_isa.sin_family = AF_INET;
    this->server_isa.sin_port = htons(port);
    if (inet_aton(ip, &this->server_isa.sin_addr) == 0) {
        return false;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    //if (setsockopt(this->sd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    //    perror("Error");
    //    return false;
    //}
    
    return true;
}

bool pm::pm_create_udp(char *ip, int port) {

    this->type = PM_TYPE_SERVER;

    // Создаем сокет.
    this->sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->sd == -1) {
        return false;
    }

    memset((char *) &this->server_isa, 0, sizeof (this->server_isa));

    this->server_isa.sin_family = AF_INET;
    this->server_isa.sin_port = htons(port);
    if (inet_aton(ip, &this->server_isa.sin_addr) == 0) {
        return false;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    //if (setsockopt(this->sd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    //    return false;
    //}
    
    //bind socket to port
    if (bind(this->sd, (struct sockaddr*) &this->server_isa, sizeof (this->server_isa)) == -1) {
        return false;
    }

    return true;
}

int pm::pm_send_udp(long type, char *msg) {
    return this->pm_send_udp(type, msg, strlen(msg));
}

int pm::pm_send_udp(long type, char *msg, int msg_size) {
    proc_msg_s _pmsg;

    _pmsg.time_send = time(NULL);
    
    _pmsg.type = type;
    memset(_pmsg.text, 0, PM_MESSAGE_SIZE); // Заполняем строку нулями.
    if (msg_size <= PM_MESSAGE_SIZE) {
        strncpy(_pmsg.text, msg, msg_size);
    } else {
        strncpy(_pmsg.text, msg, PM_MESSAGE_SIZE);
    }

    //cout << "msg " << msg << endl;
    //cout << type << endl;
    //printf("type = %x\n", type);
    //cout << "msg_size " << msg_size << endl;

    if (this->type == PM_TYPE_SERVER) {
        return sendto(this->sd, &_pmsg, sizeof (_pmsg), 0, (struct sockaddr *) &this->client_isa, sizeof (this->client_isa));
    } else {
        puts(_pmsg.text);
        return sendto(this->sd, &_pmsg, sizeof (_pmsg), 0, (struct sockaddr *) &this->server_isa, sizeof (this->server_isa));
    }

    return -1;
}

char *pm::pm_recv_udp(int *msg_type, int *msg_size) {
    int ret;

    socklen_t addr_len = sizeof (this->client_isa);

    if (this->type == PM_TYPE_SERVER) {
        ret = recvfrom(this->sd, &this->pmsg, sizeof (this->pmsg), 0, (struct sockaddr *) &this->client_isa, (socklen_t *) &addr_len);
    } else {
        ret = recvfrom(this->sd, &this->pmsg, sizeof (this->pmsg), 0, (struct sockaddr *) &this->server_isa, (socklen_t *) &addr_len);
    }

    if (ret <= 0) {
        return NULL;
    }

    /*
    if (time(NULL) - this->pmsg.time_send > 1) {
        printf("time(NULL) - this->pmsg.time_send\n");
        return NULL;
    }
    */
    *msg_size = ret;
    *msg_type = this->pmsg.type;

    return &this->pmsg.text[0];
}

void pm::pm_close_udp() {
    close(this->sd);
}

/*
int main() {

    ProcMsgUDP Pmsg_s;
    ProcMsgUDP Pmsg_c;

    cout << "create  " << Pmsg_s.create((char *) "127.0.0.1", 9090) << endl;
    cout << "connect " << Pmsg_c.connect((char *) "127.0.0.1", 9090) << endl;


    sleep(2);
    puts("handler_start");
    Pmsg_s.handler_start(PM_DATA_CLIENT_JSON);
    sleep(3);

    cout << "send: " << Pmsg_c.send(PM_DATA_CLIENT_STR, (char *) "Hello world", strlen((char *) "Hello world")) << endl;
    perror("send");

    puts("sleep 5");
    sleep(5);

    Pmsg_s.handler_wait();

    return 0;
}
*/
