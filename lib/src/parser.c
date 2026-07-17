/*
parser.c - реализация парсера аргументов.

Янин Ярослав Иванович
Группа МК-101
*/
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Функция меняет набор символов‑разделителей в структуре параметров cfg (TConfig).
// Поведение зависит от флага flag_replace :
//      flag_replace = 1 (-D) — полностью заменяет текущие разделители на новые.
//      flag_replace = 0 (-d) — добавляет новые символы к текущим, но только если их ещё нет (без дубликатов).
// new_separators - строка с новыми символами-разделителями
void update_separators(TConfig* cfg, const char* new_separators, int flag_replace) {

    // max_count_separators - максимально возможное количество разделителей, 
    // зависит от количества выделенной памяти под переменную cfg->cur_separators
    int max_count_separators = sizeof(cfg->cur_separators) - 1;  // Резервируем последний символ массива под будущий символ конца строки '\0'.

    // Режим замены
    if (flag_replace) {
        // strncpy копирует не более max_count_separators символов из new_separators в cfg->cur_separators
        // Если длина строки new_separators меньше max_count_separators, 
        // то после копирования в cfg->cur_separators добавятся нулевые символы ('\0'), 
        // чтобы довести общее количество записанных байтов до max_count_separators. 
        strncpy(cfg->cur_separators, new_separators, max_count_separators);

        // Но, если же длина new_separators больше или равна max_count_separators, 
        // то функция скопирует первые max_count_separators символов, 
        // но не гарантирует, что в конце буфера будет поставлен нулевой символ.
        cfg->cur_separators[max_count_separators] = '\0';  // Поэтому добавляем сами '\0' в конце cfg->cur_separators.    
    }
    else { // Режим добавления новых символы (-d), если их еще нет в массиве
        for (int i = 0; new_separators[i] != '\0'; i++) {
            // strchr ищет в строке cfg->cur_separators первое вхождение заданного символа new_separators[i]
            if (strchr(cfg->cur_separators, new_separators[i]) == NULL) {
                int len = strlen(cfg->cur_separators);   //  узнаём текущую длину строки разделителей.
                // Проверяем, есть ли еще место в буфере,
                if (len < max_count_separators) {
                    cfg->cur_separators[len] = new_separators[i];   // тогда дописываем символ разделителя в конец строки разделителей
                    cfg->cur_separators[len + 1] = '\0';            // и ставим новый '\0'/
                }
            }
        }
    }
}

// Вспомогательная функция проверки начала строки
static int start_str_with(const char* str, const char* prefix_str) {
    return strncmp(str, prefix_str, strlen(prefix_str)) == 0;
}

// Вспомогательная функция извлечения значения аргумента
// cur_index_arg - индекс текущего аргумента в массиве argv, передается по указаиелю, чтобы его изменения возвращались в основной код
// is_optional = 1 для -a, дял остальных случаев is_optional = 0
static char* get_arg_value(const char* arg, const char* prefix, TConfig* cfg, int* cur_index_arg, int argc, char* argv[], int is_optional) {

    // Переменная val_ptr получает адрес первого символа подстроки, которая начинается сразу после prefix в строке arg.
    char* val_ptr = (char*)(arg + strlen(prefix));  // strlen(prefix)   - длина строки prefix без учёта завершающего символа '\0'
    // arg + strlen(prefix)   - получение адреса памяти сразу после строки префикса
// Пропустили ли мы уже разделитель
    int separator_skipped = 0;

    // Пропуск разделителя (если он разрешён и есть).
    // strchr ищет в строке cfg->cur_separators первое вхождение заданного символа *val_ptr.
    if (*val_ptr != '\0' && strchr(cfg->cur_separators, *val_ptr) != NULL) {
        val_ptr++;
        separator_skipped = 1; // Запоминаем, что разделитель съеден
    }


    // Если после префикса и разделителя ничего нет — берём следующий аргумент
    if (*val_ptr == '\0') {
        // Если -a, то проверям что после него не идёт задание следующего аргумента
        if (is_optional && *cur_index_arg + 1 < argc && argv[*cur_index_arg + 1][0] == '-') {
            return NULL;
        }
        // Если не -a
        if (*cur_index_arg + 1 < argc) {    // если есть ещё аргументы после текущего
            (*cur_index_arg)++;             // переходим к следующему аргументу
            val_ptr = argv[*cur_index_arg];     // берём его значение
            // ещё раз проверяем разделитель, если он есть в начале строки аргумента,
            if (!separator_skipped && strchr(cfg->cur_separators, *val_ptr) != NULL) val_ptr++;  // то пропускаем его (переходим к следующему символу)
        }
        else return NULL;   // нет следующего аргумента — ошибка
    }
    return val_ptr; // возвращаем указатель на текущий символ в текущем аргументе по индексу cur_index_arg
}

// Функция для преобразования строки в положительное число.
//      static — функция видна только в этом файле (не экспортируется наружу).
//      val_ptr — указатель, куда запишем результат, если всё получится.
static int parse_positive_int(const char* str, int* val_ptr) {
    // Проверяем, передалась ли строка str != NULL  (!str == 1 - то это ошибка)
    if (!str || *str == '\0') return 0;     // и не пустая ли она (*str == '\0' - пустая строка).

    char* sym_error;                        // указатель на символ, в котором произошла ошибка при преобразовании строки в число
    long rezult = strtol(str, &sym_error, 10);        // rezult - полученное число
    if (*sym_error != '\0' || rezult <= 0) return 0;  // Ошибка: после числа есть лишние символы или результат отрицательное число.
    *val_ptr = (int)rezult;    // возвращаемое значение
    return 1;           // всё успешно
}

// Функция для превращения строки с вероятностями в массив чисел
// Вероятности перечислены через запятую (например, "0.1,0.5,0.4"), и кладёт их в массив veroyat.
// count — указатель на переменную, куда функция запишет количество прочитанных чисел.
static int parse_veroyatnosti(const char* str, double* veroyat, int* count) {
    if (!str || *str == '\0') return 0;                 // Если строка пустая или не существует (NULL), тогда ошибка.
    char* copy_str = (char*)malloc(strlen(str) + 1);    // Создаём копию строки: выделяем память под копию
    if (!copy_str) return 0;
    strcpy(copy_str, str);                              // и копируем в copy_str исходную строку, так как будем изменять копию (strtok).

    *count = 0;                             // Обнуляем счетчик найденных чисел.
    char* part = strtok(copy_str, ",");     // part - часть строки до запятой (или до конца строки).
    // Функция strtok модифицирует строку: заменяет разделитель "," на нулевые символы '\0', 
    // чтобы каждая часть была корректно завершёна. 
    while (part != NULL && *count < 128) {  // Пока части не пустые и количество найденных чисел не превысило 128 - парсим:
        // записываем полученные числа в массив veroyat.
        veroyat[*count] = strtod(part, NULL);   // strtod - превращает строку в double, не проверяет корректность полученных чисел.
        (*count)++;                             // Увеличиваем счетчик найденных чисел.
        part = strtok(NULL, ",");               // Берём следующую часть.
    }
    free(copy_str);
    return 1;
}


//***************************************************************************
//               Функция для парсинга аргументов командной строки
//***************************************************************************

// Игнорирует аргументы, которые не начинаются с '-'. 
// Для каждого флага (-d, -D, -minl и т.д.) извлекает его значение (если оно есть), проверяет корректность и записывает в cfg.
// Следит, чтобы опции не дублировались, и сразу сообщает об ошибках через stderr.
//      argc - количество переданных аргументов
//      argv - массив строк с аргументами
//      cfg - указатель на структуру с параметрами конфигурации
int parse_args(int argc, char* argv[], TConfig* cfg) {
    // Для каждой опции сначала проверяем, не встречалась ли она уже (защита от дубликатов)
    int flag_has_minl = 0, flag_has_maxl = 0, flag_has_n = 0, flag_has_c = 0;    // ... = 0 - не было ещё опции
    // ... = 1 - была уже опция
    char* arg;          // arg - текущий аргумент
    char* val_ptr;      // указатель на возвращаемое значение

    // Проходим по всем аргументам (начиная с cur_index_arg = 1, потому что argv[0] — это имя программы).
    for (int cur_index_arg = 1; cur_index_arg < argc; cur_index_arg++) {
        arg = argv[cur_index_arg];

        // Игнорируем аргументы, которые не начинаются с '-', т.е. не являются опцией
        if (arg[0] != '-') continue;   // arg[0] - первый символ текущего аргумента

        if (start_str_with(arg, "-d")) {  // если найдена опция "-d" в текущем аргументе
            val_ptr = get_arg_value(arg, "-d", cfg, &cur_index_arg, argc, argv, 0);
            if (val_ptr) update_separators(cfg, val_ptr, 0);    //flag_replace = 0 (-d) — добавляет новые символы к текущим, но только если их ещё нет(без дубликатов).
            continue;
        }
        if (start_str_with(arg, "-D")) {
            val_ptr = get_arg_value(arg, "-D", cfg, &cur_index_arg, argc, argv, 0);
            if (val_ptr) update_separators(cfg, val_ptr, 1);    // flag_replace = 1 (-D) — полностью заменяет текущие разделители на новые.
            continue;
        }
        if (start_str_with(arg, "-minl")) {
            if (flag_has_minl) { fprintf(stderr, "Error: option -minl is duplicated.\n"); return -1; }

            val_ptr = get_arg_value(arg, "-minl", cfg, &cur_index_arg, argc, argv, 0);
            if (!val_ptr || !parse_positive_int(val_ptr, &cfg->psw_min_len)) {
                fprintf(stderr, "Error: invalid value for -minl.\n"); return -1;
            }
            flag_has_minl = 1;
            continue;
        }
        if (start_str_with(arg, "-maxl")) {
            if (flag_has_maxl) { fprintf(stderr, "Error: option -maxl is duplicated.\n"); return -1; }

            val_ptr = get_arg_value(arg, "-maxl", cfg, &cur_index_arg, argc, argv, 0);
            if (!val_ptr || !parse_positive_int(val_ptr, &cfg->psw_max_len)) {
                fprintf(stderr, "Error: invalid value for -maxl.\n"); return -1;
            }
            flag_has_maxl = 1;
            continue;
        }
        if (start_str_with(arg, "-n")) {
            if (flag_has_n) { fprintf(stderr, "Error: option -n is duplicated.\n"); return -1; }

            val_ptr = get_arg_value(arg, "-n", cfg, &cur_index_arg, argc, argv, 0);
            if (!val_ptr || !parse_positive_int(val_ptr, &cfg->psw_len)) {
                fprintf(stderr, "Error: invalid value for -n.\n"); return -1;
            }
            flag_has_n = 1;
            continue;
        }
        if (start_str_with(arg, "-c")) {
            if (flag_has_c) { fprintf(stderr, "Error: option -c is duplicated.\n"); return -1; }

            val_ptr = get_arg_value(arg, "-c", cfg, &cur_index_arg, argc, argv, 0);
            if (!val_ptr || !parse_positive_int(val_ptr, &cfg->count_psw)) {
                fprintf(stderr, "Error: invalid value for -c.\n"); return -1;
            }
            flag_has_c = 1;
            continue;
        }
        if (start_str_with(arg, "-C")) {
            if (cfg->char_sets) { fprintf(stderr, "Error: option -C is duplicated.\n"); return -1; }

            val_ptr = get_arg_value(arg, "-C", cfg, &cur_index_arg, argc, argv, 0);
            if (!val_ptr || *val_ptr == '\0') { fprintf(stderr, "Error: missing value for -C.\n"); return -1; }

            // Переменные для подсчета количества каждого разрешенного символа
            int count_a = 0, count_A = 0, count_D = 0, count_S = 0;
            for (int j = 0; val_ptr[j] != '\0'; j++) {
                if (val_ptr[j] == 'a') count_a++;
                else if (val_ptr[j] == 'A') count_A++;
                else if (val_ptr[j] == 'D') count_D++;
                else if (val_ptr[j] == 'S') count_S++;
                else {
                    fprintf(stderr, "Error: invalid character in -C. Use only a, A, D, S.\n");
                    return -1;
                }
            }
            // Защита от дубликатов
            if (count_a > 1 || count_A > 1 || count_D > 1 || count_S > 1) {
                fprintf(stderr, "Error: duplicate characters in -C are not allowed.\n");
                return -1;
            }
            cfg->char_sets = val_ptr;
            continue;
        }
        if (start_str_with(arg, "-a")) {
            if (cfg->alphabet) { fprintf(stderr, "Error: option -a is duplicated.\n"); return -1; }

            val_ptr = get_arg_value(arg, "-a", cfg, &cur_index_arg, argc, argv, 1);

            if (!val_ptr || *val_ptr == '\0') {
                val_ptr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";
            }

            cfg->alphabet = val_ptr;
            continue;
        }
        if (start_str_with(arg, "-p")) {
            val_ptr = get_arg_value(arg, "-p", cfg, &cur_index_arg, argc, argv, 0);
            if (!val_ptr || !parse_veroyatnosti(val_ptr, cfg->veroyat, &cfg->veroyat_count)) {
                fprintf(stderr, "Error: invalid value for -p.\n"); return -1;
            }
            continue;
        }
    }
    return 0;
}


