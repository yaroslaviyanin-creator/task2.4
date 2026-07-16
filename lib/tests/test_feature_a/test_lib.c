/*
test_lib.c - модуль проверки библиотеки.

Янин Ярослав Иванович
Группа МК-101
*/

#include "unity.h"
#include "lib_main.h"
#include <string.h>

void setUp(void) {
    // Вызывается перед каждым тестом (можно оставить пустым)
}

void tearDown(void) {
    // Вызывается после каждого теста
}

// Тест 1: Проверка стартовой инициализации структуры
void test_init_config(void) {
    GeneratorConfig cfg;
    init_config(&cfg);

    // Проверяем значения
    TEST_ASSERT_EQUAL_INT(0, cfg.min_len);
    TEST_ASSERT_EQUAL_INT(1, cfg.count);
    TEST_ASSERT_EQUAL_STRING("=:", cfg.separators);
}

// Тест 2: Проверка  -d
void test_update_separators_add(void) {
    GeneratorConfig cfg;
    init_config(&cfg);

    // Добавляем восклицательный знак
    update_separators(&cfg, "!", 0);
    // Должно стать =:!
    TEST_ASSERT_EQUAL_STRING("=:!", cfg.separators);
}

// Тест 3: Проверка -D
void test_update_separators_replace(void) {
    GeneratorConfig cfg;
    init_config(&cfg);

    // Заменяем всё на тире
    update_separators(&cfg, "-", 1);
    TEST_ASSERT_EQUAL_STRING("-", cfg.separators);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_config);
    RUN_TEST(test_update_separators_add);
    RUN_TEST(test_update_separators_replace);
    return UNITY_END();
}