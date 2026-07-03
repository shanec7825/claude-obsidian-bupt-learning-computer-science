---
type: meta
title: "Hot Cache"
updated: 2026-05-17T04:30:00
tags:
  - meta
  - hot-cache
status: evergreen
related:
  - "[[index]]"
  - "[[log]]"
  - "[[Wiki Map]]"
  - "[[getting-started]]"
  - "[[DragonScale Memory]]"
---

# Recent Context

Navigation: [[index]] | [[log]] | [[overview]]

## Last Updated

2026-07-03: **Architecture Overhaul: Three-Layer Vault (v2)** — 基于 10-principle 思维循环的架构革新。核心诊断：Wiki 被当教科书用，但 CS 知识是可执行的——IDE 里的编译器才是验证层，Obsidian 应该是地图。三层架构：**Layer 1: Map**（Wiki 做导航，概念页精简为路标）→ **Layer 2: Classroom**（`playground/` 目录，代码 + LEARN.md 在 IDE 内同屏）→ **Layer 3: Synthesis**（只有跨项目洞察才进 wiki/concepts/）。创建 CS 知识地图 Canvas（晶体管→Agent应用完整依赖树）+ Snake 学习地图 Canvas。旧 wiki 概念页保留为路标风格。Index 70→72 页。

2026-06-29: **Save: Agent-Generated Learning Path** — 从 Tetris ingest + 对话提炼。核心问题：没有作者教程时，Agent 能否为任意 GitHub 仓库生成学习路径？答案：可行但验证责任转移——四步工作流（侦查→生成→交互→验证），关键原则（先自解释再看 Agent；卸载存储不卸载验证；推测标推测）。index 69→70 页，仍是 10 源。

2026-06-29: **Ingest: educ8s/Cpp-Tetris-Game-with-raylib** — GitHub 仓库摄入。创建 source 页 [[cpp-tetris-raylib]]（架构、12 步教程路径、新手友好度评分 83/100）+ concept 页 [[Cpp-Tetris-Architecture-Learning]]（Snake vs Tetris 架构对比：Scene 多态 vs Block 继承树）。关键发现：两个项目共享 raylib + C++ 技术栈但采用完全不同的架构模式——这是理解"同一技术栈、不同设计"的最小可行案例。Index 67→69 页，9→10 源。

2026-06-29: **Autoresearch: 为编程新手推荐 GitHub 学习仓库** — 3 轮搜索，15+ 来源。🥇 推荐 **educ8s/Cpp-Tetris-Game-with-raylib**（raylib + C++ 俄罗斯方块，每行代码有视频教程，与蛇项目技术栈 100% 匹配）。建立了 [[GitHub Repo Learning Evaluation Checklist|仓库三层解构模型]]（环境层 / 教学设计层 / 核心代码层）和 10 项新手友好度评分表。后备推荐：MyTinySTL（中文注释 STL）、Build Your Own Shell in C（~500 行）。Index 64→67 页，8→9 源。

2026-06-28: **Autoresearch: GitHub 开源项目学习法可行性** — 3 轮搜索，12+ 来源。核心结论：**以 GitHub 开源项目学习编程可行且有强证据**（ACM SAC 2024 RCT n=90: 支架式代码阅读对低知识新手显著优于被动阅读；ICSE 2025 n=112: 75% 学生学完后能构建复杂系统；专业开发者 58-70% 时间在阅读代码）。四阶段方案：环境准备 → [[Code Reading as Learning Strategy|结构化阅读（六步法）]] → 动手验证 → [[Developer Knowledge Base Pattern|知识库沉淀（四数据库模型）]]。关键发现：自解释是最高杠杆活动（苏格拉底引导 59% 增益 vs 自由解释 30%），新手需要结构化支架而非完全自由探索。集成四篇学习认知文章：[[合意困难-Desirable-Difficulties|含意困难]]解释为什么有效、[[认知卸载-Cognitive-Offloading|认知卸载]]划清界限、[[渐进摘要-Progressive-Summarization|渐进摘要]]提供笔记方法、[[知识库的正确打开方式-KB-Correct-Usage-Pattern|KB正确用法]]设定检索序列。Index 60→64 页，7→8 源。

2026-06-28: **Ingest: Learning Cognition Batch — 学习认知四篇** — 4 source files → 4 concept pages + 1 source page. **Framework**: [[认知卸载-Cognitive-Offloading]] (risk taxonomy: what not to offload) → [[合意困难-Desirable-Difficulties]] (theory foundation: why difficulty is good) → [[渐进摘要-Progressive-Summarization]] (note method: how to structure) → [[知识库的正确打开方式-KB-Correct-Usage-Pattern]] (workflow: correct retrieval sequence). Core operating principle across all four: **offload the mechanical, preserve the cognitive**. The KB is an answer key after self-testing, not a cheat sheet during. [[渐进摘要-Progressive-Summarization|Progressive Summarization]] Layer 4 (self-written mini-summaries) is the single highest-ROI change for retrieval. [[合意困难-Desirable-Difficulties|Desirable Difficulties]] anchors the theory: hard retrieval builds durable memory. Index 55→60 pages, 6→7 sources.

2026-06-26: **Presentation: [[snake-game-presentation]]** — 贪吃蛇课堂答辩演示文档。9 页幻灯片式结构，每页配口述要点+演示动作。覆盖：项目概述、架构（Scene 多态）、GameState 设计理由、地图字符编码、11 步碰撞优先级、三大创新机制（pending_growth / 尾部丢弃 / 独立双定时器）、AI 三级难度 BFS、程序化音效、技术栈总结。附 10 分钟演示脚本 + 3 道常见提问预案。

2026-06-25: **Final Design: [[snake-game-final-design]]** — 贪吃蛇最终概要设计文档（v2，全部常量/算法经 `.raw/贪吃蛇latest/` 13 源文件逐项校准）。核心修正：MAP_SIZE=35（所有模式统一）、MAX_SNAKE_LEN=1600(40×40)、CELL_SIZE=24、GROW_STEP=32、窗口 1140/1180×920、Direction 带 char 值 ('W'/'S'/'A'/'D')、MAP_CHAR_P1_DROP='D'/P2_DROP='E'（丢弃块可被自己当食物吃掉）。涵盖：三模式对比表（13维）、场景驱动分层架构（Mermaid图）、GameState 含 drop_requested、9 步碰撞决策树（Mermaid流程图）、apply_snake_move 含丢弃处理的 18 步伪码、AI 三级难度（含 Hard 尾可达安全检测）、独立双人定时器、PCM 4 音效合成、中文字体按需码点。ASCII 图全部替换为 Mermaid + 表格。附录含碰撞决策树和术语表（7 条）。

2026-06-23: **Ingest: Snake Game raylib Port + Curriculum Headers** — 18 new files ingested from `.raw/` across two source groups. **raylib port** (13 `.cpp`/`.h` files): cross-platform rewrite replacing EasyX, adding procedural audio (4 SFX via sine-wave synthesis), polymorphic C++ scene system (`Scene` → `MenuScene`/`PlayScene`), edge-triggered input, and full raylib rendering with TTF font support. Created source page [[snake-game-raylib]] + 3 concept pages: [[Procedural Audio for Games]], [[Scene State Machine Pattern]], [[Edge-Triggered Input]]. **Curriculum headers** (5 `snake-headers/*.h`): reverse-engineered EasyX headers with extensive Chinese annotations — teaching artifacts for [[Snake Game Curriculum]]. Created source page [[snake-curriculum-headers]]. Updated [[EasyX Graphics Library]] with contradiction note about raylib port. Index 47→52 pages, 3→5 sources.

2026-06-22: **Save: [[Snake Game Curriculum]] — 完整课程完成！** 8 章讲稿全部到位，总计 ~8000+ 行教学内容。新增：5 个头文件（scene.h/game.h/ai.h/input.h/render.h）从源码反推完成；8 章详细讲稿（ch0-ch7），每章含概念讲解+代码示范+常见错误对照+练习+参考答案+自测试题。课程从环境配置到三种模式全通，附带第七章进阶路线图。索引 39→47 页。

2026-06-22: **Save: [[Snake Game Curriculum]]** — Complete 8-chapter, 30+ session course design saved to wiki/concepts/. Covers environment setup, C++ crash course (6 sessions), game loop + rendering (4), input + movement (4), game logic (5), BFS AI (5), multiplayer (4), and advanced directions (2). Each chapter has milestones, exercises, and actionable code snippets.

2026-06-22: **Save: [[snake-game-call-graph]]** — Full function-level call graph of the Snake Game (EasyX) filed to wiki/sessions/. Explains the call hierarchy, collision resolution order, AI decision chain, input edge detection, and data flow across all 6 source files.

2026-06-22: **Ingest: C++ Snake Game (EasyX)** — 6 source files from `.raw/` ingested as [[snake-game-easyx]]. Created 3 wiki pages: [[snake-game-easyx]] (source summary), [[BFS Pathfinding for Game AI]] (BFS-based AI with 3 difficulty tiers), [[EasyX Graphics Library]] (Windows C++ graphics library background). Also noted that `.raw/claude-obsidian-ecosystem-research.md` was already ingested (delta-skipped). The C++ files have no DragonScale addresses because the `flock`-based allocator isn't available on Windows/Git Bash. Index bumped 34→37 pages, 2→3 sources.

2026-05-17 (very late, post-polish): **v1.7.1 patch + polish slice shipped locally** (branch `v1.7.0-compound-vault`, still NOT pushed). All 1 BLOCKER + 6 HIGH findings closed; then verifier agent re-pass surfaced 2 MEDIUM + 3 LOW polish items, all closed in `c2d7575`. Final verifier verdict: 0/0/0/0 SHIP. Score: 100/100 on the v1.7.1 patch dimensions (plan fidelity, behavioral correctness, test health, internal consistency, constraint honor, defect introduction, kernel application). 8 commits landed in this resumption session: `ca68bb6` (Fix 1+6 BLOCKER B1 + H6 — contextual-prefix `--allow-egress` flag default-off + `bin/setup-retrieve.sh` consent prompt + `skills/wiki-retrieve/SKILL.md` Data Privacy callout, mirror of `tiling-check.py:351` `--allow-remote-ollama` precedent), `4837d4f` (Fix 2 H1 — setup-retrieve exit 5 + 3-option recovery hint on Stage 1 failure), `7e1f187` (Fix 3 H2 — `make clean-test-state` extended to v1.7 artifacts), `7120970` (Fix 4 H3 — PostToolUse hook captures LOCK_RC directly, not via pipeline; defers commit on script error OR locks held), `722ac97` (Fix 5 H5 — `detect-transport.sh` `json_escape()` helper via `python3 json.dumps`), `3ea443f` (Fix 7 H4 — new `agents/verifier.md` read-only pre-commit specialist + CLAUDE.md reference), and the cross-cutting closeout `822c80a` (version bump 1.7.0 → 1.7.1, CHANGELOG entry, audit doc updated with §10.2 SHAs + v1.7.1 closeout block, audit benchmark scripts promoted to tracked files). `make test` ran 7/7 green after every fix. End-to-end verifications: `python3 scripts/contextual-prefix.py --peek` returns `tier=synthetic` even with `ANTHROPIC_API_KEY` set (default-deny works); `--allow-egress` correctly flips it; `echo "" | bash bin/setup-retrieve.sh` aborts at the consent prompt; `bash scripts/wiki-lock.sh acquire ...` then hook trigger correctly defers auto-commit. **Next step**: ask user whether to push + tag `v1.7.1`. Do NOT push without explicit go.

2026-05-17 (late): **v1.7.0 full audit complete; v1.7.1 fixes plan ready**. Branch `v1.7.0-compound-vault` still local-only (no push, no tag). The audit was demanded as "THROUGH and FULL on AUDIT following /best-practices" with EVERYTHING scope. Result: **31 findings (1 BLOCKER + 6 HIGH + 14 MEDIUM + 10 LOW)** in `docs/audits/v1.7.0-audit-2026-05-17.md` (481 lines). **BLOCKER**: `scripts/contextual-prefix.py:252-258` data-egress consent gap — `pick_prefix_tier()` silently sends wiki page bodies to Anthropic API whenever `ANTHROPIC_API_KEY` is set; mirror `scripts/tiling-check.py:351-352` `--allow-remote-ollama` precedent to fix (~1h). **Retrieval benchmark PASS**: 50 queries × 2 pipelines via `scripts/benchmark-runner.py`; v1.7 top-1 54.0% vs v1.6 baseline 24.0% (+30pp); error-reduction +39.5% vs ≥30% gate. Per-category breakdown in audit §6.2. **Competitor recheck (24h after compass May 16)**: kepano +1.1k★ to 31.6k, Copilot CLI integration issue still stale 3mo (genuine moat for us), NotebookLM May 2026 shipped Video Overviews + 4-tile Studio (widens derivative-outputs gap — filed M13 for v2.0 derive spec). **7-axis #1 verdict**: YES on 4 axes (compounding wiki, multi-writer safety, retrieval architecture free-tier, license openness), TIE on methodology (v1.8 closes), NO on GUI ergonomics (v2.5+) + derivative outputs (v2.0). Honest answer: #1 today on power-user-control axes, not in mainstream adoption without v2.0+v2.5.

**For post-compact resumption**: read `docs/audits/v1.7.1-fixes-plan.md` (this is your roadmap — 6 commits, ~2.5h, sequenced top-to-bottom with file paths + exact edits + verification steps + commit messages per fix). The plan starts with the BLOCKER (Fix 1) + Data Privacy callout (Fix 6) bundled. Working tree state on resume: 4 untracked files (audit doc, fixes plan, `scripts/baseline-v16.py`, `scripts/benchmark-runner.py`); `96a5505` wiki auto-commit landed the benchmark corpus at `wiki/meta/retrieval-benchmark-v1.7.md`; `make test` is 7/7 green; `bash bin/setup-retrieve.sh --no-llm` is provisioned (chunks/, bm25/, embed-cache.json exist — gitignored). User wants to "proceed" with the fixes after compact; do NOT push or tag without explicit go.

**Session record** (full prose, ~600 lines) in personal vault: `~/Documents/Obsidian Vault/sessions/2026-05-17 claude-obsidian v1.7 audit + fixes plan.md`. Ingest-log entry prepended at top of `~/Documents/Obsidian Vault/log/ingest-log.md` per global save convention.

2026-05-17: **v1.7.0 "Compound Vault" refoundation shipped locally** (branch `v1.7.0-compound-vault`, NOT pushed). Four workstreams committed as 4 separate feat commits: §3.1 substrate hard-prefer on `kepano/obsidian-skills` (9c8e510), §3.2 default transport with new `wiki-cli` skill + `scripts/detect-transport.sh` (6c7671e), §3.3 hybrid retrieval pipeline as opt-in `wiki-retrieve` skill with 4 new scripts + 2 hermetic test suites (45a5bd3), §3.4 multi-writer safety closing the latent corruption bug from v1.6 via `scripts/wiki-lock.sh` (66c11f9). Cross-cutting commit pending: version bump 1.6.0→1.7.0, README/CLAUDE.md updates, CHANGELOG entry, new `docs/compound-vault-guide.md` omnibus, this hot.md update. `make test` runs 7 suites green (was 3) — zero ollama / network dependency preserved. Plan file at `~/.claude/plans/read-in-full-the-hidden-sun.md`. User-paused at "full on review on all work done"; no push or tag until explicit go.

2026-04-24 (late night): v1.6.0 public release notes shipped. `docs/releases/v1.6.0.md` (Karpathy-style, 346 lines) establishes the release-notes convention. Three original SVGs at `wiki/meta/dragonscale-{mechanism-overview,6-test-flow,frontier-graph}.svg` carry the visual load; Wikipedia dragon curve referenced by text link only (no binary vendoring). R4 codex verifier ACCEPT WITH FIXES, 3 wording fixes applied. User runs `gh release create v1.6.0 --notes-file docs/releases/v1.6.0.md` when ready. Commits `85515bb` (docs), plus wiki/meta/ auto-commits for SVGs.

2026-04-24 (night): DragonScale end-to-end validation pass. Six-test menu run via Teams orchestration (codex gpt-5.4 for M1 dry-run, M1 commit, M4 autoresearch; chair for ollama pull, M2 allocate, M3 full tiling). All six green. First real fold committed (`wiki/folds/fold-k3-from-2026-04-23-to-2026-04-24-n8.md`, 115 lines, 8 children). First real tiling report at `wiki/meta/tiling-report-2026-04-24.md` (0 errors, 15 review pairs). M2 counter advanced 2 to 3, `c-000002` reserved-unassigned. M4 autoresearch filed 3 new concept pages (`Persistent Wiki Artifact`, `Source-First Synthesis`, `Query-Time Retrieval`) extending `[[How does the LLM Wiki pattern work?]]` with Karpathy gist + RAG + MemGPT + Obsidian docs as sources. v1.6.0 validated.

2026-04-24 (evening): v1.6.0 closeout via Teams approach (chair-led, codex gpt-5.4 for sub-agents). 2 explorers (closeout gaps + doc surface). 6 bounded writes (non-overlapping scope): `docs/dragonscale-guide.md` (new, 563 lines), `wiki/meta/2026-04-24-v1.6.0-release-session.md` (new, 346 lines), `wiki/meta/boundary-frontier-2026-04-24.md` (first real M4 run artifact, new), `docs/install-guide.md` (1.5.0 to 1.6.0 + M4 callout + flat-extractive correction), `README.md` (parenthetical + guide link), `wiki/hot.md` (drift fixes). 1 adversarial verifier returned ACCEPT WITH FIXES; all 11 fixes applied in place. Docs commit `eb1562f`. `make test` green (74+ assertions). Still no git tags for v1.5.0 / v1.5.1 / v1.6.0. User requested gpt-5.5; API rejects it on this codex CLI; gpt-5.4 used throughout.

2026-04-24 (late): Phase 4 shipped. Mechanism 4 (boundary-first autoresearch) implemented as `scripts/boundary-score.py` with expanded test coverage. `/autoresearch` without a topic now offers frontier candidates (opt-in, agenda-control labeled). Cross-file status updated. Version bumped to 1.6.0 in `plugin.json` + `marketplace.json`; no git tag created locally (only pre-DragonScale tags `v1.1` - `v1.4.3` exist).

2026-04-24 (afternoon): Phase 3.6 hardening, five surgical fixes (tiling --report path confinement, rollout baseline, AGENTS.md consistency, wiki-ingest .raw contradiction, install-guide version). v1.5.1.

2026-04-24 (morning): Phase 3.5 hardening pass. Cross-phase audit resolved 10 hold-ship items. At that point Mechanism 4 was marked NOT IMPLEMENTED (later reversed in Phase 4 the same day). `bin/setup-dragonscale.sh` + tests + Makefile added, CHANGELOG created, versions synced to 1.5.0.

2026-04-23 (3): Phase 3 complete. Semantic tiling lint shipped as opt-in. `scripts/tiling-check.py` with flock-guarded atomic cache, localhost-locked OLLAMA_URL default, symlink rejection, model-drift invalidation, and banded thresholds (error>=0.90, review>=0.80, conservative seeds). 4 codex review rounds, 10/10 accept.

2026-04-23 (2): Phase 2 complete. Deterministic page addresses MVP via `scripts/allocate-address.sh` (flock-guarded, recovers counter from max observed). New frontmatter `address: c-NNNNNN`. `wiki-ingest` and `wiki-lint` updated with opt-in Address Assignment and Validation sections. 3 codex rounds, 8/8 accept.

2026-04-23 (1): Phase 0-1 complete. DragonScale Memory spec (`wiki/concepts/DragonScale Memory.md` v0.3) plus `skills/wiki-fold/` for Mechanism 1 (log rollups, dry-run verified). Survived multi-round codex review.

## Plugin State

- **Version**: 1.7.1 (audit-driven patch on top of Compound Vault; plugin.json + marketplace.json synced; local-only branch `v1.7.0-compound-vault`, no push, no tag)
- **Install ID**: `claude-obsidian@ai-marketing-hub-claude-obsidian`
- **Skills**: 13 (wiki, wiki-ingest, wiki-query, wiki-lint, wiki-fold, save, autoresearch, canvas, defuddle, obsidian-bases, obsidian-markdown, **wiki-cli (v1.7)**, **wiki-retrieve (v1.7, opt-in)**)
- **Scripts (v1.6)**: `scripts/allocate-address.sh`, `scripts/tiling-check.py`, `scripts/boundary-score.py` (DragonScale; opt-in; feature-detected by skills)
- **Scripts (v1.7 — new)**: `scripts/detect-transport.sh`, `scripts/contextual-prefix.py`, `scripts/bm25-index.py`, `scripts/rerank.py`, `scripts/retrieve.py`, `scripts/wiki-lock.sh`
- **Setup**: `bin/setup-vault.sh` (base vault), `bin/setup-dragonscale.sh` (opt-in DragonScale), `bin/setup-multi-agent.sh` (multi-agent bootstrap), `bin/setup-retrieve.sh` (opt-in v1.7 hybrid retrieval)
- **Tests**: `make test` runs 7 suites — `test_allocate_address.sh`, `test_tiling_check.py`, `test_boundary_score.py`, **`test_bm25_index.py`**, **`test_retrieve.py`**, **`test_wiki_lock.sh`**, **`test_concurrent_write.sh`**. Zero ollama and zero network dependency for all core tests.
- **Hooks**: 4 (SessionStart, PostCompact, PostToolUse [stages wiki/, .raw/, .vault-meta/; **v1.7: defers `git add` if wiki-lock locks held**], Stop)

## DragonScale Mechanisms

1. **Fold operator** (Mechanism 1): `skills/wiki-fold/`, dry-run verified AND first real fold committed at `wiki/folds/fold-k3-from-2026-04-23-to-2026-04-24-n8.md`.
2. **Deterministic addresses** (Mechanism 2): shipped and exercised; vault counter at 3. `c-000001` on DragonScale Memory.md. `c-000002` reserved-unassigned from validation pass (gap acceptable per spec).
3. **Semantic tiling lint** (Mechanism 3): shipped and activated. `nomic-embed-text` pulled; first tiling report at `wiki/meta/tiling-report-2026-04-24.md` (0 errors, 15 review-band pairs).
4. **Boundary-first autoresearch** (Mechanism 4): shipped (Phase 4, opt-in). `scripts/boundary-score.py` + `tests/test_boundary_score.py`. `/autoresearch` without a topic surfaces top-5 frontier pages as candidates; user picks, overrides, or declines. Explicitly labeled "agenda control" in both spec and skill.

## Key Lessons from This Release Cycle

1. Cross-phase audits are essential. Individual phase reviews miss drift between phases.
2. Opt-in feature detection (`[ -x script ] && [ -f state ]`) preserves default plugin behavior for adopters and non-adopters alike.
3. PostToolUse hook matcher is `Write|Edit`, so Bash writes don't fire it. Scripts that mutate tracked state must be Bash-only to avoid side-effect commits.
4. Seed-vault self-consistency matters: if the spec says post-rollout pages need addresses, the concept page itself has to have one.
5. Codex adversarial review rounds stop when the punch list is empty, not when the author feels done.

## Style Preferences

- No em dashes (U+2014) or `--` as punctuation. Periods, commas, colons, or parentheses. Hyphens in compound words are fine.
- Short and direct responses. No trailing summaries.
- Parallel tool calls when independent.

## Active Threads

- DragonScale Mechanism 4 shipped in Phase 4 as an opt-in Topic Selection mode in `skills/autoresearch/`. All four DragonScale mechanisms are now shipped and feature-gated.
- v1.6.0 not yet pushed to GitHub (local commits only, no git tag created). User controls push and tag timing.
- CLAUDE.md has one pre-existing uncommitted change ("Release Blog Post" section) that predates this session.

## Repo Locations

- Working: `~/Desktop/claude-obsidian/`
- Public: https://github.com/AI-Marketing-Hub/claude-obsidian
