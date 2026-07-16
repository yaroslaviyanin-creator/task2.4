/*
parser.c - реализация парсера аргументов.

Янин Ярослав Иванович
Группа МК-101
*/
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Функция для обновления разделителей
// <cfg> - указатель на структуру с параметрами
// <new_seps> - строка с новыми символами-разделителями
// <replace> - флаг: 1 для замены (-D), 0 для добавления (-d)
void update_separators(GeneratorConfig* cfg, const char* new_seps, int replace) {
    if (replace) {
        // Заменяем текущие разделители (-D)
        // Оставляем 1 байт под ноль, чтобы избежать переполнения
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
static char* get_arg_value(const char* arg, const char* prefix, GeneratorConfig* cfg, int* i, int argc, char* argv[], int is_sep_option) {
    char* val = (char*)(arg + strlen(prefix));

    if (!is_sep_option && strchr(cfg->separators, *val) != NULL) {
        val++;
    }

    if (*val == '\0') {
        if (*i + 1 < argc) {
            (*i)++;
            val = argv[*i];
            if (!is_sep_option && strchr(cfg->separators, *val) != NULL) {
                val++;
            }
        }else{
            return NULL;
        }
    }

    if (!is_sep_option && strchr(cfg->separators, *val) != NULL) {
        val++;
    }
    return val;
}

// Функция для преобразования строки в положительное число
static int parse_positive_int(const char* str, int* val) {
    if (!str || *str == '\0') return 0;
    char* endptr;
    long res = strtol(str, &endptr, 10);
    if (*endptr != '\0' || res <= 0) return 0;
    *val = (int)res;
    return 1;
}

// Функция для разбиения строки с вероятностями в массив чисел
static int parse_probabilities(const char* str, double* probs, int* count) {
    if (!str || *str == '\0') return 0;
    char* copy = (char*)malloc(strlen(str) + 1);
    if (!copy) return 0;
    strcpy(copy, str);

    *count = 0;
    char* token = strtok(copy, ",");

    while (token != NULL && *count < 128) {
        probs[*count] = strtod(token, NULL);
        (*count)++;
        token = strtok(NULL, ",");
    }
    free(copy);
    return 1;
}

// Функция для парсинга аргументов командной строки
// <argc> - количество переданных аргументов
// <argv> - массив строк с аргументами
// <cfg> - указатель на структуру с параметрами генерации
int parse_args(int argc, char* argv[], GeneratorConfig* cfg) {
    int has_minl = 0, has_maxl = 0, has_n = 0, has_c = 0;

    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (arg[0] != '-') continue;

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
        if (starts_with(arg, "-minl")) {
            if (has_minl) { 
                fprintf(stderr, "Error: option -minl is duplicated.\n"); 
                return -1; 
            }
            char* val = get_arg_value(arg, "-minl", cfg, &i, argc, argv, 0);
            if (!val || !parse_positive_int(val, &cfg->min_len)) { 
                fprintf(stderr, "Error: invalid value for -minl.\n"); 
                return -1; 
            }
            has_minl = 1; 
            continue;
        }
        if (starts_with(arg, "-maxl")) {
            if (has_maxl) { 
                fprintf(stderr, "Error: option -maxl is duplicated.\n"); 
                return -1; 
            }
            char* val = get_arg_value(arg, "-maxl", cfg, &i, argc, argv, 0);
            if (!val || !parse_positive_int(val, &cfg->max_len)) { 
                fprintf(stderr, "Error: invalid value for -maxl.\n"); 
                return -1; 
            }
            has_maxl = 1; 
            continue;
        }
        if (starts_with(arg, "-n")) {
            if (has_n) { 
                fprintf(stderr, "Error: option -n is duplicated.\n"); 
                return -1; 
            }
            char* val = get_arg_value(arg, "-n", cfg, &i, argc, argv, 0);
            if (!val || !parse_positive_int(val, &cfg->exact_len)) { 
                fprintf(stderr, "Error: invalid value for -n.\n"); 
                return -1; }
            has_n = 1; 
            continue;
        }
        if (starts_with(arg, "-c")) {
            if (has_c) { 
                fprintf(stderr, "Error: option -c is duplicated.\n"); 
                return -1; 
            }
            char* val = get_arg_value(arg, "-c", cfg, &i, argc, argv, 0);
            if (!val || !parse_positive_int(val, &cfg->count)) { 
                fprintf(stderr, "Error: invalid value for -c.\n"); 
                return -1; }
            has_c = 1; 
            continue;
        }

        if (starts_with(arg, "-C")) {
            if (cfg->char_sets) { 
                fprintf(stderr, "Error: option -C is duplicated.\n"); 
                return -1; }
            char* val = get_arg_value(arg, "-C", cfg, &i, argc, argv, 0);
            if (!val || *val == '\0') { 
                fprintf(stderr, "Error: missing value for -C.\n"); 
                return -1; 
            }
            for (int j = 0; val[j] != '\0'; j++) {
                if (val[j] != 'a' && val[j] != 'A' && val[j] != 'D' && val[j] != 'S') {
                    fprintf(stderr, "Error: invalid character in -C. Use only a, A, D, S.\n");
                    return -1;
                }
            }
            cfg->char_sets = val;
            continue;
        }
        if (starts_with(arg, "-a")) {
            if (cfg->alphabet) { 
                fprintf(stderr, "Error: option -a is duplicated.\n"); 
                return -1; }
            char* val = (char*)(arg + 2);
            if (*val != '\0' && strchr(cfg->separators, *val) != NULL) val++;
            if (*val == '\0' && i + 1 < argc) {
                if (argv[i + 1][0] != '-') {
                    i++;
                    val = argv[i];
                }
            }
            if (*val == '\0') {
                val = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";
            }
            cfg->alphabet = val;
            continue;
        }
        if (starts_with(arg, "-p")) {
            char* val = get_arg_value(arg, "-p", cfg, &i, argc, argv, 0);
            if (!val || !parse_probabilities(val, cfg->probs, &cfg->probs_count)) {
                fprintf(stderr, "Error: invalid value for -p.\n");
                return -1;
            }
            continue;
        }
    }
    return 0;
}