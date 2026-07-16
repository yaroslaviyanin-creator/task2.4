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

    printf("--- Config values ---\n");
    printf("minl: %d, maxl: %d, n: %d, count: %d\n", config.min_len, config.max_len, config.exact_len, config.count);
    printf("alphabet: %s, char_sets: %s\n", config.alphabet ? config.alphabet : "(null)", config.char_sets ? config.char_sets : "(null)");

    return 0;
}