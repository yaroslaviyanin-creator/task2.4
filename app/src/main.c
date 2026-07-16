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
    // Включаем генератор случайных чисел
    srand((unsigned int)time(NULL));

    GeneratorConfig config;
    init_config(&config);

    // Парсим аргументы
    if (parse_args(argc, argv, &config) != 0) {
        return 1;
    }

    // Проверяем на ошибки логики
    if (!validate_config(&config)) {
        return 1;
    }

    // Собираем все разрешенные символы в одну строку
    char* full_alphabet = build_alphabet(&config);
    if (!full_alphabet) {
        fprintf(stderr, "Error: failed to build alphabet.\n");
        return 1;
    }

    // Считаем вероятности для каждого символа из алфавита
    double* weights = build_weights(&config, full_alphabet);
    if (!weights) {
        fprintf(stderr, "Error: failed to build weights.\n");
        free(full_alphabet);
        return 1;
    }

    // Выводим нужное количество паролей
    for (int i = 0; i < config.count; i++) {
        int current_len = 0;

        // Если юзер задал точную длину
        if (config.exact_len > 0) {
            current_len = config.exact_len;
        }
        else {
            // Если задан диапазон (например 10-15)
            int diff = config.max_len - config.min_len + 1;
            current_len = config.min_len + (rand() % diff);
        }

        // Генерируем и печатаем (вероятности пока не передаем, сделаем это в следующем шаге)
        char* password = generate_password(current_len, full_alphabet);
        if (password) {
            printf("%s\n", password);
            free(password); // Чистим память после каждого пароля
        }
    }

    // Очищаем память, выделенную под алфавит и вероятности
    free(weights);
    free(full_alphabet);

    return 0;
}