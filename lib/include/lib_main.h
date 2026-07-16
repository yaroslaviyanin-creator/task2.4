/*
lib_main.h - заголовочный файл главной библиотеки проекта.

Янин Ярослав Иванович
Группа МК-101
*/

#pragma once

// Структура для хранения всех параметров генерации паролей
typedef struct {
    int min_len;           // Минимальная длина пароля (-minl)
    int max_len;           // Максимальная длина пароля (-maxl)
    int exact_len;         // Точная длина пароля (-n)
    int count;             // Количество паролей (-c)
    char* alphabet;        // Пользовательский алфавит (-a)
    char* char_sets;       // Наборы символов (-C: a, A, D, S)
    char separators[32];   // Допустимые разделители
} GeneratorConfig;

// Инициализация структуры стандартными значениями
void init_config(GeneratorConfig* cfg);

// Обновление списка разделителей
void update_separators(GeneratorConfig* cfg, const char* new_seps, int replace);

// Функция для парсинга аргументов командной строки
int parse_args(int argc, char* argv[], GeneratorConfig* cfg);

// Валидация логической целостности параметров
int validate_config(const GeneratorConfig* cfg);

// Формирование итогового алфавита на основе выбранных опций
char* build_alphabet(const GeneratorConfig* cfg);

// Генерация случайного пароля заданной длины из переданного алфавита
char* generate_password(int length, const char* alphabet);