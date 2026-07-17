/*
config.c - реализация проверки настроек.

Янин Ярослав Иванович
Группа МК-101
*/
#include "config.h"
#include <stdio.h>
#include <string.h>

// Функция для инициализации параметров генератора значениями по умолчанию
// <cfg> - указатель на структуру с параметрами
void init_config(TConfig* cfg) {
    cfg->veroyat_count = 0;
    cfg->psw_min_len = 0;               // Минимальная длина пароля
    cfg->psw_max_len = 0;               // Максимальная длина пароля
    cfg->psw_len = 0;                   // Длина пароля
    cfg->count_psw = 1;                 // Кол-во генерируемых паролей (По умолчанию 1)
    cfg->alphabet = NULL;               // Алфавит символов (-a)
    cfg->char_sets = NULL;              // Набор символов (-C)
    strcpy(cfg->cur_separators, "=:");  // Текущии разделители (-d и -D для изменения)
}

// Функция для проверки логических конфликтов в настройках
// <cfg> - указатель на структуру с параметрами
// Возвращает 1, если настройки корректны, 0 в случае конфликта
int validate_config(const TConfig* cfg) {

    // Проверка конфликта между точной длиной и диапазоном
    // Если (-n) больше нуля и один из min/max больше нуля - конфликт
    if (cfg->psw_len > 0 && (cfg->psw_min_len > 0 || cfg->psw_max_len > 0)) {
        fprintf(stderr, "Error: options -minl/-maxl and -n are incompatible.\n"); return 0;
    }

    // Проверка, что -minl и -maxl заданы вместе
    if ((cfg->psw_min_len > 0 && cfg->psw_max_len == 0) || (cfg->psw_min_len == 0 && cfg->psw_max_len > 0)) {
        fprintf(stderr, "Error: options -minl and -maxl must be used together.\n"); return 0;
    }

    // Проверка psw_min_len < psw_max_len
    if (cfg->psw_min_len > 0 && cfg->psw_max_len > 0 && cfg->psw_min_len > cfg->psw_max_len) {
        fprintf(stderr, "Error: -minl cannot be greater than -maxl.\n"); return 0;
    }

    // Проверка конфликта наборов символов (-a и -C не совместимы)
    if (cfg->alphabet != NULL && cfg->char_sets != NULL) {
        fprintf(stderr, "Error: options -a and -C are incompatible.\n"); return 0;
    }

    // Проверка, что задана хотя бы одна опция длины
    if (cfg->psw_len == 0 && cfg->psw_min_len == 0) {
        fprintf(stderr, "Error: you must specify password length (-n or -minl/-maxl).\n"); return 0;
    }

    // Проверка, что задан хотя бы один набор символов (-a или -C)
    if (cfg->alphabet == NULL && cfg->char_sets == NULL) {
        fprintf(stderr, "Error: you must specify character set (-a or -C).\n"); return 0;
    }

    return 1;
}