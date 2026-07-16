/*
lib_main.c - главный модуль библиотеки.

Янин Ярослав Иванович
Группа МК-101
*/

#include "lib_main.h"
#include <string.h>
#include <stddef.h>

// <cfg> - указатель на структуру с параметрами
void init_config(GeneratorConfig* cfg) {
    cfg->min_len = 0;
    cfg->max_len = 0;
    cfg->exact_len = 0;
    cfg->count = 1;         // По умолчанию генерируем 1 пароль
    cfg->alphabet = NULL;
    cfg->char_sets = NULL;

    // Стандартные разделители по условию задания
    strcpy(cfg->separators, "=:");
}

// <cfg> - указатель на структуру с параметрами
// <new_seps> - строка с новыми символами-разделителями
// <replace> - флаг: 1 для замены (-D), 0 для добавления (-d)
void update_separators(GeneratorConfig* cfg, const char* new_seps, int replace) {
    if (replace) {
        // Заменяем текущие разделители (-D)
        // Оставляем 1 байт под нуль-терминатор, чтобы избежать переполнения
        strncpy(cfg->separators, new_seps, sizeof(cfg->separators) - 1);
        cfg->separators[sizeof(cfg->separators) - 1] = '\0';
    }
    else {
        // Добавляем новые символы (-d), если их еще нет в массиве
        for (int i = 0; new_seps[i] != '\0'; i++) {
            if (strchr(cfg->separators, new_seps[i]) == NULL) {
                int len = strlen(cfg->separators);
                // Проверяем, есть ли еще место в буфере
                if (len < sizeof(cfg->separators) - 1) {
                    cfg->separators[len] = new_seps[i];
                    cfg->separators[len + 1] = '\0';
                }
            }
        }
    }
}

// Вспомогательная функция проверки начала строки
static int starts_with(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

// Вспомогательная функция извлечения значения аргумента
// Сдвигает индекс i, если значение находится в следующем элементе (через пробел)
static char* get_arg_value(const char* arg, const char* prefix, GeneratorConfig* cfg, int* i, int argc, char* argv[], int is_sep_option) {
    char* val = (char*)(arg + strlen(prefix));

    // Если после префикса пусто, значит значение через пробел (в следующем argv)
    if (*val == '\0') {
        if (*i + 1 < argc) {
            (*i)++;
            return argv[*i];
        }
        return NULL; // Ошибка: значение не передано
    }

    // Если это не опция настройки разделителей, проверяем, не начинается ли значение с разделителя
    if (!is_sep_option && strchr(cfg->separators, *val) != NULL) {
        val++; // Пропускаем символ разделителя (например, '=' или ':')
    }

    return val;
}

int parse_args(int argc, char* argv[], GeneratorConfig* cfg) {
    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (arg[0] != '-') continue; // Игнорируем всё, что не является опцией

        // Обработка настройки разделителей (применяется сразу)
        if (starts_with(arg, "-d")) {
            char* val = get_arg_value(arg, "-d", cfg, &i, argc, argv, 1);
            if (val) update_separators(cfg, val, 0);
            continue;
        }
        if (starts_with(arg, "-D")) {
            char* val = get_arg_value(arg, "-D", cfg, &i, argc, argv, 1);
            if (val) update_separators(cfg, val, 1);
            continue;
        }

        // Извлечение остальных параметров (пока просто печатаем для проверки)
        if (starts_with(arg, "-minl")) {
            char* val = get_arg_value(arg, "-minl", cfg, &i, argc, argv, 0);
            if (val) printf("Parsed minl: %s\n", val);
        }
        else if (starts_with(arg, "-maxl")) {
            char* val = get_arg_value(arg, "-maxl", cfg, &i, argc, argv, 0);
            if (val) printf("Parsed maxl: %s\n", val);
        }
        else if (starts_with(arg, "-n")) {
            char* val = get_arg_value(arg, "-n", cfg, &i, argc, argv, 0);
            if (val) printf("Parsed n: %s\n", val);
        }
        else if (starts_with(arg, "-c")) {
            char* val = get_arg_value(arg, "-c", cfg, &i, argc, argv, 0);
            if (val) printf("Parsed c: %s\n", val);
        }
        else if (starts_with(arg, "-C")) {
            char* val = get_arg_value(arg, "-C", cfg, &i, argc, argv, 0);
            if (val) printf("Parsed C: %s\n", val);
        }
        else if (starts_with(arg, "-a")) {
            char* val = get_arg_value(arg, "-a", cfg, &i, argc, argv, 0);
            if (val) printf("Parsed a: %s\n", val);
        }
    }
    return 0;
}