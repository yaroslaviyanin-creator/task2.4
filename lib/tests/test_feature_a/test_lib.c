/*
test_lib.c - модуль проверки библиотеки.

Янин Ярослав Иванович
Группа МК-101
*/

#include "unity.h"
#include "lib_main.h"
#include <string.h>
#include <stdlib.h>

void setUp(void) {
    // Вызывается перед каждым тестом (можно оставить пустым)
}

void tearDown(void) {
    // Вызывается после каждого теста
}

// Проверка инициализации структуры
void test_init_config(void) {
    TConfig cfg;
    init_config(&cfg);
    TEST_ASSERT_EQUAL_INT(0, cfg.psw_min_len);
    TEST_ASSERT_EQUAL_INT(1, cfg.count_psw);
    TEST_ASSERT_EQUAL_STRING("=:", cfg.cur_separators);
}

// Проверка  -d
void test_update_separators_add(void) {
    TConfig cfg;
    init_config(&cfg);

    // Добавляем восклицательный знак
    update_separators(&cfg, "!", 0);
    // Должно стать =:!
    TEST_ASSERT_EQUAL_STRING("=:!", cfg.cur_separators);
}

// Проверка -D
void test_update_separators_replace(void) {
    TConfig cfg;
    init_config(&cfg);

    // Заменяем всё на тире
    update_separators(&cfg, "-", 1);
    TEST_ASSERT_EQUAL_STRING("-", cfg.cur_separators);
}

// Проверка настроек
void test_validate_config_success(void) {
    TConfig cfg;
    init_config(&cfg);
    cfg.psw_len = 10;
    cfg.char_sets = "aD";

    TEST_ASSERT_EQUAL_INT(1, validate_config(&cfg));
}

// Проверка ошибки длины
void test_validate_config_fail_no_length(void) {
    TConfig cfg;
    init_config(&cfg);
    cfg.char_sets = "aD";

    // psw_min_len и psw_len равны 0
    TEST_ASSERT_EQUAL_INT(0, validate_config(&cfg));
}

// Проверка конфликта -minl и -n
void test_validate_config_fail_conflict_length(void) {
    TConfig cfg;
    init_config(&cfg);
    cfg.psw_len = 10;
    cfg.psw_min_len = 5;
    cfg.psw_max_len = 15;
    cfg.char_sets = "aD";

    TEST_ASSERT_EQUAL_INT(0, validate_config(&cfg));
}

// Проверка конфликт -a и -C
void test_validate_config_fail_conflict_alphabet(void) {
    TConfig cfg;
    init_config(&cfg);
    cfg.psw_len = 10;
    cfg.alphabet = "abc";
    cfg.char_sets = "aD";

    TEST_ASSERT_EQUAL_INT(0, validate_config(&cfg));
}

// Сборка алфавита -C aD
void test_build_alphabet(void) {
    TConfig cfg;
    init_config(&cfg);
    cfg.char_sets = "aD";

    char* alph = build_alphabet(&cfg);
    TEST_ASSERT_NOT_NULL(alph);
    // 26 + 10  = 36 
    TEST_ASSERT_EQUAL_INT(36, strlen(alph));
    free(alph);
}

// Равномерная вероятность
void test_build_weights_uniform(void) {
    TConfig cfg;
    init_config(&cfg);

    char* alph = "abcd"; // 4 символа
    double* weights = build_weights(&cfg, alph);

    TEST_ASSERT_NOT_NULL(weights);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.25, weights[0]);
    free(weights);
}

// Генерация пароля
void test_generate_password_length(void) {
    char* alph = "abcdef";
    char* pass = generate_password(15, alph, NULL);

    TEST_ASSERT_NOT_NULL(pass);
    // Проверяем, что длина = 15
    TEST_ASSERT_EQUAL_INT(15, strlen(pass));
    free(pass);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_config);
    RUN_TEST(test_update_separators_add);
    RUN_TEST(test_update_separators_replace);
    RUN_TEST(test_validate_config_success);
    RUN_TEST(test_validate_config_fail_no_length);
    RUN_TEST(test_validate_config_fail_conflict_length);
    RUN_TEST(test_validate_config_fail_conflict_alphabet);
    RUN_TEST(test_build_alphabet);
    RUN_TEST(test_build_weights_uniform);
    RUN_TEST(test_generate_password_length);
    return UNITY_END();
}