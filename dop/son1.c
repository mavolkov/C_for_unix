#include "message.h"
#include "stdio.h"
#include "stdlib.h" //exit()
#include "sys/wait.h"
#include "string.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "message.h"
#include <time.h>
#include <unistd.h>

#define MSGSZ 128

typedef struct msgbuf {
    long    mtype;                  //тип сообщения
    char    mtext[MSGSZ];           //само сообщение
} message_buf;

int main() {


    //отправка сообщений
    int msqid;                      //дискриптор(id) очереди
    int msgflg = IPC_CREAT | 0666;  //флаги (с правами)
    key_t key = 10;                 //ключ очереди
    message_buf sbuf;               //структура тип сообщения + сообщение
    size_t buf_length;

    printf("Вызов msgget с ключом %d и флагом %#o \n", key, msgflg);
    if ((msqid = msgget(key, msgflg )) < 0) {
        fputs("\nОшибка при создании очереди сообщений.", stderr);
        //perror("\nОшибка при создании очереди сообщений.");
        exit(1); }
    else
        printf("\nСоздана очередь сообщений с id = %d\n",msqid);

    for (int i=0; i<3; i++) {
        sbuf.mtype = 1;                             //устанавливаем тип сообщения
        
	char buf[16]; // перевод int to const char
	sprintf(buf,"%d",i);
	const char* pointer = buf;

	strcpy(sbuf.mtext, pointer);                    //копирум текст сообщения в массив mtext
        buf_length = strlen(sbuf.mtext) + 1;        //длина сообщения

        if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
            fputs("\nОшибка при отправке сообщения.", stderr);
            exit(1);
        } else
            printf("Сообщение отправлено из сына1: %s\n", sbuf.mtext);
    }
	
	strcpy(sbuf.mtext, "-1");
	buf_length = strlen(sbuf.mtext) + 1; 
	if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
            fputs("\nОшибка при отправке сообщения.", stderr);
            exit(1);
        } else
            printf("Сообщение отправлено из сына1: %s\n", sbuf.mtext);

	message_buf rbuf;
	do{
	//начинаем читать то, что прислал отец
	if (msgrcv(msqid, &rbuf, MSGSZ, 2, 0) < 0) {
            fputs("\nОшибка при получении сообщения из очереди.\n", stderr);
              exit(1);
          } else {
		if (atoi(rbuf.mtext) >= 0) {
			int value = atoi(rbuf.mtext);

                        printf("Получено в сыне1: %c\n", (char) value);
                        //printf("%c\n", massive[atoi(rbuf.mtext)]);
                    }
          }
	} while (atoi(rbuf.mtext) >= 0); //если приходит -1, то заканчивает
	
    exit(0);
}
