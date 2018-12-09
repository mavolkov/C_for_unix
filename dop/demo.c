#include "stdio.h"
#include "stdlib.h" //exit()
#include "unistd.h" //fork(), dup(), close(), execl()
#include "sys/wait.h"
#include "string.h"


int main()
{
    int fd[2];      // тут будут лежать дескрипторы процессов. На 0 чтение, на 1 запись
    int status;     // для ожидания

    switch (fork()) // разделяет прогу на 2 процесса. В каждый процесс возвращает разную цифру. Они идут параллельно
    {
        case -1:    //все пошло плохо
            printf("Ошибка при вызове fork() #1 \n");
            exit(1);

        case 0: //сын
            pipe(fd);   //создает неименованный программный канал(однонаправленная труба, сюда можно писать и читать)
                        //отправлем туда массив fd чтобы он написал туда файловый дескриптор на чтение и запись
            switch (fork()) {
                case -1:
                    printf("Ошибка при вызове fork() #2 \n");
                    exit(2);

                case 0: //внук
                    printf("\nвнук запустился\n");
                    printf ("-из внука: PID = %d, PPID = %d\n", getpid(), getppid());
                    printf("внук всё\n\n");
                    exit(0);

                default: //сын
                    printf("\nсын запустился\n");
                    printf("сын ждет внука\n");
                    wait(&status);
                    printf("сын  дождался\n");
                    printf ("-из  сына: PID = %d, PPID = %d\n", getpid(), getppid());
                    printf("сын  всё\n\n");
                    exit(0); //сын отработал
            }
        default: //Отец, ждет пока сын завершится
            printf("\nотец запустился\n");
            printf("отец ждет сына\n");
            wait(&status);
            printf("отец дождался\n");
            printf ("-из  отца: PID = %d, PPID = %d\n", getpid(), getppid());
            printf("отец всё\n");
            exit(status);
    }
}


