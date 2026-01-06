#pragma once

#include "common.h"

// 图片资源声明
extern IMAGE* bgImage;
extern IMAGE* tagImages[4];
extern IMAGE* tagImages_mask[4];
extern IMAGE* skipImage;
extern IMAGE* skipImage_mask;
extern IMAGE* favoriteImage;
extern IMAGE* favoriteImage_mask;

// 按钮声明
extern Button play;
extern Button allSongs;
extern Button myFavorite;
extern Button pause;
extern Button back;
extern Button volup;
extern Button voldown;
extern Button speedup;
extern Button speeddown;
extern Button favor;
extern Button listPlay;
extern Button listAllSongs;
extern Button listMyFavorite;
extern Button listBack;

// 界面函数声明
void LoadImages();
void InitButtonTexts();
void DrawTagSelection();
void DrawSongList();
void draw_playing();
void HandleTagSelection(int x, int y);
void HandleSongListClick(int x, int y);
void HandlePlayingClick(int x, int y);
void HandlePlayingMouseMove(int x, int y, int isLeftDown);
void HandlePlayingMouseUp(int x, int y);

