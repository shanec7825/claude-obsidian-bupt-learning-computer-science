---
type: concept
title: "Web Scraper Reverse Engineering - smartestu.cn"
created: 2026-06-25
updated: 2026-06-25
tags:
  - concept
  - reverse-engineering
  - web-scraping
  - api-discovery
  - authentication
  - jwt
  - cookie-auth
  - python
  - requests
  - devtools
  - security-beginner
status: current
related:
  - "[[DragonScale Memory]]"
  - "[[Source-First Synthesis]]"
---

# Web Scraper Reverse Engineering — smartestu.cn

> 从零逆向一个真实 Web 平台的完整过程，适合网络安全/爬虫初学者。

## 背景

目标：爬取「数你最灵」(smartestu.cn) AI 批改数学平台的作业题目和解答，导出为 Markdown 格式离线学习。

用户背景：网络安全小白，第一次接触爬虫开发。

---

## 复盘：从零开始的爬取脚本

### 阶段 0：起步假设

任何爬虫的第一版都是**猜测**。我们最初假设了以下内容：

| 维度 | 我们的猜测 | 实际情况 |
|------|-----------|---------|
| 认证方式 | 用户名+密码 POST 登录 | JWT Cookie 认证 |
| API 请求方式 | GET 请求 | POST 请求（绝大多数） |
| API 路径 | 基于 REST 惯例猜测 | 实际路径不同 |
| 响应格式 | `{"data": [...]}` | 嵌套 DTO 结构 |
| 字段命名 | `camelCase` 通用名 | 平台特有 DTO 命名 |

**教训**：猜测是必要的起点，但**永远不要相信自己的猜测**。必须用真实数据验证。

### 阶段 1：失败的登录 — 理解认证机制

第一次尝试用 `--school bupt --username 2025211208 --password xxx` 登录，全部 4 种 payload 格式返回 404。

```python
# 我们的 4 种猜测
{"schoolUserId": "2025211208", "password": "xxx"}  # → "User does not exist"
{"email": "2025211208", "password": "xxx"}          # → "Missing credentials"
```

**关键动作**：用户提供了浏览器的一个真实请求头：

```
cookie: refreshToken=eyJhbGciOiJIUzI1NiIsInR...
```

这告诉我们：**平台用 JWT Cookie 做认证，不是用户名密码**。

**核心原理**：JWT (JSON Web Token) 由三段 base64 编码组成：`header.payload.signature`。解码 payload 就能看到：

```json
{"id":"69f0c0d7...","sessionId":"6a3c0b96...","iat":1782322767,"exp":1784912021}
```

- `iat` = issued at（签发时间）
- `exp` = expiration（过期时间）= 约30天

**教训**：爬虫开发的第一步永远是**抓取真实请求**，而不是猜测 API 格式。

### 阶段 2：Cookie vs Bearer — 两种 Token

继续分析浏览器请求，发现了两个不同的 JWT：

**refreshToken**（Cookie 里，30天有效）：
```json
{"id":"...","sessionId":"..."}
```

**accessToken**（Authorization 头里，5小时有效）：
```json
{"id":"...","username":"周萧潇","provider":"local","email":"shanec@qq.com"}
```

两者是**不同的 JWT**！refreshToken 是长期凭证，accessToken 是短期会话 token。这是标准的双 token 认证模式：

```
┌──────────────┐    refreshToken Cookie    ┌──────────────┐
│   Browser    │ ──────────────────────────→│   Backend    │
│   (SPA)      │ ←────── accessToken ──────│   Server     │
│              │    (Bearer header)         │              │
│  API calls   │ ──────────────────────────→│              │
│              │  Authorization: Bearer xxx  │              │
└──────────────┘                           └──────────────┘
```

**教训**：大型 SPA（单页应用）通常使用**双 token 模式**。爬虫需要模拟整个流程。

### 阶段 3：API 返回 HTML 而非 JSON — 路径错误

Cookie 认证看起来"成功了"（返回 200），但响应体是：

```html
<!DOCTYPE html>
<html lang="zh-CN">
  <head><title>数你最灵</title></head>
```

这是 SPA 的 **catch-all 路由**：所有不存在的 API 路径都返回 `index.html`。说明我们的 API 路径是错的！

**解决方案**：让用户打开浏览器 **DevTools → Network** 标签，过滤 **XHR/Fetch**，找到真实的 API 请求。

**发现的真实端点**：
```
POST /api/homework/student/mark/queryHomeworks
POST /api/homework/student/course/query
POST /api/homework/student/mark/queryExercisesByHomeworkId
```

**教训**：响应状态码 200 **不等于** API 调用成功。必须检查响应内容类型（JSON vs HTML）。

### 阶段 4：逐个对齐请求体格式

找到了正确的端点，但返回 `400 Bad Request`。原因是请求 body 格式不对。

**我们的 body**：
```json
{"courseId": "3294"}
```

**实际需要的 body**（从浏览器 DevTools **Payload** 标签获取）：
```json
{"studentId": "bupt-2025211208", "courseIds": [3294], "scene": "homework"}
```

三个关键差异：
1. `courseId` (string) → `courseIds` (array of int)
2. 多了 `studentId` 字段，格式为 `"{school}-{username}"`
3. 多了 `scene: "homework"` 场景标识

**教训**：API body 的**字段名、类型、必填项**都必须与真实请求完全一致。DevTools 的 **Request Payload** 是最可靠的参考。

### 阶段 5：响应体结构层层嵌套

请求成功了，但 "Found 0 homeworks"。调试日志揭示了嵌套结构：

```
queryHomeworks response keys: ['code', 'msg', 'data']
homeworks sub-keys: ['courseHomeworkDTOList']
Homework keys: ['courseId', 'courseName', 'studentCourseHomeworkDTOList']
```

真正的结构是三层嵌套：

```
response.data
  └── courseHomeworkDTOList[]        # 课程分组
       └── studentCourseHomeworkDTOList[]  # 真正的作业
            ├── id: 26522
            ├── name: "十四十五周"
            └── exercises[]           # 题目已内嵌！
                 ├── questionNum: "3"
                 ├── questions[]      # 题目内容
                 ├── answers[]        # 答案
                 └── questionStructure[]  # 子题结构
```

**关键发现**：exercises 已经包含在 homework 响应里，不需要再调用 `queryExercisesByHomeworkId`！这个发现省掉了大量多余的 API 请求。

**教训**：API 响应通常是**高度嵌套的 DTO 结构**。不要假设扁平结构，用 `json.dumps` 或调试日志查看实际嵌套层级。

### 阶段 6：字段名映射 — 最后的细节

即使拿到了 exercise 对象，生成的 Markdown 也只有标题和分数，没有题目内容。原因是字段名不匹配：

| 代码猜的字段 | 实际 API 字段 | 说明 |
|-------------|-------------|------|
| `number` | `questionNum` | 题号 |
| `question` / `content` | `questions[]` | 题目内容（数组） |
| `answer` | `answers[]` | 答案（数组） |
| `text` | `content` | 富文本节点的文字字段 |

`questions[]` 数组的每个元素是：
```json
{"type": "text", "content": "3. 利用级数收敛的定义判别..."}
```

文本字段名是 `content` 而不是 `text` — 这就是为什么所有题目内容都为空。

**教训**：即使字段含义相同，命名可能完全不同。**必须逐个对齐**，不能假设任何字段名。

---

## 完整故障清单

| # | 问题 | 症状 | 根因 | 修法 |
|---|------|------|------|------|
| 1 | `requests.utils.quote` 废弃 | `AttributeError` | 新版 requests 移除了此函数 | 改用 `urllib.parse.quote` |
| 2 | refreshToken 当 Bearer 用 | 所有 API 返回 401 | init 中 `token=refresh_token` 导致跳过登录 | 移除拷贝，正确走 `login()` |
| 3 | 图片下载泄露 Bearer token | 图片下载失败/安全隐患 | `_get()` 向外部 CDN 发送 token | 新增 `_get_raw()` 不带 auth |
| 4 | Cookie 认证失败后直接退出 | 有 refreshToken 但报 "Cannot proceed" | `login_via_refresh_token` 失败后 `login()` 返回 False | Cookie 认证失败不阻止运行 |
| 5 | 图像引用双重扫描 | 性能浪费 | 已知 key 递归后又通用递归 | `known_keys` set 去重 |
| 6 | 教材章节层级跳过 | 教材题库无数据 | chapter ID 当 section ID 用 | 增加 `getSectionsByChapter` |
| 7 | 无请求超时 | 网络差时永久卡住 | 未设置 timeout | 所有请求加 `timeout=30` |
| 8 | `queryHomeworks` 用 GET | 返回 HTML 而非 JSON | 实际是 POST | 改为 POST |
| 9 | `queryHomeworks` body 格式 | 400 Bad Request | 字段名/类型不对 | 对齐 `studentId`/`courseIds[]`/`scene` |
| 10 | 响应嵌套结构不匹配 | "Found 0 homeworks" | 作业在 `studentCourseHomeworkDTOList` 里 | 逐层解析 |
| 11 | 重复请求 exercises | 多余的 API 调用 | exercises 已在 homework 响应中 | 直接用 `hw.exercises` |
| 12 | 字段名全部不匹配 | Markdown 无内容 | 代码假设的字段名全部错误 | 根据真实 JSON 逐字段对齐 |

---

## 爬虫开发的方法论

### 1. 浏览器 DevTools 是你的眼睛

**最重要的三个标签**：
- **Network → XHR/Fetch**：只看 API 请求
- **Headers → Request Payload**：看请求体格式
- **Response**：看返回数据结构

### 2. 不要猜，要验证

每个假设都必须用真实请求验证。流程：

```
猜测 → 发请求 → 看响应 → 对齐 → 再发请求
```

### 3. 逐层突破

```
认证 → 课程列表 → 作业列表 → 题目详情 → 内容导出
```

每一层通了再进下一层。不要一次性改所有代码。

### 4. 调试日志是你的朋友

```python
# 打印响应结构的 key
self._log(f"response keys: {list(data.keys())}", "DEBUG")

# 打印嵌套结构的 key  
self._log(f"sub-keys: {list(homeworks.keys())}", "DEBUG")
```

### 5. 从安全视角看爬虫

作为网络安全学习者，爬虫开发其实就是**合法的逆向工程**：

| 爬虫技术 | 安全对应概念 |
|---------|------------|
| DevTools Network 抓包 | 流量分析 / 协议逆向 |
| JWT 解码分析 | Token 分析 / 身份伪造防护 |
| Cookie vs Bearer 区别 | 认证机制理解 |
| API 端点枚举 | 接口发现 / 攻击面测绘 |
| 响应结构映射 | 数据格式逆向 |
| 请求体重放 | 参数注入测试 |

理解这些对应关系，爬虫开发就能直接转化为安全测试能力。

---

## 当前状态

爬虫已可正常工作：
- ✅ 认证：支持 `--token`（5小时有效 accessToken）+ `--refresh-token`（30天 cookie）
- ✅ 课程列表：`POST /api/homework/student/course/query`
- ✅ 作业列表：`POST /api/homework/student/mark/queryHomeworks`（含 exercises）
- ✅ 题目导出：Markdown 格式，含 LaTeX 公式、子题结构、选项、答案
- ⚠️ 图片下载：需额外 API（`question_has_img` 标记需浏览器查看）
- ⚠️ 考试/错题本/教材：端点待确认

使用方式：
```bash
# 从浏览器复制 access token（F12 → Network → 任意 API → Authorization header）
python scraper.py --token "eyJ..." --student-id "bupt-2025211208"

# 或同时提供 refresh token 以支持自动刷新
python scraper.py --token "eyJ..." --refresh-token "eyJ..." --student-id "bupt-2025211208"
```
