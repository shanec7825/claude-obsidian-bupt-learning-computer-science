---
type: curriculum
title: "第一章：C++ 快速入门"
chapter: 1
course: "Snake Game — Zero to C++ Game Dev"
created: 2026-06-22
updated: 2026-06-22
tags:
  - curriculum
  - cpp
  - game-dev
  - education
  - snake
status: developing
related:
  - "[[Snake Game Curriculum]]"
  - "[[snake-game-easyx]]"
  - "[[snake-game-call-graph]]"
sessions: 6
duration: "9-12 小时"
goal: "能读懂 Snake 游戏 90% 的代码语法"
---

# 第一章：C++ 快速入门

> **目标**：学完本章后，你能够读懂 Snake 游戏源码中 90% 的语法结构。
> **原则**：只学 Snake 里用到的那部分 C++，暂时用不到的先跳过。不贪多，只求够用。

---

## 1.1 变量与类型 —— "给数据贴标签"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解变量是什么，能声明和使用基本类型

### 为什么需要变量？

想象你在厨房做饭。你需要放盐、糖、酱油——但如果它们全装在一样的白碗里，没有任何标签，你会不会把盐当成糖？变量就是**贴了标签的盒子**：标签告诉你盒子里装的是什么，盒子本身存着实际的内容。

在程序里，"盒子"是内存中的一小块空间，"标签"是变量名，"内容"是数据。

```cpp
int score = 0;        // 贴了标签"score"的盒子，里面装着整数 0
bool game_over = false;  // 标签"game_over"，装着一个"假"值
char ch = 'F';        // 标签"ch"，装着字符 F
```

### C++ 的基本类型（Snake 里用到的）

| 类型 | 关键字 | 存什么 | Snake 里的例子 |
|------|--------|--------|---------------|
| 整数 | `int` | 正负整数（-2,147,483,648 到 2,147,483,647） | `int score = 0;` 分数 |
| 布尔 | `bool` | 只有 `true` 或 `false` | `bool game_over = false;` 游戏是否结束 |
| 字符 | `char` | 单个字符，用单引号 | `char ch = 'F';` 地图格子里放的标记 |
| 无符号整数 | `DWORD` | 非负整数（0 到 4,294,967,295） | `DWORD now;` 时间戳 |

### 从 Snake 源码里截取的真实例子

打开 Snake 的 `game.cpp`，你会看到这些变量声明：

```cpp
// 来自 game.cpp — init_game() 函数
game->score = 0;          // int：当前得分
game->length = 3;         // int：蛇的当前长度
game->move_count = 0;     // int：已经走了多少步
game->started = false;    // bool：游戏是否已经开始
game->paused = false;     // bool：是否暂停
game->game_over = false;  // bool：游戏是否结束
game->direction = UP;     // Direction（枚举，后面会讲）：蛇的朝向

// 来自 input.cpp — key_pressed() 函数
bool now_down = (GetAsyncKeyState(vk) & 0x8000) != 0;
// bool：这一帧按键是按下的吗？
bool pressed = now_down && !was_down[vk];
// bool：这是"刚刚按下"的那个瞬间吗？
```

你不需要现在就理解每一行的含义。你只需要看得出来：`int` 存数字，`bool` 存真假，这就是它们在 Snake 里的全部用法。

### 声明与初始化

声明变量时，你可以选择是否立刻给它一个初始值：

```cpp
int a;           // 声明了，但里面是"垃圾值"——危险！
int b = 10;      // 声明 + 初始化，安全
int c(20);       // 另一种初始化写法，等价
int d = 0;       // 零初始化，最稳妥的做法
```

**类比**：声明 = 你买了一个空盒子。初始化 = 你在盒子里放好了东西。不初始化的盒子里面可能有上一个人留下的垃圾——这在 C++ 里叫"未定义行为"。

### 常见错误（你一定会遇到）

**错误 1：忘记分号**

```cpp
// WRONG ❌
int score = 0
bool game_over = false;

// CORRECT ✅
int score = 0;
bool game_over = false;
```

C++ 里每条语句必须以分号结尾。忘了就报错。刚学时最容易犯，习惯就好。

**错误 2：类型名写错**

```cpp
// WRONG ❌
interger x = 10;    // 没有 interger 这个类型
boolean flag = true; // C++ 用 bool 不是 boolean

// CORRECT ✅
int x = 10;
bool flag = true;
```

**错误 3：未初始化就使用**

```cpp
// WRONG ❌
int score;
cout << score;   // 输出什么？天知道！

// CORRECT ✅
int score = 0;
cout << score;   // 输出：0
```

**错误 4：把字符串赋给 char**

```cpp
// WRONG ❌
char ch = "F";    // 双引号是字符串，不是字符

// CORRECT ✅
char ch = 'F';    // 单引号才是字符
```

### 练一练

在 Visual Studio 中新建一个控制台项目，写以下程序。**预期输出**已经写好，你的输出应该和它一致。

```cpp
#include <iostream>
using namespace std;

int main() {
    // 1. 声明几个不同类型的变量
    int age = 25;
    double height = 1.75;   // double 是带小数的数字
    bool is_student = true;
    char grade = 'A';

    // 2. 打印出来
    cout << "Age: " << age << endl;
    cout << "Height: " << height << endl;
    cout << "Is student: " << is_student << endl;
    cout << "Grade: " << grade << endl;

    // 3. 修改变量并再打印
    age = age + 1;
    cout << "Next year age: " << age << endl;

    return 0;
}
```

**预期输出**：
```
Age: 25
Height: 1.75
Is student: 1
Grade: A
Next year age: 26
```

> 注意：`bool` 变量打印出来是 `1`（true）或 `0`（false），这是正常的。

**思考题**：如果把 `int age;` 后面不写 `= 25`，程序还能运行吗？输出会是什么？

### 参考答案

<details>
<summary>点击展开</summary>

```cpp
#include <iostream>
using namespace std;

int main() {
    int age = 25;
    double height = 1.75;
    bool is_student = true;
    char grade = 'A';

    cout << "Age: " << age << endl;
    cout << "Height: " << height << endl;
    cout << "Is student: " << is_student << endl;
    cout << "Grade: " << grade << endl;

    age = age + 1;
    cout << "Next year age: " << age << endl;

    return 0;
}
```

思考题答案：程序能编译通过，但 `age` 的值是"未定义的"——可能是 0，可能是 32894732，可能是任何东西。这就是为什么要养成**声明即初始化**的习惯。
</details>

---

## 1.2 控制流 —— "让程序做选择"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解 if/else、for、while，能读懂 Snake 里的条件判断和循环

### 为什么需要控制流？

程序如果只能一行一行往下走，那跟录音机没什么区别。控制流让程序能**做选择**（if/else）和**反复做**（for/while）。就像你在十字路口要判断"红灯还是绿灯"来决定走不走；游戏里每一帧都要判断"蛇撞墙了吗？"来决定继不继续。

### if/else —— 条件判断

```cpp
if (条件) {
    // 条件为 true 时执行
} else {
    // 条件为 false 时执行
}
```

**类比**：你去便利店买可乐——
- 如果（if）冰箱里有冰的，拿冰的
- 否则（else）拿常温的

**来自 Snake 的真实代码**（`game.cpp` 的 `move_snake()`）：

```cpp
// 如果吃到了食物 → 加分，放新食物
if (will_eat)
{
    game->score += 10;
    game->food_eaten++;
    place_food(game);
}

// 如果游戏还没开始 → 先检查方向是否安全
if (!game->started)
{
    if (can_start_in_direction(game, dir))
    {
        game->direction = dir;
        game->started = true;
    }
    return;
}
```

**来自 Snake 的真实代码**（`input.cpp` 的 `key_pressed()`）：

```cpp
// 边缘检测：这一帧按下了 且 上一帧没按下 → 这是一个"新按下"事件
bool now_down = (GetAsyncKeyState(vk) & 0x8000) != 0;
bool pressed = now_down && !was_down[vk];
was_down[vk] = now_down;
return pressed;
```

这段代码的含义：你在键盘上按住 W 不松，物理上 W 键一直是"按下"状态——但游戏只想在**你第一次按下的那个瞬间**响应一次。所以它比较"现在按着吗"和"上一帧按着吗"，只有"从没按到按"的边沿才返回 true。这就是所谓的**边缘检测**（edge detection）。

### for 循环 —— 遍历

```cpp
for (初始化; 条件; 每轮末尾的操作) {
    // 循环体
}
```

**类比**：你有一排 10 个储物柜，你要一个一个打开检查里面有什么。"从第 1 个开始，只要还没到第 10 个就继续，每检查完一个就走到下一个。"

**来自 Snake 的真实代码**（`game.cpp` 的 `move_snake()`——蛇身体移动的核心）：

```cpp
// 把蛇身体向前移动一格
// 注意：为什么是 i-- 而不是 i++？
for (int i = game->length - 1; i > 0; i--)
    game->snake[i] = game->snake[i - 1];
game->snake[0] = next;  // 新头部
```

这个循环的方向很关键。想象蛇的身体是：`[头, 身1, 身2, 尾]`。如果从前往后挪（i++），身1先占头的旧位置，然后身2又占身1的旧位置……结果整个身体塌缩成一个点。正确的做法是从后往前挪（i--）：尾巴先占前一个位置，依次往前，最后头单独处理。这是蛇移动算法的核心。

**来自 Snake 的真实代码**（`render.cpp` 的 `draw_board()`——绘制地图的每个格子）：

```cpp
// 遍历二维地图的每个格子
for (int r = 0; r < ms; r++)
    for (int c = 0; c < ms; c++)
        draw_cell(r, c, game->map[r][c]);
```

这个循环非常直观：外层遍历行（r），内层遍历列（c），把每个格子画出来。20x20 的地图就要画 400 个格子。

### while 循环 —— 只要……就一直

```cpp
while (条件) {
    // 条件为 true 就一直循环
}
```

**类比**：你玩手机"只要没到睡觉时间就一直刷"——这就是 while 循环。

**来自 Snake 的真实代码**（`main.cpp` 的主循环）：

```cpp
while (true)
{
    current->update();     // 1. 处理输入和逻辑
    current->render();     // 2. 画画面
    FlushBatchDraw();      // 3. 显示到屏幕
    Sleep(16);             // 4. 等 16 毫秒（约 60 FPS）
}
```

`while (true)` 是一个**死循环**——条件永远是 true，所以游戏会一直运行，直到你在循环内部用 `break` 跳出去或者关闭窗口。这是所有游戏的通用模式：游戏主循环（Game Loop）。

### 常见错误（踩坑合集）

**错误 1：`==` 和 `=` 搞混（第一名，没有人能逃过）**

```cpp
// WRONG ❌ —— 这是赋值，不是比较！
if (score = 100) {
    cout << "满分！" << endl;
}
// 这段代码的意思是：把 100 赋给 score（score 变成 100），
// 然后判断 100 是不是 true——在 C++ 里非零就是 true，
// 所以这个 if 永远成立！你的 score 也被悄悄改成了 100。

// CORRECT ✅
if (score == 100) {
    cout << "满分！" << endl;
}
```

**记忆口诀**：一个等号是"把右边的放进左边"，两个等号是"左右相等吗？"

**错误 2：for 循环边界条件写错**

```cpp
// WRONG ❌ —— 数组越界
int arr[5] = {1, 2, 3, 4, 5};
for (int i = 0; i <= 5; i++)   // arr[5] 不存在！
    cout << arr[i] << endl;

// CORRECT ✅
for (int i = 0; i < 5; i++)    // i 取 0 1 2 3 4，完美
    cout << arr[i] << endl;
```

数组有 5 个元素，下标是 0 到 4。用 `<=` 会访问不存在的 `arr[5]`，这是经典的**数组越界**错误。

**错误 3：while 循环忘了改变条件——死循环**

```cpp
// WRONG ❌ —— 永远停不下来
int count = 0;
while (count < 10) {
    cout << count << endl;
    // 忘记 count++ 了！
}

// CORRECT ✅
int count = 0;
while (count < 10) {
    cout << count << endl;
    count++;  // 每次加 1，10 次后停下
}
```

### 练一练

**练习 1：1 到 100 累加**

用 for 循环计算 `1 + 2 + 3 + ... + 100` 的结果并打印。

**预期输出**：
```
Sum from 1 to 100 is: 5050
```

**练习 2：九九乘法表**

用双重 for 循环打印九九乘法表。

**预期输出**（前 3 行）：
```
1*1=1  1*2=2  1*3=3  1*4=4  1*5=5  1*6=6  1*7=7  1*8=8  1*9=9
2*1=2  2*2=4  2*3=6  2*4=8  2*5=10 2*6=12 2*7=14 2*8=16 2*9=18
3*1=3  3*2=6  3*3=9  3*4=12 3*5=15 3*6=18 3*7=21 3*8=24 3*9=27
...
```

### 参考答案

<details>
<summary>点击展开练习 1</summary>

```cpp
#include <iostream>
using namespace std;

int main() {
    int sum = 0;
    for (int i = 1; i <= 100; i++) {
        sum = sum + i;
    }
    cout << "Sum from 1 to 100 is: " << sum << endl;
    return 0;
}
```
</details>

<details>
<summary>点击展开练习 2</summary>

```cpp
#include <iostream>
using namespace std;

int main() {
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            cout << i << "*" << j << "=" << i * j;
            if (i * j < 10) cout << "  ";  // 对齐用
            else cout << " ";
        }
        cout << endl;
    }
    return 0;
}
```
</details>

---

## 1.3 函数 —— "给一段代码起名字"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解函数的定义、参数、返回值，能写简单的函数

### 为什么需要函数？

你有没有过这种经历：同一个操作反复做了很多遍，每次都要重新写一遍？比如每次算两个数的和都要写 `a + b`——但如果让你算 100 次呢？函数就是**把一段常用的代码打包，起个好名字，以后直接喊名字就行**。

```cpp
// 不用函数：每次都要写一遍
int r1 = 3 + 5;
int r2 = 10 + 20;
int r3 = 100 + 200;  // 烦不烦？

// 用函数：写一次，用多次
int add(int a, int b) {
    return a + b;
}
int r1 = add(3, 5);
int r2 = add(10, 20);
int r3 = add(100, 200);  // 清爽！
```

**类比**：函数就像餐厅的菜谱。`add` 这个菜谱上写着"把两样东西加在一起"，你每次说 `add(3, 5)` 就是在点这道菜——厨房（CPU）按照菜谱做，把结果端给你。

### 函数的基本结构

```cpp
返回值类型 函数名(参数列表) {
    // 函数体
    return 返回值;  // 如果返回值类型不是 void
}
```

几个关键概念：

- **函数名**：你给这段代码起的名字，比如 `move_snake`、`place_food`
- **参数**：传给函数的数据，"原材料"。函数可以没有参数（写空括号）
- **返回值**：函数算出来的结果，"成品"。如果函数不需要返回东西，返回值类型写 `void`
- **函数体**：用花括号 `{}` 包起来的具体代码

### 有返回值的函数

**来自 Snake 的真实代码**（`game.cpp`）：

```cpp
// 判断两个点是不是同一个位置
static bool is_same_point(Point a, Point b)
{
    return a.row == b.row && a.col == b.col;
}

// 判断两个方向是否相反（比如上和下、左和右）
static bool is_opposite(Direction a, Direction b)
{
    return (a == UP    && b == DOWN)  ||
           (a == DOWN  && b == UP)    ||
           (a == LEFT  && b == RIGHT) ||
           (a == RIGHT && b == LEFT);
}
```

这两个函数都返回 `bool`（真或假）。它们的逻辑非常直观：
- `is_same_point`：如果行和列都一样，就是同一个点
- `is_opposite`：上下是一对反向，左右是一对反向

### 无返回值的函数（void）

**来自 Snake 的真实代码**（`game.cpp`）：

```cpp
// 在空地上随机放一个食物
static void place_food(GameState *game)
{
    Point cells[MAX_SNAKE_LEN];
    int count = collect_empty_cells(game, cells);
    if (count <= 0)
    {
        game->game_over = true;
        game->death_reason = DEATH_FULL_MAP;
        return;    // ← void 函数也能用 return，只是不返回任何值
    }
    int idx = rand() % count;
    game->map[cells[idx].row][cells[idx].col] = 'F';
}
```

这个函数不需要"返回一个结果"——它的任务是**修改游戏状态**：在空地上放一个食物。注意 `void` 函数里也可以用 `return;`（不带值），意思是"提前结束，后面的代码不执行了"。

**来自 Snake 的真实代码**（`game.cpp`）：

```cpp
// 把蛇的身体画到地图上
static void put_snake_on_map_impl(GameState *game, const Point *body, int len,
                                   char head_char, char body_char)
{
    for (int i = 0; i < len; i++)
    {
        Point p = body[i];
        game->map[p.row][p.col] = (i == 0) ? head_char : body_char;
    }
}
```

这个函数有 5 个参数，但也不返回值——它做的事情是把蛇的每个身体节点写到地图数组里。头用 `head_char`（比如 `'H'`），身体用 `body_char`（比如 `'B'`）。

### 参数 —— 把数据传给函数

参数就是函数的"输入"。在 Snake 里，你最常见到的参数模式是：**把一个 `GameState *` 指针传给函数**（指针我们在 1.6 节细讲）。

```cpp
void move_snake(GameState *game)   // game 是参数——装着全部游戏状态
void set_direction(GameState *game, Direction dir)  // 两个参数
void init_game(GameState *game, GameMode mode)      // 两个参数
```

### 常见错误

**错误 1：忘记 return**

```cpp
// WRONG ❌ —— 说好了返回 bool，却没有 return
bool is_even(int n) {
    n % 2 == 0;   // 这只是一条表达式语句，没有用
}
// 编译器会警告，而且函数会返回一个不可预测的值！

// CORRECT ✅
bool is_even(int n) {
    return n % 2 == 0;
}
```

**错误 2：参数类型不匹配**

```cpp
// WRONG ❌
int add(int a, int b) { return a + b; }
double result = add(3.5, 2.0);  // 3.5 被截断成 3，结果是 5，不是 5.5

// CORRECT ✅
double add_double(double a, double b) { return a + b; }
double result = add_double(3.5, 2.0);  // 5.5
```

**错误 3：void 函数里 return 了值**

```cpp
// WRONG ❌
void print_score(int s) {
    cout << s << endl;
    return 0;   // void 函数不能 return 值！
}

// CORRECT ✅
void print_score(int s) {
    cout << s << endl;
    return;     // 可以提前退出，但不带值
}
```

### 练一练

**练习 1：写一个 `is_even` 函数**

写一个函数 `bool is_even(int n)`，判断一个整数是否是偶数。然后在 main 里测试它。

**预期输出**：
```
is_even(4) = 1
is_even(7) = 0
is_even(0) = 1
is_even(-2) = 1
```

**练习 2：写一个 `add` 和 `max` 函数**

- `int add(int a, int b)` — 返回两数之和
- `int max(int a, int b)` — 返回两数中较大的那个

然后在 main 中打印 `add(35, 67)` 和 `max(35, 67)` 的结果。

**预期输出**：
```
add(35, 67) = 102
max(35, 67) = 67
```

### 参考答案

<details>
<summary>点击展开</summary>

```cpp
#include <iostream>
using namespace std;

bool is_even(int n) {
    return n % 2 == 0;
}

int add(int a, int b) {
    return a + b;
}

int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
}

int main() {
    cout << "is_even(4) = " << is_even(4) << endl;
    cout << "is_even(7) = " << is_even(7) << endl;
    cout << "is_even(0) = " << is_even(0) << endl;
    cout << "is_even(-2) = " << is_even(-2) << endl;

    cout << "add(35, 67) = " << add(35, 67) << endl;
    cout << "max(35, 67) = " << max(35, 67) << endl;

    return 0;
}
```
</details>

---

## 1.4 数组与字符串 —— "一排连续的盒子"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解一维数组、二维数组，能遍历和修改数组元素

### 为什么需要数组？

如果你只存一个人的分数，一个 `int score` 就够了。但蛇的身体有几十甚至几百个节点——每个节点都有行和列两个坐标。难道你要写 `int x1, y1, x2, y2, x3, y3 ... x100, y100`？那得写到天荒地老，而且你根本不知道蛇会长多长。

数组就是**一排连续编号的盒子**。你只需要一个名字，通过编号（下标）就能拿到任何一个盒子里的东西。

```
蛇身体用数组存：snake[0] = 头, snake[1] = 第一节身体, snake[2] = 第二节身体 ...
```

**类比**：数组就像一排储物柜。柜子的编号从 0 开始，你拿着编号就能找到对应的柜子。

### 一维数组 —— Snake 蛇身体的存储

**来自 Snake 的真实代码**（`game.cpp`）：

```cpp
// 蛇身体用 Point（坐标）数组存
Point snake[MAX_SNAKE_LEN];

// 初始化时，蛇有 3 节
game->snake[0] = {8, 5};   // 头，在第 8 行第 5 列
game->snake[1] = {8, 4};   // 身体，在第 8 行第 4 列
game->snake[2] = {9, 4};   // 尾巴，在第 9 行第 4 列
```

`MAX_SNAKE_LEN` 是一个常数（比如 400），意思是蛇最长能长到 400 节。数组声明时必须指定大小（或者初始化时自动推断）。

**来自 Snake 的真实代码**（`ai.cpp`）：

```cpp
// BFS 搜索用的方向数组——4 个方向的行偏移和列偏移
static const int AI_DR[4] = {-1, 0, 1, 0};   // 上  左  下  右
static const int AI_DC[4] = {0, -1, 0, 1};

// 使用：当前行 + AI_DR[d] = 新行号
int nr = cur.r + AI_DR[d];
int nc = cur.c + AI_DC[d];
```

这是数组的一个巧妙用法：把方向和坐标偏移量存成"查表数组"。不需要写 4 个 if-else，只需要一个循环 `for (int d = 0; d < 4; d++)` 就能遍历四个方向。大大简化了代码。

### 二维数组 —— Snake 地图的存储

**来自 Snake 的真实代码**（`game.cpp`）：

```cpp
// 地图是一个二维字符数组
// game->map[row][col] 存的字符表示这个格子里有什么：
// '#' = 墙    '.' = 空地    'F' = 食物    'H' = 玩家蛇头
// 'B' = 玩家蛇身    'A' = AI 蛇头    'C' = AI 蛇身    'O' = 障碍物

char map[MAX_MAP_SIZE][MAX_MAP_SIZE];
```

你可以把二维数组理解成**棋盘**——`map[3][5]` 就是第 3 行第 5 列那个格子。

**来自 Snake 的真实代码**（`game.cpp` 的 `clear_map()`）：

```cpp
// 初始化地图：四条边是墙，内部全是空地
static void clear_map(GameState *game)
{
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            game->map[r][c] = (r == 0 || r == game->map_size - 1 ||
                               c == 0 || c == game->map_size - 1)
                                  ? '#'
                                  : '.';
}
```

这段代码里出现了一个新东西 `? :`——这是**三元运算符**，是 if-else 的简写版：

```cpp
条件 ? 值1 : 值2
// 相当于：
// if (条件) 值1 else 值2
```

所以上面那段代码的意思是：如果这个格子靠着四条边 → 放墙 `#`；否则 → 放空地 `.`。

**来自 Snake 的真实代码**（`render.cpp` 的 `draw_board()`）：

```cpp
// 遍历地图的每个格子，根据字符决定用什么颜色画
void draw_board(const GameState *game)
{
    int ms = game->map_size;
    for (int r = 0; r < ms; r++)
        for (int c = 0; c < ms; c++)
            draw_cell(r, c, game->map[r][c]);  // 每个格子单独画
}
```

这就是双重循环遍历二维数组的标准模式：外层循环管行，内层循环管列。

### 一维数组做"按键状态表"

**来自 Snake 的真实代码**（`input.cpp`）：

```cpp
// was_down[按键码] = 上一帧这个键有没有按着
// 256 是因为键盘有 256 个虚拟键码
static bool was_down[256] = {false};  // 全部初始化为 false
```

这里 `= {false}` 的意思是**把数组所有元素初始化为 false**。C++ 里如果你只写第一个值，剩下的会自动填成"零值"（对于 bool 就是 false，对于 int 就是 0）。

### 常见错误

**错误 1：数组越界（访问不存在的下标）**

```cpp
// WRONG ❌
int arr[5] = {10, 20, 30, 40, 50};
cout << arr[5];   // arr 只有下标 0~4，arr[5] 不存在！

// CORRECT ✅
cout << arr[4];   // 最后一个元素，输出 50
```

数组越界在 C++ 里不会报错——程序会**静默地**读到一块不属于你的内存，里面的值不可预测。这是 C++ 最危险的问题之一。

**错误 2：初始化和赋值搞混**

```cpp
// WRONG ❌
int arr[5];
arr = {1, 2, 3, 4, 5};  // 数组名不能这样赋值！只能在声明时初始化

// CORRECT ✅
int arr[5] = {1, 2, 3, 4, 5};  // 声明时初始化

// 或者逐个赋值
int arr2[5];
arr2[0] = 1; arr2[1] = 2; arr2[2] = 3; arr2[3] = 4; arr2[4] = 5;
```

数组名（比如 `arr`）是一个**常量地址**，它不能被重新赋值。你只能改数组里**单个元素**的值。

**错误 3：二维数组下标写反**

```cpp
// WRONG ❌ —— 逻辑上可能导致越界
char map[20][20];
map[col][row] = '#';   // 应该是 map[row][col]
```

在 Snake 里，地图的惯例是 `map[行][列]`，即第一个下标是行（row），第二个是列（col）。如果你不小心写反了，代码可能不报错，但行为会很奇怪。**命名时用 `r` 和 `c` 而不是 `i` 和 `j` 可以帮你少出错**——看 Snake 源码里全都是 `for (int r...` `for (int c...`。

### 练一练

**练习 1：创建、遍历、修改数组**

创建一个长度为 5 的 int 数组 `{3, 7, 2, 9, 4}`，然后完成：
1. 打印所有元素
2. 计算并打印所有元素的和
3. 把第 3 个元素（下标 2）改成 10，再打印整个数组

**预期输出**：
```
Array: 3 7 2 9 4
Sum: 25
After change: 3 7 10 9 4
```

**练习 2：二维数组——打印一个 3x3 的乘法表到数组**

创建一个 3x3 的二维数组，`grid[i][j] = (i+1) * (j+1)`，然后打印出来。

**预期输出**：
```
1 2 3
2 4 6
3 6 9
```

### 参考答案

<details>
<summary>点击展开练习 1</summary>

```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[5] = {3, 7, 2, 9, 4};

    cout << "Array: ";
    for (int i = 0; i < 5; i++)
        cout << arr[i] << " ";
    cout << endl;

    int sum = 0;
    for (int i = 0; i < 5; i++)
        sum += arr[i];
    cout << "Sum: " << sum << endl;

    arr[2] = 10;  // 修改下标为 2 的元素
    cout << "After change: ";
    for (int i = 0; i < 5; i++)
        cout << arr[i] << " ";
    cout << endl;

    return 0;
}
```
</details>

<details>
<summary>点击展开练习 2</summary>

```cpp
#include <iostream>
using namespace std;

int main() {
    int grid[3][3];

    // 填充
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            grid[i][j] = (i + 1) * (j + 1);

    // 打印
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            cout << grid[i][j] << " ";
        cout << endl;
    }

    return 0;
}
```
</details>

---

## 1.5 结构体与枚举 —— "自定义类型"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解 struct 和 enum，知道为什么不能用"分开的数组"代替 struct

### 为什么需要结构体？

在 Snake 里，蛇身体的每个节点都是一个"坐标"——有行号（row）和列号（col）。如果没有结构体，你可能这样写：

```cpp
int snake_row[MAX_SNAKE_LEN];  // 存所有行号
int snake_col[MAX_SNAKE_LEN];  // 存所有列号
```

这有两个大问题：
1. **容易搞错**：`snake_row[0]` 和 `snake_col[0]` 属于同一个节点，但它们是两个数组里的两个独立变量——你必须时刻记得"第 i 个节点的行在 row[i]，列在 col[i]"。
2. **传参麻烦**：想把"蛇的一个节点"传给函数？你得传两个数组 + 下标，或者传两个 int，非常啰嗦。

结构体（struct）就是**把相关的变量打包成一个"自定义类型"**。就像你把铅笔、尺子、橡皮放进一个文具盒——然后你说"把我的文具盒拿来"，而不是"把我的铅笔拿来、尺子拿来、橡皮拿来"。

```cpp
struct Point {
    int row;   // 行号
    int col;   // 列号
};
```

有了 `Point` 之后，蛇身体就变成：

```cpp
Point snake[MAX_SNAKE_LEN];  // 一个数组，每个元素是一个完整的坐标
snake[0].row = 8;   // 用 . 访问成员
snake[0].col = 5;
```

### 来自 Snake 的真实代码（推测的头文件定义）

虽然 `.h` 文件不在源码中，但从使用方式可以推断出结构体的定义：

```cpp
// 一个坐标点
struct Point {
    int row;
    int col;
};
```

在 `game.cpp` 里，你会看到大量 `Point` 的使用：

```cpp
// 来自 game.cpp — 存两条蛇的身体
Point snake[MAX_SNAKE_LEN];
Point ai_snake[MAX_SNAKE_LEN];

// 来自 game.cpp — 临时存所有空格子
Point cells[MAX_SNAKE_LEN];
cells[count].row = r;   // 访问结构体成员
cells[count].col = c;

// 来自 game.cpp — 比较两个点
static bool is_same_point(Point a, Point b)
{
    return a.row == b.row && a.col == b.col;
}
```

### GameState —— Snake 里最核心的结构体

Snake 里有一个"超级结构体"叫 `GameState`，它包含了游戏运行时的**所有状态**：

```cpp
// 从 game.cpp 的使用方式可以推断出 GameState 的大致结构：
struct GameState {
    // 地图
    char map[MAX_MAP_SIZE][MAX_MAP_SIZE];
    int map_size;

    // 玩家蛇
    Point snake[MAX_SNAKE_LEN];
    int length;
    int score;
    Direction direction;
    bool turn_locked;

    // AI 蛇（Battle 模式）
    Point ai_snake[MAX_SNAKE_LEN];
    int ai_length;
    int ai_score;
    Direction ai_direction;
    bool ai_is_alive;

    // 游戏状态
    bool started;
    bool paused;
    bool game_over;
    GameMode mode;
    int move_count;
    DeathReason death_reason;
    // ... 还有很多字段
};
```

为什么要把这么多东西塞进一个结构体？因为**几乎所有函数都需要访问游戏状态**。如果不用结构体，你每个函数都要传十几个参数——代码会又臭又长。现在只需要：

```cpp
void move_snake(GameState *game);       // 传一个结构体就够了
void place_food(GameState *game);
void draw_board(const GameState *game);
```

函数签名干净，调用方也清楚。这就是结构体的核心价值：**把相关数据打包**。

### 枚举 —— 给数字起有意义的名字

看这段代码：

```cpp
// WRONG —— "神秘数字"
if (direction == 0) { ... }  // 0 是什么意思？上？下？左？右？
if (direction == 1) { ... }  // 鬼知道
```

三周后的你自己回来读这段代码，你会完全不知道 0、1、2、3 分别代表什么方向。枚举（enum）就是来解决这个问题的：

```cpp
// CORRECT —— 用枚举
enum Direction { UP, DOWN, LEFT, RIGHT };
if (direction == UP) { ... }    // 一目了然！
if (direction == LEFT) { ... }
```

**类比**：枚举就像给遥控器按钮贴标签。"1号键"和"电源键"是同一个按钮，但"按电源键"比"按1号键"好理解一万倍。

### 来自 Snake 的真实枚举

从 Snake 源码的使用方式可以推断出这些枚举：

```cpp
// 方向
enum Direction { UP, DOWN, LEFT, RIGHT };  // 默认值 0, 1, 2, 3

// 来自 game.cpp 的 is_opposite() 函数
static bool is_opposite(Direction a, Direction b)
{
    return (a == UP    && b == DOWN)  ||
           (a == DOWN  && b == UP)    ||
           (a == LEFT  && b == RIGHT) ||
           (a == RIGHT && b == LEFT);
}
```

```cpp
// 死亡原因
enum DeathReason {
    DEATH_NONE,        // 没死
    DEATH_WALL,        // 撞墙
    DEATH_OBSTACLE,    // 撞障碍物
    DEATH_BODY,        // 撞自己
    DEATH_ENEMY_BODY,  // 撞对手身体
    DEATH_HEAD_ON,     // 头碰头
    DEATH_FULL_MAP     // 地图满了没空位
};

// 来自 game.cpp — 使用死亡原因
if (cell == '#')
{
    if (out_reason) *out_reason = DEATH_WALL;
    return false;
}
```

```cpp
// 游戏模式
enum GameMode { MODE_CLASSIC, MODE_PVP, MODE_BATTLE };

// AI 难度
enum AIDifficulty { AI_EASY, AI_NORMAL, AI_HARD };

// 来自 scene.cpp — 菜单选择决定游戏模式
if (key_pressed('1'))
{
    selected_mode = MODE_CLASSIC;
    _next = SCENE_PLAY;
}
```

### 在 switch 中使用枚举

Snake 里大量使用 `switch` + 枚举的组合：

```cpp
// 来自 render.cpp — 根据字符决定用什么颜色画格子
static COLORREF cell_color(char cell)
{
    switch (cell)
    {
    case '#': return RGB(45, 52, 63);   // 墙：深灰色
    case 'H': return RGB(24, 124, 88);  // 玩家头：绿色
    case 'B': return RGB(73, 180, 130); // 玩家身：浅绿
    case 'A': return RGB(42, 108, 218); // AI 头：蓝色
    case 'F': return RGB(218, 72, 66);  // 食物：红色
    default:  return RGB(238, 242, 246); // 空地：浅灰白
    }
}
```

```cpp
// 来自 ai.cpp — 根据难度选择不同的 AI 策略
Direction ai_choose_direction(const GameState *game, AIDifficulty difficulty)
{
    switch (difficulty)
    {
    case AI_EASY:   return ai_dir_easy(game);
    case AI_NORMAL: return ai_dir_normal(game);
    case AI_HARD:
    default:        return ai_dir_hard(game);
    }
}
```

### 常见错误

**错误 1：访问成员用 `.` 还是 `->`**

```cpp
Point p;
p.row = 5;       // ✅ 普通结构体变量用 .

Point *ptr = &p;
ptr->row = 5;    // ✅ 指针用 ->
// ptr.row = 5;  // ❌ 指针不能用 .
```

**规则**：直接拿着结构体本身 → 用 `.`。拿着指向结构体的指针 → 用 `->`（1.6 节会深入讲）。

**错误 2：枚举值比较用错了类型**

```cpp
// WRONG ❌
Direction dir = UP;
if (dir == 0) { ... }   // 能编译通过，但可读性极差。0 是 UP 吗？

// CORRECT ✅
if (dir == UP) { ... }  // 清晰明了
```

**错误 3：switch 忘了 break**

```cpp
// WRONG ❌ —— 会"穿透"
switch (dir) {
    case UP:    cout << "上";
    case DOWN:  cout << "下";  // UP 也会执行到这里！
}

// CORRECT ✅
switch (dir) {
    case UP:    cout << "上"; break;
    case DOWN:  cout << "下"; break;
}
```

### 练一练

**练习：定义 Student 结构体**

定义一个 `Student` 结构体，包含：
- `name`（字符串，用 `char name[20]` 或 `string name`）
- `age`（int）
- `score`（int）

创建 3 个 Student，填入不同的数据。用一个数组存它们，然后：
1. 打印所有学生的信息
2. 找出分数最高的学生并打印 ta 的名字

> 提示：用 `#include <string>` 和 `string` 类型来处理名字。

**预期输出**（示例）：
```
All students:
Name: Alice, Age: 20, Score: 85
Name: Bob, Age: 21, Score: 92
Name: Charlie, Age: 19, Score: 78

Top student: Bob with score 92
```

### 参考答案

<details>
<summary>点击展开</summary>

```cpp
#include <iostream>
#include <string>
using namespace std;

struct Student {
    string name;
    int age;
    int score;
};

int main() {
    Student students[3] = {
        {"Alice",   20, 85},
        {"Bob",     21, 92},
        {"Charlie", 19, 78}
    };

    cout << "All students:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "Name: " << students[i].name
             << ", Age: " << students[i].age
             << ", Score: " << students[i].score << endl;
    }

    int best_index = 0;
    for (int i = 1; i < 3; i++) {
        if (students[i].score > students[best_index].score)
            best_index = i;
    }

    cout << endl;
    cout << "Top student: " << students[best_index].name
         << " with score " << students[best_index].score << endl;

    return 0;
}
```
</details>

---

## 1.6 指针初步 —— "存地址的变量"

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解指针是什么，`*` 和 `&` 的区别，能读懂 Snake 里 `GameState *game` 的用法

### 为什么需要指针？

到目前为止，我们讲的都是"直接把数据放在变量里"。但有时候，你需要的是**"我知道东西放在哪里，你去找"**，而不是"我把东西搬过来给你"。

**类比**：你去图书馆找一本书。你有两个选择：
1. **传值**：图书管理员把整本书复印一份给你（如果书有 500 页就很慢了）
2. **传地址**：图书管理员告诉你"在第 3 排第 5 层"——你自己去拿原书

指针就是**存地址的变量**。它不存数据本身，而是存"数据在哪里"。

### 指针的三个关键符号

| 符号 | 含义 | 例子 |
|------|------|------|
| `&变量` | 取地址——"这个变量在哪？" | `&score` 返回 score 的内存地址 |
| `类型 *` | 声明指针——"这变量存的是某类型的地址" | `int *p;` p 存的是 int 的地址 |
| `*指针` | 解引用——"去这个地址拿东西" | `*p = 10;` 把 p 指向的那个 int 改成 10 |

```cpp
int score = 100;        // 普通变量
int *ptr = &score;      // ptr 存的是 score 的地址
cout << *ptr << endl;   // 解引用：输出 100（score 的值）
*ptr = 200;             // 通过指针修改 score
cout << score << endl;  // 输出 200——score 真的被改了！
```

### 为什么 Snake 里大量用指针？

这是本章最重要的问题。看 Snake 的代码：

```cpp
void move_snake(GameState *game)    // 传的是指针
void set_direction(GameState *game, Direction dir)   // 传的是指针
void init_game(GameState *game, GameMode mode)       // 传的是指针
```

为什么几乎每个函数都要传 `GameState *game` 而不是 `GameState game`？

**答案：因为要"修改原数据"。**

如果传 `GameState game`（不用指针），C++ 会**复制一份**整个 GameState 传给函数。函数内部的修改只影响这个副本，不影响外面的原版——就像你在复印件上写字，原件不会有任何变化。

如果传 `GameState *game`（用指针），函数拿到的是原版数据的**地址**。通过这个地址修改数据，改的是原件。

**类比**：
- 传值 = 我给你一份你家钥匙的复印件——你用复印件打不开我家的门
- 传指针 = 我给你我家钥匙的原件——你拿着它就能进我家

GameState 这个结构体非常大（几百字节），里面装着地图、两条蛇、各种状态。如果每次都复制一份，不仅浪费内存，而且改了也没用。所以 Snake 里**凡是需要修改游戏状态的函数，统一传指针**。

### 来自 Snake 的真实代码（逐行分析）

```cpp
// game.cpp — move_snake() 函数签名
void move_snake(GameState *game)
//              ^^^^^^^^^^  ^^^^
//              参数类型是指向 GameState 的指针，参数名是 game
{
    // 检查游戏状态——通过指针访问成员用 ->
    if (!game->started || game->paused || game->game_over)
    //  ^^^^^              ^^^^^
    //  等价于 (*game).started，相当于"先找到 game 指向的那个东西，再取它的 started 成员"
        return;

    // 修改蛇的分数——通过指针修改原数据
    game->score += 10;        // 改的是外面那个真正的 GameState
    //  ^^^^^

    // 调用另一个函数——传的也是指针
    place_food(game);         // place_food 也需要修改 GameState
    //         ^^^^
    // 这里传的是"指向 GameState 的那根指针"本身
}
```

### `->` 操作符

`game->score` 是 `(*game).score` 的简写。因为指针访问成员太常见了（尤其是在 Snake 这种游戏代码里），C++ 专门设计了 `->` 这个箭头操作符。

```cpp
GameState g;          // 直接持有结构体
g.score = 10;         // 用 . 访问

GameState *ptr = &g;  // 指向结构体的指针
ptr->score = 10;      // 用 -> 访问，等价于 (*ptr).score = 10
```

箭头的形状（`->`）本身就是很形象的助记符：**指向**目标然后**解引用**。

### `*` 和 `&` 的区别总结

```cpp
int x = 42;

int *p = &x;   // p 的类型是 int*（指向 int 的指针），存的是 x 的地址
               // &x 的意思是 "x 的地址是多少？"

*p = 100;      // *p 的意思是 "去 p 存的地址，把那里的值改成 100"
               // 现在 x 也变成了 100——因为它们指向同一个地方

int **pp = &p; // 指向指针的指针（指向 p，p 又指向 x）
               // Snake 里不常见，知道有这个东西就行
```

### 常见错误

**错误 1：空指针**

```cpp
// WRONG ❌ —— 指针没有指向任何有效数据
int *p;
*p = 10;        // p 里面是垃圾地址！写到哪里去了？崩溃！

// CORRECT ✅
int x;
int *p = &x;    // p 指向有效的 x
*p = 10;        // 安全
```

未初始化的指针（也叫"野指针"）包含一个随机的内存地址。解引用它就是在随机位置读写内存，后果不可预测。**声明指针时必须知道它指向谁**。

**错误 2：忘记用 `->` 而是用了 `.` 操作指针**

```cpp
// WRONG ❌
GameState *game = ...;
game.score = 10;     // ❌ game 是指针不是结构体本身，不能用 .

// CORRECT ✅
game->score = 10;    // ✅ 指针访问成员用 ->
// 或
(*game).score = 10;  // ✅ 等价写法，但很少人这么写
```

**错误 3：把地址赋给错误类型的指针**

```cpp
// WRONG ❌
double pi = 3.14;
int *p = &pi;        // ❌ 类型不匹配：pi 是 double，p 指向 int

// CORRECT ✅
double pi = 3.14;
double *p = &pi;     // ✅ 类型匹配
```

**错误 4：返回局部变量的地址**

```cpp
// WRONG ❌
int* create_number() {
    int x = 10;
    return &x;       // ❌ x 出了函数就销毁了，返回的地址指向已释放的内存！
}

// CORRECT ✅ —— 用动态分配（后面会学）
// 对于 Snake 课程现阶段，不要让函数返回局部变量的地址
```

### Snake 里还能看到哪些指针用法？

**1. 函数参数里的 const 指针（只读）**

```cpp
// 来自 ai.cpp
static bool ai_can_go(const GameState *game, int r, int c)
//                    ^^^^^
// const 表示"我不会修改 game 指向的数据，我只是看看"
```

`const GameState *game` 的意思是：我拿到一个指向 GameState 的指针，但我承诺只读不写。这是一种自我约束 + 编译器检查——如果你在函数里不小心写了 `game->score = 0;`，编译器会报错。

**2. 修改参数值（输出参数）**

```cpp
// 来自 game.cpp
static bool can_enter_cell(GameState *game, Point next, bool will_grow,
                            const Point *body, int length, DeathReason *out_reason)
//                                                           ^^^^^^^^^^^^^^^^
// out_reason 是一个"输出参数"——函数通过它把死亡原因传出去
{
    if (cell == '#')
    {
        if (out_reason) *out_reason = DEATH_WALL;  // 通过指针修改外部变量
        return false;
    }
    // ...
}
```

这是一种"函数返回多个值"的常用技巧：函数本身返回 bool（能否通过），额外的信息通过指针参数"输出"。

```cpp
// 调用方式
DeathReason reason = DEATH_NONE;
if (!can_enter_cell(game, next, will_grow, snake, length, &reason))
//                                                   ^^^^^^
// 传 reason 的地址进去，函数内部可以修改 reason
{
    // 现在 reason 已经被函数填上了具体的死亡原因
    game->death_reason = reason;
}
```

**3. 指针 + 数组的关系**

在 C++ 里，数组名本质上就是一个指向数组首元素的指针：

```cpp
int arr[5] = {1, 2, 3, 4, 5};
int *p = arr;        // arr 等价于 &arr[0]
cout << *p << endl;  // 输出 1（arr[0]）
cout << *(p+1) << endl; // 输出 2（arr[1]）
cout << p[2] << endl;   // 输出 3（arr[2]），指针也能用 [] 操作符！
```

这就是为什么 Snake 里有些函数参数写 `const Point *body` 而不是 `Point body[]`——它们在底层是等价的。

### 练一练

**练习 1：用指针交换两个变量的值**

写一个函数 `void swap(int *a, int *b)`，交换两个整数的值。在 main 中测试它。

**预期输出**：
```
Before swap: x = 10, y = 20
After swap: x = 20, y = 10
```

**练习 2：通过指针修改结构体**

定义一个 `struct Player { string name; int hp; };`，写一个函数 `void damage(Player *p, int dmg)`，把玩家的 hp 减去 dmg（最低减到 0）。在 main 中测试。

**预期输出**：
```
Alice before damage: HP = 100
Alice after damage: HP = 70
Alice after damage: HP = 50
Alice after damage: HP = 0
```

### 参考答案

<details>
<summary>点击展开练习 1</summary>

```cpp
#include <iostream>
using namespace std;

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main() {
    int x = 10, y = 20;
    cout << "Before swap: x = " << x << ", y = " << y << endl;
    swap(&x, &y);
    cout << "After swap: x = " << x << ", y = " << y << endl;
    return 0;
}
```
</details>

<details>
<summary>点击展开练习 2</summary>

```cpp
#include <iostream>
#include <string>
using namespace std;

struct Player {
    string name;
    int hp;
};

void damage(Player *p, int dmg) {
    p->hp -= dmg;
    if (p->hp < 0)
        p->hp = 0;
}

int main() {
    Player alice = {"Alice", 100};
    cout << alice.name << " before damage: HP = " << alice.hp << endl;

    damage(&alice, 30);
    cout << alice.name << " after damage: HP = " << alice.hp << endl;

    damage(&alice, 20);
    cout << alice.name << " after damage: HP = " << alice.hp << endl;

    damage(&alice, 999);
    cout << alice.name << " after damage: HP = " << alice.hp << endl;

    return 0;
}
```
</details>

---

## 本章小结

恭喜你，如果你跟着做完了所有练习，你现在已经掌握了 C++ 的核心语法——**至少是 Snake 游戏里用到的全部核心语法**。我们来回顾一下学了什么：

| 节 | 内容 | 在 Snake 里的用途 |
|-----|------|------------------|
| 1.1 变量与类型 | int, bool, char, DWORD | 存分数、状态、地图字符、时间戳 |
| 1.2 控制流 | if/else, for, while | 判断碰撞、遍历蛇身、游戏主循环 |
| 1.3 函数 | 定义、参数、返回值 | move_snake、place_food、key_pressed……几乎所有逻辑 |
| 1.4 数组 | 一维数组、二维数组 | 蛇身体（Point 数组）、地图（char 二维数组）、按键状态（bool 数组） |
| 1.5 结构体与枚举 | struct, enum, switch | Point, GameState, Direction, DeathReason |
| 1.6 指针 | \*, \&, ->, const | 函数传 GameState\*、输出参数、数组与指针 |

### 你现在能读懂什么？

打开 Snake 的 `game.cpp`，你应该能看懂这些了：

```cpp
// ✅ 变量声明和初始化
game->score = 0;
game->started = false;

// ✅ if/else 条件判断
if (will_eat) { game->score += 10; }

// ✅ for 循环遍历
for (int i = game->length - 1; i > 0; i--)
    game->snake[i] = game->snake[i - 1];

// ✅ 函数定义和调用
void move_snake(GameState *game) { ... }
place_food(game);

// ✅ 结构体成员访问
game->map[p.row][p.col] = 'H';

// ✅ 枚举值使用
game->direction = UP;
case DEATH_WALL: return _T("Reason: Hit Wall");
```

**大约 90% 的 Snake 代码语法，你现在都能读懂了。** 剩下看不懂的 10%（比如 `new`、`virtual`、类继承）会在后续章节中逐步解释——它们只在 `main.cpp` 和 `scene.cpp` 的场景切换部分出现。

---

## 自我测试

以下 5 道选择题检验你对本章内容的掌握程度。答案在最后。

**1.** 以下哪个变量声明是正确的？

A. `integer score = 0;`
B. `int score = 0;`
C. `bool score = 0;`
D. `char score = "0";`

**2.** 以下代码的输出是什么？

```cpp
int x = 5;
if (x = 10) {
    cout << "A";
} else {
    cout << "B";
}
```

A. A
B. B
C. 编译错误
D. 什么也不输出

**3.** 以下代码访问了哪个数组元素？

```cpp
int arr[5] = {10, 20, 30, 40, 50};
cout << arr[3];
```

A. 10
B. 20
C. 30
D. 40

**4.** 在 Snake 里，为什么 `move_snake` 的参数是 `GameState *game` 而不是 `GameState game`？

A. 指针写法更短
B. 因为要通过指针修改原数据，传值只能改副本
C. 因为 GameState 不能作为函数参数
D. 这是 C++ 的硬性规定

**5.** 以下哪种写法能正确通过指向结构体的指针访问成员？

```cpp
struct Point { int row, col; };
Point p = {3, 5};
Point *ptr = &p;
```

A. `ptr.row = 7;`
B. `*ptr.row = 7;`
C. `ptr->row = 7;`
D. `ptr::row = 7;`

---

**答案**：1-B, 2-A, 3-D, 4-B, 5-C

**解析**：
- 第 2 题：`x = 10` 是赋值不是比较，赋值表达式返回 10（非零 = true），所以输出 A。这是最常见的陷阱，请牢记 `==` 和 `=` 的区别。
- 第 4 题：传指针让函数能够修改调用方的原始数据。如果传值，GameState 会被完整复制一份，函数内部改的是副本，外面的原数据不受影响。
- 第 5 题：`ptr->row` 是 `(*ptr).row` 的简写。指针访问成员必须用 `->`，不能用 `.`。
