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