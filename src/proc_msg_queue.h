/* 
 * File:   proc_msg.hpp
 * Author: root
 *
 * Created on 20 июля 2017 г., 18:31
 */

#ifndef PROC_MSG_QUEUE_H
#define	PROC_MSG_QUEUE_H

#include "proc_msg.h"

class ProcMsgQueue : public ProcMsg {
public:

    ProcMsgQueue();
    ~ProcMsgQueue();

    /**
     * @brief Подключается к очереди сообщений.
     * @param name
     * @param proj_id
     * @return true если очередь создана, иначе false
     */
    bool connect(char *name, int proj_id);
    
    /**
     * @brief Создает очередь сообщений.
     * @param name
     * @param proj_id
     * @return true если очередь создана, иначе false
     */
    bool create(char *name, int proj_id);

    /**
     * @brief Закрыть очередь сообщений.
     */
    void close();
    
    /**
     * @brief Отправить сообщение.
     * @param type тип сообщения сообщение (@ref QUEUE_DATA_JSON @ref QUEUE_DATA_STR @ref QUEUE_DATA_ARRAY)
     * @param msg сообщение
     * @param msg_size Длинна сообщения, сообщение не должно быть длинее чем @ref QUEUE_MESSAGE_SIZE.
     * @return -1 при ошибке, иначе 0.
     */
    int send(long type, char *msg, int msg_size);
    
    /**
     * @brief Отправить сообщение.
     * @param type тип сообщения сообщение (@ref QUEUE_DATA_JSON @ref QUEUE_DATA_STR)
     * @param msg сообщение
     * @return -1 при ошибке, иначе 0.
     */
    int send(long type, char *msg);
    
    /**
     * @brief Принять сообщение.
     * @param msg_type Тип сообщение (@ref QUEUE_DATA_JSON @ref QUEUE_DATA_STR @ref QUEUE_DATA_ARRAY)
     * @param msg_size возвращается размер полученого сообщения.
     * @return -1 при ошибке, иначе количество байтов, скопированных в массив pmsg.text
     */
    char *recv(int *msg_type, int *msg_size);

    /**
     * @brief Запуск обработчика входящих сообщений. Запускается в отдельном thred-е.
     */
    void handler_start(long _msgtype_get);
    
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
    
    std::string file_name;
    //char *file_name;
    
    thread thread_handler;
    mutex  mutex_read;
    
    /**
     * @brief Прием нового сообщения и запуск @ref handler_parser
     */
    void handler();
    
};

#endif	/* PROC_MSG_HPP */

