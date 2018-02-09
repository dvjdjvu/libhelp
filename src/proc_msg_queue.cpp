#include "proc_msg.h"

bool pm::pm_connect_queue(char *name, int proj_id) {

    this->file_name = name;
    this->type = PM_TYPE_CLIENT;

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

bool pm::pm_create_queue(char *name, int proj_id) {

    this->file_name = name;
    this->type = PM_TYPE_SERVER;

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

void pm::pm_close_queue() {
    msgctl(this->mq_id, IPC_RMID, NULL);

    if (this->type == PM_TYPE_SERVER) {
        unlink(this->file_name.c_str());
        //unlink(this->file_name);
    }
}

int pm::pm_send_queue(long type, char *msg) {    
    return this->pm_send_queue(type, msg, strlen(msg));
}

int pm::pm_send_queue(long type, char *msg, int msg_size) {
    proc_msg_s _pmsg;

    _pmsg.time_send = time(NULL);
    
    _pmsg.type = type;
    memset(_pmsg.text, 0, PM_MESSAGE_SIZE); // Заполняем строку нулями.
    if (msg_size <= PM_MESSAGE_SIZE) {
        strncpy(_pmsg.text, msg, msg_size);
    } else {
        strncpy(_pmsg.text, msg, PM_MESSAGE_SIZE);
    }

    return msgsnd(this->mq_id, &_pmsg, sizeof (_pmsg.text), 0);
}

char *pm::pm_recv_queue(int *msg_type, int *msg_size) {
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
