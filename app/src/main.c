/*
main.c - главный модуль программы.

Янин Ярослав Иванович
Группа МК-101
*/

#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    GeneratorConfig config;
    init_config(&config);

    if (parse_args(argc, argv, &config) != 0) {
        return 1;
    }

    if (!validate_config(&config)) {
        return 1;
    }

    // Тестируем сборку алфавита
    char* full_alphabet = build_alphabet(&config);
    if (full_alphabet) {
        printf("--- Generated Alphabet ---\n");
        printf("%s\n", full_alphabet);
        free(full_alphabet); // Обязательно освобождаем память
    }
    else {
        fprintf(stderr, "Error: failed to build alphabet.\n");
        return 1;
    }

    return 0;
}