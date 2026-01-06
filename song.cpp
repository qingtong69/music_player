#include "song.h"
#include "file_parser.h"
#include "recommend.h"

// 全局变量定义
Song* songs = NULL;
int songCount = 0;
int currentSongIndex = -1;
State currentState = TAG_SELECT;
TagType selectedTag = popular;
int showFavoriteOnly = 0;

// 加载歌曲、作者、歌词等等
void LoadSongs() {
    songCount = 20;
    songs = (Song*)malloc(sizeof(Song) * songCount);
    memset(songs, 0, sizeof(Song) * songCount);

    // 初始化第一首歌曲
    strcpy(songs[0].folder, "daoxiang");
    safe_strcpy(songs[0].name, _T("稻香"), 128);  // 稻香
    safe_strcpy(songs[0].artist, _T("周杰伦"), 128);  // 周杰伦
    strcpy(songs[0].mp3Path, "music\\daoxiang\\daoxiang.mp3");
    strcpy(songs[0].photoPath, "music\\daoxiang\\photo.jpg");
    safe_strcpy(songs[0].lrcPath, "music\\daoxiang\\daoxiang.lrc", 256);
    songs[0].tag = popular;
    songs[0].lyrics = NULL;
    songs[0].lyricCount = 0;
    songs[0].isFavorite = 0;

    // 初始化第二首歌曲
    strcpy(songs[1].folder, "qinghuaci");
    safe_strcpy(songs[1].name, _T("青花瓷"), 128);  // 青花瓷
    safe_strcpy(songs[1].artist, _T("周杰伦"), 128);  // 周杰伦
    strcpy(songs[1].mp3Path, "music\\qinghuaci\\qinghuaci.mp3");
    strcpy(songs[1].photoPath, "music\\qinghuaci\\photo.jpg");
    safe_strcpy(songs[1].lrcPath, "music\\qinghuaci\\qinghuaci.lrc", 256);
    songs[1].tag = national;
    songs[1].lyrics = NULL;
    songs[1].lyricCount = 0;
    songs[1].isFavorite = 0;

    // 初始化第三首歌曲
    strcpy(songs[2].folder, "yeqv");
    safe_strcpy(songs[2].name, _T("夜曲"), 128);  // 夜曲
    safe_strcpy(songs[2].artist, _T("周杰伦"), 128);  // 周杰伦
    strcpy(songs[2].mp3Path, "music\\yeqv\\yeqv.mp3");
    strcpy(songs[2].photoPath, "music\\yeqv\\photo.jpg");
    safe_strcpy(songs[2].lrcPath, "music\\yeqv\\yeqv.lrc", 256);
    songs[2].tag = popular;
    songs[2].lyrics = NULL;
    songs[2].lyricCount = 0;
    songs[2].isFavorite = 0;

    // 初始化第四首歌曲
    strcpy(songs[3].folder, "wohuainiande");
    safe_strcpy(songs[3].name, _T("我怀念的"), 128);  // 我怀念的
    safe_strcpy(songs[3].artist, _T("孙燕姿"), 128);  // 孙燕姿
    strcpy(songs[3].mp3Path, "music\\wohuainiande\\wohuainiande.mp3");
    strcpy(songs[3].photoPath, "music\\wohuainiande\\photo.jpg");
    safe_strcpy(songs[3].lrcPath, "music\\wohuainiande\\wohuainiande.lrc", 256);
    songs[3].tag = popular;
    songs[3].lyrics = NULL;
    songs[3].lyricCount = 0;
    songs[3].isFavorite = 0;

    // 初始化第五首歌曲
    strcpy(songs[4].folder, "xiaochenggushi");
    safe_strcpy(songs[4].name, _T("小城故事"), 128);  // 小城故事
    safe_strcpy(songs[4].artist, _T("邓丽君"), 128);  // 邓丽君
    strcpy(songs[4].mp3Path, "music\\xiaochenggushi\\xiaochenggushi.mp3");
    strcpy(songs[4].photoPath, "music\\xiaochenggushi\\photo.jpg");
    safe_strcpy(songs[4].lrcPath, "music\\xiaochenggushi\\xiaochenggushi.lrc", 256);
    songs[4].tag = classic;
    songs[4].lyrics = NULL;
    songs[4].lyricCount = 0;
    songs[4].isFavorite = 0;

    // 初始化第六首歌曲
    strcpy(songs[5].folder, "tongzhuodeni");
    safe_strcpy(songs[5].name, _T("同桌的你"), 128);  // 同桌的你
    safe_strcpy(songs[5].artist, _T("老狼"), 128);  // 老狼
    strcpy(songs[5].mp3Path, "music\\tongzhuodeni\\tongzhuodeni.mp3");
    strcpy(songs[5].photoPath, "music\\tongzhuodeni\\photo.jpg");
    safe_strcpy(songs[5].lrcPath, "music\\tongzhuodeni\\tongzhuodeni.lrc", 256);
    songs[5].tag = classic;
    songs[5].lyrics = NULL;
    songs[5].lyricCount = 0;
    songs[5].isFavorite = 0;

    // 初始化第七首歌曲
    strcpy(songs[6].folder, "niaozhishi");
    safe_strcpy(songs[6].name, "Poem of Birds", 128);  // 鸟之诗
    safe_strcpy(songs[6].artist, "TypeD", 128);  // 无
    strcpy(songs[6].mp3Path, "music\\niaozhishi\\niaozhishi.mp3");
    strcpy(songs[6].photoPath, "music\\niaozhishi\\photo.jpg");
    safe_strcpy(songs[6].lrcPath, "music\\niaozhishi\\niaozhishi.lrc", 256);
    songs[6].tag = pure;
    songs[6].lyrics = NULL;
    songs[6].lyricCount = 0;
    songs[6].isFavorite = 0;

    // 初始化第八首歌曲
    strcpy(songs[7].folder, "ruyuan");
    safe_strcpy(songs[7].name, _T("如愿"), 128);  // 如愿
    safe_strcpy(songs[7].artist, _T("王菲"), 128);  // 王菲
    strcpy(songs[7].mp3Path, "music\\ruyuan\\ruyuan.mp3");
    strcpy(songs[7].photoPath, "music\\ruyuan\\photo.jpg");
    safe_strcpy(songs[7].lrcPath, "music\\ruyuan\\ruyuan.lrc", 256);
    songs[7].tag = popular;
    songs[7].lyrics = NULL;
    songs[7].lyricCount = 0;
    songs[7].isFavorite = 0;

    // 初始化第九首歌曲
    strcpy(songs[8].folder, "wohewodezuguo");
    safe_strcpy(songs[8].name, "我和我的祖国", 128);  // 我和我的祖国
    safe_strcpy(songs[8].artist, _T("王菲"), 128);  // 王菲
    strcpy(songs[8].mp3Path, "music\\wohewodezuguo\\wohewodezuguo.mp3");
    strcpy(songs[8].photoPath, "music\\wohewodezuguo\\photo.jpg");
    safe_strcpy(songs[8].lrcPath, "music\\wohewodezuguo\\wohewodezuguo.lrc", 256);
    songs[8].tag = classic;
    songs[8].lyrics = NULL;
    songs[8].lyricCount = 0;
    songs[8].isFavorite = 0;

    // 初始化第十首歌曲 - 红昭愿
    strcpy(songs[9].folder, "hongzhaoyuan");
    safe_strcpy(songs[9].name, _T("红昭愿"), 128);     //红昭愿
    safe_strcpy(songs[9].artist, _T("音阕诗听"), 128);       //音阕诗听
    strcpy(songs[9].mp3Path, "music\\hongzhaoyuan\\hongzhaoyuan.mp3");
    strcpy(songs[9].photoPath, "music\\hongzhaoyuan\\hongzhaoyuan.jpg");
    safe_strcpy(songs[9].lrcPath, "music\\hongzhaoyuan\\hongzhaoyuan.lrc", 256);
    songs[9].tag = national;
    songs[9].lyrics = NULL;
    songs[9].lyricCount = 0;
    songs[9].isFavorite = 0;

    // 初始化第十一首歌曲 - 半壶纱
    strcpy(songs[10].folder, "banhusha");
    safe_strcpy(songs[10].name, _T("半壶纱"), 128);
    safe_strcpy(songs[10].artist, _T("刘珂矣"), 128);
    strcpy(songs[10].mp3Path, "music\\banhusha\\banhusha.mp3");
    strcpy(songs[10].photoPath, "music\\banhusha\\photo.jpg");
    safe_strcpy(songs[10].lrcPath, "music\\banhusha\\banhusha.lrc", 256);
    songs[10].tag = national;
    songs[10].lyrics = NULL;
    songs[10].lyricCount = 0;
    songs[10].isFavorite = 0;

    // 初始化第十二首歌曲 - 匆匆那年
    strcpy(songs[11].folder, "congcongnanian");
    safe_strcpy(songs[11].name, _T("匆匆那年"), 128);
    safe_strcpy(songs[11].artist, _T("王菲"), 128);
    strcpy(songs[11].mp3Path, "music\\congcongnanian\\congcongnanian.mp3");
    strcpy(songs[11].photoPath, "music\\congcongnanian\\photo.jpg");
    safe_strcpy(songs[11].lrcPath, "music\\congcongnanian\\congcongnanian.lrc", 256);
    songs[11].tag = classic;
    songs[11].lyrics = NULL;
    songs[11].lyricCount = 0;
    songs[11].isFavorite = 0;

    // 初始化第十三首歌曲 - Flower Dance
    strcpy(songs[12].folder, "flowerdance");
    safe_strcpy(songs[12].name, "Flower Dance", 128);
    safe_strcpy(songs[12].artist, "DJ Okawari", 128);
    strcpy(songs[12].mp3Path, "music\\flowerdance\\flowerdance.mp3");
    strcpy(songs[12].photoPath, "music\\flowerdance\\photo.jpg");
    safe_strcpy(songs[12].lrcPath, "music\\flowerdance\\flowerdance.lrc", 256);
    songs[12].tag = pure;
    songs[12].lyrics = NULL;
    songs[12].lyricCount = 0;
    songs[12].isFavorite = 0;

    // 初始化第十四首歌曲 - 关山酒
    strcpy(songs[13].folder, "guanshanjiu");
    safe_strcpy(songs[13].name, _T("关山酒"), 128);
    safe_strcpy(songs[13].artist, _T("黄雨篱"), 128);
    strcpy(songs[13].mp3Path, "music\\guanshanjiu\\guanshanjiu.mp3");
    strcpy(songs[13].photoPath, "music\\guanshanjiu\\photo.jpg");
    safe_strcpy(songs[13].lrcPath, "music\\guanshanjiu\\guanshanjiu.lrc", 256);
    songs[13].tag = national;
    songs[13].lyrics = NULL;
    songs[13].lyricCount = 0;
    songs[13].isFavorite = 0;

    // 初始化第十五首歌曲 - 好运来
    strcpy(songs[14].folder, "haoyunlai");
    safe_strcpy(songs[14].name, _T("好运来"), 128);
    safe_strcpy(songs[14].artist, _T("祖海"), 128);
    strcpy(songs[14].mp3Path, "music\\haoyunlai\\haoyunlai.mp3");
    strcpy(songs[14].photoPath, "music\\haoyunlai\\photo.jpg");
    safe_strcpy(songs[14].lrcPath, "music\\haoyunlai\\haoyunlai.lrc", 256);
    songs[14].tag = popular;
    songs[14].lyrics = NULL;
    songs[14].lyricCount = 0;
    songs[14].isFavorite = 0;

    // 初始化第十六首歌曲 - Merry Christmas
    strcpy(songs[15].folder, "merrychristmas");
    safe_strcpy(songs[15].name, "Merry Christmas", 128);
    safe_strcpy(songs[15].artist, "Traditional", 128);
    strcpy(songs[15].mp3Path, "music\\merrychristmas\\merrychristmas.mp3");
    strcpy(songs[15].photoPath, "music\\merrychristmas\\photo.jpg");
    safe_strcpy(songs[15].lrcPath, "music\\merrychristmas\\merrychristmas.lrc", 256);
    songs[15].tag = pure;
    songs[15].lyrics = NULL;
    songs[15].lyricCount = 0;
    songs[15].isFavorite = 0;

    // 初始化第十七首歌曲 - 难忘今宵
    strcpy(songs[16].folder, "nanwangjinxiao");
    safe_strcpy(songs[16].name, _T("难忘今宵"), 128);
    safe_strcpy(songs[16].artist, _T("李谷一"), 128);
    strcpy(songs[16].mp3Path, "music\\nanwangjinxiao\\nanwangjinxiao.mp3");
    strcpy(songs[16].photoPath, "music\\nanwangjinxiao\\photo.jpg");
    safe_strcpy(songs[16].lrcPath, "music\\nanwangjinxiao\\nanwangjinxiao.lrc", 256);
    songs[16].tag = classic;
    songs[16].lyrics = NULL;
    songs[16].lyricCount = 0;
    songs[16].isFavorite = 0;

    // 初始化第十八首歌曲 - 牵丝戏
    strcpy(songs[17].folder, "qiansixi");
    safe_strcpy(songs[17].name, _T("牵丝戏"), 128);
    safe_strcpy(songs[17].artist, _T("银临"), 128);
    strcpy(songs[17].mp3Path, "music\\qiansixi\\qiansixi.mp3");
    strcpy(songs[17].photoPath, "music\\qiansixi\\photo.jpg");
    safe_strcpy(songs[17].lrcPath, "music\\qiansixi\\qiansixi.lrc", 256);
    songs[17].tag = national;
    songs[17].lyrics = NULL;
    songs[17].lyricCount = 0;
    songs[17].isFavorite = 0;

    // 初始化第十九首歌曲 - Summer
    strcpy(songs[18].folder, "summer");
    safe_strcpy(songs[18].name, "Summer", 128);
    safe_strcpy(songs[18].artist, _T("久石让"), 128);
    strcpy(songs[18].mp3Path, "music\\summer\\summer.mp3");
    strcpy(songs[18].photoPath, "music\\summer\\photo.jpg");
    safe_strcpy(songs[18].lrcPath, "music\\summer\\summer.lrc", 256);
    songs[18].tag = pure;
    songs[18].lyrics = NULL;
    songs[18].lyricCount = 0;
    songs[18].isFavorite = 0;

    // 初始化第二十首歌曲 - 永远同在
    strcpy(songs[19].folder, "yongyuantongzai");
    safe_strcpy(songs[19].name, _T("永远同在"), 128);
    safe_strcpy(songs[19].artist, _T("筠子"), 128);
    strcpy(songs[19].mp3Path, "music\\yongyuantongzai\\yongyuantongzai.mp3");
    strcpy(songs[19].photoPath, "music\\yongyuantongzai\\photo.jpg");
    safe_strcpy(songs[19].lrcPath, "music\\yongyuantongzai\\yongyuantongzai.lrc", 256);
    songs[19].tag = popular;
    songs[19].lyrics = NULL;
    songs[19].lyricCount = 0;
    songs[19].isFavorite = 0;

    // 加载每首歌的歌词
    for (int i = 0; i < songCount; i++) {
        ParseLrcFile(songs[i].lrcPath, &songs[i]);
    }

    // 初始化推荐相关特征
    Init();

    LoadFavoriteList();
}

// 加载收藏列表
void LoadFavoriteList() {
    FILE* fp = fopen("favorites.txt", "r");
    if (!fp) return;

    char folder[64];
    while (fgets(folder, sizeof(folder), fp)) {
        // 去除换行符
        char* newline = strchr(folder, '\n');
        if (newline) *newline = '\0';

        for (int i = 0; i < songCount; i++) {
            if (strcmp(songs[i].folder, folder) == 0) {
                songs[i].isFavorite = 1;
                break;
            }
        }
    }

    fclose(fp);
}

// 保存收藏列表
void SaveFavoriteList() {
    FILE* fp = fopen("favorites.txt", "w");
    if (!fp) return;

    for (int i = 0; i < songCount; i++) {
        if (songs[i].isFavorite) {
            fprintf(fp, "%s\n", songs[i].folder);
        }
    }

    fclose(fp);
}

// 添加到收藏
void AddToFavorite(int index) {
    if (index < 0 || index >= songCount) return;

    songs[index].isFavorite = (songs[index].isFavorite == 0) ? 1 : 0;
    SaveFavoriteList();
}

// 获取推荐歌曲
void GetRecommendedSong(TagType tag) {
    // 收集该标签下的所有歌曲索引
    int* indices = (int*)malloc(sizeof(int) * songCount);
    int count = 0;

    for (int i = 0; i < songCount; i++) {
        int shouldInclude = 0;
        if (showFavoriteOnly) {
            shouldInclude = songs[i].isFavorite;
        }
        else if (tag == all_songs) {
            shouldInclude = 1;
        }
        else {
            shouldInclude = (songs[i].tag == tag);
        }

        if (shouldInclude) {
            indices[count++] = i;
        }
    }

    if (count > 0) {
        // 随机选择一首
        srand((unsigned int)time(NULL));
        currentSongIndex = indices[rand() % count];
    }
    else {
        currentSongIndex = -1;
    }

    free(indices);
}

