/*
main.c - главный модуль программы.

Янин Ярослав Иванович
Группа МК-101
*/

#include "lib_main.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    GeneratorConfig config;
    
    // Инициализируем настройки дефолтными значениями
    init_config(&config);
    
    // Временный вывод для проверки
    printf("Config initialized. Default separators: '%s'\n", config.separators);
    
    return 0;
}