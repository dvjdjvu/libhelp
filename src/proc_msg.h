/* 
 * File:   proc_msg.hpp
 * Author: root
 *
 * Created on 20 июля 2017 г., 18:31
 */

#ifndef PM_HPP
#define	PM_HPP

#include <errno.h>

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <zmq.h>

#include <thread>
#include <mutex>

#include <iostream>

using namespace std;

#define PM_TYPE_SERVER 0x0010
#define PM_TYPE_CLIENT 0x0011

#define PM_MESSAGE_SIZE 1024

#define PM_DATA_SERVER_JSON     0x1000
#define PM_DATA_SERVER_STR      0x1001
#define PM_DATA_SERVER_ARRAY    0x1002

#define PM_DATA_CLIENT_JSON     0x2000
#define PM_DATA_CLIENT_STR      0x2001
#define PM_DATA_CLIENT_ARRAY    0x2002

#define PM_QUEUE "queue"
#define PM_ZMQ  "zmq"
#define PM_UDP  "udp"

class pm {
public:

    pm();
    ~pm();

    struct proc_msg_s {
        long type; // Тип передаваемых данных.
        time_t time_send;
        char text[PM_MESSAGE_SIZE]; // Данные. 
    };
       
    /**
     * @brief Печать ошибки @ref errno
     */
    void pm_perror();

    /**
     * @brief Печать ошибки @ref errno
     */
    void pm_perror(const char *s);

    /**
     * @brief Установить тип связи.
     * @param type @ref PM_ZMQ, @ref PM_QUEUE или др.
     * @return true или false
     */
    bool pm_init(char *comm_type);
    
    /**
     * @brief Подключается к очереди сообщений.
     * @param addr
     * @return true если очередь создана, иначе false
     */
    bool pm_connect(char *addr);
    
    /**
     * @brief Подключается к очереди сообщений.
     * @param name
     * @param proj_id
     * @return true если очередь создана, иначе false
     */
    bool pm_connect(char *name, int proj_id);
    
    /**
     * @brief Создает очередь сообщений.
     * @param addr
     * @return true если очередь создана, иначе false
     */
    bool pm_create(char *addr);
    
    /**
     * @brief Создает очередь сообщений.
     * @param name
     * @param proj_id
     * @return true если очередь создана, иначе false
     */
    bool pm_create(char *name, int proj_id);

    /**
     * @brief Отправить сообщение.
     * @param type тип сообщения сообщение (@ref PM_DATA_JSON @ref PM_DATA_STR @ref PM_DATA_ARRAY)
     * @param msg сообщение
     * @param msg_size Длинна сообщения, сообщение не должно быть длинее чем @ref PM_MESSAGE_SIZE.
     * @return -1 при ошибке, иначе 0.
     */
    int pm_send(long type, char *msg, int msg_size);
    
    /**
     * @brief Отправить сообщение.
     * @param type тип сообщения сообщение (@ref PM_DATA_JSON @ref PM_DATA_STR)
     * @param msg сообщение
     * @return -1 при ошибке, иначе 0.
     */
    int pm_send(long type, char *msg);
    
    /**
     * @brief Принять сообщение.
     * @param msg_type Тип сообщение (@ref PM_DATA_JSON @ref PM_DATA_STR @ref PM_DATA_ARRAY)
     * @param msg_size возвращается размер полученого сообщения.
     * @return -1 при ошибке, иначе количество байтов, скопированных в массив pmsg.text
     */
    char *pm_recv(int *msg_type, int *msg_size);
    
    /**
     * @brief Закрыть очередь сообщений.
     */
    void pm_close();

    /**
     * @brief Запуск обработчика входящих сообщений. Запускается в отдельном thred-е.
     */
    void pm_handler_start(long _msgtype_get);
    
    /**
     * @brief Остановка обработчика входящих сообщений.
     */
    void pm_handler_stop();
    
    /**
     * @brief Ожидание остановки обработчика входящих сообщений.
     */
    void pm_handler_wait();
    
    void pm_answer_get();
    
    /**
     * @brief Обработка входящего сообщения.
     * @param type Тип сообщение (@ref PM_DATA_JSON @ref PM_DATA_STR @ref PM_DATA_ARRAY)
     * @param data Данные.
     * @param size Размер данных.
     */
    virtual void pm_handler_parser(long type, char *data, int size) {
        if (data == NULL) {
            puts("data == NULL");
            return;
        }
        
        cout << data << endl;
        cout << type << endl;
        cout << size << endl;
    }
    
private:
    
    /**
     * @brief Используемый тип связи (message queue, zmq или др.).
     */
    std::string comm_type;
    
    /**
     * @brief Передаваемое сообщение.
     */
    proc_msg_s pmsg;
    
    /**
     * @brief Тип запрашиваемых сообщений.
     */
    long msgtype_get = 0;
    
    int type = 0;
    
    key_t ipckey;
    int mq_id;
    
    std::string file_name;
    //char *file_name;
    
    thread thread_handler;
    mutex  mutex_read;
    
    /**
     * @brief Прием нового сообщения и запуск @ref.handler_parser
     */
    void pm_handler();
    
    // queue
    bool pm_connect_queue(char *name, int proj_id);
    bool pm_create_queue(char *name, int proj_id);
    int pm_send_queue(long type, char *msg, int msg_size);
    int pm_send_queue(long type, char *msg);
    char *pm_recv_queue(int *msg_type, int *msg_size);
    void pm_close_queue();
    
    // zmq
    void *zmq_context;
    void *zmq_responder;
    
    bool pm_connect_zmq(char *addr);
    bool pm_create_zmq(char *addr);
    int pm_send_zmq(long type, char *msg, int msg_size);
    int pm_send_zmq(long type, char *msg);
    char *pm_recv_zmq(int *msg_type, int *msg_size);
    void pm_close_zmq();
    
    /*
     * UDP
     */
    bool pm_connect_udp(char *ip, int port);
    bool pm_create_udp(char *ip, int port);
    int pm_send_udp(long type, char *msg, int msg_size);
    int pm_send_udp(long type, char *msg);
    char *pm_recv_udp(int *msg_type, int *msg_size);
    void pm_close_udp();
    
    // Структура сервера.
    struct sockaddr_in server_isa;
    // Структура клиента.
    struct sockaddr_in client_isa;
    struct sockaddr_in client_server_isa;
    
    
    // Дескриптор сокета.
    int sd = 0;
    int sd_s = 0;
    int sd_c = 0;
    int sd_c_s = 0;
    
};

#endif	/* PM_HPP */

