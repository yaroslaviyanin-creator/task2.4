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