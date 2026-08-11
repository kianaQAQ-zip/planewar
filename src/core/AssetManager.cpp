#include "AssetManager.h"

AssetManager& AssetManager::Instance()
{
    static AssetManager instance;   // Meyers 单例：C++11 起静态局部变量线程安全
    return instance;
}

bool AssetManager::LoadTexture(SpriteId id, const std::string& path)
{
    auto tex = std::make_unique<sf::Texture>();
    if (!tex->loadFromFile(path))
        return false;
    textures_[id] = std::move(tex);
    return true;
}

const sf::Texture* AssetManager::GetTexture(SpriteId id) const
{
    auto it = textures_.find(id);
    return (it != textures_.end()) ? it->second.get() : nullptr;
}

bool AssetManager::LoadFont(const std::string& path)
{
    font_ = std::make_unique<sf::Font>();
    return font_->loadFromFile(path);
}

const sf::Font* AssetManager::GetFont() const
{
    return font_.get();
}
