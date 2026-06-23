---
type: concept
title: "第三章：键盘输入与蛇移动"
created: 2026-06-22
tags: [curriculum, cpp, game-dev, input, movement]
status: developing
related:
  - "[[Snake Game Curriculum]]"
  - "[[snake-game-call-graph]]"
---

# 第三章：键盘输入与蛇移动

> **目标**：让蛇动起来 -- 能转向，不掉头，不抽搐。
> **学时**：4 课时，约 6--8 小时。
> **前提**：已读完第一章（C++ 基础语法）和第二章（游戏主循环与场景切换）。

这一章是整个 Snake 项目的心脏。读完第一章时你大概能看懂变量的声明，读第二章时你能理解场景怎么切换，但蛇还不动。这一章之后，你的蛇会真正在屏幕上走起来，你用 WASD 控制方向，它不会莫名其妙地掉头撞死自己，也不会因为你的手速太快而抽搐。

---

## 3.1 按键检测 -- "你按了什么键？"

> **课时**：1 课时（约 1.5--2 小时）
> **目标**：理解 `key_pressed()` 的工作原理，能区分"按下瞬间"和"持续按住"

### 3.1.1 游戏怎么知道你按了键？

你坐在电脑前按下 W 键，蛇往上走了。这中间发生了什么？操作系统（Windows）一直在监听键盘硬件。每次你按下一个键，键盘芯片给主板发一个信号，操作系统把它包装成"按键消息"扔给正在前台运行的程序。

问题是：操作系统的按键消息是一个"事件队列"。比如说你在 1 秒内狂按了 50 次 W，Windows 会给你扔 50 条消息进队列。如果你一帧只处理一条消息，剩下 49 条就堆在那里等你。这在游戏里会出大问题——蛇在玩家手里会变得很迟钝。

所以 Snake 游戏**不走消息队列**。它用了一个更暴力的办法：不管消息队列里有多少，直接问 Windows："W 键现在有没有按着？"

这个"直接问"的函数叫 `GetAsyncKeyState`。

### 3.1.2 逐行拆解 key_pressed()

打开 `input.cpp`，找到这个函数。你看到的其实就是十一行代码，但十一行代码里有四个知识点：

```cpp
// input.cpp — key_pressed()
static bool was_down[256] = {false};   // 第 7 行

bool key_pressed(int vk)               // 第 9 行
{
    bool now_down = (GetAsyncKeyState(vk) & 0x8000) != 0;  // 第 11 行
    bool pressed = now_down && !was_down[vk];               // 第 12 行
    was_down[vk] = now_down;                                // 第 13 行
    return pressed;                                         // 第 14 行
}
```

我们一行一行来。

**第 7 行：`static bool was_down[256] = {false};`**

这是一个数组，256 个 `bool`，全部初始化为 `false`。256 是因为 Windows 的虚拟键码（Virtual Key Code）最多 256 个值。`'W'` 是 87，`VK_ESCAPE` 是 27，`VK_SPACE` 是 32——每个键都有一个数字编号，`was_down` 数组的每一个槽位就对应一个键。

`was_down[87]` 存的是"上一帧 W 键有没有按着"。`was_down[32]` 存的是"上一帧空格有没有按着"。

这个数组是整个按键系统的**记忆**。没有它，你就没法判断一个键是"刚刚按下"还是"一直在按"。

`static` 关键字让这个数组只在 input.cpp 文件内部可见，外部代码碰不到。

**第 9 行：`bool key_pressed(int vk)`**

函数的签名。`vk` 是 Virtual Key code（虚拟键码）。你可以传 `'W'`（字符）、`VK_ESCAPE`（Windows 定义的宏）、`VK_UP`（方向键）等等。函数返回 `true` 表示"这个键刚刚被按下的那一瞬间"。

**第 11 行：`bool now_down = (GetAsyncKeyState(vk) & 0x8000) != 0;`**

这一行是整章最"硬核"的一行。我们拆成三部分。

**第一部分：GetAsyncKeyState 是啥？**

`GetAsyncKeyState` 是 Windows API 提供的一个函数，来自 `<windows.h>`。你传给它一个虚拟键码，它返回一个 `SHORT` 类型的值。`SHORT` 就是 16 位整数（2 个字节），范围 -32768 到 32767。

注意这个名字里的 "Async"：异步。意思是"不管你的程序现在在处理什么消息，我就直接告诉你键盘现在的物理状态"。它不排队，不缓冲，问就是当前时刻。

**第二部分：& 0x8000 是什么？**

`0x8000` 是一个十六进制数。我们把它写成二进制：

```
0x8000 = 1000 0000 0000 0000  (二进制)
```

这是 16 位的二进制数字，只有最高位是 1，其他全是 0。

`GetAsyncKeyState` 返回的 `SHORT` 值的最高位就是"这个键目前是否按着"的标志。如果最高位是 1，键是按下状态；最高位是 0，键是松开状态。

`&` 是按位与运算符。它把两个数字的每一位对比，两个都是 1 结果位才是 1：

```
假设 GetAsyncKeyState 返回了:
  1100 0000 0000 0001   (随便编的一个值)
& 1000 0000 0000 0000   (0x8000)
------------------------
= 1000 0000 0000 0000   (最高位被保留了，其他位全被清零)
```

如果按键没有按着，最高位是 0，按位与的结果就是：
```
  0100 0000 0000 0001
& 1000 0000 0000 0000
------------------------
= 0000 0000 0000 0000   = 0
```

所以 `(GetAsyncKeyState(vk) & 0x8000) != 0` 这句话翻译成人话就是："这个键的最高位是 1 吗？"如果是 1，`now_down` 为 `true`；如果是 0，`now_down` 为 `false`。

**第三部分：为啥不直接比较？**

你可能会想：直接把 `GetAsyncKeyState` 的返回值和某个数字比不就行了？为什么还要 `&` 一下？

因为 `GetAsyncKeyState` 返回值的**最低位**也有含义——它表示"在上一次调用 `GetAsyncKeyState` 之后，这个键有没有被按过"。如果你直接用 `==` 比较整个值，最低位会干扰你的判断。用 `& 0x8000` 就是**只关心最高位**，其他位不管——不管它们是什么，都被清零了。

这是一个很典型的"掩码"（mask）操作。在嵌入式开发和底层系统编程里非常常见。

**第 12 行：`bool pressed = now_down && !was_down[vk];`**

这是"上升沿检测"的数学表达。

- `now_down`：这一帧，键是按着的吗？
- `!was_down[vk]`：上一帧，键是没按着的吗？（`!` 是逻辑非，取反）

两个条件同时成立，就是"从没按→按下"的那个瞬间。电工学里管这个叫"上升沿"（rising edge）：信号从低电平跳到高电平的那一瞬间。

```
时间轴:  ...|____|‾‾‾‾|‾‾‾‾|____|...
帧:         F1    F2    F3    F4   F5
now_down:   0     1     1     1    0
was_down:   0     0     1     1    1
pressed:    0     1     0     0    0
                                  ↑ 只有 F2 返回 true
```

F2 那一帧，`now_down` 是 1（按着），`was_down` 是 0（上一帧没按着），所以 `pressed` 为 `true`。F3 和 F4 虽然键还按着，但 `was_down` 已经是 1 了，`!was_down` 为 `false`，所以 `pressed` 返回 `false`。

这就是 `pressed` 和 `held` 的本质区别，我们马上会讲到。

**第 13 行：`was_down[vk] = now_down;`**

这是更新"记忆"。把这一帧的状态存进 `was_down`，下一帧才能判断"这一帧相对于上一帧的变化"。

这一行放在 `return` 之前，但没关系——C++ 里赋值语句是先执行赋值，再执行 `return`。这行执行完之后，`was_down[vk]` 已经更新了，然后第 14 行把之前算好的 `pressed` 值返回出去。

### 3.1.3 key_pressed vs key_held

同一个文件里还有一个更简单的函数：

```cpp
// input.cpp — key_held()
bool key_held(int vk)
{
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}
```

它不做上升沿检测。只要键是按着的，每一帧都返回 `true`。

**什么时候用 pressed，什么时候用 held？**

| 场景 | 用哪个 | 为什么 |
|------|--------|--------|
| 蛇转向（WASD） | `key_pressed` | 按一下就转一次，不能连续转 |
| 菜单选择（数字键） | `key_pressed` | 按一下 1 就进经典模式，不是一直按着一直进 |
| 暂停/继续（空格） | `key_pressed` | 按一下切换一次状态，不能来回闪 |
| 加速（Shift/F） | **`key_held`** | 按住就一直加速，松手就恢复 |
| 重新开始（R） | `key_pressed` | 按一次重启一次 |

如果你把蛇转向写成 `key_held`，按住 W 的时候每一帧都会触发转向——但 `set_direction` 里有 `turn_locked` 保护（3.2 节会讲），所以不会出 bug。可菜单就不一样了：按住 1 不放，每一帧都在"选择经典模式"，可能导致莫名其妙的闪烁或死循环。所以记住一条简单的原则：

> **一次性动作用 pressed，持续效果用 held。**

### 3.1.4 从 scene.cpp 看实际用法

打开 `scene.cpp`，找 `PlayScene::update()`。你看到的是这样的代码：

```cpp
// scene.cpp — PlayScene::update() 中的输入处理部分
if (key_pressed('W'))
    set_direction(&_game, UP);
else if (key_pressed('S'))
    set_direction(&_game, DOWN);
else if (key_pressed('A'))
    set_direction(&_game, LEFT);
else if (key_pressed('D'))
    set_direction(&_game, RIGHT);

// 加速键——用 held，不是 pressed
if (key_held(VK_SHIFT) || key_held('F'))
    interval = MOVE_INTERVAL_MS / 4;  // 160ms → 40ms，4 倍速
```

注意这里的 `else if` 链：W、A、S、D 四个键只可能触发其中一个。如果你按了 W，就不会再检查 A、S、D。这是故意的——一帧只允许改一次方向。

而 Shift/F 的加速没有用 `else if`，它是独立的 `if`，因为加速和转向可以同时发生（一边按着 Shift 加速，一边按 W 转向，完全合理）。

### 3.1.5 常见错误

**错误 1：把 pressed 写成 held——菜单闪烁**

```cpp
// WRONG ❌：每一帧都在触发，菜单在"经典模式"和"啥也不选"之间疯狂跳动
if (key_held('1'))
    selected_mode = MODE_CLASSIC;

// CORRECT ✅：只触发一次
if (key_pressed('1'))
    selected_mode = MODE_CLASSIC;
```

**错误 2：忘记 & 0x8000，直接比大小**

```cpp
// WRONG ❌：GetAsyncKeyState 返回的不只是"按没按"
bool down = GetAsyncKeyState('W') > 0;  // 逻辑正确但语义模糊

// CORRECT ✅：明确只检查最高位
bool down = (GetAsyncKeyState('W') & 0x8000) != 0;
```

虽然第一种写法在大多数情况下也能用（因为最低位的那个标志很少为真），但第二种写法的意图清晰——"我就是只看最高位，其他位我不管"。代码不仅是写给编译器的，也是写给下一个读代码的人的。

**错误 3：混淆 VK_ESCAPE 和 'ESC'**

```cpp
// WRONG ❌：'ESC' 不是单个字符，根本编译不过
if (key_pressed('ESC'))

// CORRECT ✅：用 Windows 定义的宏
if (key_pressed(VK_ESCAPE))
```

### 3.1.6 练一练

写一个独立的 EasyX 程序（不需要 Snake 的完整代码），在黑色窗口里画一个 20x20 的白色方块。按上下左右键，方块每次移动 20 像素。按一次键走一次，不能按住一直走。

**要求：**
1. 用 `initgraph` 创建 640x480 窗口
2. 用 `cleardevice` 清屏，用 `solidrectangle` 画方块
3. **自己实现** `key_pressed`（复制上面那十几行代码，含 `was_down` 数组）
4. 不需要 `key_held`——这个练习只用 `pressed`
5. 按 ESC 退出

**预期行为：**
- 按一下右键，方块往右走一格（20px），松手不动
- 按住右键不放，方块只走一格，不会连走
- 方块不会走出窗口边界（建议加边界检查）

<details>
<summary>点击查看参考答案</summary>

```cpp
#include <graphics.h>
#include <windows.h>

// -------------------------------------------------------
// 复制 input.cpp 的按键检测核心逻辑
// -------------------------------------------------------
static bool was_down[256] = {false};

bool key_pressed(int vk)
{
    bool now_down = (GetAsyncKeyState(vk) & 0x8000) != 0;
    bool pressed = now_down && !was_down[vk];
    was_down[vk] = now_down;
    return pressed;
}

// -------------------------------------------------------
// 主程序
// -------------------------------------------------------
int main()
{
    initgraph(640, 480);
    BeginBatchDraw();

    int x = 100, y = 100;
    int size = 20;

    while (true)
    {
        // 输入
        if (key_pressed(VK_UP)    && y - size >= 0)     y -= size;
        if (key_pressed(VK_DOWN)  && y + size <= 480)   y += size;
        if (key_pressed(VK_LEFT)  && x - size >= 0)     x -= size;
        if (key_pressed(VK_RIGHT) && x + size <= 640)   x += size;
        if (key_pressed(VK_ESCAPE)) break;

        // 渲染
        cleardevice();
        setfillcolor(WHITE);
        solidrectangle(x - size / 2, y - size / 2,
                       x + size / 2, y + size / 2);
        FlushBatchDraw();
        Sleep(16);
    }

    EndBatchDraw();
    closegraph();
    return 0;
}
```

运行之后你会发现：如果不用 `key_pressed` 而是改成 `key_held`，按住右键方块就会飞出去。这就是 `pressed` 的"只触发一次"的效果。
</details>

---

## 3.2 方向锁定 -- "不能掉头，不能抽风"

> **课时**：1 课时（约 1.5--2 小时）
> **目标**：理解 `turn_locked` 和 `is_opposite` 的防错机制，能手动模拟玩家狂按 WASD 时程序的行为

### 3.2.1 一个真实的问题

假设你现在玩的 Snake 游戏**没有**方向锁定机制。你正在往上走，突然决定往右转。你的手指很诚实地按下了 D 键。目前为止一切正常。

但你的手速很快。在同一帧里（一帧只有 16 毫秒），你的手指在 WASD 上敲出了这样的序列：

```
W(松开) → D(按下) → S(按下) → A(按下)
```

四个键在 **同一帧内** 全部被检测到。游戏会怎么反应？

- 检测到 W 松开：不管
- 检测到 D：蛇头向右
- 检测到 S：蛇头向下——等等，这和"向右"差了 90 度，合法
- 检测到 A：蛇头向左——从"向下"变成"向左"，也合法

你的蛇在一帧之内完成了：上 → 右 → 下 → 左。它自己撞上了自己的身体——因为蛇头从向上直接变成了向左，而前一秒蛇身还在它后面。

更恐怖的情况是 180 度掉头。你正在往右走，在同一帧内按了左键。蛇头直接从右变成左，"啃"到了紧跟在自己后面的第二节身体——当场死亡。

这就是 `turn_locked` 和 `is_opposite` 要解决的两个问题：

| 问题 | 解决方案 |
|------|----------|
| 一帧内多次转向（导致抽风） | `turn_locked` |
| 180 度掉头（导致撞自己） | `is_opposite` |

### 3.2.2 is_opposite() —— 数学上的"反方向"

打开 `game.cpp`，这是一个非常短但非常关键的工具函数：

```cpp
// game.cpp — is_opposite()
static bool is_opposite(Direction a, Direction b)
{
    return (a == UP    && b == DOWN)  ||
           (a == DOWN  && b == UP)    ||
           (a == LEFT  && b == RIGHT) ||
           (a == RIGHT && b == LEFT);
}
```

它判断两个方向是否互为 180 度反向。四种情况：

- 你在往上，玩家按了下 → **掉头**，return true
- 你在往下，玩家按了上 → **掉头**，return true
- 你在往左，玩家按了右 → **掉头**，return true
- 你在往右，玩家按了左 → **掉头**，return true

其他所有组合（上→右、下→左、右→下等等）都是 90 度转弯，return false——合法。

注意 `static` 关键字：这个函数只在 `game.cpp` 内部使用，外部代码不需要、也不应该单独调用它。

### 3.2.3 set_direction() —— 逐行拆解

这是控制蛇方向的核心函数，在 `game.cpp` 里：

```cpp
// game.cpp — set_direction()
void set_direction(GameState *game, Direction dir)
{
    // 第一步：本帧已经转过向了吗？
    if (game->turn_locked)      // 第 683 行
        return;                 // 转过了，直接走人

    // 第二步：游戏还没开始？（玩家的首次输入）
    if (!game->started)         // 第 685 行
    {
        if (can_start_in_direction(game, dir))  // 第 687 行
        {
            game->direction = dir;              // 第 689 行
            game->started = true;               // 第 690 行
            game->turn_locked = true;           // 第 691 行
        }
        return;
    }

    // 第三步：180 度掉头？
    if (!is_opposite(game->direction, dir))     // 第 696 行
    {
        game->direction = dir;                  // 第 698 行
        game->started = true;                   // 第 699 行
        game->turn_locked = true;               // 第 700 行
    }
}
```

我们逐步推演。

**第一步（第 683--684 行）：检查 turn_locked**

如果 `turn_locked` 为 `true`，说明本帧已经有其他按键触发了方向变更。二话不说，`return` 直接退出——后面的代码根本不会执行。

这就是解决"一帧内多次转向"的唯一手段。不管你在这一帧里按了多少个键，只有第一个被 `set_direction` 处理的键会生效，后面的全部被 `turn_locked` 挡在门外。

**第二步（第 685--693 行）：游戏还没开始的情况**

`game->started` 初始值是 `false`。蛇待在原地，等你按第一个方向键。

`can_start_in_direction(game, dir)` 是一个安全检查：你按的方向会不会开局就撞墙？如果蛇头紧贴着上边界，你按 W 就会直接死——所以这个函数会拦截。如果检查通过，设置方向、标记 `started = true`、锁定 `turn_locked = true`。

**第三步（第 696--701 行）：180 度掉头检查**

这是游戏进行中的正常转向逻辑。`!is_opposite(game->direction, dir)` 就是说"新方向和当前方向不是 180 度反向才放行"。如果不是掉头，更新方向并锁定；如果是掉头（比如当前向上、你按了 S），直接跳过——蛇继续保持原方向前进。

注意 `game->started = true` 在第 699 行又写了一次。这行看上去是冗余的（游戏已经开始了），但它是**防御性编程**的体现：万一有其他代码把 `started` 设成了 `false`，这行能保证游戏不会莫名其妙地"停住"。

### 3.2.4 解锁时机 —— turn_locked 什么时候归位？

`turn_locked` 被设为 `true` 之后，谁来把它改回 `false`？答案是 `move_snake` 函数的末尾：

```cpp
// game.cpp — move_snake() 最后一行（第 315 行）
game->turn_locked = false;
```

**逻辑**：转向只是一次"意图声明"。蛇真正移动是在 `move_snake` 里执行的。也就是说，方向变更（`set_direction`）和物理移动（`move_snake`）不在同一时刻发生：

```
帧 1: set_direction → turn_locked = true  （声明：我要往右）
       move_snake    → 蛇往右走一格        （执行：实际移动）
                     → turn_locked = false （解锁：下一帧可以再次转向）

帧 2: set_direction → turn_locked = true  （声明：我要往下）
       move_snake    → 蛇往下走一格
                     → turn_locked = false
```

所以 `turn_locked` 的"锁定窗口"只有一帧。下一帧开始，`turn_locked` 已经恢复 `false`，玩家又可以转向了。

### 3.2.5 常见错误

**错误 1：混淆 `is_opposite` 的调用方向**

```cpp
// WRONG ❌：方向反了。game->direction 是当前方向，dir 是玩家想去的方向
is_opposite(dir, game->direction)

// CORRECT ✅
is_opposite(game->direction, dir)
```

虽然这个函数的参数是对称的（`is_opposite(A, B)` 和 `is_opposite(B, A)` 结果一样），但语义上一定要**当前方向在前、新方向在后**，这样读代码的人一眼就知道你在问"从当前方向变成新方向，算不算掉头"。

**错误 2：忘记在 move_snake 末尾解锁**

```cpp
// WRONG ❌：move_snake 末尾少了这一行
// game->turn_locked = false;

// 后果：蛇只能转一次向，之后永远直走
```

如果你在实验课里故意注释掉 `turn_locked = false` 这一行，你会看到：蛇按照你第一次按键的方向一路走到底，之后按什么都没反应。这就是"忘了解锁"的症状。

### 3.2.6 练一练

**实验 1（观察 bug）**：找到 `game.cpp` 的 `set_direction` 函数，把第 683 行的 `if (game->turn_locked) return;` 注释掉（前面加 `//`）。重新编译运行。在游戏里狂按 WASD（四个键轮流快速敲），观察蛇的行为。你会看到什么现象？

<details>
<summary>点击查看预期结果</summary>

蛇会在某些帧里一次转好几个弯。如果你手速够快，一帧内连续按 W→D→S→A，蛇可能头朝上来一个 180 度翻转，直接撞上自己的身体。这就是没有 `turn_locked` 时的"抽风"现象。

改回去之后记得把注释删掉。
</details>

**实验 2（观察掉头）**：注释掉第 696 行的 `is_opposite` 检查，改成 `if (true)` 让它永远通过。运行游戏，故意在往右走的时候按 A（左）。发生了什么？

<details>
<summary>点击查看预期结果</summary>

蛇头直接撞上了第二节身体，游戏立刻 Game Over。因为蛇正在往右（→），第二节身体恰好在蛇头的左边。当你按 A（←）时，蛇头 180 度转向，走到了第二节身体所在的格子上——蛇不能和自己共处一格，判定为撞身死亡。

没有 `is_opposite` 保护的 Snake 基本没法玩——稍微手滑一下就会死。
</details>

---

## 3.3 蛇的移动逻辑 -- "追尾算法"

> **课时**：1 课时（约 1.5--2 小时）
> **目标**：完全理解蛇身"追尾移动"的算法，能手动在纸上模拟蛇走 5 步，能读懂 `move_snake()` 的每一行代码

### 3.3.1 蛇是怎么"走"的？

初学者最容易犯的错误是认为：蛇的每一节都是独立移动的。蛇头往右走一格，蛇身的每一节也都往右走一格。**这是错的。**

正确的理解是：蛇的身体不是"各自往一个方向走"，而是**像一个火车**。火车头拉着后面的车厢，每一节车厢追着前一节车厢走。更准确地说，每一节身体**取代前一节身体刚才的位置**。

我们把这个过程用文字图来演示。假设一条 4 节长的蛇（H=头, B=身体），正在往右走：

```
初始状态:
列:  0   1   2   3   4   5
    [ ][ ][H][B1][B2][B3]    ← 头在列2，B1在列1，B2在列0，B3在列-1（咦？不对）

重画，让蛇更直观。网格坐标(col, row)，只关心 col 的变化：
初始: [B3][B2][B1][H]  方向→右，等待移动
     col=0   1   2   3
```

好吧，用 ASCII 网格来画更清楚。一条 4 节蛇，横着放：

```
Step 0（初始状态）:
  列:  0   1   2   3   4   5   6
     [  ][  ][B3][B2][B1][H ][  ]
              蛇尾        蛇头
           ← 身体从尾到头排列 →
```

蛇要往右走一步（方向 = RIGHT，即 col+1）。按照 `move_snake` 的逻辑，分四步：

**Step 1: 计算新蛇头位置**

新头 = (旧头的 col + 1, 旧头的 row)。新头在列 6。那个格子是什么？空地。安全。

**Step 2: 不移除尾巴（本步是普通移动，不长蛇）**

- 在地图上把尾巴格子（列 3）从 `'B'` 改成 `'.'`（空地）
- 蛇身数组实际长度不变，但尾巴格子被"回收"了

```
Step 2（移除尾巴后）:
  列:  0   1   2   3   4   5   6
     [  ][  ][B3][  ][B1][H ][  ]
              蛇尾消失   旧B1 旧头
```

**Step 3: 身体前移——从尾到头依次移位**

```cpp
for (int i = length - 1; i > 0; i--)
    snake[i] = snake[i - 1];
```

把 B3 移到 B2 的位置，B2 移到 B1 的位置，B1 移到旧头的位置：

```
Step 3（身体前移后，蛇身数组状态）:
  snake[3] = 旧 snake[2] → B3 移到列 4（原来是 B2 的位）
  snake[2] = 旧 snake[1] → B2 移到列 5（原来是 B1 的位）
  snake[1] = 旧 snake[0] → B1 移到列 6（原来是 H 的位）
  snake[0] 还没填，马上填
```

```
网格视图:
  列:  0   1   2   3   4   5   6
     [  ][  ][  ][  ][B3][B2][B1]
                           旧头变身体
```

**Step 4: 蛇头放到新位置**

```cpp
snake[0] = next;
game->map[next.row][next.col] = 'H';
```

```
最终状态:
  列:  0   1   2   3   4   5   6
     [  ][  ][  ][  ][B3][B2][H ]
              旧尾巴       旧头变成B1 新头
```

对比初始和最终：蛇整体往右平移了一格。但注意每节身体的移动方式——B1 移到了旧头的位置，B2 移到了旧 B1 的位置，B3 移到了旧 B2 的位置。**每一节都在追前面那节刚才站的位置。**

这就是"追尾算法"。

### 3.3.2 逐行拆解 move_snake()

打开 `game.cpp`，核心函数在 268--316 行。我们一起来读：

```cpp
// game.cpp — move_snake()（经典/障碍模式）
void move_snake(GameState *game)
{
    // 前置检查：游戏开始了吗？暂停了吗？结束了吗？
    if (!game->started || game->paused || game->game_over)
        return;

    // 计算新蛇头位置
    Point old_head = game->snake[0];
    Point next = next_head(old_head, game->direction);

    // 新蛇头位置是食物吗？
    bool will_eat = (game->map[next.row][next.col] == 'F');
    // 自动增长（每 grow_step 步长一节）
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
    // 两种情况下都会变长
    bool will_grow = will_eat || auto_grow;
```

先看 `next_head` 这个辅助函数（在 game.cpp 第 198 行）：

```cpp
static Point next_head(Point head, Direction dir)
{
    Point next = head;
    switch (dir)
    {
    case UP:    next.row--; break;
    case DOWN:  next.row++; break;
    case LEFT:  next.col--; break;
    case RIGHT: next.col++; break;
    }
    return next;
}
```

非常简单：根据方向，把行或列加减 1。

回到 `move_snake`，继续往下：

```cpp
    // 碰撞检测
    DeathReason reason = DEATH_NONE;
    if (!can_enter_cell(game, next, will_grow, game->snake, game->length, &reason))
    {
        game->game_over = true;
        game->death_reason = reason;
        return;  // 死了，不再移动
    }
```

`can_enter_cell` 检查新蛇头能不能进入目标格子（后续章节会详细展开，现在你只需要知道它检查了墙、障碍物、蛇身）。

```cpp
    // 不移除尾巴 = 变长；移除尾巴 = 正常移动
    if (!will_grow)
    {
        Point tail = game->snake[game->length - 1];
        game->map[tail.row][tail.col] = '.';
    }
    else if (game->length < MAX_SNAKE_LEN)
    {
        game->length++;  // 长了一节！
    }

    // 旧头变身体
    game->map[old_head.row][old_head.col] = 'B';
```

**关键理解**：`if (!will_grow)` 分支只在地图上把尾巴格子清空为 `'.'`。蛇身数组 `snake[]` 的长度没有变，尾巴坐标还留在数组里——但在下一步的 `for` 循环里，它会被整体前移覆盖掉。

如果 `will_grow` 为 `true`（吃到了食物或者本步是自动增长步），尾巴格子**不清空**，并且 `length++`——蛇多了一节。这就是"吃食物变长"的实现。

```cpp
    // 身体整体前移——从尾巴往头方向遍历
    for (int i = game->length - 1; i > 0; i--)
        game->snake[i] = game->snake[i - 1];

    // 蛇头放到新位置
    game->snake[0] = next;
    game->map[next.row][next.col] = 'H';
    game->move_count++;

    // 吃到了食物？加分，放新食物
    if (will_eat)
    {
        game->score += 10;
        game->food_eaten++;
        place_food(game);
    }

    // 解锁方向——下一帧可以再次转向
    game->turn_locked = false;
}
```

### 3.3.3 为什么是 i-- 而不是 i++？

`for (int i = game->length - 1; i > 0; i--)` 这个循环**从尾巴遍历到头**。

假设我们从 `i = 1` 开始（头后面第一节），往 `i++` 方向遍历：

```
初始: snake[0]=H  snake[1]=B1  snake[2]=B2  snake[3]=B3

如果 i=1→2→3 (i++):
  i=1: snake[1] = snake[0] = H   → snake[1] 变成了 H，B1 丢失！
  i=2: snake[2] = snake[1] = H   → snake[2] 也变成了 H！
  i=3: snake[3] = snake[2] = H   → 全乱了

如果 i=3→2→1 (i--):
  i=3: snake[3] = snake[2] = B2  → B3 被覆盖为 B2 ✓
  i=2: snake[2] = snake[1] = B1  → B2 被覆盖为 B1 ✓
  i=1: snake[1] = snake[0] = H   → B1 被覆盖为 H ✓
  然后 snake[0] = next           → H 被更新为新位置 ✓
```

**正向遍历（i++）** 会覆盖还未被读取的值——你把 `snake[0]` 的值复制到 `snake[1]`，然后下一步又要用 `snake[1]` 去覆盖 `snake[2]`，但 `snake[1]` 已经不是原来的值了。这叫"写入污染源数据"。

**反向遍历（i--）** 从最远的尾巴开始，被覆盖的永远是不会再被用到的位置。这是数组元素移位的一个经典技巧，不仅 Snake 里用得到，在任何需要"把数组全体往后挪一格"的场景里都需要这样做。

### 3.3.4 "长蛇"的特殊处理

当蛇吃到食物（或者遇到自动增长步）时，`will_grow` 为 `true`：

```cpp
if (!will_grow)           // ← 这个条件为 false，不执行
{
    // 清除尾巴
}
else if (game->length < MAX_SNAKE_LEN)  // ← 这个条件执行
{
    game->length++;       // 蛇多了一节！
}
```

注意：`length++` 发生在身体前移的 `for` 循环之前。也就是说，`snake[数组]` 现在多了一个槽位（原来的 `snake[length]` 现在被包含进来了，它是一个还未初始化的位置）。

然后 `for (int i = game->length - 1; i > 0; i--)` 从新的尾巴位置开始遍历——原本的倒数第一节仍然用旧值，但倒数第二节（新多出来的）会被倒数第三节覆盖。最终效果是：蛇多了一节，新多出来的那节留在原来的尾巴位置不动，而其他身体节正常前移。

用人话说：**蛇头往前进，尾巴不收回来——蛇就长了一节。**

过程图示（吃食物时）：

```
吃食物前: [B3][B2][B1][H]  (length=4)
                  新蛇头在食物位置
吃食物后: [B3][B2][B1][H][新H]  (length=5, 尾巴B3没有被清除)
                  旧头变身体  新头
```

### 3.3.5 常见错误

**错误 1：认为每节身体独立移动**

```text
// WRONG ❌ 理解：
// 蛇头往右一格，B1 也往右一格，B2 也往右一格……各自走各自的

// CORRECT ✅ 理解：
// 每节身体"继承"前一节的位置，像一列火车
```

**错误 2：正序遍历身体数组**

```cpp
// WRONG ❌：从前往后覆盖，数据被污染
for (int i = 1; i < game->length; i++)
    game->snake[i] = game->snake[i - 1];

// CORRECT ✅：从后往前覆盖
for (int i = game->length - 1; i > 0; i--)
    game->snake[i] = game->snake[i - 1];
```

### 3.3.6 练一练

拿一张白纸，画一条 3 节长的蛇，初始位置如下（蛇头朝右）：

```
初始:
snake[0] = (row=5, col=5)  // 蛇头
snake[1] = (row=5, col=4)  // 身体 1
snake[2] = (row=5, col=3)  // 身体 2（尾巴）
```

在纸上画一个坐标网格（row 0--10, col 0--10）。手动模拟这条蛇走 5 步（每次往右走一格），在每一步里标出蛇头（H）和身体（B1, B2）的新位置。

要求：
1. 每一步都在纸上写出 `snake[0]`, `snake[1]`, `snake[2]` 的新坐标
2. 在坐标网格上标出 H、B1、B2
3. 验证每次移动后蛇是不是整体往右平移了一格

<details>
<summary>点击查看参考答案</summary>

```
Step 0（初始）:
  snake[0] = (5,5) 蛇头
  snake[1] = (5,4) B1
  snake[2] = (5,3) B2

  col:  0 1 2 3 4 5 6 7 8 9
row 5:  . . . B2 B1 H  . . . .


Step 1（往右）:
  新头 next = (5,6)
  移除尾巴: 地图上 (5,3) 变空
  身体前移: snake[2] = 旧 snake[1] = (5,4)
            snake[1] = 旧 snake[0] = (5,5)
  头放新位: snake[0] = (5,6)

  结果: snake[0]=(5,6) snake[1]=(5,5) snake[2]=(5,4)

  col:  0 1 2 3 4 5 6 7 8 9
row 5:  . . . . B2 B1 H  . . .


Step 2（往右）:
  新头 next = (5,7)
  移除尾巴: (5,4) 变空
  身体前移: snake[2]=(5,5) snake[1]=(5,6)
  头放新位: snake[0] = (5,7)

  snake[0]=(5,7) snake[1]=(5,6) snake[2]=(5,5)


Step 3（往右）:
  snake[0]=(5,8) snake[1]=(5,7) snake[2]=(5,6)


Step 4（往右）:
  snake[0]=(5,9) snake[1]=(5,8) snake[2]=(5,7)


Step 5（往右）:
  snake[0]=(5,10) snake[1]=(5,9) snake[2]=(5,8)
```

验证：每一步蛇的整体位置都往右平移了一格，节与节之间的相对位置（每一节都在前一节左边一格）保持不变。
</details>

---

## 3.4 定时移动 -- "不在键盘上敲木鱼"

> **课时**：1 课时（约 1.5--2 小时）
> **目标**：理解游戏循环中帧更新与游戏逻辑更新的分离，能用 `GetTickCount` 实现定时器

### 3.4.1 每帧都移动？太快了！

你的游戏主循环是这样的（回顾第二章）：

```cpp
while (true)
{
    current->update();   // 处理输入和逻辑
    current->render();   // 画画面
    FlushBatchDraw();
    Sleep(16);           // 约 60 FPS
}
```

一秒钟跑大约 60 帧，每帧只有 16 毫秒。如果蛇每帧都移动一格，一秒钟蛇走 60 格——在 20x20 的地图里，蛇不需要两秒就能从左边撞到右边墙。这根本不是游戏，是光速自杀。

所以我们需要一个**定时器**：不是每帧都移动，而是每隔固定时间（比如 160 毫秒）才移动一次。这样蛇每秒大约走 6 格，在地图里游走是合理的节奏。

### 3.4.2 GetTickCount() —— 系统的心脏跳动

`GetTickCount()` 是 Windows API 提供的函数（`<windows.h>`）。它返回一个 `DWORD`（无符号 32 位整数），代表**从 Windows 启动到现在的毫秒数**。

它的典型用法是：记录上一次"做某件事"的时间戳，然后每帧检查"现在的时间戳减去上一次的时间戳"是否超过了设定的阈值。

在 `scene.cpp` 里，`PlayScene` 有一个私有成员变量：

```cpp
// scene.h — PlayScene 的私有成员（简化）
DWORD _last_move;   // 上次蛇移动的时间戳
```

每帧在 `update()` 里检查：

```cpp
// scene.cpp — PlayScene::update() 的定时移动部分（第 106--122 行）
DWORD now = GetTickCount();
DWORD interval = MOVE_INTERVAL_MS;  // 160

// 加速：按住 Shift 或 F 时，间隔缩短为 1/4
if (key_held(VK_SHIFT) || key_held('F'))
    interval = MOVE_INTERVAL_MS / 4;  // 160 / 4 = 40

if (now - _last_move >= interval)
{
    // 时间到了，让蛇走一步
    if (_game.mode == MODE_PVP && !_game.game_over)
        move_pvp_snakes(&_game);
    else if (_game.mode == MODE_BATTLE && !_game.game_over)
        move_battle_snakes(&_game);
    else
        move_snake(&_game);

    _last_move = now;  // 重置计时器
}
```

我们一行一行理解。

**`DWORD now = GetTickCount();`**

拿到当前的时间戳。`DWORD` 就是 `unsigned long`，存的是毫秒数。假设你的电脑开机了 3 小时，`GetTickCount()` 返回大约 `3 × 3600 × 1000 = 10,800,000`。

**`DWORD interval = MOVE_INTERVAL_MS;`**

`MOVE_INTERVAL_MS` 是在 `game.h` 里定义的常量——160 毫秒。也就是说正常情况下，蛇每 160ms 走一格，约每秒 6.25 格。

**加速机制**

```cpp
if (key_held(VK_SHIFT) || key_held('F'))
    interval = MOVE_INTERVAL_MS / 4;
```

注意这里是 `key_held`，不是 `key_pressed`。因为加速不是"按一下加一次速"，而是"按住就一直加速"。当你按住 Shift 时，`interval` 变成 40 毫秒（160 / 4 = 40），蛇每秒走 25 格——几乎是平时的 4 倍速度。

**`if (now - _last_move >= interval)`**

这是定时器的核心判断。如果上一次移动距离现在已经超过了设定的间隔，就执行移动。

注意 `now - _last_move` 用的是无符号减法。当 `now` 小于 `_last_move` 时（比如系统运行时间太长导致 `DWORD` 溢出回绕），无符号减法的结果是一个非常大的数，反而会触发一次移动——这不是 bug，而是一个安全余量。它保证了计时器在溢出时不会"卡住"。

**`_last_move = now;`**

移动完毕后，把当前时间设为"上一次移动的时间"。这样下一帧检查时，时间差又要重新累积到 `interval` 才会触发下一次移动。

### 3.4.3 重要理解：蛇是自动走的

很多刚接触 Snake 的玩家会有一个误解：蛇是"按一下方向键走一格"。**不是的。** 蛇是自动持续移动的。你按方向键只是**改变它的前进方向**，而不是让它走一步。

这个设计的后果是：

- 你不按任何键，蛇也会一直往前走（直到撞墙）
- 你按住一个方向键不放，蛇不会走得更快，因为方向只有改变的时候才生效
- 加速靠的是 Shift/F（改变移动间隔），而不是猛按方向键

这就是为什么方向键用 `key_pressed`（只捕获"按下瞬间"的变化），而加速键用 `key_held`（持续检测）。方向是"声明式"的——你声明一个方向，蛇一直沿那个方向走，直到你再次声明新方向。加速是"持续式"的——你按住它就一直有效，松手就失效。

### 3.4.4 帧更新 vs 游戏逻辑更新的分离

这是一个很重要的游戏编程概念。

| 更新类型 | 频率 | 做什么 |
|----------|------|--------|
| 帧更新（渲染） | 60 FPS（约每 16ms） | 清屏、画画、翻页 |
| 逻辑更新（蛇移动） | ~6.25 Hz（每 160ms） | 移动蛇、碰撞检测、吃食物 |
| 输入处理 | 60 FPS | 检测按键 |

这三种更新在同一个 `update()` 方法里，但频率不同。输入和渲染每帧都跑，蛇移动只在时间间隔到了才执行。这就是"解耦"——把渲染节奏和游戏逻辑节奏分开。

这个模式的正式名称叫**固定时间步长**（Fixed Timestep）。你会在几乎所有游戏引擎里看到这个概念——Unity 的 `FixedUpdate`、Unreal 的 `Tick` 都是这个思想的产物。

### 3.4.5 常见错误

**错误 1：把移动放在每帧逻辑里，没有间隔判断**

```cpp
// WRONG ❌：蛇一秒钟走 60 格，瞬间撞墙
void update()
{
    handle_input();
    move_snake(&_game);     // 每帧都移动！
    _last_move = now;
}

// CORRECT ✅：只在间隔到了才移动
void update()
{
    handle_input();
    if (now - _last_move >= interval)
    {
        move_snake(&_game);
        _last_move = now;
    }
}
```

**错误 2：忘记重置 _last_move**

```cpp
// WRONG ❌：移动了但没更新时间戳，下一帧再次移动
if (now - _last_move >= interval)
{
    move_snake(&_game);
    // 忘了写 _last_move = now;
}

// 后果：每一帧都会触发移动，跟没写间隔判断一样
```

**错误 3：用 Sleep 代替定时器**

```cpp
// WRONG ❌：阻塞了整个游戏循环，画面冻结 160ms
while (true)
{
    handle_input();
    move_snake(&_game);
    render();
    Sleep(160);   // 卡死 160ms，渲染也停了，输入也停了
}
```

正确的做法是每帧只 Sleep 16ms（保持流畅渲染），然后把移动逻辑和累积时间绑定。这样画面始终保持 60 FPS 的流畅度，但蛇的移动节奏是独立的。

### 3.4.6 练一练

**实验**：打开 `game.h`，找到 `MOVE_INTERVAL_MS` 的定义（第 37 行）。把它改成不同的值，观察效果。

```cpp
// game.h 第 37 行
#define MOVE_INTERVAL_MS    160
```

**步骤：**

1. **改为 500**：编译运行。蛇走得很慢，每半秒走一格。你会感觉自己在玩"慢动作版 Snake"，很容易预判和转向。这是一个"初学者友好"的速度。
2. **改为 50**：编译运行。蛇快得像子弹，每秒 20 格。在 20x20 的地图里一秒就能从左边到右边。这个速度下你根本来不及反应，很快就死了。
3. **改回 160**：正常速度。这大概是你玩 Google Snake 时的默认速度体验。

思考：为什么 160 是一个"合适"的值？因为 20x20 的地图宽度是 20 格。160ms/格 × 20 格 = 3.2 秒横穿地图。这个时间给了玩家足够的反应窗口。

<details>
<summary>点击查看预期输出对比</summary>

```
interval = 500ms:
  - 每秒走 2 格
  - 横穿 20 格地图需要 10 秒
  - 感觉：慢悠悠，容易操控，适合新手

interval = 160ms（默认）:
  - 每秒走约 6.25 格
  - 横穿 20 格地图需要约 3.2 秒
  - 感觉：适中，有紧张感但不至于手忙脚乱

interval = 50ms:
  - 每秒走 20 格
  - 横穿 20 格地图需要 1 秒
  - 感觉：极快，几乎无法反应，几秒就死
```

</details>

---

## 本章小结

这一章我们完成了 Snake 游戏的核心——让蛇动起来。回顾你学到的东西：

| 课时 | 内容 | 核心概念 |
|------|------|----------|
| 3.1 按键检测 | `key_pressed()` 与 `key_held()` | 上升沿检测、位运算掩码、`was_down` 状态记忆 |
| 3.2 方向锁定 | `set_direction()` 与 `is_opposite()` | 一帧一转向、禁止 180 度掉头、`turn_locked` 的锁定/解锁时机 |
| 3.3 蛇的移动 | `move_snake()` | 追尾算法、反向遍历避免数据污染、长蛇/普通蛇的差异 |
| 3.4 定时移动 | `GetTickCount()` 与间隔判断 | 帧更新与逻辑更新分离、固定时间步长、加速机制 |

这些概念是你理解后续章节（碰撞检测、AI 寻路、食物生成）的基石。蛇动起来之后，下一章我们要解决"蛇为什么会死"——完整的碰撞检测系统。

**下一章**：[[ch4-collision|第四章：碰撞检测与游戏结束]]

---

*本章代码引用自项目源码：`input.cpp`（按键检测）、`game.cpp`（移动逻辑、方向锁定）、`scene.cpp`（定时移动、输入分发）、`game.h`（常量与结构体定义）。*
