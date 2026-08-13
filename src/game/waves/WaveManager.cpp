#include "WaveManager.h"

WaveManager::WaveManager()
{
    // 关卡配置表：三波由易到难，演示「数据驱动」——调难度只改这里。
    // 每种敌机用不同的 SpriteId / 颜色 / 速度，即使没有美术素材也能靠占位圆区分。
    waves_ = {
        // 第 1 波：慢速基础敌机，数量少、间隔大
        // 注意：这里用 C++17 兼容的按顺序聚合初始化，避免 designated initializers
        // 触发 MSVC C7555（/std:c++17 不支持命名字段初始化）。
        { 8,  1.1f, SpriteId::EnemyBasic, 110.f, 22.f, 0xFF4444FF, 100 },

        // 第 2 波：更快的敌机，数量增多
        { 12, 0.8f, SpriteId::EnemyFast,  200.f, 18.f, 0xFFAA33FF, 150 },

        // 第 3 波：密集基础敌机，压力大
        { 16, 0.55f, SpriteId::EnemyTank, 95.f,  28.f, 0xAA44FFFF, 200 },
    };
}

void WaveManager::Reset()
{
    waveIndex_     = 0;
    spawnedInWave_ = 0;
    spawnTimer_    = 0.f;
    finished_      = false;
}

void WaveManager::Update(float dt, ObjectPool<Enemy>& enemies)
{
    if (finished_)
        return;

    spawnTimer_ -= dt;
    if (spawnTimer_ > 0.f)
        return;

    const WaveDef& wave = waves_[waveIndex_];

    // 从对象池借一架敌机，按当前波的 WaveDef 填好属性后投入战场。
    if (Enemy* e = enemies.Acquire())
    {
        e->SetSprite(wave.sprite);
        e->SetRadius(wave.radius);
        e->SetFallbackColor(wave.color);
        e->SetVelocity({0.f, wave.speed});   // OnSpawn 设过的默认速度在此被波次覆盖

        // 顶部随机 x 出生（留出半径边距，避免一半卡在屏幕外）
        const float r = wave.radius;
        const float x = r + (static_cast<float>(std::rand()) / RAND_MAX) *
                            (GameConfig::WORLD_WIDTH - 2.f * r);
        e->SetPosition({x, -r});

        ++spawnedInWave_;
    }

    // 本波生成完毕 → 进入下一波；波表耗尽则结束。
    if (spawnedInWave_ >= wave.count)
    {
        ++waveIndex_;
        spawnedInWave_ = 0;
        spawnTimer_    = 1.0f;   // 波与波之间留 1 秒喘息
        if (waveIndex_ >= static_cast<int>(waves_.size()))
            finished_ = true;
    }
    else
    {
        spawnTimer_ = wave.interval;
    }
}
