#include "stdio.h"
#include "stdlib.h" //exit()
#include "unistd.h" //fork(), dup(), close(), execl()
#include "sys/wait.h"
#include "string.h"
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "cmake-build-debug/message.h"

#define MSGSZ 128

typedef struct msgbuf {
    long    mtype;                  //тип сообщения
    char    mtext[MSGSZ];           //само сообщение
} message_buf;

char massive[100] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int main()
{
    switch (fork()) { // разделяет прогу на 2 процесса. В каждый процесс возвращает разную цифру. Они идут параллельно
        case -1:
            printf("Ошибка при вызове fork() #1 \n");
            exit(1);
        case 0:         //сын1
            execl("son1","son1",NULL);
        default:break;
    }


    switch (fork()) {
        case -1:
            printf("Ошибка при вызове fork() #2 \n");
            exit(1);
        case 0:         //сын2
            sleep(2);
            execl("son2", "son2", NULL);


        default:        //Отец
            sleep(4);

            //прием сообщений
            int msqid;
            key_t key = 10;
            message_buf rbuf;
            message_buf sbuf;               //структура тип сообщения + сообщение
            size_t buf_length;

            //вытаскиваем очередь
            if ((msqid = msgget(key, 0666)) < 0) {
                fputs("\nОшибка при получении дискриптора очереди через msgget при чтении.\n", stderr);
                exit(1);
            } else {
                printf("\nНайдена очередь сообщений с id = %d\n",msqid); }
            do { //вынимает сообщение из очереди в rbuf, пока не -1
                  //msgrcv(дискриптор,куда,размер,тип,флаги)
                if (msgrcv(msqid, &rbuf, MSGSZ, 1, 0) < 0) {
                    fputs("\nОшибка при получении сообщения из очереди.\n", stderr);
                    exit(1);
                } else {
                    if (atoi(rbuf.mtext) >= 0) {
                        printf("Получено в отце от сына1: %s\n", rbuf.mtext);
                        //printf("%c\n", massive[atoi(rbuf.mtext)]);
                    }
                }
                if ((atoi(rbuf.mtext) >= 0)) {
                    sbuf.mtype = 2;                             //устанавливаем тип сообщения
                    char buf[16]; // перевод int to const char
                    sprintf(buf,"%d",massive[atoi(rbuf.mtext)]);
                    const char* pointer = buf;

                    strcpy(sbuf.mtext, pointer);                    //копирум текст сообщения в массив mtext
                    buf_length = strlen(sbuf.mtext) + 1;        //длина сообщения

                    if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
                        fputs("\nОшибка при отправке сообщения.", stderr);
                        exit(1);
                    } //else
                        //printf("Отправлено cообщение от отца к сыну1: %s\n", sbuf.mtext);
                }
                massive[atoi(rbuf.mtext)] = *"#";
            } while (atoi(rbuf.mtext) >= 0); //если приходит -1, то заканчивает
            sbuf.mtype = 2;
            strcpy(sbuf.mtext, "-1");
            buf_length = strlen(sbuf.mtext) + 1;
            msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT);






            //прием сообщений от 2 сына
            int msqid2;
            key_t key2 = 15;
            message_buf rbuf2;
            message_buf sbuf2;               //структура тип сообщения + сообщение
            size_t buf_length2;

            //вытаскиваем очередь
            if ((msqid2 = msgget(key2, 0666)) < 0) {
                fputs("\nОшибка при получении дискриптора очереди через msgget при чтении.\n", stderr);
                exit(1);
            } else {
                printf("\nНайдена очередь сообщений с id = %d\n",msqid2); }
            do { //вынимает сообщение из очереди в rbuf2, пока не -1
                //msgrcv(дискриптор,куда,размер,тип,флаги)
                if (msgrcv(msqid2, &rbuf2, MSGSZ, 1, 0) < 0) {
                    fputs("\nОшибка при получении сообщения из очереди.\n", stderr);
                    exit(1);
                } else {
                    if (atoi(rbuf2.mtext) >= 0) {
                        printf("Получено в отце от сына2: %s\n", rbuf2.mtext);
                        //printf("%c\n", massive[atoi(rbuf2.mtext)]);
                    }
                }
                if ((atoi(rbuf2.mtext) >= 0)) {
                    sbuf2.mtype = 2;                             //устанавливаем тип сообщения
                    char buf[16]; // перевод int to const char
                    sprintf(buf,"%d",massive[atoi(rbuf2.mtext)]);
                    const char* pointer = buf;

                    strcpy(sbuf2.mtext, pointer);                    //копирум текст сообщения в массив mtext
                    buf_length2 = strlen(sbuf2.mtext) + 1;        //длина сообщения

                    if (msgsnd(msqid2, &sbuf2, buf_length2, IPC_NOWAIT) < 0) {
                        fputs("\nОшибка при отправке сообщения.", stderr);
                        exit(1);
                    } //else
                    //printf("Отправлено cообщение от отца к сыну1: %s\n", sbuf2.mtext);
                }
                massive[atoi(rbuf2.mtext)] = *"#";
            } while (atoi(rbuf2.mtext) >= 0); //если приходит -1, то заканчивает
            sbuf2.mtype = 2;
            strcpy(sbuf2.mtext, "-1");
            buf_length2 = strlen(sbuf2.mtext) + 1;
            msgsnd(msqid2, &sbuf2, buf_length2, IPC_NOWAIT);
           

            sleep(3);
            printf("отец ended\n");
            puts(massive);
            exit(0);
    }
}
