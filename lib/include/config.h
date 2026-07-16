/*
generator_config.h - структуры данных и логика проверки настроек.

Янин Ярослав Иванович
Группа МК-101
*/
#pragma once

typedef struct {
    int min_len;
    int max_len;
    int exact_len;
    int count;
    char* alphabet;
    char* char_sets;
    char separators[32];
    double probs[128];
    int probs_count;
} GeneratorConfig;

void init_config(GeneratorConfig* cfg);
int validate_config(const GeneratorConfig* cfg);