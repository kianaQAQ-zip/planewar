#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "SpriteId.h"
#include "Math.h"

class AssetManager;

// Core 层渲染器：把 SpriteId 查表成 sf::Sprite 并绘制。
// Game 层只调用 DrawSprite / DrawEntity，从不知道底层到底是贴图还是兜底圆。
class Renderer
{
public:
    Renderer(sf::RenderWindow& window, const AssetManager& assets);

    void Clear(const sf::Color& color);

    // 背景等：按左上角贴图（有图贴图，无图跳过）
    void DrawSprite(SpriteId id, const sf::Vector2f& position);

    // 实体专用：按中心绘制，目标直径约为 2*radius。
    // 若对应 SpriteId 未加载纹理，则用 fallbackColor 画一个实心圆占位——
    // 这样「没有美术素材也能看到 demo 跑起来」，且 Game 层完全无感。
    void DrawEntity(SpriteId id, const Vec2& position, float radius, unsigned int fallbackColor);

    void Display();

private:
    sf::RenderWindow& window_;
    const AssetManager& assets_;
    std::unordered_map<SpriteId, sf::Sprite> spriteCache_;  // 按 id 缓存 sprite，避免每帧重建
};
