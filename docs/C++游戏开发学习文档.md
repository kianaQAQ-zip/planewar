# 从 Java / Python 到 C++ 游戏开发 · 学习文档

> 对象：**只会 Java 和 Python，要上 C++ 做游戏**的你
> 目标：用最短路径补上 C++ 与那两门语言最大的鸿沟，并衔接 [`设计思路与整体规划.md`](./设计思路与整体规划.md) 的飞机大战项目
> 风格：不堆语法八股，只讲**会让你写出 bug / 面试被问死**的差异点

---

## 0. 先调心态：C++ 最大的不同

| 你习惯的 | C++ 的现实 |
|---|---|
| Java/Python 有 GC，对象不用管死活 | **C++ 没有 GC。你分配，你负责释放。** 这是负担，也是面试亮点 |
| 一切皆引用，传参几乎不担心拷贝 | 默认**值语义**：`T a = b;` 是拷贝。大对象要显式传引用 |
| 运行时报错、报错友好 | **编译期**抓大量错误，报错又臭又长，习惯就好 |
| 一个文件一个类，import 自动 | 头文件 `.h` + 实现 `.cpp`，`#include` 手动管理，还有「链接」这一步 |

**一句话**：C++ 把内存和性能的控制权交给你。游戏每秒 60 帧、成千上万个对象，正是这种控制力值钱的地方。你不需要一口吃成专家，但下面这几块**必须过关**，否则飞机大战写不下去。

---

## 1. 概念映射表（先建立直觉）

把你脑子里已有的东西，直接对号入座：

| Java / Python | C++ 对应 | 注意点 |
|---|---|---|
| `class` | `class` | 几乎一样，但**析构函数 `~Class()` 代替 finalize** |
| `interface` / 抽象类 | 纯虚类 `class I { virtual void f() = 0; }` | 没有 `interface` 关键字，靠纯虚函数模拟 |
| `List<T>` | `std::vector<T>` | 默认顺序数组，**不是链表**。下标访问 O(1) |
| `Map<K,V>` | `std::unordered_map<K,V>` | 哈希表；要有序用 `std::map`（红黑树） |
| `Set<T>` | `std::set` / `std::unordered_set` | — |
| `null` | `nullptr` | 别再用 `NULL` 或 `0` |
| `try/finally` | **RAII**（析构自动清理） | C++ 几乎不用 finally，靠对象离开作用域自动析构 |
| `String` | `std::string` | 别用 C 的 `char*` |
| 包 / module | `namespace` | 用命名空间防名字冲突 |
| 泛型 `<T>` | 模板 `template<typename T>` | 对象池 `ObjectPool<T>` 就靠它 |
| Lambda | Lambda `[&](int x){...}` | 游戏里回调/事件常用 |
| 异常 `throw` | 异常**但游戏项目常禁用** | 用返回值/错误码代替，性能 & 可预测性 |

> 记住这表，你读 C++ 代码的速度直接翻倍。

---

## 2. C++ 硬核要点（游戏必备，逐条过关）

### 2.1 栈 vs 堆，值语义 vs 引用
```cpp
Player p1;                 // 在栈上，函数结束自动销毁（RAII）
Player* p2 = new Player(); // 在堆上，必须 delete，否则泄漏
```
- **能放栈就放栈**（快、自动清理）。只有「生命周期跨函数 / 数量运行时决定」才放堆。
- `T a = b;` 是**拷贝**！想共享用引用 `T&` 或指针 `T*`。

### 2.2 指针与引用（你最该练的肌肉）
- 引用 `T&`：别名，必非空，传参首选 `void Hit(const Enemy& e)`。
- 指针 `T*`：可能为空，表示「可选/不拥有」。判空用 `if (p)` 或 `if (p != nullptr)`。
- **悬空指针**：指向已释放的内存。C++ 里 `delete` 后指针不会自动变 null——这就是对象池和智能指针存在的理由。

### 2.3 智能指针（对应 Java 的引用 / Python 的对象管理）
这是你从"会漏内存"到"安全"的桥梁，**必学三种**：

| 智能指针 | 语义 | 游戏里怎么用 |
|---|---|---|
| `std::unique_ptr<T>` | 独占拥有，不能拷贝只能移动 | 对象池拥有子弹；大多数"主人"用它 |
| `std::shared_ptr<T>` | 共享拥有，引用计数 | 多个系统共有一个资源时（少用） |
| `std::weak_ptr<T>` | 不拥有，防循环引用 | 缓存/观察者里临时拿，避免悬空 |

```cpp
std::unique_ptr<Bullet> b = std::make_unique<Bullet>(); // 推荐 make_*
// 离开作用域自动 delete，零泄漏
```
> 面试金句：「能用 `unique_ptr` 就不用 `shared_ptr`——引用计数是开销，多数时候一个对象只有一个主人。」

### 2.4 const 正确性与引用传参
```cpp
void Draw(const Renderer& r);   // 只读，不拷贝，不修改
```
- 大对象**传 `const T&`**，别传值（白拷贝一份）。
- 成员函数不修改状态就标 `void Update() const;`，编译器帮你守住。

### 2.5 RAII —— C++ 的"自动 try/finally"
**核心思想**：资源（内存、文件、锁）的获取即初始化，释放交给析构。对象一出作用域，析构自动跑。
```cpp
{ 
    std::ofstream f("log.txt");  // 构造即打开
    f << "hi";
} // 离开作用域，析构自动关闭文件——不用手动 close
```
游戏里：子弹出池自动复位、锁自动释放、纹理析构自动卸载。**这就是 C++ 没有 GC 却很少漏资源的原因。**

### 2.6 移动语义（浅尝，不深究）
`std::move(x)` 把对象"搬走"而非拷贝，省一次大拷贝。对象池、返回大容器时用。理解「移动 = 偷指针，原对象置空」即可，先会用，原理以后补。

### 2.7 STL 容器选型
- 默认 `std::vector`——顺序存储、缓存友好，游戏性能首选。
- 哈希查找用 `std::unordered_map`。
- 别用 `std::list`（缓存差），除非真要频繁中间插入。

### 2.8 头文件与包含守卫
```cpp
// Foo.h
#pragma once            // 防重复包含（主流编译器都支持）
class Foo { ... };
```
`.h` 放声明，`.cpp` 放实现。改实现只重编那个 `.cpp`，快。

---

## 3. 环境搭建（Windows，照做即可）

你已验证过编译器能跑（`test_env.cpp` 是冒烟测试）。接下来接 SFML：

1. **编译器**：Visual Studio 2022（Community 免费，带 MSVC）**或** MinGW-w64 + GCC。新手首选 VS，调试体验最好。
2. **装 SFML**：去 [sfml-dev.org](https://www.sfml-dev.org) 下预编译包（选匹配你编译器的版本，比如 `SFML-2.6.1-windows-vc2022-64-bit`）。解压到 `C:/libs/sfml`。
3. **CMake**：装 CMake，在项目 `CMakeLists.txt` 里 `find_package(SFML ...)`（见规划文档第 7 节）。
4. **跑最小窗口**（验证 SFML 链路通了再写游戏）：
```cpp
#include <SFML/Graphics.hpp>
int main() {
    sf::RenderWindow w(sf::VideoMode(800, 600), "PlaneWar");
    while (w.isOpen()) {
        sf::Event e; while (w.pollEvent(e)) if (e.type == sf::Event::Closed) w.close();
        w.clear(); w.display();
    }
}
```
> 卡在这一步很正常，SFML 的 include/link 路径是新手第一道坎。打通它，你就已经比一半"只会语法"的求职者强。

---

## 4. 游戏循环与基础（结合本项目）

- **deltaTime（dt）**：每帧耗时，所有运动 `pos += vel * dt`，否则帧率一变速度就变。
- **固定步长**：见规划文档第 5 节——这是你和专业新手的分水岭，务必实现。
- **渲染**：SFML 里 `sf::Texture` 加载图片，`sf::Sprite` 挂纹理并设位置，`w.draw(sprite)`。
- **输入**：`sf::Keyboard::isKeyPressed(sf::Keyboard::Left)` 轮询状态（按住移动用轮询，非事件）。

---

## 5. 渐进学习路线（按周安排，配练习）

| 周 | 主题 | 练习（务必动手） |
|---|---|---|
| **W1** | C++ 语法过渡 + 环境 | 写一个**控制台贪吃蛇**或**打砖块**：练类、vector、const、RAII |
| **W2** | 智能指针 + 对象池 + 固定步长 | 给打砖块加对象池管理小球；改固定步长循环 |
| **W3** | 本项目 M1（规划文档） | 玩家移动+射击+一种敌机+碰撞。**先跑起来** |
| **W4** | 设计模式落地 | 套用工厂/事件总线/状态机重构 M1；写 2~3 个 Catch2 测试 |
| **W5** | M2~M3 | 波次+分数+HUD+Boss+道具；用 ASan 验证零泄漏 |
| **W6** | 面试包装 | 写 README、背熟模式话术、录一段运行 demo 视频 |

**铁律**：每个阶段都必须有「能编译、能跑」的东西，不要憋大招。

---

## 6. Java / Python 开发者最常踩的坑（看熟，别中招）

1. **返回局部变量的引用/指针**：`T& f(){ T x; return x; }` —— x 已销毁，返回悬空。返回对象就返回值或 `unique_ptr`。
2. **浅拷贝共享状态**：`std::vector` 拷贝是深拷贝（还好），但自己写的类如果只拷指针，两个对象会共享同一块内存，改一个炸另一个。需要深拷贝就自己写拷贝构造/`Clone()`。
3. **忘了释放 / 双重释放**：用 `new` 就想着 `delete`，或者智能指针接管后**别再手动 delete**。
4. **在循环里狂 `new`**：每帧 new 子弹 = 内存碎片 + 卡顿。对象池解决。
5. **全局 `using namespace std;`**：放头文件里会污染所有包含者。只在 `.cpp` 顶部谨慎用，或直接写 `std::`。
6. **把异常当流程控制**：游戏项目常禁用异常（`-fno-exceptions`），用返回值表示错误。
7. **忘记 `virtual` 析构**：基类指针 `delete` 派生对象时，不写 `virtual ~Base() = default;` 会只调基类析构，派生部分泄漏。

---

## 7. 推荐资源

- **书**：《C++ Primer》（第 5 版，前 12 章足够入门）、《Effective C++》（面试前翻一遍，全是陷阱）。
- **SFML 官方教程**：[sfml-dev.org/tutorials](https://www.sfml-dev.org/tutorials.php) —— 跟着敲一遍窗口/精灵/输入。
- **Catch2**：单头单元测试框架，给碰撞/对象池写测试，工程能力加分。
- **AddressSanitizer**：编译加 `-fsanitize=address`，一键查泄漏。

---

## 8. 与本项目衔接

学完哪一节，就能写规划的哪一块：

| 你掌握的 | 可以动手的模块 |
|---|---|
| 第 2.1~2.5（类/指针/智能指针/RAII） | `Entity` 基类、`Player`、`Bullet`（`ObjectPool` 用 `unique_ptr`） |
| 第 2.3 + 第 3 节（SFML 环境） | `core/Window`、`Renderer`、`Input` |
| 第 4 节（游戏循环） | `main.cpp` + `Time` 固定步长 |
| 第 1 表 + 2.7（STL） | `WaveManager` 的波次表、`CollisionSystem` 实体列表 |
| 规划文档第 3 节（模式） | 事件总线、策略 AI、状态机——**边写边对照** |

---

## 9. 给面试官的一句话底牌

> 「我清楚 C++ 没有 GC，所以在这个项目里我用对象池做零分配热路径、用 `unique_ptr` 明确拥有权、用 RAII 管资源，并用 AddressSanitizer 验证过零泄漏。设计上我刻意用了对象池、状态机、事件总线和策略模式来解耦系统——每个模式都为解决具体问题，不是为用而用。」

这句话，配上能跑的代码，足以让中小/独立工作室的面试官点头。去写吧，先让 M1 跑起来。
