#include "AudioManager.h"

AudioManager& AudioManager::Instance()
{
    static AudioManager instance;   // Meyers 单例：线程安全
    return instance;
}

bool AudioManager::LoadSound(const std::string& name, const std::string& path)
{
    auto buf = std::make_unique<sf::SoundBuffer>();
    if (!buf->loadFromFile(path))
        return false;               // 缺资源：返回 false，不抛异常
    buffers_[name] = std::move(buf);
    return true;
}

void AudioManager::Play(const std::string& name)
{
    auto bit = buffers_.find(name);
    if (bit == buffers_.end())
        return;                     // 缺资源：静默跳过（Game 层完全无感）

    auto sit = sounds_.find(name);
    if (sit == sounds_.end())
    {
        sf::Sound s;
        s.setBuffer(*bit->second);
        sit = sounds_.emplace(name, std::move(s)).first;
    }
    sit->second.play();
}

void AudioManager::StopAll()
{
    for (auto& kv : sounds_)
        kv.second.stop();
}
