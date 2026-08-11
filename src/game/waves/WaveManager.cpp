#include "WaveManager.h"

WaveManager::WaveManager()
{
    // 关卡配置表：三波由易到难，演示「数据驱动」——调难度只改这里。
    // 每种敌机用不同的 SpriteId / 颜色 / 速度，即使没有美术素材也能靠占位圆区分。
    waves_ = {
        // 第 1 波：慢速基础敌机，数量少、间隔大
        { .count = 8,  .interval = 1.1f, .sprite = SpriteId::EnemyBasic,
          .speed = 110.f, .radius = 22.f, .color = 0xFF4444FF, .score = 100 },

        // 第 2 波：更快的敌机，数量增多
        { .count = 12, .interval = 0.8f, .sprite = SpriteId::EnemyFast,
          .speed = 200.f, .radius = 18.f, .color = 0xFFAA33FF, .score = 150 },

        // 第 3 波：密集基础敌机，压力大
        { .count = 16, .interval = 0.55f, .sprite = SpriteId::EnemyTank,
          .speed = 95.f,  .radius = 28.f, .color = 0xAA44FFFF, .score = 200 },
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
