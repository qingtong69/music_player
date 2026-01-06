#pragma once

#include "common.h"

// 歌曲管理函数声明
void LoadSongs();
void LoadFavoriteList();
void SaveFavoriteList();
void AddToFavorite(int index);
void GetRecommendedSong(TagType tag);

// 全局变量声明
extern Song* songs;
extern int songCount;
extern int currentSongIndex;
extern State currentState;
extern TagType selectedTag;
extern int showFavoriteOnly;
