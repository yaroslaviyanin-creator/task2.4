/*
password_gen.c - реализация алгоритмов рандома и генерации.

Янин Ярослав Иванович
Группа МК-101
*/
#include "password_gen.h"
#include <stdlib.h>
#include <string.h>

// Функция для создания строки со всеми допустимыми символами для пароля
// <cfg> - указатель на структуру с параметрами
// Возвращает динамически выделенную строку (необходимо освободить через free), либо NULL при ошибке
char* build_alphabet(const GeneratorConfig* cfg) {
    if (cfg->alphabet) {
        char* res = (char*)malloc(strlen(cfg->alphabet) + 1);
        if (res) strcpy(res, cfg->alphabet);
        return res;
    }

    if (cfg->char_sets) {
        char* res = (char*)malloc(128);
        if (!res) return NULL;
        res[0] = '\0';
        for (int i = 0; cfg->char_sets[i] != '\0'; i++) {
            char c = cfg->char_sets[i];
            if (c == 'a')       strcat(res, "abcdefghijklmnopqrstuvwxyz");
            else if (c == 'A')  strcat(res, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            else if (c == 'D')  strcat(res, "0123456789");
            else if (c == 'S')  strcat(res, "!@#$%^&*()_+-=[]{}|;:,.<>?");
        }
        return res;
    }
    return NULL;
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

    if (cfg->probs_count == 0) {
        for (int i = 0; i < alph_len; i++) weights[i] = 1.0 / alph_len;
        return weights;
    }

    if (cfg->alphabet != NULL) {
        double sum = 0.0;
        int specified = (cfg->probs_count < alph_len) ? cfg->probs_count : alph_len;
        for (int i = 0; i < specified; i++) sum += cfg->probs[i];
        if (sum > 1.0) sum = 1.0;

        double remaining = 1.0 - sum;
        int unset_count = alph_len - specified;
        double default_weight = (unset_count > 0) ? (remaining / unset_count) : 0.0;

        for (int i = 0; i < alph_len; i++) {
            if (i < specified) weights[i] = cfg->probs[i];
            else weights[i] = default_weight;
        }
        return weights;
    }

    if (cfg->char_sets != NULL) {
        int sets_count = strlen(cfg->char_sets);
        double sum = 0.0;
        int specified = (cfg->probs_count < sets_count) ? cfg->probs_count : sets_count;
        for (int i = 0; i < specified; i++) sum += cfg->probs[i];
        if (sum > 1.0) sum = 1.0;

        double remaining = 1.0 - sum;
        int unset_count = sets_count - specified;
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
            int rand_index = rand() % alph_len;
            password[i] = alphabet[rand_index];
        }
        else {
            double r = (double)rand() / RAND_MAX;
            double cumulative = 0.0;
            int selected_index = alph_len - 1;

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