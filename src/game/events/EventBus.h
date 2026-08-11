#pragma once
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <functional>
#include <utility>
#include "core/Math.h"   // BossFireEvent 用到 Vec2

// 事件总线（设计模式：观察者 / 发布-订阅）
// ---------------------------------------------------------------------------
// 解耦「谁产生事件」和「谁响应事件」。例如碰撞系统检测到子弹击杀敌机时，
// 不直接写死「加分 / 播音效 / 弹粒子」，而是 Emit 一个 EnemyKilledEvent；
// ScoreManager、音效、粒子系统各自 Subscribe，互不知道对方存在。
//
// 这是 Unity/UE 事件系统的迷你版，体现「低耦合」不是嘴上说说。
// 实现要点：用 std::type_index 做事件类型的 key，handler 存成「接受 void*」的
// 通用闭包，分发时再安全地 static_cast 回具体类型——既类型安全又不要求事件继承基类。

// ---- 事件定义（纯数据，无行为）----

// 敌机被击杀：携带敌机类型与应得分数，供计分/统计订阅。
struct EnemyKilledEvent
{
    int scoreValue = 0;
};

// 玩家被敌机撞击：供扣命、闪屏、音效订阅（M2 中 World 直接处理扣命，事件留给后续扩展）。
struct PlayerHitEvent
{
    int remainingLives = 0;
};

// ---- M3 新增事件 ----
// Boss 发射请求：Boss 自己不知道子弹池，只发「我在哪、用什么弹幕、玩家在哪」，
// 由 World 订阅并生成敌弹——Boss 与子弹系统互不认识（解耦）。
struct BossFireEvent
{
    Vec2 origin  = {};   // 发射原点（Boss 位置）
    int  pattern = 1;    // 弹幕模式：1=五路扇形，2=八路环形
    Vec2 target  = {};   // 玩家位置（瞄准用，预留）
};

struct BossSpawnedEvent  {};   // Boss 进场：触发音效 / 提示
struct BossHitEvent      {};   // Boss 被击中：音效 / 火花
struct BossDefeatedEvent {};   // Boss 被击毁：大爆炸 / 胜利
struct ItemPickupEvent   {};   // 拾取道具：音效 / 粒子

class EventBus
{
public:
    // 订阅某类型事件。fn 接收该事件的 const 引用。
    template <typename T>
    void Subscribe(std::function<void(const T&)> fn)
    {
        // 用一层适配把「具体类型回调」包成「void* 回调」，统一存到一个桶里。
        handlers_[typeid(T)].emplace_back(
            [fn](const void* e) { fn(*static_cast<const T*>(e)); });
    }

    // 发布事件：通知所有订阅者。没有订阅者时静默返回。
    template <typename T>
    void Emit(const T& event)
    {
        auto it = handlers_.find(typeid(T));
        if (it == handlers_.end())
            return;
        for (const auto& h : it->second)
            h(&event);
    }

    // 清空全部订阅（World::Reset 时调用，避免残留上一局的回调持有旧状态）。
    void Clear()
    {
        handlers_.clear();
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> handlers_;
};
