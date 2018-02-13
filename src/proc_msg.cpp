#include "proc_msg.h"

pm::pm() {
}

pm::~pm() {

}

void pm::pm_perror() {
    printf("%s\n", strerror(errno));
}

void pm::pm_perror(const char *s) {
    printf("%s: %s\n", s, strerror(errno));
}

void pm::pm_handler() {
    while (1) {
        this->pm_answer_get();
    }
}

void pm::pm_answer_get() {
    int size = 0;
    int type = 0;
    char *data = NULL;
    
    data = this->pm_recv(&type, &size);
    this->pm_handler_parser(type, data, size);
    
    return;
}

void pm::pm_handler_start(long _msgtype_get) {
    this->msgtype_get = _msgtype_get;
    this->thread_handler = thread(&pm::pm_handler, this);
}

void pm::pm_handler_stop() {
    this->thread_handler.detach();
}

void pm::pm_handler_wait() {
    this->thread_handler.join();
}

bool pm::pm_init(char *comm_type) {
    this->comm_type = comm_type;

    if (this->comm_type != PM_QUEUE || this->comm_type != PM_ZMQ || this->comm_type != PM_UDP) {
        return false;
    }

    return true;
}

bool pm::pm_connect(char *addr) {
    if (this->comm_type == PM_ZMQ) {
        return this->pm_connect_zmq(addr);
    }
}

bool pm::pm_connect(char *name, int proj_id) {
    if (this->comm_type == PM_QUEUE) {
        return this->pm_connect_queue(name, proj_id);
    } else if (this->comm_type == PM_UDP) {
        return this->pm_connect_udp(name, proj_id);
    }

    return false;
}

bool pm::pm_create(char *addr) {
    if (this->comm_type == PM_ZMQ) {
        return this->pm_create_zmq(addr);
    }

    return false;
}

bool pm::pm_create(char *name, int proj_id) {
    if (this->comm_type == PM_QUEUE) {
        return this->pm_create_queue(name, proj_id);
    } else if (this->comm_type == PM_UDP) {
        return this->pm_create_udp(name, proj_id);
    }

    return false;
}

void pm::pm_close() {
    if (this->comm_type == PM_QUEUE) {
        this->pm_close_queue();
    } else if (this->comm_type == PM_ZMQ) {
        this->pm_close_zmq();
    } else if (this->comm_type == PM_UDP) {
        this->pm_close_udp();
    }

    return;
}

int pm::pm_send(long type, char *msg) {
    return this->pm_send(type, msg, strlen(msg));
}

int pm::pm_send(long type, char *msg, int msg_size) {
    if (this->comm_type == PM_QUEUE) {
        return this->pm_send_queue(type, msg, msg_size);
    } else if (this->comm_type == PM_ZMQ) {
        return this->pm_send_zmq(type, msg, msg_size);
    } else if (this->comm_type == PM_UDP) {
        return this->pm_send_udp(type, msg, msg_size);
    }

    return -2;
}

char *pm::pm_recv(int *msg_type, int *msg_size) {
    if (this->comm_type == PM_QUEUE) {
        return this->pm_recv_queue(msg_type, msg_size);
    } else if (this->comm_type == PM_ZMQ) {
        return this->pm_recv_zmq(msg_type, msg_size);
    } else if (this->comm_type == PM_UDP) {
        return this->pm_recv_udp(msg_type, msg_size);
    }

    return NULL;
}





/*
int main() {

    proc_msg Pmsg_s;
    proc_msg Pmsg_c;

    puts("init");
    //cout << Pmsg_s.queue_create((char *) "/tmp/qqqq", 100) << endl;
    cout << Pmsg_c.queue_connect((char *) "/tmp/qqqq", 100) << endl;

    sleep(2);
    puts("handler_start");
    Pmsg_c.handler_start(PM_DATA_CLIENT_JSON);
    sleep(3);


    puts("send");
    cout << Pmsg_s.send(PM_DATA_CLIENT_STR, (char *) "Hello world", strlen((char *) "Hello world")) << endl;

    puts("sleep 5");
    sleep(5);

    Pmsg_c.handler_wait();
    
    return 0;
}
 */
