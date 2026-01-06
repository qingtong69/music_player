#pragma once
#include "common.h"

// 音乐控制函数声明
void PlayMusic(int index);
void PauseMusic();
void ResumeMusic();
void StopMusic();
void SetVolume(int vol);
void SetSpeed(double spd);
void SeekTo(double timeSec);
void UpdateCurrentTime();

// 播放状态变量声明
extern int isPlaying;
extern int isPaused;
extern double currentTime;
extern double totalTime;
extern int volume;
extern double speed;
extern int currentLyricIndex;
