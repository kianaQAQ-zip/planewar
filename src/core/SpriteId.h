#pragma once
#include <type_traits>
#include <functional>

// 精灵标识。Game 层只认这个枚举，绝不直接见 sf::Texture / sf::Sprite。
enum class SpriteId
{
    None,           // 无效/占位（实体默认）
    Background,
    Player,
    EnemyBasic,
    EnemyFast,
    EnemyTank,
    Boss,
    BulletPlayer,
    BulletEnemy,
    PowerUp
};

// enum class 默认没有 std::hash 特化；提供后它才能当 unordered_map 的 key。
namespace std
{
    template<> struct hash<SpriteId>
    {
        size_t operator()(SpriteId id) const noexcept
        {
            using UT = std::underlying_type_t<SpriteId>;
            return std::hash<UT>{}(static_cast<UT>(id));
        }
    };
}
