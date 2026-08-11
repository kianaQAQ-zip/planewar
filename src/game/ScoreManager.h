#pragma once
#include "events/EventBus.h"

// 分数管理器
// ---------------------------------------------------------------------------
// 只关心「分数是多少」。它通过 EventBus 订阅 EnemyKilledEvent 来加分，
// 完全不知道碰撞系统、敌人长什么样——这正是事件总线解耦的价值：
// 碰撞系统 Emit 一个事件，ScoreManager 自己决定怎么处理。
class ScoreManager
{
public:
    void Reset() { score_ = 0; }

    int  GetScore() const { return score_; }

    // 注册事件订阅。传入的 bus 必须是 World 持有的同一个实例（依赖注入，不自己 new）。
    void RegisterEvents(EventBus& bus)
    {
        bus.Subscribe<EnemyKilledEvent>([this](const EnemyKilledEvent& e) {
            score_ += e.scoreValue;
        });
    }

private:
    int score_ = 0;
};
