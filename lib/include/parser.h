/*
parser.h - разбор аргументов командной строки.

Янин Ярослав Иванович
Группа МК-101
*/
#pragma once
#include "config.h"

void update_separators(TConfig* cfg, const char* new_seps, int replace);
int parse_args(int argc, char* argv[], TConfig* cfg);