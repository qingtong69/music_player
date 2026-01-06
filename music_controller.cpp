#include "music_controller.h"
#include "song.h"

// 播放状态变量定义
int isPlaying = 0;      //播放的标志
int isPaused = 0;       //暂停的标志
double currentTime = 0.0;       //
double totalTime = 0.0;
int volume = 50;            // 音量 0-100
double speed = 1.0;         // 播放速度
int currentLyricIndex = 0;  // 当前歌词索引

// 播放音乐
void PlayMusic(int index) {
    if (index < 0 || index >= songCount) return;

    StopMusic();

    currentSongIndex = index;
    Song* song = &songs[index];

    // 更新播放统计信息（用于推荐）
    song->features.playCount += 1;
    song->features.lastPlayed = time(NULL);

    char cmd[512];
    sprintf(cmd, "open \"%s\" alias music", song->mp3Path);
    mciSendString(cmd, NULL, 0, NULL);

    sprintf(cmd, "play music");
    mciSendString(cmd, NULL, 0, NULL);

    SetVolume(volume);
    SetSpeed(speed);

    // 获取总时长
    char timeStr[64];
    mciSendString("status music length", timeStr, sizeof(timeStr), NULL);
    totalTime = atoi(timeStr) / 1000.0;

    isPlaying = 1;
    isPaused = 0;
    currentTime = 0.0;
    currentLyricIndex = 0;
}

// 暂停音乐
void PauseMusic() {
    if (isPlaying && !isPaused) {
        mciSendString("pause music", NULL, 0, NULL);
        isPaused = 1;
    }
}

// 恢复播放
void ResumeMusic() {
    if (isPlaying && isPaused) {
        mciSendString("resume music", NULL, 0, NULL);
        isPaused = 0;
    }
}

// 停止音乐
void StopMusic() {
    if (isPlaying) {
        mciSendString("close music", NULL, 0, NULL);
        isPlaying = 0;
        isPaused = 0;
        currentTime = 0.0;
        currentLyricIndex = 0;
    }
}

// 设置音量
void SetVolume(int vol) {
    char cmd[64];
    int mciVol = vol * 10;  // MCI音量范围是0-1000
    sprintf(cmd, "setaudio music volume to %d", mciVol);
    mciSendString(cmd, NULL, 0, NULL);
}

// 设置播放速度
void SetSpeed(double spd) {
    char cmd[64];
    int speedPercent = (int)(spd * 100);
    sprintf(cmd, "set music speed to %d", speedPercent);
    mciSendString(cmd, NULL, 0, NULL);
}

// 跳转到指定时间（秒）
void SeekTo(double timeSec) {
    if (!isPlaying || totalTime <= 0) return;

    if (timeSec < 0) timeSec = 0;
    if (timeSec > totalTime) timeSec = totalTime;

    int ms = (int)(timeSec * 1000);
    char cmd[64];

    // 先调整播放位置
    sprintf(cmd, "seek music to %d", ms);
    mciSendString(cmd, NULL, 0, NULL);

    // 如果当前是播放状态，则从新位置继续播放
    if (!isPaused) {
        sprintf(cmd, "play music from %d", ms);
        mciSendString(cmd, NULL, 0, NULL);
    }

    // 更新当前时间
    currentTime = timeSec;

    // 同步更新歌词索引
    if (currentSongIndex >= 0 && currentSongIndex < songCount) {
        Song* song = &songs[currentSongIndex];
        struct LyricLine* current = song->lyrics;
        int index = 0;
        int newIndex = 0;

        int currentMs = (int)(currentTime * 1000);

        while (current) {
            int lyricTime = current->minute * 60000 + current->second * 1000 + current->millisecond;
            if (lyricTime <= currentMs) {
                newIndex = index;
            }
            else {
                break;
            }
            current = current->next;
            index++;
        }

        currentLyricIndex = newIndex;
    }
}

// 更新当前播放时间
void UpdateCurrentTime() {
    if (!isPlaying || isPaused) return;

    char timeStr[64];
    mciSendString("status music position", timeStr, sizeof(timeStr), NULL);
    currentTime = atoi(timeStr) / 1000.0;

    // 更新歌词索引
    if (currentSongIndex >= 0 && currentSongIndex < songCount) {
        Song* song = &songs[currentSongIndex];
        struct LyricLine* current = song->lyrics;
        int index = 0;
        int newIndex = 0;

        int currentMs = (int)(currentTime * 1000);

        while (current) {
            int lyricTime = current->minute * 60000 + current->second * 1000 + current->millisecond;
            if (lyricTime <= currentMs) {
                newIndex = index;
            }
            else {
                break;
            }
            current = current->next;
            index++;
        }

        currentLyricIndex = newIndex;
    }

    // 检查是否播放完毕
    if (currentTime >= totalTime && totalTime > 0) {
        StopMusic();
    }
}