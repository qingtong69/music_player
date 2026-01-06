#include "common.h"
#include "song.h"
#include "ui.h"
#include "music_controller.h"
#include "file_parser.h"

int main() {
    // 初始化图形窗口
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

    // 加载文本、图片、歌曲初始化
    InitButtonTexts();
    LoadImages();
    LoadSongs();

    // 主循环
    MOUSEMSG msg;
    while (true) {
        // 处理鼠标消息
        if (MouseHit()) {
            msg = GetMouseMsg();
            int x = msg.x;
            int y = msg.y;

            switch (msg.uMsg) {
            case WM_LBUTTONDOWN:
                switch (currentState) {
                case TAG_SELECT:
                    HandleTagSelection(x, y);
                    break;
                case SONG_LIST:
                case FAVORITE:
                    HandleSongListClick(x, y);
                    break;
                case PLAYING:
                    HandlePlayingClick(x, y);
                    break;
                }
                break;
            case WM_MOUSEMOVE:
                if (currentState == PLAYING && msg.mkLButton) {
                    HandlePlayingMouseMove(x, y, msg.mkLButton);
                }
                break;
            case WM_LBUTTONUP:
                if (currentState == PLAYING) {
                    HandlePlayingMouseUp(x, y);
                }
                break;
            default:
                break;
            }
        }

        // 更新播放时间
        if (currentState == PLAYING) {
            UpdateCurrentTime();
        }

        // 绘制界面
        BeginBatchDraw();
        cleardevice();

        switch (currentState) {
        case TAG_SELECT:
            DrawTagSelection();
            break;
        case SONG_LIST:
        case FAVORITE:
            DrawSongList();
            break;
        case PLAYING:
            draw_playing();
            break;
        }

        EndBatchDraw();

        //Sleep(50);  // 控制刷新率
    }

    // 清理资源
    StopMusic();

    // 释放图片资源
    if (bgImage) delete bgImage;
    for (int i = 0; i < 4; i++) {
        if (tagImages[i]) delete tagImages[i];
        if (tagImages_mask[i]) delete tagImages_mask[i];
    }
    if (skipImage) delete skipImage;
    if (skipImage_mask) delete skipImage_mask;

    // 释放歌曲资源
    for (int i = 0; i < songCount; i++) {
        struct LyricLine* current = songs[i].lyrics;
        while (current) {
            struct LyricLine* next = current->next;
            free(current);
            current = next;
        }
    }
    if (songs) free(songs);

    closegraph();
    return 0;
}