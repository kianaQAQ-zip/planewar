#include "Hud.h"
#include "core/Renderer.h"
#include "core/Math.h"
#include "GameConfig.h"
#include <string>

void Hud::Draw(Renderer& renderer, int score, int lives, int wave) const
{
    // 左上角：分数
    renderer.DrawText("Score: " + std::to_string(score), {12.f, 10.f}, 24, 0xFFFFFFFF);

    // 右上角：生命（用 ♥ 占位，无字体时也能显示文本）
    renderer.DrawText("Lives: " + std::to_string(lives), {GameConfig::WORLD_WIDTH - 130.f, 10.f}, 24, 0xFF6688FF);

    // 中上：波次
    renderer.DrawText("Wave: " + std::to_string(wave), {GameConfig::WORLD_WIDTH * 0.5f - 40.f, 10.f}, 24, 0xFFFFAAFF);
}
