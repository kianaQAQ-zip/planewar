#pragma once
#include <vector>
#include <cstddef>
#include <utility>

// 对象池（设计模式：Object Pool）
// ---------------------------------------------------------------------------
// 飞机大战里子弹/敌机高频生成与销毁，若每次 new/delete 会产生大量碎片与 GC 抖动。
// 对象池在构造时一次性预分配固定容量，运行时只做「激活/回收」状态切换，
// Acquire 取一个空闲对象，Release（或对象自行 SetActive(false)）归还。
//
// 要求 T 提供：
//   - 默认构造
//   - SetActive(bool) / IsActive() const
//   - OnSpawn()  （Acquire 时调用，用于重置速度/状态）
//
// 注意：池内对象始终存活（vector 拥有），外部只持有裸指针/引用；
// 指针在对象回收后不可再解引用——本项目中对象只在 World 的同一帧循环内使用，安全。
template <typename T>
class ObjectPool
{
public:
    explicit ObjectPool(std::size_t capacity)
    {
        pool_.resize(capacity);
        for (auto& obj : pool_)
            obj.SetActive(false);
    }

    // 取一个空闲对象并标记为激活；池满返回 nullptr。
    T* Acquire()
    {
        for (auto& obj : pool_)
        {
            if (!obj.IsActive())
            {
                obj.SetActive(true);
                obj.OnSpawn();
                return &obj;
            }
        }
        return nullptr;
    }

    // 归还对象（也可由对象在 Update 中自行 SetActive(false)）。
    void Release(T* obj)
    {
        if (obj) obj->SetActive(false);
    }

    std::size_t Capacity() const   { return pool_.size(); }

    std::size_t ActiveCount() const
    {
        std::size_t n = 0;
        for (const auto& o : pool_)
            if (o.IsActive()) ++n;
        return n;
    }

    // 遍历所有「激活中」的对象，fn 接收 T&。
    template <typename F>
    void ForEachActive(F&& fn)
    {
        for (auto& o : pool_)
            if (o.IsActive()) fn(o);
    }

    // 暴露原始容器（World 做碰撞双重遍历时用，避免重复接口）。
    std::vector<T>&       Raw()       { return pool_; }
    const std::vector<T>& Raw() const { return pool_; }

private:
    std::vector<T> pool_;
};
