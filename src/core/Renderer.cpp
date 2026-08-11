#include "Renderer.h"
#include "AssetManager.h"
#include <string>

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

void Renderer::DrawText(const std::string& text, const Vec2& position,
                        unsigned int charSize, unsigned int color)
{
    // Core 兜底：没有字体就不画，绝不崩溃。HUD 调用方完全无感。
    const sf::Font* font = assets_.GetFont();
    if (!font)
        return;

    sf::Text t(text, *font, charSize);
    t.setFillColor(sf::Color(color));
    t.setPosition(position.x, position.y);
    window_.draw(t);
}
