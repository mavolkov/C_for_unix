#include "message.h"
#include "stdio.h"      //sprintf, perror
#include "stdlib.h"     //exit
#include "unistd.h"     //getpid, read, write
#include "sys/stat.h"   //mkfifo
#include "fcntl.h"      //O_RDWR, open
#include "string.h"     //strcpy

void filter2(int fdChannel);

int main (int argc, char **argv)
{
    struct message msg;
    char errorMessage[100];

    //Задание имени личного канала
    sprintf(msg.privateChannel, "Fifo%d", getpid()); //process id
    strcpy(msg.filename, argv[1]);

    //Создание личного именованного канала с правом на чтение/запись для всех
    if (mkfifo(msg.privateChannel, S_IFIFO|0666) == -1)
    {
        strcat(errorMessage, "Client - Cannot create private channel #");
        strcat(errorMessage, msg.privateChannel);
        perror(errorMessage); //выводим ошибку
        exit(1);
    }

    //Открытие общего именованного канала перед обращением к серверу
    int fdPublic = open("public", O_WRONLY); //ассоциируем файловый дискриптор fdpublic c каналом паблик на запись
    if (fdPublic == -1)
    {
        perror("Client - Cannot open public channel");
        exit(2);
    }

    //Запись в общий канал имени личного канала и требуемого файла
    write(fdPublic, (char*)&msg, sizeof(msg)); //отпр. в канал преобр в чары msg
    close(fdPublic);

    //Чтение ответа сервера из личного именованного канала
    int fdPrivate = open(msg.privateChannel, O_RDONLY); //асс
    if (fdPrivate == -1)
    {
        strcat(errorMessage, "Client - Cannot open private channel #");
        strcat(errorMessage, msg.privateChannel);
        perror(errorMessage);
        exit(3);
    }

    //Обработка данных фильтром и вывод
    filter2(fdPrivate);

    //Закрытие и удаление личного именованного канала
    close(fdPrivate);
    if(unlink(msg.privateChannel) == 1) //чтобы не было утечки памяти удаляем канал
    {
        strcat(errorMessage, "Client - Cannot delete private channel #");
        strcat(errorMessage, msg.privateChannel);
        perror(errorMessage);
    }
    exit(0);
}



void filter2 (int fdChannel)
{
    int ch;                             // символ-ходунок
    while (read(fdChannel, &ch, 1) > 0) {   // ОЖИДАЕМ СЕРВЕР
        if (ch == *" ") {                   // если символ пробел (переходим по адресу!)
            putchar(*"_");                  // меняем его на ниж. подчерк. (переходим по адресу!)
        } else {                            // в другом случае
            putchar(ch);                    // печатаем его
        }
    }
}

//mkfifo public
//gcc server.c -o server
//gcc client.c -o client
//./server & //запускаем в фоновом режиме
//[1] 7366 //работае процесс
//ps //выводит все текущие процессы
//./client kek.txt //запускаем клиент, отправляем имя файла и имя приватного канал (имя приватного в коде)
//kill 7366
