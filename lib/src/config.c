/*
generator_config.c - реализация проверки настроек.

Янин Ярослав Иванович
Группа МК-101
*/
#include "config.h"
#include <stdio.h>
#include <string.h>

// Функция для инициализации параметров генератора значениями по умолчанию
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