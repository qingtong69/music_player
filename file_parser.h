#pragma once

#include "common.h"

// 文件解析函数声明
void ParseLrcFile(const char* lrcPath, Song* song);
int ParseTimeTag(const char* timeStr);

