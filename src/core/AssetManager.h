#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "SpriteId.h"

// 单例：集中缓存纹理与字体，并维护 SpriteId -> 纹理 的映射。
// 只有真正「进程唯一」的资源才用单例；系统编排不要学它（改用依赖注入）。
class AssetManager
{
public:
    static AssetManager& Instance();

    bool LoadTexture(SpriteId id, const std::string& path);
    const sf::Texture* GetTexture(SpriteId id) const;  // 未加载返回 nullptr

    bool LoadFont(const std::string& path);
    const sf::Font* GetFont() const;                   // 未加载返回 nullptr

private:
    AssetManager() = default;

    std::unordered_map<SpriteId, std::unique_ptr<sf::Texture>> textures_;
    std::unique_ptr<sf::Font> font_;
};
