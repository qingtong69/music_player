#ifndef COMMON_H
#define COMMON_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <graphics.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

// 窗口大小
#define WINDOW_WIDTH 1357
#define WINDOW_HEIGHT 757

// 标签类型
enum TagType {
    all_songs = 0,   // 全部歌曲
    popular = 1,    // 流行
    classic = 2,    // 经典
    national = 3,   // 国风
    pure = 4,       // 纯音乐
    favorite = 5    // 收藏
};

// 程序状态
enum State {
    TAG_SELECT,   // 标签选择界面
    SONG_LIST,    // 歌曲列表界面
    PLAYING,      // 播放界面
    FAVORITE      // 我的歌单界面
};

// 歌词行结构
struct LyricLine {
    int minute;         //由于lrc文件中有时间，所以这里可以根据歌词前的时间设计滚动播放
    int second;
    int millisecond;
    char text[256];     //歌词文本部分
    struct LyricLine* next;     //利用单链表实现滚动效果
};

// 歌曲特征结构（用于推荐）
struct SongFeatures {
    int playCount;          // 播放次数
    time_t lastPlayed;      // 最近播放时间
    double playFrequency;   // 播放频率（0-1）
    double mood;            // 情绪值
    double style;           // 风格匹配度
    double tempo;           // 节奏
    double basePreference;  // 静态偏好基准
};

// 歌曲信息结构
struct Song {
    char folder[64];        // 文件夹名
    char name[128];         // 歌曲名
    char artist[128];        // 作者
    char mp3Path[256];      // mp3路径
    char photoPath[256];    // 歌曲对应的照片路径
    char lrcPath[256];      // 歌词路径
    TagType tag;            // 歌曲标签
    struct LyricLine* lyrics; // 歌词链表
    int lyricCount;         // 歌词行数
    int isFavorite;         // 是否收藏
    SongFeatures features;  // 推荐相关特征
};

// 按钮设计
struct Button {
    int x, y, w, h;
    char text[64];
};

// 辅助函数声明
void safe_strcpy(char* dest, const char* src, int maxLen);
void draw_text(int x, int y, const char* text, int fontSize, COLORREF color);
void draw_button(Button* btn, int isHover);
int is_inbutton(int x, int y, Button* btn);

#endif