#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <SFML/Audio.hpp>

// 音频管理器（Core 层，进程唯一 → 单例）
// ---------------------------------------------------------------------------
// 与 AssetManager 同理：只有真正「进程唯一」的引擎服务才用单例；系统编排仍走依赖注入。
// 优雅降级：缺音频文件时 LoadSound 返回 false、Play 静默返回，游戏照常运行不崩溃——
// 没有素材也能把 M3 跑起来（和缺贴图/字体一个思路）。
class AudioManager
{
public:
    static AudioManager& Instance();

    bool LoadSound(const std::string& name, const std::string& path);
    void Play(const std::string& name);     // 同名音效重新播放（demo 足够）
    void StopAll();

private:
    AudioManager() = default;

    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> buffers_;
    std::unordered_map<std::string, sf::Sound>                        sounds_;
};
