---
type: session
title: "Architecture Overhaul: Three-Layer Vault Redesign"
created: 2026-07-03
tags:
  - architecture
  - think
  - vault-design
  - methodology
status: evergreen
related:
  - "[[index]]"
  - "[[LLM Wiki Pattern]]"
  - "[[认知卸载-Cognitive-Offloading]]"
  - "[[Developer Knowledge Base Pattern]]"
---

# Architecture Overhaul: Three-Layer Vault Redesign

## The Problem

通过 10-principle 思维循环诊断 LLM Wiki 在 CS 学习中的三个结构性问题：

1. **文档与实操断层**：Wiki 作为纯文档+Canvas 适合纯理科（数学、物理），但 CS 的可执行属性要求代码必须在 IDE 里跑。Wiki 页面对代码概念的描述可以错，没有警报会响——IDE 里的编译器才是验证层。
2. **知识的网络属性被扁平化**：CS 从晶体管到 Agent 应用是一个有向无环依赖图（DAG），但 wiki 是扁平的概念列表。学习者不知道"我在哪"、"接下来去哪"。
3. **学习循环被切碎**：Snake/Tetris 的 wiki 页面在 Obsidian 里（几千字的架构图、代码片段），但代码在 IDE 里。读→写→跑→调→懂的循环被切成两个应用，认知上下文每次切换都丢失。

## The Solution: Three-Layer Architecture

```
Layer 1: MAP (wiki/)        → 导航、依赖关系、跨项目合成
Layer 2: CLASSROOM (playground/) → 代码 + LEARN.md，IDE 内同屏
Layer 3: SYNTHESIS (wiki/concepts/) → 只有跨项目洞察才写这里
```

### Layer 1: Wiki as Map

概念页从"教材"变成"路标"：
- 旧：几千字的架构描述 + 代码片段，全在 Obsidian 里
- 新：一句话直觉 + 链接到 playground 代码 + 对比关系 + 下一步去哪

### Layer 2: Playground as Classroom

每个代码项目自带 `LEARN.md`：
- 和源代码在同一目录，IDE 内切换标签页就能看
- Agent 可以直接读代码 + 写 LEARN.md，不需要"理解了再翻译到 wiki"
- 版本控制和代码一起演进

### Layer 3: Synthesis

规则：如果只适用于一个项目 → 在 LEARN.md 里。如果跨多个项目 → 在 wiki/concepts/ 里。

## What Was Created

| Artifact | Purpose |
|----------|---------|
| `playground/` | Hands-on code learning space |
| `playground/snake/LEARN.md` | IDE-ready snake learning guide with exercises |
| `playground/tetris/LEARN.md` | IDE-ready tetris learning guide with comparison table |
| `wiki/canvases/cs-knowledge-graph.canvas` | Full CS dependency map (transistor → agent apps) |
| `wiki/canvases/snake-learning-map.canvas` | Snake architecture visualization |
| Simplified Makefiles | Learning-focused (not production raylib template) |

## Design Principles

1. **Wiki 只在学习开始和结束时打开**：开始时看地图选目的地，结束时写合成笔记。中间全在 IDE。
2. **LEARN.md 替代 wiki 概念页的教程功能**：单一项目的细节不放 wiki。
3. **CS 知识地图回答"我在哪"**：Canvas 可视化依赖关系，标记已完成节点。
4. **方法论比例控制**：每 5 个 CS 概念页允许 1 个方法论页。

## Related Frameworks

- [[认知卸载-Cognitive-Offloading]]：卸载存储（Wiki 记导航），不卸载验证（代码必须在 IDE 里跑通）
- [[合意困难-Desirable-Difficulties]]：LEARN.md 里的 exercise 设计为需要动手才能完成
- [[知识库的正确打开方式-KB-Correct-Usage-Pattern]]：Wiki 是答案钥匙（验证用），不是作弊纸（偷看用）
