#ifndef DOP_MESSAGE_H
#define DOP_MESSAGE_H
#define MSGSZ 128

struct message{
    long mtype;        // Тип сообщения
    char mesg [MSGSZ]; // Само сообщение, длиной MSGSZ.
};

#endif //DOP_MESSAGE_H
