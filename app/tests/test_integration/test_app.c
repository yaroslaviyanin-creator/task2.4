/*
test_app.c - интеграционные тесты программы.

Янин Ярослав Иванович
Группа МК-101
*/

#include "unity.h"
#include "lib_main.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_dummy(void) {
    // Простая заглушка, чтобы файл компилировался без ошибок
    TEST_ASSERT_TRUE(1);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dummy);
    return UNITY_END();
}