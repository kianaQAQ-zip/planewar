#pragma once

class Renderer;   // 前向声明：Game 层头文件不引入 SFML

// HUD（抬头显示）：分数 / 剩余生命 / 当前波次
// ---------------------------------------------------------------------------
// 纯展示层，从 World 传入的当前数值读取并绘制。它只调用 Core 的 Renderer::DrawText，
// 从不持有也不绘制任何 SFML 对象——和实体一样遵守「Game 层不碰 SFML」的纪律。
class Hud
{
public:
    void Draw(Renderer& renderer, int score, int lives, int wave) const;
};
