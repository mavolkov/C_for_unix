#include "unistd.h" //read, write
#include "stdio.h"  //perror
#include "fcntl.h"  //open, O_RDONLY
#include "stdlib.h" //exit
#include "string.h" //strcat
#include "message.h"



void filter1(int fdChannel, int fdFile);

void main(int argc, char **argv)
{
    struct message msg;
    int fdPublic; //файловый дескриптор общедоступного канала
    int fdPrivate; //файловый дескриптор приватного канала
    int fdFile; //файловый дескриптор запрашиваемого файла
    char errorMessage[100];

    while (1) {
        /* Открытие общедоступного канала (open() блокируется,
        пока клиент не откроет другой конец канала) */
        fdPublic = open("public", O_RDONLY); //сервер будет ОЖИДАТЬ

        if (fdPublic == -1){
            perror("Server - Cannot open public channel");
            break;
        }

        /*Считывание из общего канала имен личного канала и
         * запращеваемого файла */
        while (read(fdPublic, (char*)&msg, sizeof(msg)) > 0) {
            fdFile = open(msg.filename, O_RDONLY); //открывает файл
            if (fdFile == -1){
                strcat(errorMessage, "Server - Cannot open file ");
                strcat(errorMessage, msg.filename);
                perror(errorMessage);
                break;
            }
        }

        /*Открытие личного канала для записи. Сервер спит, пока
         * клиент не откроет другой конец канала для чтения*/
        fdPrivate = open(msg.privateChannel, O_WRONLY);
        if (fdPrivate == -1)
        {
            strcat(errorMessage, "Server - Cannot open private channel");
            strcat(errorMessage, msg.privateChannel);
            perror(errorMessage);
            break;
        }

        /*Обработка файла и запись в личный канал*/
        filter1(fdPrivate, fdFile);

        close(fdFile);
        close(fdPrivate);

        /*Закрытие общего канала, чтобы сервер мог спать на open*/
        close(fdPublic);
    }
}


void filter1(int fdChannel, int fdFile)
{
    char ch;                                    // символ-ходунок
    char ch_prev;                               // символ предыдущий
    ch_prev = 0;                                // инициализируем (просит компилятор)
    //ch_prev = *" ";                           // если не нужен пробел в начале строки, то инициализируем пробелом
    while (read(fdFile, &ch, 1) > 0) {          // читаем, пока не конец файла
        if (!(ch_prev == ' ' && ch == ' '))     // если предыдущий код-пробел, то не печатаем (переходим по адресу!)
            write(fdChannel, &ch, 1);           //в приватный
        ch_prev = ch;                           // запоминаем предыдущий
    }
}
