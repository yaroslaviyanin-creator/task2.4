/*
lib_main.c - главный модуль библиотеки.

Янин Ярослав Иванович
Группа МК-101
*/
/*
#include "lib_main.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>


// <cfg> - указатель на структуру с параметрами
void init_config(GeneratorConfig* cfg) {
    cfg->probs_count = 0;   
    cfg->min_len = 0;               // Минимальная длина пароля
    cfg->max_len = 0;               // Максимальная длина пароля
    cfg->exact_len = 0;             // Длина пароля
    cfg->count = 1;                 // Кол-во генерируемых паролей (По умолчанию 1)
    cfg->alphabet = NULL;           // Алфавит символов (-a)
    cfg->char_sets = NULL;          // Набор символов (-C)
    strcpy(cfg->separators, "=:");  // Текущии разделители (-d и -D для изменения)
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
// <arg> - текущий аргумент из консоли (например "-n=10")
// <prefix> - опция, которую мы ищем (например "-n")
// <cfg> - указатель на структуру с параметрами
// <i> - указатель на счетчик цикла
// <argc>, <argv> - классические аргументы из main
// <is_sep_option> - флаг (1 или 0). 
    // Если 0: функция пропустит символ-разделитель (для взятия числа)
    // Если 1: функция запретит пропускать разделитель (для -d/-D)
static char* get_arg_value(const char* arg, const char* prefix, GeneratorConfig* cfg, int* i, int argc, char* argv[], int is_sep_option) {
    char* val = (char*)(arg + strlen(prefix)); // Сдвигаем указатель вперед на длину префикса

    // Если после префикса пусто, значит значение через пробел, оно в следующие элементе argv
    if (*val == '\0') {
        if (*i + 1 < argc) {
            (*i)++;
            return argv[*i];
        }
        return NULL; // Ошибка: значение не передано
    }

    // Если не -d/-D, проверяем, не начинается ли значение с разделителя
    if (!is_sep_option && strchr(cfg->separators, *val) != NULL) {
        val++; // Пропускаем разделителя
    }

    return val;
}

// Функция для преобразования строки в положительное число
// <str> - исходная строковое представление числа
// <val> - указатель на переменную для записи результата
// Возвращает 1 в случае успеха, 0 при ошибке формата
static int parse_positive_int(const char* str, int* val) {
    if (!str || *str == '\0') return 0;

    char* endptr;
    // strtol превращает строку в int
    long res = strtol(str, &endptr, 10);

    // Проверяем, что вся строка была числом и больше нуля
    if (*endptr != '\0' || res <= 0) {
        return 0;
    }

    *val = (int)res;
    return 1;
}


// Функция для разбиения строки с вероятностями в массив чисел
// <str> - исходная строковое представление числа
// <probs> - указатель на массив результатов
// <count> - указатель на переменную-счетчик
static int parse_probabilities(const char* str, double* probs, int* count) {
    if (!str || *str == '\0') return 0;

    // Делаем копию строки, чтоб мы могли заменить разделители нулями
    char* copy = (char*)malloc(strlen(str) + 1);
    if (!copy) return 0;
    strcpy(copy, str);

    *count = 0; // Счётчик вероятностей
    char* token = strtok(copy, ",");  // strtok ищет первую запятую в строке copy

    // Пока слова есть или пока не заполнили массив
    while (token != NULL && *count < 128) {
        // strtod переводит строку в double
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
    int has_minl = 0, has_maxl = 0, has_n = 0, has_c = 0; // Флаги: 1 - ввели значение, 0 - не ввели

    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (arg[0] != '-') continue; // Ищем аргумент

        // starts_with проверяет с чего начинается строка
        if (starts_with(arg, "-d")) {
            // Достаем разделитель
            char* val = get_arg_value(arg, "-d", cfg, &i, argc, argv, 1);
            if (val) update_separators(cfg, val, 0);
            continue;
        }
        if (starts_with(arg, "-D")) {
            // Достаем разделитель
            char* val = get_arg_value(arg, "-D", cfg, &i, argc, argv, 1);
            if (val) update_separators(cfg, val, 1);
            continue;
        }

        // Обработка числовых параметров
        if (starts_with(arg, "-minl")) {
            if (has_minl) {                 // Если флаг уже равен 1
                fprintf(stderr, "Error: option -minl is duplicated.\n");
                return -1;
            }
            // Достаем значение аргумента
            char* val = get_arg_value(arg, "-minl", cfg, &i, argc, argv, 0);
            if (!val || !parse_positive_int(val, &cfg->min_len)) {  // Переводим строку в число
                fprintf(stderr, "Error: invalid value for -minl.\n");
                return -1;
            }
            has_minl = 1; // Поднимаем флаг
            continue;
        }
        if (starts_with(arg, "-maxl")) {
            if (has_maxl) {
                fprintf(stderr, "Error: option -maxl is duplicated.\n");
                return -1;
            }
            // Достаем значение аргумента
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
            // Достаем значение аргумента
            char* val = get_arg_value(arg, "-n", cfg, &i, argc, argv, 0);
            if (!val || !parse_positive_int(val, &cfg->exact_len)) {
                fprintf(stderr, "Error: invalid value for -n.\n");
                return -1;
            }
            has_n = 1;
            continue;
        }
        if (starts_with(arg, "-c")) {
            if (has_c) {
                fprintf(stderr, "Error: option -c is duplicated.\n");
                return -1;
            }
            // Достаем значение аргумента
            char* val = get_arg_value(arg, "-c", cfg, &i, argc, argv, 0);
            if (!val || !parse_positive_int(val, &cfg->count)) {
                fprintf(stderr, "Error: invalid value for -c.\n");
                return -1;
            }
            has_c = 1;
            continue;
        }

        // Обработка строковых параметров
        if (starts_with(arg, "-C")) {   
            if (cfg->char_sets) {   // Если флаг уже равен 1
                fprintf(stderr, "Error: option -C is duplicated.\n");
                return -1;
            }
            // Достаем значение аргумента
            char* val = get_arg_value(arg, "-C", cfg, &i, argc, argv, 0);
            if (!val || *val == '\0') {
                fprintf(stderr, "Error: missing value for -C.\n");
                return -1;
            }
            // Проверка, что используются только допустимые символы {a, A, D, S}
            for (int j = 0; val[j] != '\0'; j++) {
                if (val[j] != 'a' && val[j] != 'A' && val[j] != 'D' && val[j] != 'S') {
                    fprintf(stderr, "Error: invalid character in -C. Use only a, A, D, S.\n");
                    return -1;
                }
            }
            cfg->char_sets = val; // Сохраняем адрес строки
            continue;
        }
        if (starts_with(arg, "-a")) {
            if (cfg->alphabet) { fprintf(stderr, "Error: option -a is duplicated.\n"); return -1; }

            char* val = (char*)(arg + 2); // Пропускаем сам префикс "-a"

            // Если есть разделитель (например -a=), пропускаем его
            if (*val != '\0' && strchr(cfg->separators, *val) != NULL) {
                val++;
            }

            // Если значение слитно не написано, смотрим следующий аргумент в консоли
            if (*val == '\0' && i + 1 < argc) {
                // Берем его ТОЛЬКО если он НЕ начинается с минуса 
                // (если начинается с минуса, значит это другая опция, например -m2, и мы её не трогаем)
                if (argv[i + 1][0] != '-') {
                    i++;
                    val = argv[i];
                }

            }

            // Если значение так и осталось пустым, инициализируем его полным алфавитом!
            if (*val == '\0') {
                val = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";
            }

            cfg->alphabet = val;
            continue;
        }

        // Парсинг вероятностей
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

// Функция для проверки логических конфликтов в настройках
// <cfg> - указатель на структуру с параметрами
// Возвращает 1, если настройки корректны, 0 в случае конфликта
int validate_config(const GeneratorConfig* cfg) {
    // Проверка конфликта между точной длиной и диапазоном
    // Если (-n) больше нуля и один из min/max больше нуля - конфликт
    if (cfg->exact_len > 0 && (cfg->min_len > 0 || cfg->max_len > 0)) {
        fprintf(stderr, "Error: options -minl/-maxl and -n are incompatible.\n");
        return 0;
    }

    // Проверка, что -minl и -maxl заданы вместе
    if ((cfg->min_len > 0 && cfg->max_len == 0) || (cfg->min_len == 0 && cfg->max_len > 0)) {
        fprintf(stderr, "Error: options -minl and -maxl must be used together.\n");
        return 0;
    }

    // Проверка min_len < max_len
    if (cfg->min_len > 0 && cfg->max_len > 0 && cfg->min_len > cfg->max_len) {
        fprintf(stderr, "Error: -minl cannot be greater than -maxl.\n");
        return 0;
    }

    // Проверка конфликта наборов символов (-a и -C не совместимы)
    if (cfg->alphabet != NULL && cfg->char_sets != NULL) {
        fprintf(stderr, "Error: options -a and -C are incompatible.\n");
        return 0;
    }

    // Проверка, что задана хотя бы одна опция длины
    if (cfg->exact_len == 0 && cfg->min_len == 0) {
        fprintf(stderr, "Error: you must specify password length (-n or -minl/-maxl).\n");
        return 0;
    }

    // Проверка, что задан хотя бы один набор символов (-a или -C)
    if (cfg->alphabet == NULL && cfg->char_sets == NULL) {
        fprintf(stderr, "Error: you must specify character set (-a or -C).\n");
        return 0;
    }

    return 1;
}

// Функция для создания строки со всеми допустимыми символами для пароля
// <cfg> - указатель на структуру с параметрами
// Возвращает динамически выделенную строку (необходимо освободить через free), либо NULL при ошибке
char* build_alphabet(const GeneratorConfig* cfg) {
    if (cfg->alphabet) { // Сработала опция -a
        // Если передан пользовательский алфавит (-a), просто копируем его
        char* res = (char*)malloc(strlen(cfg->alphabet) + 1);
        if (res) {
            strcpy(res, cfg->alphabet);
        }
        return res;
    }

    if (cfg->char_sets) { // Сработала опция -C
        // Выделяем буфер 26+26+10+26 = 88 символов, поэтому 128 байт
        char* res = (char*)malloc(128);
        if (!res) return NULL;

        res[0] = '\0'; // Пустая строка

        for (int i = 0; cfg->char_sets[i] != '\0'; i++) {
            char c = cfg->char_sets[i];
            // Функция strcat ("string concatenate") приклеивает вторую строку в конец первой
            if (c == 'a') {
                strcat(res, "abcdefghijklmnopqrstuvwxyz");
            }
            else if (c == 'A') {
                strcat(res, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            }
            else if (c == 'D') {
                strcat(res, "0123456789");
            }
            else if (c == 'S') {
                strcat(res, "!@#$%^&*()_+-=[]{}|;:,.<>?");
            }
        }
        return res;
    }

    return NULL;
}

// Функция для генерации случайного пароля
// <length> - требуемая длина пароля
// <alphabet> - строка с допустимыми символами
// <weights> - массив вероятностей для каждого символа (размер равен длине алфавита)
// Возвращает динамически выделенную строку с паролем, либо NULL при ошибке
char* generate_password(int length, const char* alphabet, const double* weights) {
    if (length <= 0 || !alphabet) return NULL;

    int alph_len = strlen(alphabet);
    if (alph_len == 0) return NULL;

    char* password = (char*)malloc(length + 1);
    if (!password) return NULL;

    for (int i = 0; i < length; i++) {
        if (weights == NULL) {
            // Обычный рандом, если вероятности не передали
            int rand_index = rand() % alph_len;
            password[i] = alphabet[rand_index];
        }
        else {
            // Взвешенный рандом
            // rand() / RAND_MAX дает случайное число от 0.0 до 1.0
            double r = (double)rand() / RAND_MAX;
            double cumulative = 0.0;            // Копим вероятности
            int selected_index = alph_len - 1;  // Защита от погрешностей, чтоб если r = 1.0 не противоречил условию

            // Бежим по массиву
            for (int j = 0; j < alph_len; j++) {
                cumulative += weights[j];
                if (r <= cumulative) {
                    selected_index = j;
                    break;
                }
            }
            password[i] = alphabet[selected_index];
        }
    }
    password[length] = '\0';

    return password;
}


// Функция для расчета вероятности каждого конкретного символа
// <cfg> - указатель на структуру с параметрами
// <alphabet> - итоговая строка алфавита
// Возвращает динамический массив double, либо NULL
double* build_weights(const GeneratorConfig* cfg, const char* alphabet) {
    if (!alphabet) return NULL;
    int alph_len = strlen(alphabet);
    if (alph_len == 0) return NULL;

    double* weights = (double*)malloc(alph_len * sizeof(double));
    if (!weights) return NULL;

    // Если вероятности вообще не задали - делаем всем одинаково
    if (cfg->probs_count == 0) {
        for (int i = 0; i < alph_len; i++) {
            weights[i] = 1.0 / alph_len;
        }
        return weights;
    }

    // Логика для кастомного алфавита (-a)
    if (cfg->alphabet != NULL) {
        double sum = 0.0;
        // Берём меньшее кол-во вероятностей, чтоб не выйти за пределы массива
        int specified = (cfg->probs_count < alph_len) ? cfg->probs_count : alph_len;
        // Считаем сумму вероятностей
        for (int i = 0; i < specified; i++) {
            sum += cfg->probs[i];
        }
        if (sum > 1.0) sum = 1.0; // Защита от 100+%

        double remaining = 1.0 - sum; // Считаем, сколько вероятности осталось
        int unset_count = alph_len - specified; // Кол-во букв без вероятностей
        double default_weight = (unset_count > 0) ? (remaining / unset_count) : 0.0;
        // Записываем вес каждой букве
        for (int i = 0; i < alph_len; i++) {
            if (i < specified) weights[i] = cfg->probs[i];
            else weights[i] = default_weight;
        }
        return weights;
    }

    // Логика для наборов символов (-C)
    if (cfg->char_sets != NULL) {
        int sets_count = strlen(cfg->char_sets); // Кол-во наборов
        // Считаем сумму вероятностей
        double sum = 0.0;
        int specified = (cfg->probs_count < sets_count) ? cfg->probs_count : sets_count;
        for (int i = 0; i < specified; i++) {
            sum += cfg->probs[i];
        }
        if (sum > 1.0) sum = 1.0; // Защита от 100+%

        double remaining = 1.0 - sum; // Считаем, сколько вероятности осталось
        int unset_count = sets_count - specified; // Кол-во наборов без вероятностей
        double default_set_weight = (unset_count > 0) ? (remaining / unset_count) : 0.0;

        int current_idx = 0;
        for (int i = 0; i < sets_count; i++) {
            char c = cfg->char_sets[i];
            double set_weight = (i < specified) ? cfg->probs[i] : default_set_weight;

            int chars_in_set = 0;
            if (c == 'a' || c == 'A') chars_in_set = 26;
            else if (c == 'D') chars_in_set = 10;
            else if (c == 'S') chars_in_set = 26;

            double weight_per_char = chars_in_set > 0 ? (set_weight / chars_in_set) : 0.0;
            // Записываем вес каждой букве набора
            for (int j = 0; j < chars_in_set; j++) {
                if (current_idx < alph_len) {
                    weights[current_idx] = weight_per_char;
                    current_idx++;
                }
            }
        }
        return weights;
    }

    return weights;
}
*/