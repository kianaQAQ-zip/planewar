#pragma once
#include "core/SpriteId.h"

// 一波敌人的「数据定义」（数据驱动设计的核心载体）
// ---------------------------------------------------------------------------
// 以前「生成哪种敌机、生成多少、间隔多久」是写死在代码里的；现在抽成一张配置表。
// 加关卡 / 调难度只改这份数据，不碰生成逻辑——这正是面试讲的「数据驱动 vs 硬编码」。
// MVP 阶段用 C++ 结构体数组（编译期安全、IDE 可跳转）；JSON 等外部数据源留到 M3 之后，
// 届时只需加一个 LoadWaves(path) 解析，WaveManager 接口不变。
struct WaveDef
{
    int      count   = 0;     // 本波敌机数量
    float    interval = 1.0f; // 敌机之间的生成间隔（秒）
    SpriteId sprite  = SpriteId::EnemyBasic; // 敌机种类（决定贴图/占位色）
    float    speed   = 130.f; // 下落速度 px/s
    float    radius  = 22.f;  // 碰撞半径
    unsigned int color = 0xFF4444FF; // 缺纹理时的占位圆颜色
    int      score   = 100;   // 击杀得分
};
