#include "Hud.h"
#include "core/Renderer.h"
#include "core/Math.h"
#include "GameConfig.h"
#include <string>

void Hud::Draw(Renderer& renderer, int score, int lives, int wave,
               int bossHpPercent, int weaponLevel, bool shielded) const
{
    // 左上角：分数
    renderer.DrawText("Score: " + std::to_string(score), {12.f, 10.f}, 24, 0xFFFFFFFF);

    // 右上角：生命
    renderer.DrawText("Lives: " + std::to_string(lives), {GameConfig::WORLD_WIDTH - 130.f, 10.f}, 24, 0xFF6688FF);

    // 中上：波次
    renderer.DrawText("Wave: " + std::to_string(wave), {GameConfig::WORLD_WIDTH * 0.5f - 40.f, 10.f}, 24, 0xFFFFAAFF);

    // 玩家状态：护盾 / 武器等级（第二行）
    std::string status;
    if (shielded)        status += "[SHIELD] ";
    if (weaponLevel > 0) status += "WEAPON W" + std::to_string(weaponLevel);
    if (!status.empty())
        renderer.DrawText(status, {12.f, 40.f}, 18, 0x88DDFFFF);

    // Boss 血条（仅当 bossHpPercent >= 0）
    if (bossHpPercent >= 0)
    {
        const float barW = 400.f, barH = 14.f;
        const float x = GameConfig::WORLD_WIDTH * 0.5f - barW * 0.5f;
        const float y = 44.f;
        renderer.DrawText("BOSS", {x - 52.f, y - 2.f}, 18, 0xFF88CCFF);
        renderer.DrawRect({x, y}, barW, barH, 0x330011FF);                       // 暗红底
        const float ratio = static_cast<float>(bossHpPercent) / 100.f;
        renderer.DrawRect({x, y}, barW * ratio, barH, 0xFF3366FF);               // 前景
    }
}
