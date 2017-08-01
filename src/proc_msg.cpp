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

void ProcMsg::handler() {
    int size = 0;
    int type = 0;
    char *data = NULL;
    while (1) {
        size = 0;
        type = 0;
        data = this->recv(&type, &size);
        this->handler_parser(type, data, size);
    }
}

void ProcMsg::handler_start(long _msgtype_get) {
    this->msgtype_get = _msgtype_get;
    this->thread_handler = thread(&ProcMsg::handler, this);
}

void ProcMsg::handler_stop() {
    this->thread_handler.detach();
}

void ProcMsg::handler_wait() {
    this->thread_handler.join();
}

bool ProcMsg::queue_connect(char *name, int proj_id) {

    this->file_name = name;
    this->type = QUEUE_TYPE_CLIENT;

    // Generate the ipc key
    this->ipckey = ftok(name, proj_id);
    if (this->ipckey == -1) {
        return false;
    }

    // Set up the message queue
    this->mq_id = msgget(this->ipckey, 0);
    if (this->mq_id == -1) {
        return false;
    }

    return true;
}

bool ProcMsg::queue_create(char *name, int proj_id) {

    this->file_name = name;
    this->type = QUEUE_TYPE_SERVER;

    // Создаем файл ключа если он не существует.
    int fd = open(name, O_WRONLY | O_CREAT, 0);
    if (fd == -1) {
        return false;
    }

    if (close(fd) == -1) {

    }

    // Generate the ipc key
    this->ipckey = ftok(name, proj_id);
    if (this->ipckey == -1) {
        return false;
    }

    // Set up the message queue
    this->mq_id = msgget(this->ipckey, IPC_CREAT | 0666);
    if (this->mq_id == -1) {
        return false;
    }

    return true;
}

int ProcMsg::send(long type, char *msg) {    
    return this->send(type, msg, strlen(msg));
}

int ProcMsg::send(long type, char *msg, int msg_size) {
    proc_msg_s _pmsg;

    _pmsg.time_send = time(NULL);
    
    _pmsg.type = type;
    memset(_pmsg.text, 0, QUEUE_MESSAGE_SIZE); // Заполняем строку нулями.
    if (msg_size <= QUEUE_MESSAGE_SIZE) {
        strncpy(_pmsg.text, msg, msg_size);
    } else {
        strncpy(_pmsg.text, msg, QUEUE_MESSAGE_SIZE);
    }

    return msgsnd(this->mq_id, &_pmsg, sizeof (_pmsg.text), 0);
}

char *ProcMsg::recv(int *msg_type, int *msg_size) {
    int ret = msgrcv(this->mq_id, &this->pmsg, sizeof (this->pmsg.text), this->msgtype_get, 0);

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

void ProcMsg::queue_close() {
    msgctl(this->mq_id, IPC_RMID, NULL);

    if (this->type == QUEUE_TYPE_SERVER) {
        unlink(this->file_name.c_str());
        //unlink(this->file_name);
    }
}

int main() {

    ProcMsg Pmsg_s;
    ProcMsg Pmsg_c;

    puts("init");
    //cout << Pmsg_s.queue_create((char *) "/tmp/qqqq", 100) << endl;
    cout << Pmsg_c.queue_connect((char *) "/tmp/qqqq", 100) << endl;

    sleep(2);
    puts("handler_start");
    Pmsg_c.handler_start(QUEUE_DATA_CLIENT_JSON);
    sleep(3);


    puts("send");
    cout << Pmsg_s.send(QUEUE_DATA_CLIENT_STR, (char *) "Hello world", strlen((char *) "Hello world")) << endl;

    puts("sleep 5");
    sleep(5);

    Pmsg_c.handler_wait();
    
    return 0;
}
