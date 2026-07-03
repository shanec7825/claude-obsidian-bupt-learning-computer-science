---
type: meta
title: "Wiki Index"
updated: 2026-04-07
tags:
  - meta
  - index
status: evergreen
related:
  - "[[overview]]"
  - "[[log]]"
  - "[[hot]]"
  - "[[dashboard]]"
  - "[[Wiki Map]]"
  - "[[concepts/_index]]"
  - "[[entities/_index]]"
  - "[[sources/_index]]"
  - "[[LLM Wiki Pattern]]"
  - "[[Hot Cache]]"
  - "[[Compounding Knowledge]]"
  - "[[Andrej Karpathy]]"
---

# Wiki Index

Last updated: 2026-07-03 | Total pages: 72 | Sources ingested: 10

Navigation: [[overview]] | [[log]] | [[hot]] | [[dashboard]] | [[Wiki Map]] | [[getting-started]] | [[../playground/README|Playground]]

---

## 🆕 Architecture (v2: Three-Layer Model)

> See [[cs-knowledge-graph]] canvas for the full CS dependency map.

- **Layer 1: Map** — Wiki is the navigation layer. Concept pages are signposts, not textbooks.
- **Layer 2: Classroom** — `playground/` directory. Code + LEARN.md side by side in the IDE.
- **Layer 3: Synthesis** — Cross-project insights filed here in `wiki/concepts/`.

Started projects:
- [[../playground/snake/LEARN|🐍 Snake]] — Scene polymorphism, BFS AI, procedural audio
- [[../playground/tetris/LEARN|🧱 Tetris]] — Class inheritance, grid modeling, 7-piece system

---

## Concepts

- [[LLM Wiki Pattern]] — the pattern for building persistent, compounding knowledge bases using LLMs (status: mature)
- [[Hot Cache]] — ~500-word session context file, updated after every ingest and session (status: mature)
- [[Compounding Knowledge]] — why wiki knowledge grows more valuable over time, unlike RAG (status: mature)
- [[cherry-picks]] — prioritized feature backlog from ecosystem research; 13 features to add to claude-obsidian (status: current)
- [[SVG Diagram Style Guide]] — canonical visual style for all diagrams: Space Grotesk, #0A0A0A dark theme, #E07850 accent, full design tokens (status: evergreen)
- [[Pro Hub Challenge]] — community challenge pattern for building claude-seo/claude-blog extensions; first challenge produced 6 submissions, 5 integrated in v1.9.0 (status: evergreen)
- [[Semantic Topic Clustering]] — SERP-based keyword grouping replacing paid tools; hub-spoke architecture with interactive visualization (status: evergreen)
- [[Search Experience Optimization]] — "read SERPs backwards" methodology for page-type mismatch detection and persona scoring (status: evergreen)
- [[SEO Drift Monitoring]] — "git for SEO" baseline/diff/track with 17 comparison rules and SQLite persistence (status: evergreen)
- [[DragonScale Memory]] — memory-layer spec inspired by the Heighway dragon curve; fold operator, deterministic page addresses, semantic tiling, boundary-first autoresearch (status: shipped v0.4, all four mechanisms opt-in)
- [[Persistent Wiki Artifact]]: durable Markdown page as the LLM's memory object, distinct from ephemeral chat turns (status: developing)
- [[Source-First Synthesis]]: provenance discipline; raw sources stay immutable while the wiki layer is synthesized and cited (status: developing)
- [[Query-Time Retrieval]]: wiki query path synthesizes with citations; complementary to Obsidian's in-vault search (status: developing)
- [[BFS Pathfinding for Game AI]] — breadth-first search applied to grid-based game AI with 3 difficulty tiers (status: current)
- [[EasyX Graphics Library]] — lightweight Windows C++ graphics library, popular in Chinese C++ education (status: current)
- [[Snake Game Curriculum]] — 完整8章30+课时零基础C++游戏开发课程，含头文件、讲稿、练习、参考答案（status: complete）
- [[Procedural Audio for Games]] — procedural sine-wave SFX generation at runtime; no asset files, 4 sound effects with linear fade-out envelopes (status: current)
- [[Scene State Machine Pattern]] — polymorphic C++ scene hierarchy for game flow; MenuScene→PlayScene transitions with virtual update/render (status: current)
- [[Edge-Triggered Input]] — distinguishing key-pressed (edge) from key-held (level) detection; turn-locking pattern for grid-based games (status: current)
- [[Web Scraper Reverse Engineering - smartestu.cn]] — 从零逆向真实 Web 平台全流程；12个Bug修复记录；适合安全初学者 (status: current)
- [[snake-game-final-design]] — 贪吃蛇图形化版最终概要设计文档；基于 raylib 移植版 13 源文件，含架构/数据结构/碰撞算法/AI策略/音效合成完整设计 (status: evergreen)
- [[snake-game-presentation]] — 贪吃蛇课堂答辩演示文档；9 页幻灯片结构 + 口述要点 + 10 分钟演示脚本 + 提问预案 (status: complete)
- [[认知卸载-Cognitive-Offloading]] — 认知卸载三层分类（辅助/替代/破坏）；Google 效应；关键区分：卸载存储 vs 卸载思考 (status: evergreen)
- [[合意困难-Desirable-Difficulties]] — Bjork 的含意困难理论；Performance≠Learning；Storage Strength vs Retrieval Strength；五种核心策略 (status: evergreen)
- [[渐进摘要-Progressive-Summarization]] — Tiago Forte 五层笔记提炼法；自写摘要的不可替代性；解决笔记检索困难 (status: evergreen)
- [[知识库的正确打开方式-KB-Correct-Usage-Pattern]] — 知识库检索序列规则：先回忆再验证；三层认知分工；Summary-Trigger 机制 (status: evergreen)
- [[Code Reading as Learning Strategy]] — 结构化代码阅读六步法；自解释效应；研究证据：支架式阅读优于被动阅读 (status: evergreen)
- [[Developer Knowledge Base Pattern]] — 开发者知识库四数据库模型；知识成熟度管道；代码学习的专属笔记方法 (status: evergreen)
- [[GitHub Repo Learning Evaluation Checklist]] — 仓库三层解构（环境/教学/核心）；新手友好度评分表；识别环境噪音 (status: evergreen)
- [[Cpp-Tetris-Architecture-Learning]] — Snake vs Tetris 架构对比：Scene 多态 vs Block 继承树；同一技术栈不同设计 (status: developing)
- [[Agent-Generated Learning Path]] — 无教程时 Agent 替代作者教程的四步工作流；验证责任转移模型；关键原则 (status: developing)

---

## Entities

- [[Andrej Karpathy]] — AI researcher, creator of the LLM Wiki pattern, former Tesla AI director (status: developing)
- [[Ar9av-obsidian-wiki]] — multi-agent compatible LLM Wiki plugin; delta tracking manifest (status: current)
- [[Nexus-claudesidian-mcp]] — native Obsidian plugin + MCP bridge; workspace memory, task management (status: current)
- [[ballred-obsidian-claude-pkm]] — goal cascade PKM; auto-commit hooks, /adopt command (status: current)
- [[rvk7895-llm-knowledge-bases]] — 3-depth query system, Marp slides, parallel deep research (status: current)
- [[kepano-obsidian-skills]] — official skills from Obsidian creator; defuddle, obsidian-bases (status: current)
- [[Claudian-YishenTu]] — native Obsidian plugin embedding Claude Code; plan mode, @mention (status: current)
- [[Claude SEO]] — Tier 4 Claude Code skill for SEO analysis; 23 skills, 17 agents, 30 scripts at v1.9.0 (status: evergreen)

---

## Sources

- [[claude-obsidian-ecosystem-research]] — 2026-04-08 | web research across 16+ repos | 8 wiki pages created
- [[snake-game-easyx]] — 2026-06-22 | C++ Snake game using EasyX (6 source files) | 3 wiki pages created
- [[snake-game-raylib]] — 2026-06-23 | C++ Snake game ported to raylib (13 files) | audio, scene system, proper headers | 3 wiki pages created
- [[snake-curriculum-headers]] — 2026-06-23 | reverse-engineered EasyX headers with Chinese annotations (5 files) | curriculum artifact
- [[smartestu-scraper-reverse-engineering]] — 2026-06-25 | 逆向 smartestu.cn 教育平台 API；12个Bug逐个修复 | 1 wiki page
- [[learning-cognition-batch]] — 2026-06-28 | 四篇学习认知元方法论：含意困难、渐进摘要、KB正确用法、认知卸载 | 4 wiki pages
- [[github-codebase-learning-research]] — 2026-06-28 | 多源研究合成：开源项目学习法的可行性与方法 | 3 wiki pages
- [[github-learning-repo-research]] — 2026-06-29 | 仓库推荐研究：评级 C++ 学习仓库；15+ 来源 | 2 wiki pages
- [[cpp-tetris-raylib]] — 2026-06-29 | educ8s/Cpp-Tetris-Game-with-raylib 仓库分析；12 步教程；83/100 推荐指数 | 1 wiki page

---

## Sessions

- [[snake-game-call-graph]] — 2026-06-22 | full function-level call graph for the EasyX Snake game (status: developing)

---

## Questions

- [[How does the LLM Wiki pattern work]] — how the pattern works and why it outperforms RAG at human scale (status: developing)
- [[Research-GitHub-开源项目学习法可行性]] — 以 GitHub 开源项目为原点学习编程：可行、有证据、有方案；四阶段框架 (status: developing)
- [[Research-为编程新手推荐-GitHub-学习仓库]] — 推荐 educ8s/Cpp-Tetris-Game-with-raylib + 备选；三层解构方法；评估体系 (status: developing)

---

## Comparisons

- [[Wiki vs RAG]] — when to use a wiki knowledge base versus RAG; verdict: wiki wins at <1000 pages
- [[claude-obsidian-ecosystem]] — feature matrix of 16+ Claude+Obsidian projects; where claude-obsidian wins and gaps

---

## Decisions

- [[2026-04-14-community-cta-rollout]] - Skool community CTA footer added to 6 skill repos with per-tool frequency rules (status: active)
- [[2026-04-15-slides-and-release-session]] - Claude SEO v1.9.0 slides (15-slide HTML deck) + GitHub release v1.9.0 with PDF asset (status: complete)
- [[2026-04-15-release-report-session]] - Claude SEO v1.9.0 Release Report PDF: dark theme, 13 pages, WeasyPrint layout fixes, Challenge v2 added (status: complete)
- [[2026-04-14-claude-seo-v190-session]] - Claude SEO v1.9.0 Pro Hub Challenge integration: 5 submissions, 4 new skills, 4 review rounds, cybersecurity audit (status: complete)

---

## Domains

<!-- Add domain entries here after scaffold -->
