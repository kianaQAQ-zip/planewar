# PlaneWar · C++ 飞机大战

> 一个用 **纯 C++17 + SFML 2.6** 写的纵向卷轴飞机大战，作为**游戏开发岗面试 demo**。
> 重点不是画面多炫，而是向你展示**扎实的 C++ 功底、清晰的代码结构和经典设计模式的应用**。

---

## 这是什么

入门级飞机大战：玩家操控飞机移动、射击，抵御一波波下落的敌机，击杀计分、被撞扣命。

项目刻意**不使用 Unreal / Unity 等大型引擎**——因为入门 demo 用引擎会被大量样板代码淹没，面试官反而看不出你的 C++ 基本功。所有游戏循环、实体管理、碰撞、对象池、事件系统都是手写实现的。

---

## 技术栈

- **语言**：C++17
- **渲染/窗口/输入**：[SFML](https://www.sfml-dev.org/) 2.6.2（MSVC / vc17，64-bit）
- **构建系统**：CMake 3.20+
- **美术资源**：[Kenney 飞机大战素材包](https://kenney.nl/assets)（CC0，可商用）

---

## 架构分层

代码切成三层，**只允许上层依赖下层**，这一刀本身就是解耦意识的体现：

```
┌──────────────────────────────────────────┐
│  App 层   main.cpp         启动 + 主循环   │  唯一的 SFML 入口
├──────────────────────────────────────────┤
│  Game 层  Player / Enemy / Bullet / Boss / │  纯玩法，头文件零 SFML 引用
│           PowerUp / Particle /             │  （渲染细节下沉到 Core）
│           WaveManager / ScoreManager /     │
│           EventBus / Hud / World           │
├──────────────────────────────────────────┤
│  Core 层  Window / Renderer / AssetManager │  对 SFML 的薄封装 + 可复用工具
│           AudioManager / ObjectPool<T> /   │
│           Math / Time / SpriteId           │
└──────────────────────────────────────────┘
```

**关键纪律**：`Game` 层不直接 `#include <SFML/...>`。所有 SFML 调用只在 `Core` 层。
以后想换渲染后端（或加单元测试），只动 Core，Game 一行不动。

---

## 编译与运行

### 1. 安装 SFML 2.6.2（vc17，64-bit）

- 下载 **「Visual C++ 2017–2022 (64-bit)」** 预编译包：
  https://www.sfml-dev.org/download/sfml/2.6.2/
- 解压到 `D:\SFML-2.6.2`（或 `C:\SFML-2.6.2`），确保路径下有
  `lib\cmake\SFML\SFMLConfig.cmake`
- ⚠️ **必须用 64-bit 版**，且和你的编译器匹配（VS2022 → vc17）。32-bit 包会导致架构不匹配报错。

### 2. 用 Visual Studio 打开并运行

1. VS2022 → **打开本地文件夹** → 选择本项目根目录（`planewar`）
2. VS 会自动 CMake 配置并探测到 `D:\SFML-2.6.2`
   - 若未探测到，在「项目 → CMake 设置 → CMake 变量和缓存」里手动设
     `SFML_DIR = <你的路径>/lib/cmake/SFML`
3. 选择 `x64-Debug` → 按 **Ctrl+F5** 编译运行

### 3.（可选）放入美术资源

把素材包里的图片丢进 `assets/` 即可显示真飞机贴图（无需改代码）。
缺少图片也能跑——`Core` 会自动用彩色圆占位，游戏逻辑完全不受影响。

| 文件名 | 用途 |
|---|---|
| `assets/background.png` | 背景 |
| `assets/player.png` | 玩家机 |
| `assets/enemy-basic.png` | 基础敌机 |
| `assets/enemy-fast.png` | 快速敌机 |
| `assets/enemy-tank.png` | 重型敌机 |
| `assets/bullet.png` | 玩家子弹 |
| `assets/bullet-enemy.png` | 敌弹 / Boss 弹 |
| `assets/boss.png` | Boss |
| `assets/powerup.png` | 道具 |
| `assets/font.ttf` | HUD 字体（缺失则 HUD 跳过，不影响游戏） |

### 4.（可选）音效

M3 加入了音效（`AudioManager`，基于 SFML Audio）。把以下 `.wav` 放进
`assets/sfx/` 即可听到；**缺失则静默降级，游戏照常运行**。音效开启还需
`openal32.dll`（SFML 音频的运行期依赖，CMake 会在构建后自动拷贝到 exe 旁）。

| 文件名 | 触发时机 |
|---|---|
| `assets/sfx/shoot.wav` | 玩家开火 |
| `assets/sfx/explosion.wav` | 敌机被击毁 |
| `assets/sfx/player-hit.wav` | 玩家被击中 |
| `assets/sfx/powerup.wav` | 拾取道具 |
| `assets/sfx/boss-spawn.wav` | Boss 进场 |
| `assets/sfx/boss-hit.wav` | Boss 被击中 |
| `assets/sfx/boss-death.wav` | Boss 被击毁 |

---

## 操作

| 按键 | 功能 |
|---|---|
| 方向键 / WASD | 移动 |
| 空格 | 射击 |
| 关闭窗口 | 退出 |

---

## 开发里程碑

| 阶段 | 目标 | 状态 |
|---|---|---|
| **M0** | 环境 + 空窗口 + Core 层（窗口/资源/渲染/固定步长时钟） | ✅ 完成 |
| **M1** | 最小可玩：玩家 + 射击 + 敌机 + 圆形碰撞 + 对象池 | ✅ 完成 |
| **M2** | 数据驱动波次系统 + 事件总线 + 分数 + HUD | ✅ 完成 |
| **M3** | Boss（状态机）+ 道具（策略）+ 音效 + 粒子 | ✅ 完成 |
| **M4** | ASan 零泄漏 + 单元测试 + 文档打磨 | ⏳ 待做 |

---

## 用到的设计模式（面试重点）

| 模式 | 用在哪 | 解决什么 |
|---|---|---|
| **对象池 Object Pool** | 子弹 / 敌机 / 敌弹 / 道具 / 粒子 | 热路径零分配，避免频繁 new/delete 的碎片与悬空指针 |
| **状态机 State** | `Boss` 的 Entering/Phase1/Phase2/Defeated | 替代 if 分支地狱；新增阶段不改旧逻辑（开闭原则） |
| **策略 Strategy** | `PowerUp` 的 `IItemEffect`（回血/速射/护盾） | 同一种容器装不同效果对象，免 if-else 分支 |
| **观察者 / 事件总线 EventBus** | 击杀 → 加分、Boss 发射 → 生成敌弹 | 系统互不认识，靠事件解耦 |
| **数据驱动** | 波次 `WaveDef` 配置表 | 改关卡只改数据，不碰逻辑 |
| **单例（克制）** | `AssetManager` / `AudioManager` | 仅资源/音频缓存用单例；系统编排走依赖注入 |

---

## 目录结构

```
planewar/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp                 # App 层入口（主循环）
│   ├── core/                    # 对 SFML 的薄封装 + 工具
│   │   ├── Window / Renderer / AssetManager / AudioManager
│   │   ├── ObjectPool.h / Math.h / Time.h
│   │   └── SpriteId.h
│   └── game/                    # 纯玩法（头文件不引用 SFML）
│       ├── Entity / Player / Bullet / Enemy
│       ├── Boss / PowerUp / Particle
│       ├── World                # 系统编排器
│       ├── events/EventBus
│       ├── waves/WaveDef + WaveManager
│       ├── ScoreManager / Hud
│       ├── Collision / Input / GameConfig
├── assets/                      # 图片/字体/音效（占位即可）
└── docs/                        # 设计规划 + 学习文档
```

---

## 许可

代码以 MIT 许可证开源（见 `LICENSE.txt`）。美术素材版权归 Kenney（CC0）。
