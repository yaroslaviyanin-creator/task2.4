/*
config.h - структуры данных и логика проверки настроек.

Янин Ярослав Иванович
Группа МК-101
*/
#pragma once

typedef struct {
    int psw_min_len;            // Минимальная длина пароля
    int psw_max_len;            // Максимальная длина пароля
    int psw_len;                // Длина пароля
    int count_psw;              // Кол-во генерируемых паролей 
    char* alphabet;             // Алфавит символов (-a)
    char* char_sets;            // Набор символов (-C)
    char cur_separators[32];    // Текущии разделители (-d и -D для изменения)
    double veroyat[128];        // Массив чисел с вероятностями
    int veroyat_count;
} TConfig;

void init_config(TConfig* cfg);
int validate_config(const TConfig* cfg);