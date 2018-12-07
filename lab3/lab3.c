#include <locale.h>    //для русского языка
#include <stdio.h>
#include <sys/types.h> //библиотеки системного вызова
#include <sys/stat.h>  //отсюда структура stat (см. тело структуры в комменте внизу)
#include <unistd.h>    //для getcwd
#include <string.h>    //для strcpy
#include <dirent.h>    //для DIR (открытие каталога)

int main(int argc, const char * argv[])
{
    setlocale(LC_ALL, "rus");

    char cwd[500];                      //cwd = current working directory = pwd
    getcwd(cwd, sizeof(cwd));           //получение cwd
    printf("Текущая директория: \n%s\n",cwd);     //вывод директории

    struct dirent *d;                   //указатель для открытия директории
    DIR *dir;                           //содержимое каталога

    dir = opendir(cwd);                 //открываем директорию
    printf("\nВсё в директории: \n");
    while( (d=readdir(dir)) != NULL)    //проход по содержимому
    {
        char path[500] = "";
        strcpy(path, "/");                          //добавляем слеш
        strcpy(path, d->d_name);                    //добавляем имя файла, получаем полный путь к файлу
        struct stat file_info;                      //создали структуру для записи инфо о файле
        stat(path, &file_info);

        char flag[20] = "";                           //переменная для флага
        sprintf(flag,"%o",file_info.st_mode);          //записывает флаг "простой ли файл" в flag в 8й c.c.  %o

        if(flag[0] == '1')
            printf("флаг - %8s; простой? - да ; файл - %s\n", flag, d->d_name);
        else
            printf("флаг - %8s; простой? - нет; файл - %s\n", flag, d->d_name);
    }
    closedir(dir);                                  //закрываем директорию


    return 0;
}


/*
Из этой структуры достаем всю инфу о файле:

struct stat
    {
         dev_t st_dev;    // device file
         ino_t st_ino;    // file serial inode
         ushort st_mode;  // file mode
         short st_nlink;  // number of links
         ushort st_uid;   // user ID
         ushort st_gid;   // group ID
         dev_t st_rdev;   // device ident
         off_t st_size;   // size of file
         time_t st_atime; // last access time
         time_t st_mtime; // last modify time
         time_t st_ctime; // last status change
    };

Поле st_mode содержит флаги, описывающие файл:
S_IFMT   0170000 – тип файла
S_IFDIR  0040000 – каталог
S_IFCHR  0020000 – байт-ориентированный специальный файл
S_IFBLK  0060000 – блок-ориентированный специальный файл
S_IFREG  0100000 – обычный/regilar файл
S_IFFIFO 0010000 – файл FIFO
S_ISUID  04000   – идентификатор владельца
S_ISGID  02000   – идентификатор группы
S_ISREAD 00400   – владельцу разрешено чтение
S_IWRITE 00200   – владельцу разрешена запись
S_IEXEC  00100   – владельцу разрешено выполнение

 */
