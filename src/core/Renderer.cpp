#include "Renderer.h"
#include "AssetManager.h"

Renderer::Renderer(sf::RenderWindow& window, const AssetManager& assets)
    : window_(window), assets_(assets)
{
}

void Renderer::Clear(const sf::Color& color)
{
    window_.clear(color);
}

void Renderer::DrawSprite(SpriteId id, const sf::Vector2f& position)
{
    auto it = spriteCache_.find(id);
    if (it == spriteCache_.end())
    {
        const sf::Texture* tex = assets_.GetTexture(id);
        if (!tex) return;                 // Core 兜底：缺图直接跳过，Game 层无感
        sf::Sprite sprite;
        sprite.setTexture(*tex);
        it = spriteCache_.emplace(id, std::move(sprite)).first;
    }
    it->second.setPosition(position);
    window_.draw(it->second);
}

void Renderer::DrawEntity(SpriteId id, const Vec2& position, float radius, unsigned int fallbackColor)
{
    auto it = spriteCache_.find(id);
    if (it == spriteCache_.end())
    {
        const sf::Texture* tex = assets_.GetTexture(id);
        if (!tex)
        {
            // 缺纹理兜底：画一个实心圆占位，保证没有美术资源也能看到实体在动
            sf::CircleShape circle(radius > 0.f ? radius : 1.f);
            circle.setFillColor(sf::Color(fallbackColor));
            circle.setOrigin(radius, radius);
            circle.setPosition(position.x, position.y);
            window_.draw(circle);
            return;
        }
        sf::Sprite sprite;
        sprite.setTexture(*tex);
        it = spriteCache_.emplace(id, std::move(sprite)).first;
    }

    sf::Sprite& sprite = it->second;
    const sf::Texture* tex = sprite.getTexture();
    const float texW = static_cast<float>(tex->getSize().x);
    const float texH = static_cast<float>(tex->getSize().y);
    const float texMax = (texW > texH ? texW : texH);
    const float scale = (texMax > 0.f) ? (radius * 2.f / texMax) : 1.f;

    sprite.setOrigin(texW * 0.5f, texH * 0.5f);   // 以中心为锚点
    sprite.setScale(scale, scale);
    sprite.setPosition(position.x, position.y);
    window_.draw(sprite);
}

void Renderer::Display()
{
    window_.display();
}
