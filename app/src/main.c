/*
main.c - главный модуль программы.

Янин Ярослав Иванович
Группа МК-101
*/

#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

int main(int argc, char* argv[]) {
    // Инициализируем генератор случайных чисел текущим временем
    srand((unsigned int)time(NULL));

    GeneratorConfig config;
    init_config(&config);

    if (parse_args(argc, argv, &config) != 0) {
        return 1;
    }

    if (!validate_config(&config)) {
        return 1;
    }

    char* full_alphabet = build_alphabet(&config);
    if (!full_alphabet) {
        fprintf(stderr, "Error: failed to build alphabet.\n");
        return 1;
    }

    // Определяем длину для теста (если задана точная - берем ее, иначе минимальную)
    int test_len = config.exact_len > 0 ? config.exact_len : config.min_len;

    // Генерируем 1 тестовый пароль
    char* password = generate_password(test_len, full_alphabet);
    if (password) {
        printf("Test Password: %s\n", password);
        free(password); // Освобождаем память пароля
    }

    free(full_alphabet); // Освобождаем память алфавита

    return 0;
}