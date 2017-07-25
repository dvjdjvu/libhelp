/* 
 * File:   proc_msg.hpp
 * Author: root
 *
 * Created on 20 июля 2017 г., 18:31
 */

#ifndef PROC_MSG_HPP
#define	PROC_MSG_HPP

#include <errno.h>

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>

#include <thread>
#include <mutex>

#include <iostream>

using namespace std;

#define QUEUE_TYPE_SERVER 0x0010
#define QUEUE_TYPE_CLIENT 0x0011

#define QUEUE_MESSAGE_SIZE 1024

#define QUEUE_DATA_JSON     0x1000
#define QUEUE_DATA_STR      0x1001
#define QUEUE_DATA_ARRAY    0x1002

class ProcMsg {
public:

    ProcMsg();
    ~ProcMsg();

    struct proc_msg_s {
        long type; // Тип передаваемых данных.
        char text[QUEUE_MESSAGE_SIZE]; // Данные. 
    };

    proc_msg_s pmsg;

    /**
     * @brief Печать ошибки @ref errno
     */
    void perror();

    /**
     * @brief Печать ошибки @ref errno
     */
    void perror(const char *s);

    /**
     * @brief Подключается к очереди сообщений.
     * @param name
     * @param proj_id
     * @return true если очередь создана, иначе false
     */
    bool queue_connect(char *name, int proj_id);
    
    /**
     * @brief Создает очередь сообщений.
     * @param name
     * @param proj_id
     * @return true если очередь создана, иначе false
     */
    bool queue_create(char *name, int proj_id);

    /**
     * @brief Отправить сообщение.
     * @param type тип сообщения сообщение (@ref QUEUE_DATA_JSON @ref QUEUE_DATA_STR @ref QUEUE_DATA_ARRAY)
     * @param msg сообщение
     * @param msg_size Длинна сообщения, сообщение не должно быть длинее чем @ref QUEUE_MESSAGE_SIZE.
     * @return -1 при ошибке, иначе 0.
     */
    int send(long type, char *msg, int msg_size);
    
    /**
     * @brief Принять сообщение.
     * @param msg_type Тип сообщение (@ref QUEUE_DATA_JSON @ref QUEUE_DATA_STR @ref QUEUE_DATA_ARRAY)
     * @param msg_size возвращается размер полученого сообщения.
     * @return -1 при ошибке, иначе количество байтов, скопированных в массив pmsg.text
     */
    char *recv(int *msg_type, int *msg_size);
    
    /**
     * @brief Закрыть очередь сообщений.
     */
    void queue_close();

    /**
     * @brief Запуск обработчика входящих сообщений. Запускается в отдельном thred-е.
     */
    void handler_start();
    
    /**
     * @brief Остановка обработчика входящих сообщений.
     */
    void handler_stop();
    
    /**
     * @brief Ожидание остановки обработчика входящих сообщений.
     */
    void handler_wait();
    
    /**
     * @brief Обработка входящего сообщения.
     * @param type Тип сообщение (@ref QUEUE_DATA_JSON @ref QUEUE_DATA_STR @ref QUEUE_DATA_ARRAY)
     * @param data Данные.
     * @param size Размер данных.
     */
    virtual void handler_parser(long type, char *data, int size) {
        if (data == NULL) {
            puts("data == NULL");
            return;
        }
        
        cout << data << endl;
        cout << type << endl;
        cout << size << endl;
    }
    
private:
    
    int type = 0;
    
    key_t ipckey;
    int mq_id;
    
    string name;
    
    thread thread_handler;
    mutex  mutex_read;;
    
    /**
     * @brief Прием нового сообщения и запуск @ref.handler_parser
     */
    void handler();
    
};

#endif	/* PROC_MSG_HPP */

