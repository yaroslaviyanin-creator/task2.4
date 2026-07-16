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

    printf("--- Start parsing ---\n");
    parse_args(argc, argv, &config);
    printf("--- End parsing ---\n");
    printf("Final separators: '%s'\n", config.separators);

    return 0;
}