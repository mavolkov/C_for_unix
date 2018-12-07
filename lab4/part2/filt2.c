#include "stdio.h"
#include "stdlib.h"

int main(int *argc, char *argv[])
{
    int ch;                             // символ-ходунок
    while ((ch=getchar()) != EOF) {     // читаем, пока не конец файла
        if (ch == *" ") {               // если символ пробел (переходим по адресу!)
            putchar(*"_");              // меняем его на ниж. подчерк. (переходим по адресу!)
        } else {                        // в другом случае
            putchar(ch);                // печатаем его
        }

    }
    return 0;
}
