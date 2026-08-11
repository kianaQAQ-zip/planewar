// App 层：启动 + 主循环（固定步长）
// 纪律：只有 App 层允许直接碰 SFML；Game 层通过 World / InputState 间接交互。
#include "core/Window.h"
#include "core/AssetManager.h"
#include "core/Renderer.h"
#include "core/Time.h"
#include "game/World.h"
#include "game/Input.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    // 中文 Windows 控制台默认 GBK；源码/字符串是 UTF-8，必须切到 UTF-8 才不乱码。
    SetConsoleOutputCP(CP_UTF8);
#endif

    Window window(800, 600, "PlaneWar");
    auto& assets = AssetManager::Instance();

    Renderer renderer(window.GetRaw(), assets);
    Time time;
    World world;
    world.LoadAssets(assets);   // 统一在此登记所有贴图与字体（缺资源自动兜底）

    std::cout << "[M2] PlaneWar 启动：方向键/WASD 移动，空格射击。\n";

    bool gameOverPrinted = false;

    while (window.IsOpen())
    {
        window.PollEvents();

        // App 层把 SFML 按键映射成引擎无关的 InputState（Game 层不认 sf::Keyboard）
        InputState input;
        input.left  = window.IsKeyPressed(sf::Keyboard::Left)  || window.IsKeyPressed(sf::Keyboard::A);
        input.right = window.IsKeyPressed(sf::Keyboard::Right) || window.IsKeyPressed(sf::Keyboard::D);
        input.up    = window.IsKeyPressed(sf::Keyboard::Up)    || window.IsKeyPressed(sf::Keyboard::W);
        input.down  = window.IsKeyPressed(sf::Keyboard::Down)  || window.IsKeyPressed(sf::Keyboard::S);
        input.fire  = window.IsKeyPressed(sf::Keyboard::Space);

        // 固定步长：逻辑按 60Hz 推进，渲染每帧一次（与帧率解耦）
        time.Tick();
        while (time.ShouldStep())
            world.Update(Time::FIXED_STEP, input);

        renderer.Clear(sf::Color(10, 12, 24));          // 深空蓝兜底色
        renderer.DrawSprite(SpriteId::Background, {0.f, 0.f}); // 有图就画，无图跳过
        world.Render(renderer);
        renderer.Display();

        if (world.IsGameOver() && !gameOverPrinted)
        {
            std::cout << "[M2] 游戏结束！得分: " << world.GetScore() << "\n";
            gameOverPrinted = true;
        }
    }
    return 0;
}
