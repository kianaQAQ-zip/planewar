#pragma once
#include "WaveDef.h"
#include "ObjectPool.h"
#include "GameConfig.h"
#include "Enemy.h"
#include <vector>
#include <cstdlib>

// 波次管理器（数据驱动）
// ---------------------------------------------------------------------------
// 持有若干 WaveDef，按时间把敌机「生成」进对象池。当前波打满数量后自动进入下一波；
// 全部波次结束则标记 finished_（M2 里结束后停止生成，可扩展为循环 / 通关画面）。
//
// 注意：生成逻辑只负责「填数据」（种类/速度/半径/出生点），真正的移动与回收由 Enemy 自己负责。
// 这样新增一种敌机形态，只需在 waves_ 里加一行 WaveDef + 在 SpriteId 注册一个枚举值。
class WaveManager
{
public:
    WaveManager();

    void Reset();
    void Update(float dt, ObjectPool<Enemy>& enemies);

    int  GetCurrentWave() const { return waveIndex_ + 1; } // 人类友好的波号（从 1 开始）
    bool IsFinished()     const { return finished_; }

private:
    std::vector<WaveDef> waves_;  // 波次配置表（数据驱动）
    int   waveIndex_     = 0;     // 当前波次下标
    int   spawnedInWave_ = 0;     // 当前波已生成数量
    float spawnTimer_    = 0.f;   // 距下一架敌机的剩余秒数
    bool  finished_      = false;
};
