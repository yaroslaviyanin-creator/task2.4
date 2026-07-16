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
        // Ошибка уже выведена внутри parse_args
        return 1;
    }

    printf("--- Config values ---\n");
    printf("minl: %d, maxl: %d, n: %d, count: %d\n",
        config.min_len, config.max_len, config.exact_len, config.count);

    return 0;
}