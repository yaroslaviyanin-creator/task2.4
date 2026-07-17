/*
password_gen.c - реализация алгоритмов рандома и генерации.

Янин Ярослав Иванович
Группа МК-101
*/
#include "password_gen.h"
#include <stdlib.h>
#include <string.h>

// Функция для создания строки со всеми допустимыми символами для пароля
// cfg - указатель на структуру с параметрами
// Возвращает динамически выделенную строку, либо NULL при ошибке
char* build_alphabet(const TConfig* cfg) {
    char* rezult;

    // Если в конфигурации уже есть cfg->alphabet, функция просто копирует эту строку в новую область памяти и возвращает копию.
    // Например, пользователь указал опцию -a.
    if (cfg->alphabet) {
        rezult = (char*)malloc(strlen(cfg->alphabet) + 1);
        if (rezult) strcpy(rezult, cfg->alphabet);
        return rezult;
    }

    // Пользователь выбрал набор символов через -C.
    // Собираем алфавит из предопределённых блоков в зависимости от символов: 'a', 'A', 'D', 'S'. 
    if (cfg->char_sets) {
        char c;     // c - текущий символ
        rezult = (char*)malloc(128);
        if (!rezult) return NULL;
        rezult[0] = '\0';
        for (int i = 0; cfg->char_sets[i] != '\0'; i++) {
            c = cfg->char_sets[i];
            // strcat  - склеивает строки, результат в rezult
            if (c == 'a')       strcat(rezult, "abcdefghijklmnopqrstuvwxyz");   // строчные буквы
            else if (c == 'A')  strcat(rezult, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");   // заглавные буквы
            else if (c == 'D')  strcat(rezult, "0123456789");                   // цифры
            else if (c == 'S')  strcat(rezult, "!@#$%^&*()_+-=[]{}|;:,.<>?");   // спецсимволы
        }
        return rezult;
    }
    return NULL;
}

// Функция для расчета вероятности каждого конкретного символа
// cfg - указатель на структуру с параметрами
// alphabet - итоговая строка алфавита
// Возвращает динамический массив double, либо NULL
double* build_weights(const TConfig* cfg, const char* alphabet) {
    int i, j;                           // i, j - переменные циклов

    if (!alphabet) return NULL;         // если алфавита не существует - выходим из функции
    int alph_len = strlen(alphabet);    // длина алфавита
    if (alph_len == 0) return NULL;     // если алфавита пустой - выходим из функции

    // Выделяет память под массив весов: по одному double на каждый символ алфавита.
    double* weights = (double*)malloc(alph_len * sizeof(double));
    if (!weights) return NULL;

    // Если пользователь не указал вероятности (-p), все символы получают одинаковый вес: 1 / длина_алфавита.
    if (cfg->veroyat_count == 0) {
        for (i = 0; i < alph_len; i++) weights[i] = 1.0 / alph_len;
        return weights;
    }

    int real_count_veroyat;     // real_count_veroyat — сколько вероятностей реально задано
    double sum_veroyat;         // sum_veroyat - сумма вероятностей
    double remaining_veroyat;   // double remaining_veroyat - оставшаяся вероятность
    int unset_count;            // unset_count - количество «неучтённых» символов
    double default_veroyat;     // default_veroyat - вероятность для «неучтённых» символов

    // Первые real_count_veroyat символов получают веса из cfg->veroyat, остальные — одинаковый «остаточный» вес.
    // Символы заданы адфавитом
    if (cfg->alphabet != NULL) {
        sum_veroyat = 0.0;
        // specified_veroyat — сколько вероятностей реально задано (не больше длины алфавита).
        real_count_veroyat = (cfg->veroyat_count < alph_len) ? cfg->veroyat_count : alph_len;
        // Суммируем заданные вероятности 
        for (int i = 0; i < real_count_veroyat; i++) sum_veroyat += cfg->veroyat[i];
        // Если сумма больше 1, её обрезаем до 1 (защита от некорректного ввода).
        if (sum_veroyat > 1.0) sum_veroyat = 1.0;

        // Оставшуюся вероятность (1.0 - sum) равномерно распределяем между всеми «неучтёнными» символами.
        remaining_veroyat = 1.0 - sum_veroyat;
        unset_count = alph_len - real_count_veroyat; // Символов осталось
        default_veroyat = (unset_count > 0) ? (remaining_veroyat / unset_count) : 0.0;

        for (i = 0; i < alph_len; i++) {
            if (i < real_count_veroyat) weights[i] = cfg->veroyat[i];
            else weights[i] = default_veroyat;
        }
        return weights;
    }


    // Символы заданы набором символов
    // Вероятности задаются не на отдельные символы, а на группы символов (строчные, заглавные, цифры, спецсимволы)
    if (cfg->char_sets != NULL) {
        int sets_count = strlen(cfg->char_sets);        // количество наборов
        sum_veroyat = 0.0;
        real_count_veroyat = (cfg->veroyat_count < sets_count) ? cfg->veroyat_count : sets_count;
        // Заданные веса на наборы суммируются, 
        for (i = 0; i < real_count_veroyat; i++) sum_veroyat += cfg->veroyat[i];
        if (sum_veroyat > 1.0) sum_veroyat = 1.0;

        // остаток распределяется между «неучтёнными» наборами.
        remaining_veroyat = 1.0 - sum_veroyat;
        unset_count = sets_count - real_count_veroyat;
        default_veroyat = (unset_count > 0) ? (remaining_veroyat / unset_count) : 0.0;

        char char_set;                  // char_set - текущий символ из строки параметров
        int count_chars_in_set;         // count_chars_in_set - количество символов в наборе
        double set_weight;              // set_weight - вес набора
        double weight_per_char;         // weight_per_char - вес на долю символа в наборе
        int cur_index_in_set = 0;       // текущий индекс символа в наборе, сквозная нумерация между наборами

        // Для каждого набора
        for (i = 0; i < sets_count; i++) {
            char_set = cfg->char_sets[i];
            set_weight = (i < real_count_veroyat) ? cfg->veroyat[i] : default_veroyat;

            // Устанавливается, сколько в нём символов
            count_chars_in_set = 0;
            if (char_set == 'a' || char_set == 'A') count_chars_in_set = 26;    // 26 для букв
            else if (char_set == 'D') count_chars_in_set = 10;           // 10 для цифр
            else if (char_set == 'S') count_chars_in_set = 26;           // 26 для спецсимволов

            // вес набора делится поровну между всеми его символами
            weight_per_char = count_chars_in_set > 0 ? (set_weight / count_chars_in_set) : 0.0;

            // эти веса записываются в массив weights подряд, в том же порядке, в котором наборы идут в cfg->char_sets
            for (j = 0; j < count_chars_in_set; j++) {
                if (cur_index_in_set < alph_len) {          // alph_len - длина итогового алфавита
                    weights[cur_index_in_set] = weight_per_char;
                    cur_index_in_set++;
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
    // Проверка на некорректную длину или отсутствие алфавита
    if (length <= 0 || !alphabet) return NULL;
    int alph_len = strlen(alphabet);
    // Проверка на пустой алфавит
    if (alph_len == 0) return NULL;

    char* password = (char*)malloc(length + 1);
    if (!password) return NULL;

    for (int i = 0; i < length; i++) {
        // Если не введена вероятность
        if (weights == NULL) {
            // rand() генерирует число от 0 до RAND_MAX. 
            // Остаток от деления на длину алфавита даёт случайный индекс от 0 до alph_len - 1
            int rand_index = rand() % alph_len;
            password[i] = alphabet[rand_index];
        }
        // Если введена вероятность
        else {
            // Генерируем случайное вещественное число r в диапазоне от 0.0 до 1.0
            double r = (double)rand() / RAND_MAX;
            double cumulative = 0.0;
            // Установка значения по умолчанию на последний символ алфавита
            int selected_index = alph_len - 1;

            for (int j = 0; j < alph_len; j++) {
                cumulative += weights[j];
                // Как только накопленная сумма превышает или равна нашему случайному числу r
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