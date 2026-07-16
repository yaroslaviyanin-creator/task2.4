/*
main.c - главный модуль программы.

Янин Ярослав Иванович
Группа МК-101
*/

#include "lib_main.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    GeneratorConfig config;
    init_config(&config);

    if (parse_args(argc, argv, &config) != 0) {
        return 1;
    }

    if (!validate_config(&config)) {
        return 1;
    }

    printf("--- Config is valid! ---\n");
    return 0;
}