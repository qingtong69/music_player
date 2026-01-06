#pragma once

#include "song.h"
#include <time.h>

// 决策树节点结构
struct DecisionTreeNode {
    int featureIndex;               // 特征索引 (0:播放频率, 1:情绪, 2:风格匹配, 3:节奏)
    double threshold;               // 分割阈值
    int songIndex;                  // 如果是叶子节点，存储推荐的歌曲索引
    DecisionTreeNode* left;         // 左子树
    DecisionTreeNode* right;        // 右子树
};

// 推荐歌曲按钮结构
struct RecButton {
    int x, y, w, h;
    int songIndex;
    char displayText[128];
};

// 全局变量声明
extern const char* feaNames[];
extern RecButton recButtons[];
extern int recSongCount;
extern int currentTag;

// 函数声明
void Init();
double FeaFrequency(Song* song);
void Other(Song* song);
double NewFeature(int featureIndex, Song* song);
double Gini(int* indices, int count, int featureIndex, double threshold);
void FindBestSplit(int* indices, int count, int* bestFeature, double* bestThreshold, double* bestScore);
DecisionTreeNode* Build(int* indices, int count, int depth);
void FreeTree(DecisionTreeNode* root);
int Recommend(TagType tag);
int GenerateSongs(TagType tag);
void DrawRec();
void RecClick(int x, int y);
void TestTree();