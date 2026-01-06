#include "ui.h"
#include "music_controller.h"
#include "song.h"
#include "recommend.h"
#include <stdio.h>
#include <wchar.h>

// 贴图
IMAGE* bgImage = NULL;
IMAGE* tagImages[4] = { NULL };      // 标签白底图
IMAGE* tagImages_mask[4] = { NULL };  // 标签掩码图
IMAGE* skipImage = NULL;        //跳过按钮
IMAGE* skipImage_mask = NULL;
IMAGE* favoriteImage = NULL;        //收藏按钮
IMAGE* favoriteImage_mask = NULL;

Button play = { 350, 480, 150, 40, "" };        //播放键
Button allSongs = { 550, 480, 150, 40, "" };        //跳转全部歌曲
Button myFavorite = { 750, 480, 150, 40, "" };      //收藏
Button pause = { 406, 668, 80, 30, "" };      //暂停
Button back = { 530, 668, 80, 30, "" };         //返回
Button volup = { 642, 668, 60, 30, "" };      //调高音量
Button voldown = { 752, 668, 60, 30, "" };        //调低音量
Button speedup = { 865, 668, 60, 30, "" };        //调高速率
Button speeddown = { 971, 668, 60, 30, "" };      //调低速率
Button favor = { 1084, 668, 60, 30, "" };      //收藏

// 歌曲列表界面右侧竖排按钮
Button listPlay = { 1187, 150, 150, 40, "" };
Button listAllSongs = { 1187, 210, 150, 40, "" };
Button listMyFavorite = { 1187, 270, 150, 40, "" };
Button listBack = { 1187, 330, 150, 40, "" };

// 播放进度条区域（需与 draw_playing / 交互保持一致）
static const int PROGRESS_X = 156;
static const int PROGRESS_Y = 632;
static const int PROGRESS_W = 1056;
static const int PROGRESS_H = 10;
static const int PROGRESS_VERTICAL_TOLERANCE = 10;

static int isDraggingProgress = 0;

//辅助函数:当缓冲区还有位置且源文件未结束时复制字符再输出
void safe_strcpy(char* dest, const char* src, int maxLen) {
    int i = 0;
    while (i < maxLen - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// 绘制透明文字封装
void draw_text(int x, int y, const char* text, int fontSize, COLORREF color) {
    settextcolor(color);
    settextstyle(fontSize, 0, _T("黑体"));
    setbkmode(TRANSPARENT);
    outtextxy(x, y, text);
}

// 绘制按钮
void draw_button(Button* btn, int isHover) {
    COLORREF bgColor = isHover ? RGB(122, 140, 226) : RGB(254, 204, 213);
    COLORREF borderColor = RGB(200, 200, 200);

    setfillcolor(bgColor);
    fillrectangle(btn->x, btn->y, btn->x + btn->w, btn->y + btn->h);

    setlinecolor(borderColor);
    rectangle(btn->x, btn->y, btn->x + btn->w, btn->y + btn->h);

    draw_text(btn->x + 10, btn->y + 10, btn->text, 20, WHITE);
}

// 判断点是否在按钮内
int is_inbutton(int x, int y, Button* btn) {
    return (x >= btn->x && x <= btn->x + btn->w &&
        y >= btn->y && y <= btn->y + btn->h);
}

// 加载图片资源
void LoadImages() {
    bgImage = new IMAGE;
    loadimage(bgImage, "music/back1.jpg");

    // 加载标签图片
    char path[256];
    for (int i = 0; i < 4; i++) {
        sprintf(path, "music/biaoqiantu/biaoqian%d.jpg", i + 1);
        tagImages[i] = new IMAGE;
        loadimage(tagImages[i], path, 250, 250, 1);

        sprintf(path, "music/biaoqiantu/biaoqian%d.1.jpg", i + 1);
        tagImages_mask[i] = new IMAGE;
        loadimage(tagImages_mask[i], path, 250, 250, 1);
    }

    // 加载跳过按钮
    //skipImage = new IMAGE;
    //loadimage(skipImage, "music/biaoqiantu/biaoqian5.jpg", 69*3, 46*3, 1);
    //skipImage_mask = new IMAGE;
    //loadimage(skipImage_mask, "music/biaoqiantu/biaoqian5.1.jpg", 69*3, 46*3, 1);
}

// 初始化按钮文本
void InitButtonTexts() {
    strcpy(play.text, _T("播放音乐"));  // 播放音乐
    strcpy(allSongs.text, _T("全部歌曲"));  // 全部歌曲
    strcpy(myFavorite.text, _T("我的歌单"));  // 我的歌单
    strcpy(pause.text, _T("暂停"));  // 暂停
    strcpy(back.text, _T("返回"));  // 返回
    strcpy(volup.text, _T("音量+"));  // 音量+
    strcpy(voldown.text, _T("音量-"));  // 音量-
    strcpy(speedup.text, _T("速度+"));  // 速度+
    strcpy(speeddown.text, _T("速度-"));  // 速度-
    strcpy(favor.text, _T("收藏"));  // 收藏
    // 歌曲列表界面右侧按钮文本
    safe_strcpy(listPlay.text, play.text, 64);
    safe_strcpy(listAllSongs.text, allSongs.text, 64);
    safe_strcpy(listMyFavorite.text, myFavorite.text, 64);
    safe_strcpy(listBack.text, back.text, 64);
}

// 绘制标签选择界面
void DrawTagSelection() {
    // 绘制背景
    putimage(0, 0, bgImage);

    // 标题与说明（GBK转义输出，避免编码问题）
    draw_text(WINDOW_WIDTH / 2 - 200, 40,
        _T("欢迎来到萌萌音乐"), 56, WHITE);  //欢迎来到萌萌音乐
    draw_text(WINDOW_WIDTH / 2 - 450, 130,
        _T("请选择你喜欢的标签"), 36, WHITE);      //请选择你喜欢的标签
    // 绘制四个标签按钮
    int tagX[] = { 271, 271 * 2, 271 * 3, 271 * 4 };
    int tagY = 200;
    char tagNames[5][32];
    strcpy(tagNames[0], _T("流行"));  // 流行
    strcpy(tagNames[1], _T("经典"));  // 经典
    strcpy(tagNames[2], _T("国风"));  // 国风
    strcpy(tagNames[3], _T("纯音乐"));  // 纯音乐
    strcpy(tagNames[4], _T("跳过"));        // 跳过

    int i;
    for (i = 0; i < 4; i++) {
        // 使用掩码图实现透明效果
        putimage(tagX[i] - 100, tagY, tagImages_mask[i], NOTSRCERASE);
        putimage(tagX[i] - 100, tagY, tagImages[i], SRCINVERT);

        // 绘制标签文字
        draw_text(tagX[i], tagY + 200, tagNames[i], 36, WHITE);
    }

    // 绘制跳过按钮（右下角），透明仅文字
    int skipX = WINDOW_WIDTH - 220;
    int skipY = WINDOW_HEIGHT - 160;
    draw_text(skipX, skipY, tagNames[4], 36, WHITE);
}

// 绘制歌曲列表界面
void DrawSongList() {
    // 绘制背景
    putimage(0, 0, bgImage);

    // 绘制标题
    char tagNames[7][32];
    strcpy(tagNames[0], _T("全部歌曲"));  // 全部歌曲
    strcpy(tagNames[1], _T("流行"));  // 流行
    strcpy(tagNames[2], _T("经典"));  // 经典
    strcpy(tagNames[3], _T("国风"));  // 国风
    strcpy(tagNames[4], _T("纯音乐"));  // 纯音乐
    strcpy(tagNames[5], _T("我的歌单"));  // 我的歌单


    char title[128];
    if (showFavoriteOnly) {
        sprintf(title, "%s", tagNames[5]);
    }
    else if (selectedTag == all_songs) {
        sprintf(title, "%s", tagNames[0]);
    }
    else {
        char temp[128];
        sprintf(temp,
            _T("%s - 歌曲列表"), tagNames[selectedTag]);  // 歌曲列表
        strcpy(title, temp);
    }
    draw_text(50, 50, title, 32, WHITE);

    // 绘制歌曲列表
    int y = 120;
    int count = 0;
    int twoColumn = (!showFavoriteOnly && selectedTag == all_songs) ? 1 : 0;
    int colX1 = 100;
    int colX2 = 650;
    int rowHeight = 48;
    for (int i = 0; i < songCount; i++) {
        int shouldShow = 0;
        if (showFavoriteOnly) {
            // 我的歌单i
            shouldShow = songs[i].isFavorite;
        }
        else if (selectedTag == all_songs) {
            // 全部歌曲
            shouldShow = 1;
        }
        else {
            // 按标签筛选
            shouldShow = (songs[i].tag == selectedTag);
        }

        if (shouldShow) {
            char songInfo[256];
            sprintf(songInfo, "%d. %s - %s", count + 1, songs[i].name, songs[i].artist);
            // 高亮当前选中的歌曲
            COLORREF color = (i == currentSongIndex) ? YELLOW : WHITE;
            settextcolor(color);
            settextstyle(24, 0, "黑体");
            setbkmode(TRANSPARENT);
            if (twoColumn) {
                int row = count / 2;
                int col = count % 2;
                int x = (col == 0) ? colX1 : colX2;
                int yy = y + row * rowHeight;
                outtextxy(x, yy, songInfo);
            }
            else {
                outtextxy(100, y, songInfo);
                y += rowHeight;
            }
            count++;
        }
    }

    // 右侧竖排按钮
    draw_button(&listAllSongs, 0);
    draw_button(&listPlay, 0);
    draw_button(&listMyFavorite, 0);
    draw_button(&listBack, 0);

    // 底部推荐区域
    DrawRec();
}

// 进度条辅助函数
static int IsPointInProgressBar(int x, int y) {
    return (x >= PROGRESS_X && x <= PROGRESS_X + PROGRESS_W &&
        y >= PROGRESS_Y - PROGRESS_VERTICAL_TOLERANCE &&
        y <= PROGRESS_Y + PROGRESS_H + PROGRESS_VERTICAL_TOLERANCE);
}

static void SeekByProgressPosition(int x) {
    if (totalTime <= 0) return;

    int clampedX = x;
    if (clampedX < PROGRESS_X) clampedX = PROGRESS_X;
    if (clampedX > PROGRESS_X + PROGRESS_W) clampedX = PROGRESS_X + PROGRESS_W;

    double ratio = (double)(clampedX - PROGRESS_X) / PROGRESS_W;
    double newTime = totalTime * ratio;
    SeekTo(newTime);
}

// 绘制播放界面
void draw_playing() {

    //检查索引是否在歌单内
    if (currentSongIndex < 0 || currentSongIndex >= songCount)
        return;

    Song* song = &songs[currentSongIndex];

    putimage(0, 0, bgImage);

    // 在界面左侧显示照片
    IMAGE* photo = new IMAGE;
    if (loadimage(photo, song->photoPath, 352, 352, 1) == 0) {
        int photoSize = 352;
        int photoX = 157;
        int photoY = 110;
        putimage(photoX, photoY, photo);

        // 照片下方显示歌曲名和作者
        draw_text(photoX, photoY + photoSize + 20, song->name, 30, WHITE);
        draw_text(photoX, photoY + photoSize + 50, song->artist, 24, WHITE);
    }
    delete photo;

    //在界面右侧显示歌词,并做出滚动效果
    int lyricX = 635;       //歌词区域坐标
    int lyricY = 111;
    int maxLines = 10;      //最多显示十行歌词
    int lineHeight = 50;        //歌词的高度

    // 找到当前应该显示的歌词索引 ，计算滚动的起始位置
    int displayStart = currentLyricIndex;   //当前歌词索引
    if (displayStart < 0)
        displayStart = 0;
    if (displayStart > song->lyricCount - maxLines) {
        displayStart = song->lyricCount - maxLines;
        if (displayStart < 0)
            displayStart = 0;
    }

    //遍历歌词链表，找到显示起始位置的节点
    struct LyricLine* current = song->lyrics;       //遍历指针
    int index = 0;
    while (current && index < displayStart) {       //当前指针不为空且索引没有达到起始节点
        current = current->next;
        index++;
    }

    int yOffset = lyricY;       //歌词绘制的坐标
    int highlightIndex = currentLyricIndex - displayStart;      //高光显示当前播放的相对位置

    //绘制歌词
    for (int i = 0; i < maxLines && current; i++) {
        COLORREF color = (i == highlightIndex) ? YELLOW : WHITE;        //黄色显示高光
        int fontSize = (i == highlightIndex) ? 42 : 34;
        draw_text(lyricX, yOffset, current->text, fontSize, color);
        yOffset += lineHeight;
        current = current->next;
    }

    // 底部：进度条
    setfillcolor(RGB(254, 204, 213));
    fillrectangle(PROGRESS_X, PROGRESS_Y, PROGRESS_X + PROGRESS_W, PROGRESS_Y + PROGRESS_H);

    // 绘制进度
    if (totalTime > 0) {
        int progress = (int)((currentTime / totalTime) * PROGRESS_W);
        setfillcolor(RGB(122, 140, 226));
        fillrectangle(PROGRESS_X, PROGRESS_Y, PROGRESS_X + progress, PROGRESS_Y + PROGRESS_H);
    }

    // 显示时间
    char timeStr[64];
    int curMin = (int)(currentTime / 60);
    int curSec = (int)(currentTime) % 60;
    int totMin = (int)(totalTime / 60);
    int totSec = (int)(totalTime) % 60;
    sprintf(timeStr, "%02d:%02d / %02d:%02d", curMin, curSec, totMin, totSec);
    draw_text(PROGRESS_X, PROGRESS_Y + 15, timeStr, 16, WHITE);

    // 绘制控制按钮
    draw_button(&pause, 0);
    draw_button(&back, 0);
    draw_button(&volup, 0);
    draw_button(&voldown, 0);
    draw_button(&speedup, 0);
    draw_button(&speeddown, 0);
    draw_button(&favor, 0);

    // 显示音量和速度
    char infoStr[128];
    char volText[32], speedText[32];
    strcpy(volText, _T("音量"));  // 音量
    strcpy(speedText, _T("速度"));  // 速度
    sprintf(infoStr, "%s: %d%%  %s: %.1fx", volText, volume, speedText, speed);
    draw_text(1071, 548, infoStr, 24, WHITE);

    // 显示收藏状态
    if (song->isFavorite) {
        char favText[32];
        strcpy(favText, _T("已收藏"));  // 已收藏
        draw_text(1160, 30, favText, 30, RED);
    }
}

// 处理标签选择点击
void HandleTagSelection(int x, int y) {
    // 与绘制时保持一致：按钮绘制在 (tagX[i]-100, tagY)，尺寸 250x250
    int tagX[] = { 271, 271 * 2, 271 * 3, 271 * 4 };
    int tagY = 200;
    int tagW = 250;
    int tagH = 250;

    // 检查标签按钮
    for (int i = 0; i < 4; i++) {
        int x1 = tagX[i] - 100;
        int y1 = tagY;
        if (x >= x1 && x <= x1 + tagW &&
            y >= y1 && y <= y1 + tagH) {
            selectedTag = (TagType)(i + 1);
            showFavoriteOnly = 0;
            GetRecommendedSong(selectedTag);
            currentState = SONG_LIST;
            return;
        }
    }

    // 检查跳过按钮：跳转到“全部歌曲”列表，并生成决策树推荐
    int skipX = WINDOW_WIDTH - 220;
    int skipY = WINDOW_HEIGHT - 160;
    if (x >= skipX && x <= skipX + 207 && y >= skipY && y <= skipY + 138) {
        showFavoriteOnly = 0;
        selectedTag = all_songs;
        // 控制台输出一次详细推荐理由
        Recommend(all_songs);
        // 生成界面上的推荐按钮
        GenerateSongs(all_songs);
        currentState = SONG_LIST;
        return;
    }
}

// 处理歌曲列表点击
void HandleSongListClick(int x, int y) {
    // 检查播放音乐按钮（列表界面右侧）
    if (is_inbutton(x, y, &listPlay)) {
        if (currentSongIndex >= 0) {
            PlayMusic(currentSongIndex);
            currentState = PLAYING;
        }
        else {
            // 如果没有选中歌曲，推荐一首歌曲,高亮显示
            GetRecommendedSong(selectedTag);
            if (currentSongIndex >= 0) {
                PlayMusic(currentSongIndex);
                currentState = PLAYING;
            }
        }
        return;
    }

    // 返回到标签选择
    if (is_inbutton(x, y, &listBack)) {
        currentState = TAG_SELECT;
        showFavoriteOnly = 0;
        return;
    }

    // 检查全部歌曲按钮
    if (is_inbutton(x, y, &listAllSongs)) {
        selectedTag = all_songs;  // 显示所有歌曲
        showFavoriteOnly = 0;
        currentState = SONG_LIST;
        return;
    }

    // 检查我的歌单按钮
    if (is_inbutton(x, y, &listMyFavorite)) {
        showFavoriteOnly = 1;  // 显示我的歌单
        currentState = SONG_LIST;
        return;
    }

    // 检查是否点击了歌曲列表项（坐标计算需与 DrawSongList 完全一致）
    int startY = 120;           // 列表起始 y 坐标
    int lineHeight = 48;        // 行高，与 DrawSongList 中的 rowHeight 一致
    int count = 0;              //用于计算当前是第几首

    //当展示的不是“我的歌单”且要显示全部歌曲时进行两列绘制
    int twoColumn = (!showFavoriteOnly && selectedTag == all_songs) ? 1 : 0;

    for (int i = 0; i < songCount; i++) {

        //判断这首歌要不要显示
        int shouldShow = 0;
        if (showFavoriteOnly) {
            shouldShow = songs[i].isFavorite;       //我的歌单
        }
        else if (selectedTag == all_songs) {
            shouldShow = 1;                         //全部歌曲
        }
        else {
            shouldShow = (songs[i].tag == selectedTag);     //根据标签选择
        }

        // 判定可以展示才参与点击判断
        if (shouldShow) {
            if (twoColumn) {
                // 双列模式：行列计算与 DrawSongList 一致，
                // 但左右列各自有独立的点击区域，避免右列触发左列歌曲
                int row = count / 2;
                int col = count % 2;

                int colX1 = 100;   // 左列文本起点
                int colX2 = 650;   // 右列文本起点
                int itemY = startY + row * lineHeight;
                int maxX = WINDOW_WIDTH - 200;

                if (y >= itemY && y <= itemY + lineHeight) {
                    if (col == 0) {
                        // 左列：只在左半区域内响应
                        int itemX1 = colX1;
                        int itemX2 = colX2 - 50;  // 留一点空隙，避免与右列重叠
                        if (x >= itemX1 && x <= itemX2) {
                            currentSongIndex = i;
                            PlayMusic(i);
                            currentState = PLAYING;
                            return;
                        }
                    }
                    else {
                        // 右列：只在右半区域内响应
                        int itemX1 = colX2;
                        int itemX2 = maxX;
                        if (x >= itemX1 && x <= itemX2) {
                            currentSongIndex = i;
                            PlayMusic(i);
                            currentState = PLAYING;
                            return;
                        }
                    }
                }
            }
            else {
                int itemY = startY + count * lineHeight;
                int maxX = WINDOW_WIDTH - 200;
                if (x >= 100 && x <= maxX && y >= itemY && y <= itemY + lineHeight) {
                    currentSongIndex = i;
                    PlayMusic(i);
                    currentState = PLAYING;
                    return;
                }
            }
            count++;
        }
    }

    // 检查是否点击了底部推荐歌曲按钮
    RecClick(x, y);
}

// 处理播放界面点击
void HandlePlayingClick(int x, int y) {
    if (is_inbutton(x, y, &pause)) {
        if (isPaused) {
            ResumeMusic();
        }
        else {
            PauseMusic();
        }
        return;
    }

    if (is_inbutton(x, y, &back)) {
        StopMusic();
        currentState = SONG_LIST;
        return;
    }

    if (is_inbutton(x, y, &volup)) {
        volume = (volume + 10 > 100) ? 100 : volume + 10;
        SetVolume(volume);
        return;
    }

    if (is_inbutton(x, y, &voldown)) {
        volume = (volume - 10 < 0) ? 0 : volume - 10;
        SetVolume(volume);
        return;
    }

    if (is_inbutton(x, y, &speedup)) {
        speed = (speed + 0.1 > 2.0) ? 2.0 : speed + 0.1;
        SetSpeed(speed);
        return;
    }

    if (is_inbutton(x, y, &speeddown)) {
        speed = (speed - 0.1 < 0.5) ? 0.5 : speed - 0.1;
        SetSpeed(speed);
        return;
    }

    if (is_inbutton(x, y, &favor)) {
        AddToFavorite(currentSongIndex);
        return;
    }

    // 点击进度条进入拖动模式
    if (IsPointInProgressBar(x, y)) {
        isDraggingProgress = 1;
        SeekByProgressPosition(x);
        return;
    }
}

void HandlePlayingMouseMove(int x, int y, int isLeftDown) {
    if (!isDraggingProgress) return;

    if (!isLeftDown) {
        isDraggingProgress = 0;
        return;
    }

    SeekByProgressPosition(x);
}

void HandlePlayingMouseUp(int x, int y) {
    if (!isDraggingProgress) return;

    SeekByProgressPosition(x);
    isDraggingProgress = 0;
}
