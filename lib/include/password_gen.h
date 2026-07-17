/*
password_gen.h - функции генерации пароля.

Янин Ярослав Иванович
Группа МК-101
*/
#pragma once
#include "config.h"

char* build_alphabet(const TConfig* cfg);
double* build_weights(const TConfig* cfg, const char* alphabet);
char* generate_password(int length, const char* alphabet, const double* weights);