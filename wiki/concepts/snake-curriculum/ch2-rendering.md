---
type: concept
title: "第二章：游戏框架与绘图"
chapter: 2
course: "Snake Game — Zero to C++ Game Dev"
created: 2026-06-22
updated: 2026-06-22
tags:
  - curriculum
  - cpp
  - game-dev
  - easyx
  - rendering
status: developing
related:
  - "[[Snake Game Curriculum]]"
  - "[[EasyX Graphics Library]]"
  - "[[snake-game-call-graph]]"
sessions: 4
duration: "6-8 小时"
goal: "理解游戏循环，能用 EasyX 画出基本图形"
---

# 第二章：游戏框架与绘图

> **目标**：学完本章后，你能够理解游戏循环的原理，用 EasyX 在屏幕上画出基本图形，并且看懂 Snake 源码中所有渲染相关的代码。
> **原则**：先理解"为什么"，再动手写代码。每一节都从一个你会遇到的问题出发。

---

## 2.1 游戏循环模式 —— "所有游戏的心跳"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解游戏循环的 input-update-render 结构，知道帧率是什么

### 你写的程序 vs 游戏的区别

到目前为止，你在第一章里写的所有程序都是一种模式：启动、算完结果、打印、退出。整个程序的生命周期可能不到一毫秒。

```cpp
int main() {
    int sum = 0;
    for (int i = 1; i <= 100; i++)
        sum += i;
    cout << sum << endl;   // 输出 5050
    return 0;              // 结束了
}
```

这种程序叫**一次性程序**（one-shot program）。它就像一个自动售货机：你投币、选饮料、拿到饮料、结束。

但游戏不是这样的。游戏更像一个**永不停歇的旋转木马**：它在每一瞬间都在做三件事——听你的按键、算蛇该往哪走、把新的画面画出来。然后马上又做下一轮。只要游戏没结束，它就一直在转。

这个"旋转木马"的学名叫**游戏循环**（Game Loop）。

### 一个最简单的游戏循环

```cpp
while (游戏还没结束) {
    处理玩家按键();        // input
    更新游戏逻辑();        // update
    绘制画面();           // render
}
```

让我们拆开来讲每一步在 Snake 里具体做什么。

#### Input（输入）—— "玩家刚才按了什么？"

游戏需要知道你有没有按下 W、A、S、D 或者空格键。Snake 的 `input.cpp` 里专门有一个 `key_pressed()` 函数来检测按键。但你不只是在检测"键有没有被按着"——你在检测"键是不是**刚刚**被按下"。这两者的区别非常关键：

- "按着"：你手指一直压在 W 上，每一帧检测都是 true。游戏如果每一帧都响应，你的蛇会疯狂转向，根本控制不了。
- "刚刚按下"：从"没按"变成"按"的那个瞬间才触发一次。这就是**边缘检测**（edge detection）。

这个细节在第一章 1.2 节我们分析过，这里你只需要记住：游戏循环的 input 阶段，只关心**状态变化**，不关心**持续状态**。

#### Update（更新）—— "蛇该动一下了"

拿到方向之后，游戏要做逻辑计算。在 Snake 里，更新包括：
- 根据方向算出蛇头的新位置
- 判断新位置是食物？墙？障碍物？自己的身体？对手的身体？
- 如果是食物：加分、增长蛇身、在别处放新食物
- 如果是墙/障碍物/身体：游戏结束
- 把整条蛇向前挪一格（从尾到头依次移位）

在 `game.cpp` 里，这个逻辑被封装在 `move_snake()` 函数里，大约 80 行。你现在不用看懂每一行，只需要知道：**update 阶段负责修改数据，不画任何东西**。

#### Render（渲染）—— "把数据变成画面"

数据（分数、蛇的坐标、地图的字符）只是一堆数字和字符，玩家看不到。渲染阶段的工作就是把这些数据**翻译成**屏幕上的颜色和形状。

在 Snake 里，渲染阶段做这些事：
- 遍历地图的每个格子，根据字符（`#` / `.` / `F` / `H` / `B` 等）选择颜色，画一个实心方块
- 在右侧面板上写分数、长度、状态等文字
- 如果游戏结束，在屏幕中间写"GAME OVER"

渲染在 `render.cpp` 里完成，核心函数叫 `draw_game()`。我们会在 2.4 节详细拆解它的实现。

### 从 Snake 源码的 main.cpp 逐行讲解

现在我们来看真实的游戏循环。打开 Snake 的 `main.cpp`，核心代码在这里：

```cpp
int main()
{
    initgraph(MENU_WINDOW_WIDTH, MENU_WINDOW_HEIGHT);  // ①
    disable_ime();                                       // ②
    BeginBatchDraw();                                    // ③

    Scene *current = new MenuScene();                    // ④

    while (true)                                         // ⑤
    {
        current->update();                               // ⑥ input + update
        current->render();                               // ⑦ render
        FlushBatchDraw();                                // ⑧
        Sleep(16);                                       // ⑨

        SceneType next = current->next_scene();          // ⑩
        if (next == SCENE_EXIT)
            break;
        // ... 场景切换逻辑（你现在可以跳过这部分）
    }

    delete current;
    EndBatchDraw();
    closegraph();
    return 0;
}
```

逐行解释：

| 行 | 代码 | 做什么 | 你现在需要理解的程度 |
|----|------|--------|---------------------|
| ① | `initgraph(W, H)` | 打开一个图形窗口，指定宽度和高度 | 每一帧画面都会画在这个窗口里 |
| ② | `disable_ime()` | 关掉输入法（否则按 W 可能会触发中文输入） | 不影响理解，知道有这么回事就行 |
| ③ | `BeginBatchDraw()` | 开始双缓冲模式（2.3 节细讲） | 先记住，画游戏要写这一行 |
| ④ | `new MenuScene()` | 创建主菜单场景 | 相当于"游戏启动，先显示主菜单" |
| ⑤ | `while (true)` | **死循环**。游戏一直跑，直到内部 break | 这就是游戏循环的开始 |
| ⑥ | `current->update()` | 处理输入 + 更新逻辑 | **input + update** |
| ⑦ | `current->render()` | 绘制画面 | **render** |
| ⑧ | `FlushBatchDraw()` | 把幕后画好的画面一次性显示到屏幕 | 双缓冲的"切换"动作 |
| ⑨ | `Sleep(16)` | 让程序休息 16 毫秒 | 控制帧率，下面细讲 |
| ⑩ | `current->next_scene()` | 检查是否需要切换场景 | 比如按 Esc 回到菜单 |

### 帧率概念：60 FPS 是什么意思？

**FPS** = Frames Per Second，每秒画面刷新次数。

- 60 FPS = 每秒画 60 次画面 = 每帧大约 16.67 毫秒
- 30 FPS = 每秒画 30 次画面 = 每帧大约 33.33 毫秒
- 10 FPS = 每秒画 10 次画面 = 你看到的是幻灯片，不是动画

动画的本质就是**快速切换静止画面**。就像翻页动画书——如果你翻得够快（每秒翻 24 次以上），大脑会觉得画面是"连续运动"的。这就是电影的 24 FPS 标准。

游戏一般追求 60 FPS，因为玩家需要实时操作反馈——延迟超过 16ms 就会感觉"不跟手"。

#### Sleep(16) 的作用

代码里有一行 `Sleep(16)`。它的意思是："暂停 16 毫秒"。

为什么要暂停？因为 CPU 太快了。如果没有这行 Sleep，游戏循环可能一秒钟跑几千次——蛇在一瞬间就撞墙了，你根本来不及操作。而且你的 CPU 风扇会狂转。

**Sleep(16) 不等于精确计时**。`Sleep(16)` 的准确含义是"至少等 16ms"——实际等待时间可能略长一点（比如 16.3ms、17ms、甚至如果你电脑在忙着干别的事可能是 20ms）。所以 Sleep 是一种**简易帧率控制**，不是精确计时。

更精确的做法是用高精度计时器（`QueryPerformanceCounter`），但对于 Snake 这个项目，Sleep(16) 完全够用。

> **小知识**：1000ms / 60 = 16.67ms。理论上应该 Sleep(16.67)，但 Sleep() 只接受整数毫秒，所以取 16。

### 与普通"一次性程序"的区别（一张表看明白）

| 特性 | 一次性程序 | 游戏（游戏循环） |
|------|-----------|-----------------|
| 运行方式 | 算完就退出 | 一直循环，直到用户关窗口 |
| 时间概念 | 没有 | 每一帧是一个时间片 |
| 用户交互 | 可能就一两次输入 | 每一帧都在检测输入 |
| 输出 | 打印到控制台 | 画到图形窗口 |
| 退出方式 | `return 0` | 循环内的 `break` 或关窗口 |
| 状态 | 用完就丢 | 持续存在，每帧更新 |

### 常见错误

**错误 1：游戏循环里没有 Sleep——CPU 100%**

```cpp
// WRONG ❌
while (true)
{
    current->update();
    current->render();
    FlushBatchDraw();
    // 没有 Sleep！循环跑得飞快
}
// 后果：CPU 一个核心占满 100%，风扇狂转，游戏快到没法玩
```

```cpp
// CORRECT ✅
while (true)
{
    current->update();
    current->render();
    FlushBatchDraw();
    Sleep(16);   // 控制帧率约 60 FPS
}
```

**错误 2：把 Sleep 放在循环的最前面**

```cpp
// WRONG ❌
while (true)
{
    Sleep(16);               // 先睡再干活
    current->update();
    current->render();
    FlushBatchDraw();
}
// 问题：第一帧也延迟了 16ms，玩家会感觉"按了键没反应"
```

```cpp
// CORRECT ✅
while (true)
{
    current->update();       // 先干活
    current->render();
    FlushBatchDraw();
    Sleep(16);               // 干完活再睡
}
```

**错误 3：在 update 里画图，或者在 render 里改数据**

```cpp
// WRONG ❌ —— 职责混淆
void update() {
    score += 10;
    outtextxy(100, 100, "Score: 10");  // update 不应该画图！
}

// CORRECT ✅
// update 只改数据，render 只画画面
void update() {
    score += 10;    // 只改数据
}

void render() {
    // 只画画面，从数据读取
    char buf[32];
    sprintf(buf, "Score: %d", score);
    outtextxy(100, 100, buf);
}
```

这个职责分离非常重要。把计算和绘制混在一起，代码会变成一团解不开的意大利面。Snake 的源码严格遵循这个分离：`update()` 只改 GameState，`render()` 只读 GameState 然后画图。

### 练一练

写一个最简单的动画程序：在 EasyX 窗口中画一个实心红色圆，让它每帧向右移动 1 个像素。

**要求**：
1. 窗口大小 640x480
2. 圆的初始位置在 (50, 240)，半径 20
3. 每帧清屏、在当前位置画圆、刷新、Sleep(16)
4. 圆移到右边超出窗口后退出循环

**预期效果**：红色圆从左边平滑移动到右边，移动过程流畅不闪烁。

> **提示**：你需要用到 `initgraph()`、`cleardevice()`、`solidcircle(x, y, r)`、`FlushBatchDraw()`、`Sleep(16)`、`closegraph()`。双缓冲部分如果还不理解没关系，下一节会详细讲——现在先照着写，用到 `BeginBatchDraw()` 和 `FlushBatchDraw()`。

### 参考答案

<details>
<summary>点击展开</summary>

```cpp
#include <graphics.h>

int main()
{
    initgraph(640, 480);
    BeginBatchDraw();

    int x = 50;            // 圆的 x 坐标
    int y = 240;           // 圆的 y 坐标（居中）
    int r = 20;            // 半径

    while (x <= 640 + r)   // 圆完全移出窗口后停止
    {
        cleardevice();                     // 清屏
        setfillcolor(RGB(255, 0, 0));      // 选红色画笔
        setlinecolor(RGB(255, 0, 0));      // 边框也用红色
        solidcircle(x, y, r);              // 画实心圆
        FlushBatchDraw();                  // 显示到屏幕
        Sleep(16);                         // 等待约 16ms

        x = x + 1;                         // 每帧右移 1 像素
    }

    EndBatchDraw();
    closegraph();
    return 0;
}
```

> 观察到什么了？圆的移动速度大约每秒 60 像素（因为 60 FPS x 1 像素/帧 = 60 像素/秒）。如果你把 Sleep(16) 改成 Sleep(32)，圆的速度大约减半——就是因为每秒执行的帧数减半了。

</details>

---

## 2.2 EasyX 绘图基础 —— "在屏幕上画东西"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：掌握 EasyX 的坐标系统、基本形状绘制、颜色设置和文字输出

### 坐标系：左上角是原点

EasyX 的坐标系和数学课上学的不太一样。在数学课上，原点通常在左下角，x 向右增大，y 向上增大。但在 EasyX（以及所有主流图形库和网页）中：

```
(0,0) ────────── x 轴（向右增大）
  │
  │   窗口
  │
  y 轴（向下增大）
```

- **原点 (0,0)** 在窗口的**左上角**
- **x** 向右增大（和数学课一样）
- **y** 向下增大（和数学课**相反**）

为什么这样设计？因为老式 CRT 显示器的电子束是从左上角开始逐行扫描的——硬件决定了软件的惯例，这个习惯一直延续到今天。

> **停下来画一画**：拿张纸画一个 10x10 的格子。标记左上角是 (0,0)，右下角是 (9,9)。然后试着在脑子里想象：坐标 (0,9) 在哪？（答案：左下角。）坐标 (9,0) 在哪？（答案：右上角。）

### 基本形状：实心 vs 空心

EasyX 画形状的函数命名很有规律：带 `solid` 前缀的是**实心**，不带 `solid` 的是**空心**（只画边框）。

| 函数 | 画什么 | 参数含义 |
|------|--------|---------|
| `solidrectangle(left, top, right, bottom)` | 实心矩形 | 左上角坐标 + 右下角坐标 |
| `rectangle(left, top, right, bottom)` | 空心矩形（只有边框） | 同上 |
| `solidcircle(x, y, r)` | 实心圆 | 圆心坐标 + 半径 |
| `circle(x, y, r)` | 空心圆（只有边框） | 同上 |
| `solidellipse(left, top, right, bottom)` | 实心椭圆 | 外接矩形的左上角和右下角 |

**来自 Snake 的真实代码**（`render.cpp` 的 `draw_cell()`）：

```cpp
static void draw_cell(int row, int col, char cell)
{
    int left = BOARD_LEFT + col * CELL_SIZE;
    int top = BOARD_TOP + row * CELL_SIZE;
    int right = left + CELL_SIZE - 1;
    int bottom = top + CELL_SIZE - 1;

    setfillcolor(cell_color(cell));
    setlinecolor(RGB(215, 221, 228));

    if (cell == 'F')    // 食物：实心圆 + 稍小的椭圆（画成苹果的样子）
    {
        solidrectangle(left, top, right, bottom);
        setfillcolor(RGB(232, 88, 80));
        solidellipse(left + 5, top + 5, right - 5, bottom - 5);
    }
    else
    {
        solidrectangle(left, top, right, bottom);
    }
}
```

这段代码展示了几个重要概念：
- 每个格子的屏幕坐标是通过 `BOARD_LEFT + col * CELL_SIZE` 计算的——把"逻辑坐标"（第几行第几列）转换成"屏幕坐标"（像素位置）
- `CELL_SIZE` 是 25（在 `game.h` 里定义），所以每个格子 25x25 像素
- `- 1` 是因为如果一个格子从 0 画到 25，它的宽度是 26 像素（0 到 25 包含两个端点）。`- 1` 后宽度就是 25。这看起来是个小细节，但如果不处理，格子和格子之间会有重叠的线条

### RGB 颜色：三个数字定义一个颜色

```cpp
RGB(红, 绿, 蓝)
```

`RGB(255, 0, 0)` 就是纯红色——红色分量拉满（255），绿色和蓝色都关掉（0）。

为什么是 0 到 255？因为一个字节（byte）有 8 个二进制位，能表示 2^8 = 256 个不同的值（0 到 255）。RGB 每个颜色通道用一个字节存，所以取值范围是 0~255。

常见的颜色组合：

| RGB 值 | 颜色 | 记忆方法 |
|--------|------|----------|
| `RGB(255, 0, 0)` | 纯红 | 红灯 |
| `RGB(0, 255, 0)` | 纯绿 | 绿灯 |
| `RGB(0, 0, 255)` | 纯蓝 | 蓝灯 |
| `RGB(255, 255, 255)` | 白色 | 所有灯全开 |
| `RGB(0, 0, 0)` | 黑色 | 所有灯全关 |
| `RGB(128, 128, 128)` | 灰色 | 所有灯开一半 |
| `RGB(255, 255, 0)` | 黄色 | 红 + 绿 = 黄（颜料和灯光不一样！） |
| `RGB(255, 0, 255)` | 品红（紫色） | 红 + 蓝 |
| `RGB(0, 255, 255)` | 青色 | 绿 + 蓝 |

> **关键区别**：RGB 是**加色混合**（光的混合），不是你小时候学的颜料混合（减色混合）。光混：红+绿=黄。颜料混：红+绿=棕黑。因为你是在屏幕上**发光**，不是在纸上**涂颜料**。

### 先选颜色，再画形状

EasyX 有一个重要的设计：画笔颜色是"全局状态"。你必须先用 `setfillcolor()` 和 `setlinecolor()` **选好颜色**，然后再调用画形状的函数。画形状的函数本身不接受颜色参数。

```cpp
// 正确的顺序：先选颜色，再画
setfillcolor(RGB(255, 0, 0));       // 选红色填充
setlinecolor(RGB(0, 0, 0));         // 选黑色边框
solidrectangle(0, 0, 100, 100);     // 现在画的矩形会用刚才选的颜色

// 改变颜色，再画下一个
setfillcolor(RGB(0, 0, 255));       // 换成蓝色
solidcircle(200, 200, 50);          // 现在画的圆是蓝色的
```

**类比**：就像你画画时先拿起一支红笔，再在纸上画线——然后换蓝笔，再画别的。EasyX 的 `setfillcolor` 就是"换笔"的动作。

### 文字输出：outtextxy 和 settextstyle

画文字用 `outtextxy(x, y, 字符串)`，在指定坐标输出文字。

但直接输出的话，文字可能会遮住背景色。在 Snake 里，信息面板上的文字需要**透明背景**（否则文字后面会有一块白色方块挡住地图）：

```cpp
setbkmode(TRANSPARENT);  // 设置文字背景为透明
```

来自 Snake 的真实代码：

```cpp
// 来自 render.cpp 的 draw_panel()
setbkmode(TRANSPARENT);

// 设置字体：高度 28 像素，宽度自适应（0），Consolas 字体
settextstyle(28, 0, _T("Consolas"));
settextcolor(RGB(35, 42, 52));   // 选文字颜色
outtextxy(px, 48, _T("Snake"));  // 输出文字
```

`settextstyle(高度, 宽度, 字体名)` 的三个参数：
- **高度**：字体的像素高度。28 表示大约 28 像素高
- **宽度**：0 表示自动按比例缩放。如果你写 15，字体会被压扁或拉宽
- **字体名**：`_T("Consolas")` 是等宽字体（程序员最爱）。你也可以用 `_T("宋体")`、`_T("微软雅黑")` 等

`_T(...)` 是一个宏，用于处理 Unicode 字符编码。你照着写就行，暂时不需要深究它背后的原理。

### 常见错误

**错误 1：忘了先设置颜色**

```cpp
// WRONG ❌
solidcircle(100, 100, 50);  // 用的是什么颜色？取决于上一次 setfillcolor
                            // 如果从来没设置过，可能是默认的白色——画了就看不见

// CORRECT ✅
setfillcolor(RGB(255, 0, 0));
setlinecolor(RGB(255, 0, 0));
solidcircle(100, 100, 50);  // 红的
```

**错误 2：坐标搞反——x 和 y 写错位置**

```cpp
// WRONG ❌
solidcircle(100, 200, 30);
// 你以为：x=200, y=100
// 实际上：x=100, y=200（y 更大 → 更靠下）
// 结果：圆出现在比预期更低的位置

// CORRECT ✅ —— 心里默念：第一个是 x（左右），第二个是 y（上下）
solidcircle(200, 100, 30);
```

**错误 3：把颜色设置放在循环外面，后面改了忘记**

```cpp
// WRONG ❌
setfillcolor(RGB(255, 0, 0));  // 设一次红色
for (int i = 0; i < 10; i++) {
    // 如果这里有一个分支条件改了 setfillcolor，
    // 下一轮循环可能用的是被改过的颜色，而不是你期望的红色
    solidrectangle(i * 30, 0, i * 30 + 25, 25);
}
// CORRECT ✅
for (int i = 0; i < 10; i++) {
    setfillcolor(RGB(255, 0, 0));  // 每次循环都明确设置，确保颜色正确
    solidrectangle(i * 30, 0, i * 30 + 25, 25);
}
```

> 这个错误在 Snake 的实际代码里没有出现——`draw_cell()` 函数每画一个格子就调一次 `setfillcolor(cell_color(cell))`，确保每个格子的颜色是正确的。这就是为什么 `cell_color()` 要写成单独的函数——它让每次设置颜色变得简单。

**错误 4：用 `outtextxy` 输出数字**

```cpp
// WRONG ❌
int score = 100;
outtextxy(10, 10, score);  // ❌ outtextxy 不接受 int！

// CORRECT ✅
int score = 100;
char buf[32];
sprintf(buf, "%d", score);     // 把数字转成字符串
outtextxy(10, 10, buf);        // 输出字符串
```

或者用 `_stprintf`（Snake 源码里用的方式，处理 `TCHAR` 类型）：

```cpp
TCHAR buf[64];
_stprintf(buf, _T("%d"), score);
outtextxy(10, 10, buf);
```

### 练一练

**练习 1：画一面五星红旗**

在 640x480 的窗口中画一面中国国旗。国旗的规格是：
- 红色背景（RGB(238, 28, 37)）
- 左上角（约窗口 1/4 处）有一颗大五角星（黄色，RGB(255, 255, 0)）
- 大五角星右边有四颗小五角星，每颗的一个角指向大五角星中心

> 简化版要求：只要红色背景 + 一个黄色的实心五角星图案（可以用五个圆近似摆成五角星的形状，或者在五角星的五个顶点画圆就行）。目标是练习坐标计算和颜色设置。

**练习 2：画一个国际象棋棋盘**

在窗口中画一个 8x8 的国际象棋棋盘。每一个格子是 50x50 像素。深色格子用 `RGB(100, 70, 40)`，浅色格子用 `RGB(240, 217, 181)`。

> 提示：用双重 for 循环。判断一个格子是深色还是浅色的规则：`(row + col) % 2 == 0` 是一种颜色，`== 1` 是另一种颜色。

**预期效果**：一个完整的棋盘图案，左上角是浅色格子。

### 参考答案

<details>
<summary>点击展开练习 1（五星红旗简化版）</summary>

```cpp
#include <graphics.h>
#include <math.h>

int main()
{
    initgraph(640, 480);
    BeginBatchDraw();

    // 红色背景
    cleardevice();
    setfillcolor(RGB(238, 28, 37));
    solidrectangle(0, 0, 640, 480);

    // 大五角星的中心位置（国旗左上区域）
    int cx = 160, cy = 140;
    int R = 40;  // 大星半径

    setfillcolor(RGB(255, 255, 0));
    setlinecolor(RGB(255, 255, 0));

    // 在五角星的 5 个顶点画圆（近似表达）
    // 五角星的 5 个顶点，从正上方开始，每个间隔 72°
    for (int i = 0; i < 5; i++)
    {
        double angle = (i * 72.0 - 90.0) * 3.14159 / 180.0;
        int x = cx + (int)(R * cos(angle));
        int y = cy + (int)(R * sin(angle));
        solidcircle(x, y, 5);
    }

    // 大星中心画一个小圆，让五个顶点连起来
    solidcircle(cx, cy, 10);

    FlushBatchDraw();
    Sleep(5000);  // 显示 5 秒
    EndBatchDraw();
    closegraph();
    return 0;
}
```

> 真正的五角星需要用 `polygon` 或 `solidpolygon` 函数来画——那需要你计算五个顶点的精确坐标。这个简化版让你先习惯用坐标和循环来表达图形。

</details>

<details>
<summary>点击展开练习 2（国际象棋棋盘）</summary>

```cpp
#include <graphics.h>

int main()
{
    initgraph(50 * 8 + 20, 50 * 8 + 20);  // 窗口比棋盘稍大一点
    BeginBatchDraw();

    cleardevice();

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            int left = 10 + col * 50;
            int top = 10 + row * 50;
            int right = left + 49;   // 50 - 1，避免重叠
            int bottom = top + 49;

            // (row + col) 的奇偶性决定颜色
            if ((row + col) % 2 == 0)
                setfillcolor(RGB(240, 217, 181));  // 浅色
            else
                setfillcolor(RGB(100, 70, 40));    // 深色

            setlinecolor(RGB(240, 217, 181));
            solidrectangle(left, top, right, bottom);
        }
    }

    FlushBatchDraw();
    Sleep(5000);  // 显示 5 秒
    EndBatchDraw();
    closegraph();
    return 0;
}
```

> 思考：为什么 `right = left + 49` 而不是 `left + 50`？因为如果格子的宽度是 50 像素，会跟下一个格子重叠 1 像素。`49` 就是 `50 - 1`——我们在 Snake 源码的 `draw_cell()` 里看到了同样的技巧。

</details>

---

## 2.3 双缓冲 —— "先在幕后画好，再变到台前"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解画面闪烁的原因，掌握 BeginBatchDraw/FlushBatchDraw 的原理和使用

### 问题演示：如果不双缓冲会怎样？

回忆上一节 2.1 练一练里的移动红圆程序。如果把 `BeginBatchDraw()` 和 `FlushBatchDraw()` 两行删掉，运行起来会出现什么？

你会看到圆在**闪烁**——不是流畅的移动，而是圆在消失和出现之间快速切换。这就是**画面撕裂**（screen tearing）和**闪烁**（flickering）。

原因是什么？因为你的每一帧都在做三件事：

1. `cleardevice()` —— 把整个屏幕清空（全黑）
2. `solidcircle()` —— 画一个红圆
3. `Sleep(16)` —— 等待

在步骤 1 和步骤 2 之间，屏幕上有一瞬间是**全黑的**。虽然只有几毫秒，但你的眼睛能捕捉到这个闪动。60 次闪动叠加起来，画面就像在抖。

### 双缓冲的原理

解决方法是：**不要直接往屏幕上画。先在幕后准备一张完整的画面，准备好了之后一次性换上去。**

```
单缓冲（闪烁版）：
    [屏幕] ← cleardevice（黑了！你看到了！） ← solidcircle（画好了） ← Sleep
           ↑
           玩家能看到中间过程

双缓冲（不闪版）：
    [幕后画布] ← cleardevice ← solidcircle（画好了）
                              ↓
    [屏幕     ] ← FlushBatchDraw 一次性切换（玩家永远看到完整画面）
```

**类比**：魔术师变魔术。

- **单缓冲** = 魔术师站在台前，一件一件地把道具从兜里掏出来。观众看到每一个中间步骤：先掏出一张手帕、再掏出半个鸽子、最后才把鸽子完整变出来。
- **双缓冲** = 魔术师先在幕后把所有道具准备好，摆成一个完美的造型，然后幕布一拉——观众只看到最终成品。

`BeginBatchDraw()` 就是"把幕布拉上，准备在幕后操作"。
`FlushBatchDraw()` 就是"画好了，幕布拉起来！"

### 来自 Snake 源码的真实使用

```cpp
// main.cpp
int main()
{
    initgraph(MENU_WINDOW_WIDTH, MENU_WINDOW_HEIGHT);
    BeginBatchDraw();          // ← 游戏一开始就进入双缓冲模式

    while (true)
    {
        current->update();     // 更新逻辑数据
        current->render();     // 在幕后画布上画画
        FlushBatchDraw();      // ← 把幕后画布显示到屏幕
        Sleep(16);
        // ...
    }

    EndBatchDraw();            // ← 退出双缓冲模式
    closegraph();
    return 0;
}
```

注意 `BeginBatchDraw()` 在 `while (true)` **外面**——只需要调用一次。而 `FlushBatchDraw()` 在循环**里面**——每一帧都要调用一次，把本帧的画面"推"到屏幕上。

在 `render.cpp` 里，`draw_game()` 内部还调用了一次 `cleardevice()`：

```cpp
void draw_game(const GameState *game)
{
    setbkcolor(RGB(247, 249, 252));  // 设置背景色
    cleardevice();                    // 用背景色清空幕后画布
    draw_board(game);                 // 画棋盘
    draw_panel(game);                 // 画信息面板
    if (game->game_over)
        draw_game_over(game);         // 如果游戏结束，画覆盖文字
}
```

因为有双缓冲，`cleardevice()` 清空的是**幕后画布**，而不是屏幕。玩家完全看不到这个清空过程——他们只看到 `FlushBatchDraw()` 之后展示的完整画面。

### 什么时候用双缓冲，什么时候可以不用？

| 场景 | 需要双缓冲吗 | 原因 |
|------|------------|------|
| 游戏主循环（60 FPS 动画） | **必须用** | 每一帧都在清屏+重绘，闪烁非常明显 |
| 静态画面（画一次就不动了） | 不需要 | 只在初始化时画一次，没有清屏-重绘循环 |
| 简单测试（画一个圆看看效果） | 不需要 | 没有动画，不会有闪烁 |
| 每一帧只加东西不清屏 | 可选 | 如果不调 `cleardevice()`，理论上不需要，但用双缓冲没坏处 |

简单记忆：**只要你的循环里有 `cleardevice()`，就一定要双缓冲**。

### 常见错误

**错误 1：写了 BeginBatchDraw 忘了 FlushBatchDraw**

```cpp
// WRONG ❌
BeginBatchDraw();
while (true)
{
    cleardevice();
    solidcircle(x, y, 20);
    // 忘了 FlushBatchDraw()！
    Sleep(16);
}
// 后果：画面一直在幕后画布上，屏幕上永远是第一帧的画面（或者黑屏）
// 玩家看不到任何动画，因为幕后画布从没被推到屏幕上
```

```cpp
// CORRECT ✅
BeginBatchDraw();
while (true)
{
    cleardevice();
    solidcircle(x, y, 20);
    FlushBatchDraw();    // 别忘了把画好的推上去
    Sleep(16);
}
EndBatchDraw();
```

**错误 2：把 BeginBatchDraw 写在循环里**

```cpp
// WRONG ❌
while (true)
{
    BeginBatchDraw();    // 每帧都创建新的双缓冲——浪费性能，而且可能内存泄漏
    cleardevice();
    solidcircle(x, y, 20);
    FlushBatchDraw();
    Sleep(16);
}
```

```cpp
// CORRECT ✅
BeginBatchDraw();        // 循环外，只调用一次
while (true)
{
    cleardevice();
    solidcircle(x, y, 20);
    FlushBatchDraw();    // 循环内，每帧一次
    Sleep(16);
}
EndBatchDraw();
```

**错误 3：在单缓冲模式下频繁清屏（闪烁的根源）**

```cpp
// WRONG ❌ —— 没有双缓冲的动画循环
// 没有 BeginBatchDraw()
while (true)
{
    cleardevice();                // 屏幕瞬间变黑——玩家看到了
    solidcircle(x, y, 20);        // 圆出现了——玩家才看到
    x++;
    Sleep(16);
}
// 后果：肉眼可见的闪烁。每帧前半段是黑的，后半段是圆。
```

```cpp
// CORRECT ✅
BeginBatchDraw();
while (true)
{
    cleardevice();                // 清的是幕后画布，玩家看不到
    solidcircle(x, y, 20);        // 在幕后画布上画圆
    FlushBatchDraw();             // 一次性把完整画面推到屏幕
    x++;
    Sleep(16);
}
EndBatchDraw();
```

### 练一练

画 100 个随机位置的彩色圆，对比有双缓冲和没有双缓冲的差别。

**要求**：
1. 窗口 800x600
2. 每帧生成 100 个随机位置的圆（随机 x、y、颜色、半径 5~30）
3. **版本 A**：使用双缓冲（BeginBatchDraw / FlushBatchDraw）
4. **版本 B**：不使用双缓冲（注释掉这两行）
5. 每帧运行约 200 帧后退出
6. 观察两个版本的视觉差异

**预期结果**：
- 版本 A：画面每帧干净切换，没有闪烁
- 版本 B：画面剧烈闪烁，眼睛不舒服

> 提示：用 `rand() % 最大值` 来生成随机数。每次循环前调用一次 `cleardevice()`。

### 参考答案

<details>
<summary>点击展开</summary>

```cpp
#include <graphics.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand((unsigned)time(NULL));  // 初始化随机种子
    initgraph(800, 600);

    // ★★★ 试验 1：有双缓冲 ★★★
    // 取消下面两行的注释来看有双缓冲的效果
    // BeginBatchDraw();
    // 注释掉上面两行来看无双缓冲的闪烁

    int frame = 0;
    while (frame < 200)
    {
        cleardevice();

        // 画 100 个随机圆
        for (int i = 0; i < 100; i++)
        {
            int x = rand() % 800;
            int y = rand() % 600;
            int r = 5 + rand() % 26;  // 5~30
            int red = rand() % 256;
            int green = rand() % 256;
            int blue = rand() % 256;

            setfillcolor(RGB(red, green, blue));
            setlinecolor(RGB(red, green, blue));
            solidcircle(x, y, r);
        }

        // ★★★ 试验 1：有双缓冲 ★★★
        // 取消下面这行的注释
        // FlushBatchDraw();

        Sleep(16);
        frame++;
    }

    // EndBatchDraw();  // 如果启用了双缓冲，取消这行的注释
    closegraph();
    return 0;
}
```

**如何做这个实验**：
1. 先把 `BeginBatchDraw()`、`FlushBatchDraw()`、`EndBatchDraw()` 全部**注释掉**运行一次。你会看到画面剧烈闪烁——因为每帧都在清屏然后慢慢画 100 个圆。
2. 然后**取消注释**这三行再运行一次。画面稳定光滑，没有闪烁。

这就是双缓冲的价值：**从"不能看"变成"能看"的区别**。

</details>

---

## 2.4 地图网格渲染 —— "把数据变成画面"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解 Snake 是如何把二维字符数组渲染成彩色格子的，能自己定义一个地图数组并渲染出来

### 回顾：地图数据长什么样

在 Snake 里，地图不是一张图片——它是一个**二维字符数组**。每个字符的含义：

| 字符 | 含义 | 颜色 |
|------|------|------|
| `#` | 边界墙 | 深灰 `RGB(45, 52, 63)` |
| `.` | 空地 | 浅灰白 `RGB(238, 242, 246)` |
| `O` | 障碍物 | 中灰 `RGB(103, 112, 124)` |
| `F` | 食物 | 红色 `RGB(218, 72, 66)` |
| `H` | 玩家蛇头 | 深绿 `RGB(24, 124, 88)` |
| `B` | 玩家蛇身 | 浅绿 `RGB(73, 180, 130)` |
| `A` | AI 蛇头 | 深蓝 `RGB(42, 108, 218)` |
| `C` | AI 蛇身 | 浅蓝 `RGB(97, 157, 237)` |

这种设计有个巨大的好处：**渲染逻辑和游戏逻辑完全解耦**。游戏逻辑只管往 `map[][]` 里写字符，渲染逻辑只管读字符然后挑颜色。它们之间唯一的约定就是"这些字符各代表什么"。

> **思考**：如果要新增一种格子类型（比如"毒蘑菇"），你需要改什么？只需要在游戏逻辑里写一种新字符（比如 `M`），在渲染的 `cell_color()` 里加一个 case 返回你想要的蘑菇颜色。渲染的其余部分不用动，游戏逻辑的其余部分也不用动。这就是"字符地图"这个设计的妙处。

### 逐行拆解 draw_board()

这是 Snake 里最关键的三层函数调用，让我们从外到内看。

**第一层：draw_board() —— 遍历地图的每一个格子**

```cpp
// render.cpp
void draw_board(const GameState *game)
{
    int ms = game->map_size;          // 地图边长（经典模式 20，对战 35）

    for (int r = 0; r < ms; r++)      // 外层：遍历每一行
        for (int c = 0; c < ms; c++)  // 内层：遍历每一列
            draw_cell(r, c, game->map[r][c]);
            // 把这一格的字符传给 draw_cell，让它决定颜色和形状

    // 画棋盘的外边框
    setlinecolor(RGB(54, 63, 74));
    rectangle(BOARD_LEFT, BOARD_TOP,
              BOARD_LEFT + ms * CELL_SIZE,
              BOARD_TOP + ms * CELL_SIZE);
}
```

这个函数做的事非常纯粹：**双重循环遍历二维数组，对每个元素调用一个绘制函数**。这是你在第一章 1.4 节练习过的模式——只是现在调用的是画图函数而不是 `cout`。

注意 `const GameState *game`——这个 `const` 告诉编译器："我只是读数据来画图，我不会修改游戏状态"。这是一种自我约束，也是好的编程习惯。

**第二层：draw_cell() —— 把一个字符画成一个屏幕上的格子**

```cpp
// render.cpp
static void draw_cell(int row, int col, char cell)
{
    // 1. 把"逻辑坐标"（第几行第几列）换算成"屏幕坐标"（像素）
    int left = BOARD_LEFT + col * CELL_SIZE;
    int top = BOARD_TOP + row * CELL_SIZE;
    int right = left + CELL_SIZE - 1;
    int bottom = top + CELL_SIZE - 1;

    // 2. 选颜色：根据字符决定用什么色填
    setfillcolor(cell_color(cell));
    setlinecolor(RGB(215, 221, 228));  // 格子的边框线，统一的浅灰

    // 3. 画格子（食物特殊处理——画成苹果的样子）
    if (cell == 'F')
    {
        solidrectangle(left, top, right, bottom);   // 红色方块底
        setfillcolor(RGB(232, 88, 80));             // 稍亮的红色
        solidellipse(left + 5, top + 5, right - 5, bottom - 5); // 内部椭圆
    }
    else
    {
        solidrectangle(left, top, right, bottom);   // 普通格子
    }
}
```

坐标换算公式值得你停下来想一想：

```
left   = BOARD_LEFT + col * CELL_SIZE
top    = BOARD_TOP  + row * CELL_SIZE
right  = left + CELL_SIZE - 1
bottom = top  + CELL_SIZE - 1
```

- `BOARD_LEFT = 20`，`BOARD_TOP = 20`：棋盘从窗口左上角往右 20 像素、往下 20 像素开始
- `CELL_SIZE = 25`：每个格子 25x25 像素
- `col * CELL_SIZE`：第 col 列，离棋盘左边界 col x 25 像素
- 举例：第 0 行第 0 列的格子，左上角在 (20, 20)，右下角在 (44, 44)
- 第 3 行第 5 列的格子，左上角在 (20+5x25, 20+3x25) = (145, 95)

**第三层：cell_color() —— 字符到颜色的映射**

```cpp
// render.cpp
static COLORREF cell_color(char cell)
{
    switch (cell)
    {
    case '#': return RGB(45, 52, 63);    // 墙：深灰色
    case 'O': return RGB(103, 112, 124); // 障碍物：中灰色
    case 'H': return RGB(24, 124, 88);   // 玩家蛇头：深绿色
    case 'B': return RGB(73, 180, 130);  // 玩家蛇身：浅绿色
    case 'A': return RGB(42, 108, 218);  // AI 蛇头：深蓝色
    case 'C': return RGB(97, 157, 237);  // AI 蛇身：浅蓝色
    case 'F': return RGB(218, 72, 66);   // 食物：红色
    default:  return RGB(238, 242, 246); // 空地（. 和其他未知字符）：浅灰白
    }
}
```

这个函数特别简单——就是一个 `switch` 语句。但它是整个渲染管线的"翻译"：把机器可读的字符变成人眼可识别的颜色。

> **为什么蛇头比蛇身颜色深？** 因为人类视觉会本能地被**高对比度**的区域吸引。深绿色的蛇头在浅绿色蛇身前面，你一眼就知道"头在哪"。对比度 = 信息。如果全身都是同一个颜色，你根本看不出蛇的朝向。

### 颜色设计分析：为什么选这些颜色？

打开 Snakes 的游戏画面，你会觉得颜色"舒服"。这不是偶然的，背后有设计考量：

| 元素 | 颜色 | 设计原因 |
|------|------|---------|
| 背景/空地 | 浅灰白 `(238,242,246)` | 中性不刺眼，和其他所有颜色都有足够对比度 |
| 墙 | 深灰 `(45,52,63)` | 清晰但有"后退感"，不抢主角的颜色 |
| 玩家蛇头 | 深绿 `(24,124,88)` | 绿色 = 玩家 = "我"。深色醒目 |
| 玩家蛇身 | 浅绿 `(73,180,130)` | 和蛇头同色系但有明度差，一眼看出头在哪 |
| AI 蛇头 | 深蓝 `(42,108,218)` | 蓝色 = 对手 = "他"。和玩家绿色形成强烈对比 |
| AI 蛇身 | 浅蓝 `(97,157,237)` | 和 AI 头同理，同色系、明度差 |
| 食物 | 红色 `(218,72,66)` | 红色 = 危险/重要/吃我。在浅灰白背景上最显眼 |

两个核心原则：
1. **对比度**：不同角色（墙/食物/玩家/AI）的颜色在色相上拉开距离（灰 vs 红 vs 绿 vs 蓝）
2. **直觉**：红 = 危险/吃（食物），绿 = 自己（玩家），蓝 = 对手（注意！），灰 = 背景（忽略）

这就是为什么你看一眼屏幕就知道"敌人在哪、食物在哪、我在哪"——颜色在替你思考。

### 常见错误

**错误 1：遍历二维数组时行列下标搞反**

```cpp
// WRONG ❌
for (int r = 0; r < ms; r++)
    for (int c = 0; c < ms; c++)
        draw_cell(c, r, game->map[r][c]);
//               ^  ^          ^^^^^^^^^
// draw_cell 的第一个参数是 row，第二个是 col
// 这里把 col 当 row 传了，格子的位置就乱了

// CORRECT ✅
for (int r = 0; r < ms; r++)
    for (int c = 0; c < ms; c++)
        draw_cell(r, c, game->map[r][c]);
//               ^  ^
//        row ──┘  └── col
```

**错误 2：坐标换算忘记加 BOARD_LEFT/BOARD_TOP**

```cpp
// WRONG ❌
int left = col * CELL_SIZE;    // 忘了加 BOARD_LEFT
int top = row * CELL_SIZE;     // 忘了加 BOARD_TOP
// 后果：棋盘从窗口左上角 (0,0) 开始画，没有边距

// CORRECT ✅
int left = BOARD_LEFT + col * CELL_SIZE;
int top = BOARD_TOP + row * CELL_SIZE;
```

**错误 3：switch 语句忘了写 default**

```cpp
// WRONG ❌
switch (cell)
{
case '#': return RGB(45, 52, 63);
case 'H': return RGB(24, 124, 88);
// ... 只处理了已知的几种字符
}
// 如果出现一个未知字符（比如 '.' 空地），函数没有 return 语句！
// 编译器可能警告，返回值不可预测

// CORRECT ✅
switch (cell)
{
case '#': return RGB(45, 52, 63);
case 'H': return RGB(24, 124, 88);
// ...
default:  return RGB(238, 242, 246);  // 未知字符 → 画成空地颜色
}
```

### 练一练

定义一个 5x5 的 char 数组作为你的"迷你地图"，用程序渲染出来。

**地图规格**：
```
# # # # #
# . . . #
# . H . #
# . . F #
# # # # #
```

- `#` = 墙（深灰色 `RGB(45, 52, 63)`）
- `.` = 空地（浅灰白 `RGB(238, 242, 246)`）
- `H` = 蛇头（深绿色 `RGB(24, 124, 88)`，画成实心圆而不是方块）
- `F` = 食物（红色 `RGB(218, 72, 66)`，画成实心方块）

**要求**：
1. 每个格子的 CELL_SIZE 设为 40（比 Snake 的 25 大一点，看得更清楚）
2. 窗口大小根据格子和数量自动计算（5 个格子 + 边距）
3. 使用双缓冲
4. 实现一个 `cell_color()` 函数（仿照 Snake 源码的写法）
5. 蛇头位置（`H`）用实心圆代替方块

**预期效果**：窗口中出现一个 5x5 的彩色网格地图。四边是深灰色的墙，中间有一小块空地，左上方有一个绿色蛇头，右下方有一个红色食物块。

### 参考答案

<details>
<summary>点击展开</summary>

```cpp
#include <graphics.h>

// 字符 → 颜色映射（仿照 Snake 源码的 cell_color 函数）
COLORREF cell_color(char cell)
{
    switch (cell)
    {
    case '#': return RGB(45, 52, 63);    // 墙：深灰
    case '.': return RGB(238, 242, 246); // 空地：浅灰白
    case 'H': return RGB(24, 124, 88);   // 蛇头：深绿
    case 'F': return RGB(218, 72, 66);   // 食物：红色
    default:  return RGB(200, 200, 200); // 未知：灰色（安全兜底）
    }
}

// 画一个格子
void draw_cell(int row, int col, char cell, int board_left, int board_top, int cell_size)
{
    int left = board_left + col * cell_size;
    int top = board_top + row * cell_size;
    int right = left + cell_size - 1;
    int bottom = top + cell_size - 1;

    setfillcolor(cell_color(cell));
    setlinecolor(RGB(180, 180, 180));  // 格子边框

    if (cell == 'H')
    {
        // 蛇头：画成实心圆（比方块稍小一点）
        int cx = left + cell_size / 2;
        int cy = top + cell_size / 2;
        int r = cell_size / 2 - 2;
        solidcircle(cx, cy, r);
    }
    else
    {
        solidrectangle(left, top, right, bottom);
    }
}

// 遍历地图数组，画所有格子
void draw_map(char map[][5], int map_size, int board_left, int board_top, int cell_size)
{
    for (int r = 0; r < map_size; r++)
        for (int c = 0; c < map_size; c++)
            draw_cell(r, c, map[r][c], board_left, board_top, cell_size);

    // 画外边框
    setlinecolor(RGB(54, 63, 74));
    rectangle(board_left, board_top,
              board_left + map_size * cell_size,
              board_top + map_size * cell_size);
}

int main()
{
    // 定义 5x5 地图
    char map[5][5] = {
        {'#', '#', '#', '#', '#'},
        {'#', '.', '.', '.', '#'},
        {'#', '.', 'H', '.', '#'},
        {'#', '.', '.', 'F', '#'},
        {'#', '#', '#', '#', '#'}
    };

    int map_size = 5;
    int cell_size = 40;
    int board_left = 30;
    int board_top = 30;

    // 窗口大小 = 棋盘 + 两边边距
    int win_w = board_left * 2 + map_size * cell_size;
    int win_h = board_top * 2 + map_size * cell_size;

    initgraph(win_w, win_h);
    BeginBatchDraw();

    cleardevice();
    draw_map(map, map_size, board_left, board_top, cell_size);
    FlushBatchDraw();

    Sleep(5000);  // 显示 5 秒

    EndBatchDraw();
    closegraph();
    return 0;
}
```

> **恭喜！** 你刚刚写的这段代码，本质上就是 Snake 渲染系统的**核心骨架**。Snake 的 `draw_board()` 做的事情一模一样，只是：
> - 地图更大（20x20 或 35x35）
> - 字符种类更多（8 种）
> - `draw_cell` 对食物做了特殊处理（画成苹果形状）
> - 后面多了一个 `draw_panel()` 画信息面板
>
> 你现在理解了 Snake 渲染系统的 70%。剩下的 30% 是信息面板的文字排版——那个没有新概念，就是更多的坐标计算。

</details>

---

## 本章小结

恭喜你完成了第二章！从"只会往控制台打印数字"到"能把一个二维数组渲染成彩色网格地图"——这个跨越非常大。

我们来回顾一下学了什么：

| 节 | 内容 | 在 Snake 里的用途 |
|-----|------|------------------|
| 2.1 游戏循环 | input-update-render 结构、帧率、Sleep | `main.cpp` 的 `while(true)` 主循环 |
| 2.2 EasyX 绘图基础 | 坐标系统、形状绘制、RGB 颜色、文字输出 | `draw_cell()` 里所有形状绘制和颜色设置 |
| 2.3 双缓冲 | BeginBatchDraw/FlushBatchDraw 原理 | `main.cpp` 和 `draw_game()` 的防闪烁机制 |
| 2.4 地图网格渲染 | 二维数组遍历 → 字符到颜色映射 → 绘制格子 | `draw_board()` → `draw_cell()` → `cell_color()` 三层调用链 |

### 你现在能看懂什么？

打开 Snake 的 `render.cpp`，你应该能完全理解这些了：

```cpp
// ✅ 游戏主循环的结构
while (true) {
    current->update();    // input + update
    current->render();    // render
    FlushBatchDraw();
    Sleep(16);
}

// ✅ 坐标换算
int left = BOARD_LEFT + col * CELL_SIZE;
int top = BOARD_TOP + row * CELL_SIZE;

// ✅ 字符到颜色的映射
switch (cell) {
case '#': return RGB(45, 52, 63);    // 墙
case 'H': return RGB(24, 124, 88);   // 蛇头
// ...
}

// ✅ 画格子
setfillcolor(cell_color(cell));
solidrectangle(left, top, right, bottom);

// ✅ 画文字
setbkmode(TRANSPARENT);
settextstyle(28, 0, _T("Consolas"));
outtextxy(px, 48, _T("Snake"));

// ✅ 双缓冲
BeginBatchDraw();
// ... 画东西 ...
FlushBatchDraw();
EndBatchDraw();
```

**你现在的水平**：给你一个字符二维数组，你能把它画成一个彩色网格。下一章（第三章：输入与状态机），我们会学习 Snake 是怎么处理键盘输入的——那个让蛇"活起来"的部分。

---

## 自我测试

以下 5 道选择题检验你对本章内容的掌握程度。答案在最后。

**1.** 游戏主循环的正确顺序是？

A. render → update → input → Sleep
B. input → update → render → Sleep
C. Sleep → render → update → input
D. update → Sleep → input → render

**2.** 在 EasyX 坐标系统中，坐标 (100, 200) 在窗口中的位置是？

A. 距离左边界 200 像素，距离顶部 100 像素
B. 距离左边界 100 像素，距离顶部 200 像素
C. 距离左边界 100 像素，距离底部 200 像素
D. 距离右边界 100 像素，距离顶部 200 像素

**3.** 以下哪组 RGB 值表示纯绿色？

A. `RGB(255, 0, 0)`
B. `RGB(0, 255, 0)`
C. `RGB(0, 0, 255)`
D. `RGB(255, 255, 0)`

**4.** 以下关于双缓冲的说法，哪项是**正确的**？

A. `BeginBatchDraw()` 应该写在游戏循环内部，每帧调用一次
B. 双缓冲的作用是让画面显示得更快
C. 双缓冲的"幕后画布"对玩家不可见，只有 `FlushBatchDraw()` 时才显示完整画面
D. 如果我的游戏只有静态画面（不刷新），也必须使用双缓冲

**5.** 在 Snake 的 `draw_cell()` 里，坐标换算公式是 `left = BOARD_LEFT + col * CELL_SIZE`。如果 `BOARD_LEFT = 20`，`CELL_SIZE = 25`，那么第 3 列（col=3）的格子的左边界像素是？

A. 20
B. 75
C. 95
D. 120

---

**答案**：1-B, 2-B, 3-B, 4-C, 5-C

**解析**：
- 第 1 题：游戏循环是 input（检测按键）→ update（更新逻辑）→ render（绘制画面）→ Sleep（控制帧率）。
- 第 2 题：EasyX 坐标系中，第一个参数是 x（距左边界的像素），第二个参数是 y（距顶部的像素）。
- 第 3 题：RGB 三个参数依次是红、绿、蓝。纯绿就是 `(0, 255, 0)`——绿色通道拉满，红蓝关掉。
- 第 4 题：双缓冲的核心就是在幕后画完整张画面后一次性切换，避免玩家看到中间过程。A 错（BeginBatchDraw 只在循环外调用一次），B 错（双缓冲不提升速度，只是消除闪烁），D 错（静态画面不需要双缓冲）。
- 第 5 题：`20 + 3 × 25 = 20 + 75 = 95`。注意 col 从 0 开始，所以 col=3 是第 4 列。
