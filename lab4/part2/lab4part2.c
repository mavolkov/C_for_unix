#include "stdio.h"
#include "stdlib.h" //exit()
#include "unistd.h" //fork(), dup(), close(), execl()
#include "sys/wait.h"

#define PATH "/home/misha/Рабочий стол/kek.txt"
#define R 0
#define W 1

void filt1(FILE *stream);

void main()
{
    int p[2], q[2]; //две трубы, создали 2 дискриптора (места для хранения труб)
    pipe(p);        //создаем сами трубы и заси каналы с 3 по 6
    pipe(q);        // труба p - от бати к сыну, труба q от сына к бате
    switch(fork())  //распараллелили процесс, поделили как амебу
    {
        case -1:
            fputs("Ошибка при вызове fork", stderr);
            exit(1);

        case 0: /* Потомок
                * p[R] - чтение, q[W] - запись*/
            //Переназначение stdin на p[R]
            close(p[W]);
            close(0); //cin
            dup(p[R]);
            close(p[R]);
            //Переназначение stdout на q[W]
            close(q[R]);
            close(1);
            dup(q[W]);
            close(q[W]);
            execl("filt2", "filt2", NULL); //используем системный вызов, кот превращает текущий процесс в новый проц.
            printf("filt2: Ошибка при вызове");//в случае плохого расклада
            exit(2);

        default: /*Основной процесс - батя
                  * q[R] - чтение, p[W] - запись*/
            close(p[R]);//закрыли ненужные потоки
            close(q[W]);
            FILE *fp = fdopen(p[W], "w"); //ассоциируем файловый дескриптор с потоком p[w]
            filt1(fp);  //отправляем файловый дескриптор в филт1
            fclose(fp); //деасссоциируем файловый дескриптор с этим потоков
            close(0);  //дропает cin
            dup(q[R]); //дюпает, копирует в наименьший слот
            close(q[R]); //закрывает старый
            char ch;
            while((ch = getchar()) != EOF) //ждет из нулевого потока (ввода)
                putchar(ch); //после ожидания печатает
            int status;
            wait(&status);//ожидаем завершения процесса потомка
            exit(0);
    }
}

//удаляет избыточные пробелы
void filt1(FILE *stream)
{
    FILE *file = fopen(PATH, "r");              // файловый дескриптор
    if (!file)                                  // если файла не сущетсвует
        fputs("File not found", stderr);        // говорим об этом

    int ch;                                     // символ-ходунок
    int ch_prev;                                // символ предыдущий
    ch_prev = 0;                                // инициализируем (просит компилятор)
    //ch_prev = *" ";                           // если не нужен пробел в начале строки, то инициализируем пробелом
    while ((ch=fgetc(file)) != EOF) {           // читаем, пока не конец файла
        if (!(ch_prev == *" " && ch == *" "))   // если предыдущий код-пробел, то не печатаем (переходим по адресу!)
            //putchar(ch);                        // печатаем символ
            putc(ch, stream);
        ch_prev = ch;                           // запоминаем предыдущий
    }
    fclose(file);                               // закрываем файл
}
