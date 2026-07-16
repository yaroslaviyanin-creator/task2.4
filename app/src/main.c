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
    // Обновляем рандом
    srand((unsigned int)time(NULL));

    GeneratorConfig config; // Создаём структуру
    init_config(&config);   // Инициализируем её

    // Парсим аргументы
    if (parse_args(argc, argv, &config) != 0) {
        return 1;
    }

    // Проверяем на ошибки
    if (!validate_config(&config)) {
        return 1;
    }

    // Собираем все разрешенные символы в одну строку
    char* full_alphabet = build_alphabet(&config); // Создаём алфавит
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
            // Если задан диапазон
            int diff = config.max_len - config.min_len + 1;
            current_len = config.min_len + (rand() % diff);
        }

        // Генерируем и печатаем
        char* password = generate_password(current_len, full_alphabet, weights);
        if (password) {
            printf("%s\n", password);
            free(password); // Чистим память после каждого пароля
        }
    }

    free(weights);
    free(full_alphabet);

    return 0;
}