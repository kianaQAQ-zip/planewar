#pragma once

// 引擎无关的输入快照。App 层负责把 SFML 的按键映射成这个结构体，
// Game 层只认 InputState，从而与具体输入设备/引擎解耦。
struct InputState
{
    bool up    = false;
    bool down  = false;
    bool left  = false;
    bool right = false;
    bool fire  = false;
};
