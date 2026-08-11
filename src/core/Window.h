#pragma once
#include <string>
#include <SFML/Graphics.hpp>

// 对 sf::RenderWindow 的薄封装：只暴露「开关 / 事件 / 原始窗口」三件事。
class Window
{
public:
    Window(unsigned int width, unsigned int height, const std::string& title);

    bool IsOpen() const;
    void PollEvents();                 // 处理关闭等窗口事件
    sf::RenderWindow& GetRaw();        // 仅 Core 层（Renderer）使用

    // 实时按键状态查询（用于持续移动/射击）。引擎相关，仅 App 层调用，
    // 并由 App 映射成 Game 层引擎无关的 InputState。
    bool IsKeyPressed(sf::Keyboard::Key key) const;

private:
    sf::RenderWindow window_;
};
