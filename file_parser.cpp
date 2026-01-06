#include "file_parser.h"
#include <stdio.h>

// 解析LRC歌词文件
void ParseLrcFile(const char* lrcPath, Song* song) {
    FILE* fp = fopen(lrcPath, "r");
    if (!fp) return;

    char line[256];  // 减小缓冲区大小
    struct LyricLine* head = NULL;
    struct LyricLine* tail = NULL;
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        char* current = line;

        // 去除行尾换行符
        line[strcspn(line, "\r\n")] = '\0';

        while (*current == '[') {

            //解析时间标签，获得总时长
            int time = ParseTimeTag(current);
            //接收到-1直接跳出循环
            if (time < 0)
                break;

            char* tagEnd = strchr(current, ']');

            if (!tagEnd)
                break;

            // 分配内存并检查是否成功
            struct LyricLine* lyric = (struct LyricLine*)malloc(sizeof(struct LyricLine));
            if (!lyric) {
                fclose(fp);
                return;  // 内存分配失败
            }

            lyric->minute = time / 60000;
            lyric->second = (time % 60000) / 1000;
            lyric->millisecond = time % 1000;
            lyric->next = NULL;

            current = tagEnd + 1;       //将指针移动到"]"后面

            // 复制歌词文本
            if (*current != '[' && *current != '\0') {
                strncpy(lyric->text, current, sizeof(lyric->text) - 1);
                lyric->text[sizeof(lyric->text) - 1] = '\0';
            }
            else {
                lyric->text[0] = '\0';      //只有时间标签和有多个时间标签时为空
            }

            // 利用链表
            if (!head) {
                head = tail = lyric;
            }
            else {
                tail->next = lyric;
                tail = lyric;
            }
            count++;

            if (*current != '[') break;
        }
    }

    fclose(fp);
    song->lyrics = head;
    song->lyricCount = count;
}

// 解析时间标签 [mm:ss.xx]，返回毫秒数
int ParseTimeTag(const char* timeStr) {
    int minute = 0, second = 0, millisecond = 0;
    if (sscanf(timeStr, "[%d:%d.%d]", &minute, &second, &millisecond) == 3) {
        return minute * 60000 + second * 1000 + millisecond;
    }
    return -1;
}