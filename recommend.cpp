#include "recommend.h"
#include "music_controller.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <graphics.h>

// 全局变量定义
const char* feaNames[] = {
    "播放频率",
    "情绪风格",
    "风格匹配",
    "节奏"
};

RecButton recButtons[5];
int recSongCount = 0;
int currentTag = -1;        

// 计算播放频率（结合播放次数和最近播放时间）
double FeaFrequency(Song* song) {
    double frequency = 0.0;

    // 基础播放次数权重
    frequency += song->features.playCount * 0.1;

    // 最近播放时间权重（24小时内播放过加分）
    if (song->features.lastPlayed > 0) {
        time_t now = time(NULL);
        double hoursSincePlayed = difftime(now, song->features.lastPlayed) / 3600.0;
        if (hoursSincePlayed < 24) {
            frequency += 0.3 * (1.0 - hoursSincePlayed / 24.0);     //播放越久远，奖励越少
        }
    }

    // 收藏歌曲额外加分
    if (song->isFavorite) {
        frequency += 0.2;
    }

    // 限制在0-1之间
    if (frequency > 1.0) frequency = 1.0;
    if (frequency < 0.0) frequency = 0.0;

    return frequency;
}

// 基于标签设置情绪风格和风格特征
void Other(Song* song) {
    switch (song->tag) {
    case popular:    // 流行：情绪积极，风格现代
        song->features.mood = 0.8;
        song->features.style = 0;
        song->features.tempo = 0.7;
        break;
    case classic:    // 经典：情绪温和，风格传统
        song->features.mood = 0.6;
        song->features.style = 0;
        song->features.tempo = 0.5;
        break;
    case national:   // 国风：情绪优雅，风格民族
        song->features.mood = 0.7;
        song->features.style = 0;
        song->features.tempo = 0.6;
        break;
    case pure:       // 纯音乐：情绪平静，风格抽象
        song->features.mood = 0.4;
        song->features.style = 0;
        song->features.tempo = 0.4;
        break;
    default:
        song->features.mood = 0.5;
        song->features.style = 0;
        song->features.tempo = 0.5;
    }
}

// 初始化歌曲特征
void Init() {
    printf("初始化歌曲特征\n");

    for (int i = 0; i < songCount; i++) {
        // 设置基础特征
        Other(&songs[i]);

        // 初始化播放数据
        songs[i].features.playCount = 0;
        songs[i].features.lastPlayed = 0;

        // 计算播放频率
        songs[i].features.playFrequency = FeaFrequency(&songs[i]);

        printf("歌曲%d[%s]: 播放频率=%.2f, 情绪风格=%.2f, 风格=%.2f, 节奏=%.2f, 标签=%d\n",
            i, songs[i].name,
            songs[i].features.playFrequency,
            songs[i].features.mood,
            songs[i].features.style,
            songs[i].features.tempo,
            songs[i].tag);
    }
}

// 计算特征值
double NewFeature(int featureIndex, Song* song) {
    // 先更新播放频率（因为播放数据可能已改变）
    song->features.playFrequency = FeaFrequency(song);

    switch (featureIndex) {
    case 0: // 播放频率
        return song->features.playFrequency;
    case 1: // 情绪风格
        return song->features.mood;
    case 2: // 风格匹配（当前选择标签与歌曲标签的匹配度）
        return (song->tag == selectedTag) ? 1.0 : 0.0;
    case 3: // 节奏
        return song->features.tempo;
    default:
        return 0.0;
    }
}

// 计算基尼不纯度（用于选择最佳分割）,用播放次数计算混合度
//      当前节点的歌曲索引，歌曲数量，    特征索引，       分割阈值
double Gini(int* indices, int count, int featureIndex, double threshold) {
    int leftCount = 0, rightCount = 0;
    int leftPlayCount = 0, rightPlayCount = 0;
    int lcount[11] = { 0 };      //假设最多播放10次
    int rcount[11] = { 0 };

    // 分割数据并统计播放次数
    for (int i = 0; i < count; i++) {
        double featureVal = NewFeature(featureIndex, &songs[indices[i]]);
        if (featureVal <= threshold) {
            leftCount++;
            leftPlayCount += songs[indices[i]].features.playCount;
            for (int j = 0; j <= 10; j++)
            {
                if (songs[indices[i]].features.playCount == j)
                    lcount[j]++;
            }

        }
        else {
            rightCount++;
            rightPlayCount += songs[indices[i]].features.playCount;
            for (int j = 0; j <= 10; j++)
            {
                if (songs[indices[i]].features.playCount == j)
                    rcount[j]++;
            }
        }
    }

    // 如果某个子集为空，返回最差的不纯度
    if (leftCount == 0 || rightCount == 0) {
        return 1.0;
    }

    if (leftPlayCount == 0)     leftPlayCount = 1;
    if (rightPlayCount == 0)     rightPlayCount = 1;

    // 基于播放次数的加权Gini系数
    double leftWeight = (double)leftCount / count;
    double rightWeight = (double)rightCount / count;

    double leftGini = 0;
    double rightGini = 0;

    for (int i = 0; i <= 10; i++) {
        leftGini += lcount[i] / leftPlayCount;
        rightGini += rcount[i] / rightPlayCount;
    }

    // 计算左右子集的Gini系数
    leftGini = 1.0 - pow(leftGini, 2);
    rightGini = 1.0 - pow(rightGini, 2);

    // 返回加权平均Gini不纯度
    return leftWeight * leftGini + rightWeight * rightGini;
}

// 找到最佳分割点
void FindBestSplit(int* indices, int count, int* bestFeature, double* bestThreshold, double* bestScore) {
    *bestScore = 1.0; // 初始化为最差分数
    *bestFeature = -1;
    *bestThreshold = 0.0;
    bool found = false;

    // 遍历所有特征
    for (int featureIndex = 0; featureIndex < 4; featureIndex++) {
        // 计算该特征的值范围
        double minVal = 1.0, maxVal = 0.0;
        for (int i = 0; i < count; i++) {
            double val = NewFeature(featureIndex, &songs[indices[i]]);
            if (val < minVal) minVal = val;
            if (val > maxVal) maxVal = val;
        }

        // 测试多个分割点
        for (int splitTry = 0; splitTry < 3; splitTry++) {
            double threshold = minVal + (maxVal - minVal) * (0.3 + 0.4 * splitTry / 2.0);
            double gini = Gini(indices, count, featureIndex, threshold);

            // 如果找到更好的分割，更新最佳参数
            if (!found || gini <= *bestScore) {
                *bestScore = gini;
                *bestFeature = featureIndex;
                *bestThreshold = threshold;
                found = true;
            }
        }
    }
}

//创建叶子节点
static DecisionTreeNode* CreateLeafNode(int* indices, int count) {
    DecisionTreeNode* leaf = new DecisionTreeNode;
    leaf->featureIndex = -1;

    time_t bestTime = (time_t)LLONG_MAX;
    int bestSongIndex = -1;

    for (int i = 0; i < count; i++) {
        int idx = indices[i];
        Song* song = &songs[idx];
        time_t lastPlayed = song->features.lastPlayed;

        if (lastPlayed < bestTime) {
            bestTime = lastPlayed;
            bestSongIndex = idx;
            continue;
        }

        if (lastPlayed == bestTime) {
            int bestPlayCount = (bestSongIndex >= 0) ? songs[bestSongIndex].features.playCount : INT_MAX;
            if (song->features.playCount < bestPlayCount) {
                bestSongIndex = idx;
                continue;
            }
            if (song->features.playCount == bestPlayCount && rand() % 2 == 0) {
                bestSongIndex = idx;
            }
        }
    }

    if (bestSongIndex == -1 && count > 0) {
        bestSongIndex = indices[rand() % count];
    }

    leaf->songIndex = bestSongIndex;
    leaf->left = leaf->right = NULL;

    printf("  创建叶子节点: 歌曲%d (最久未播放)\n", leaf->songIndex);
    return leaf;
}

// 构建决策树
DecisionTreeNode* Build(int* indices, int count, int depth) {
    printf("构建决策树: 深度=%d, 样本数=%d\n", depth, count);

    // 终止条件1
    if (count == 0) return NULL;

    // 终止条件2，达到最大深度或样本太少，创建叶子节点
    if (depth >= 3 || count <= 2) {
        return CreateLeafNode(indices, count);
    }

    // 寻找最佳分割
    int bestFeature;
    double bestThreshold, bestScore;
    FindBestSplit(indices, count, &bestFeature, &bestThreshold, &bestScore);

    if (bestFeature == -1) {
        return CreateLeafNode(indices, count);
    }

    // 分割数据
    int leftCount = 0, rightCount = 0;
    int* leftIndices = new int[count];
    int* rightIndices = new int[count];

    for (int i = 0; i < count; i++) {
        double featureVal = NewFeature(bestFeature, &songs[indices[i]]);
        if (featureVal <= bestThreshold) {
            leftIndices[leftCount++] = indices[i];
        }
        else {
            rightIndices[rightCount++] = indices[i];
        }
    }

    printf("  分割结果: 左%d首, 右%d首\n", leftCount, rightCount);

    if (leftCount == 0 || rightCount == 0) {
        DecisionTreeNode* leaf = CreateLeafNode(indices, count);
        delete[] leftIndices;
        delete[] rightIndices;
        return leaf;
    }

    // 创建节点
    DecisionTreeNode* node = new DecisionTreeNode;
    node->featureIndex = bestFeature;
    node->threshold = bestThreshold;
    node->songIndex = indices[0]; // 临时值

    // 递归构建子树
    node->left = Build(leftIndices, leftCount, depth + 1);
    node->right = Build(rightIndices, rightCount, depth + 1);

    // 清理临时数组
    delete[] leftIndices;
    delete[] rightIndices;

    return node;
}

// 释放决策树内存
void FreeTree(DecisionTreeNode* root) {
    if (!root) return;
    FreeTree(root->left);
    FreeTree(root->right);
    delete root;
}

// 使用决策树推荐歌曲
int Recommend(TagType tag) {
    printf("决策树推荐开始\n");
    printf("目标标签: %d\n", tag);

    // 收集候选歌曲
    int* candidates = (int*)malloc(sizeof(int) * songCount);
    int candidateCount = 0;

    for (int i = 0; i < songCount; i++) {
        int shouldInclude = 0;

        // 根据标签筛选歌曲
        if (tag == all_songs) {
            shouldInclude = 1;  // 全部歌曲：包括所有
        }
        else if (tag == favorite) {
            shouldInclude = songs[i].isFavorite;  // 收藏：只包括收藏的
        }
        else {
            shouldInclude = (songs[i].tag == tag);  // 特定标签：匹配标签
        }

        if (shouldInclude) {
            candidates[candidateCount++] = i;
            printf("候选%d: %s (标签:%d, 收藏:%d, 播放:%d次)\n",
                i, songs[i].name, songs[i].tag, songs[i].isFavorite, songs[i].features.playCount);
        }
    }

    printf("候选歌曲数量: %d\n", candidateCount);

    if (candidateCount == 0) {
        printf("没有符合条件的歌曲\n");
        free(candidates);
        return -1;
    }

    // 如果只有1-2首，直接返回
    if (candidateCount <= 2) {
        int result = candidates[rand() % candidateCount];
        printf("歌曲较少，直接推荐: %s\n", songs[result].name);
        result = candidates[rand() % candidateCount];
        free(candidates);
        printf("歌曲较少，直接推荐: %s\n", songs[result].name);
        return result;
    }

    // 构建决策树
    DecisionTreeNode* tree = Build(candidates, candidateCount, 0);

    // 遍历决策树进行推荐
    DecisionTreeNode* current = tree;
    int decisionPath[10];
    int pathLength = 0;

    while (current && current->featureIndex != -1) {
        double featureVal = NewFeature(current->featureIndex, &songs[current->songIndex]);
        decisionPath[pathLength++] = current->featureIndex;

        printf("决策%d: %s=%.2f %s %.2f -> ",
            pathLength, feaNames[current->featureIndex], featureVal,
            (featureVal <= current->threshold) ? "<=" : ">",
            current->threshold);

        if (featureVal <= current->threshold) {
            current = current->left;
            printf("左分支\n");
        }
        else {
            current = current->right;
            printf("右分支\n");
        }

        if (pathLength > 5) break; // 防止无限循环
    }

    int recommendedIndex = current ? current->songIndex : candidates[rand() % candidateCount];

    printf("推荐结果: 歌曲%d - %s\n", recommendedIndex, songs[recommendedIndex].name);
    printf("决策路径: ");
    for (int i = 0; i < pathLength; i++) {
        printf("%s", feaNames[decisionPath[i]]);
        if (i < pathLength - 1) printf(" -> ");
    }
    printf(" -> 推荐\n");

    // 显示推荐理由
    printf("推荐理由:\n");
    printf("播放次数: %d次\n", songs[recommendedIndex].features.playCount);
    printf("情绪值: %.2f\n", songs[recommendedIndex].features.mood);
    printf("风格匹配: %s\n", (songs[recommendedIndex].tag == tag) ? "是" : "否");
    printf("节奏: %.2f\n", songs[recommendedIndex].features.tempo);
    printf("播放频率: %.2f\n", songs[recommendedIndex].features.playFrequency);
    printf("收藏状态: %s\n", songs[recommendedIndex].isFavorite ? "已收藏" : "未收藏");

    // 释放决策树内存
    FreeTree(tree);

    printf("=== 推荐完成 ===\n");
    free(candidates);
    return recommendedIndex;
}

// 生成推荐歌曲列表（用于界面显示）
int GenerateSongs(TagType tag) {
    printf("\n=== 生成推荐歌曲列表 ===\n");
    printf("目标标签: %d\n", tag);

    currentTag = tag;
    recSongCount = 0;

    // 收集候选歌曲
    int* candidates = (int*)malloc(sizeof(int) * songCount);
    int candidateCount = 0;

    for (int i = 0; i < songCount; i++) {
        int shouldInclude = 0;

        // 根据标签筛选歌曲
        if (tag == all_songs) {
            shouldInclude = 1;
        }
        else if (tag == favorite) {
            shouldInclude = songs[i].isFavorite;
        }
        else {
            shouldInclude = (songs[i].tag == tag);
        }

        if (shouldInclude) {
            candidates[candidateCount++] = i;
        }
    }

    printf("候选歌曲数量: %d\n", candidateCount);

    if (candidateCount == 0) {
        printf("没有符合条件的歌曲\n");
        free(candidates);
        return -1;
    }

    // 使用决策树生成多个推荐（最多3首）
    for (int rec = 0; rec < 3 && rec < candidateCount; rec++) {
        // 构建决策树进行推荐
        DecisionTreeNode* tree = Build(candidates, candidateCount, 0);

        // 遍历决策树找到推荐歌曲
        DecisionTreeNode* current = tree;
        while (current && current->featureIndex != -1) {
            double featureVal = NewFeature(current->featureIndex, &songs[current->songIndex]);
            if (featureVal <= current->threshold) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        int recommendedIndex = current ? current->songIndex : candidates[rand() % candidateCount];

        // 添加到推荐列表（避免重复）
        int alreadyAdded = 0;
        for (int i = 0; i < recSongCount; i++) {
            if (recButtons[i].songIndex == recommendedIndex) {
                alreadyAdded = 1;
                break;
            }
        }

        if (!alreadyAdded) {
            RecButton* btn = &recButtons[recSongCount];
            btn->songIndex = recommendedIndex;

            // 格式化显示文本（限制长度）
            char temp[128];
            sprintf(temp, "%s - %s", songs[recommendedIndex].name, songs[recommendedIndex].artist);
            // 如果太长就截断
            if (strlen(temp) > 25) {
                temp[22] = '.';
                temp[23] = '.';
                temp[24] = '.';
                temp[25] = '\0';
            }
            strcpy(btn->displayText, temp);

            // 设置按钮位置（水平排列）
            btn->w = 280;
            btn->h = 40;
            btn->x = 80 + rec * 300;
            btn->y = 680;

            printf("推荐%d: %s\n", rec + 1, btn->displayText);
            recSongCount++;
        }

        FreeTree(tree);

        // 如果已经推荐了足够多的歌曲，退出循环
        if (recSongCount >= 3) break;
    }

    int result = (recSongCount > 0) ? recButtons[0].songIndex : -1;
    free(candidates);
    return result;
}

// 绘制推荐歌曲按钮（在歌曲列表界面中调用）
void DrawRec() {
    if (recSongCount <= 0) return;

    // 标题
    draw_text(50, 615, "猜你喜欢", 36, YELLOW);

    for (int i = 0; i < recSongCount; i++) {
        Button btn;
        btn.x = recButtons[i].x;
        btn.y = recButtons[i].y;
        btn.w = recButtons[i].w;
        btn.h = recButtons[i].h;
        safe_strcpy(btn.text, recButtons[i].displayText, sizeof(btn.text));
        draw_button(&btn, 0);
    }
}

// 处理点击推荐歌曲按钮（在 HandleSongListClick 中调用）
void RecClick(int x, int y) {
    for (int i = 0; i < recSongCount; i++) {
        int x1 = recButtons[i].x;
        int y1 = recButtons[i].y;
        int x2 = x1 + recButtons[i].w;
        int y2 = y1 + recButtons[i].h;

        if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
            int idx = recButtons[i].songIndex;
            currentSongIndex = idx;
            PlayMusic(idx);
            currentState = PLAYING;
            return;
        }
    }
}

void TestTree() {
    Recommend(all_songs);
}