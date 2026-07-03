---
type: source
title: "GitHub Codebase Learning Research — 开源项目学习法研究汇编"
created: 2026-06-28
updated: 2026-06-28
tags:
  - source
  - learning-methodology
  - code-reading
  - academic-research
status: current
source_type: research-synthesis
pages_created:
  - "[[Research-GitHub-开源项目学习法可行性]]"
  - "[[Code Reading as Learning Strategy]]"
  - "[[Developer Knowledge Base Pattern]]"
---

# GitHub Codebase Learning Research — 开源项目学习法研究汇编

> 多源研究合成：以 GitHub 开源项目为原点学习编程的可行性与方法

研究日期: 2026-06-28 | 3 轮搜索 | 12+ 来源获取

---

## Primary Sources

### Academic Papers

1. **Oli, Banjade, Narayanan, Brusilovsky, Rus (2024)** — "Exploring The Effectiveness of Reading vs. Tutoring For Enhancing Code Comprehension For Novices." ACM SAC '24. RCT with 90 university students. Scaffolded self-explanation outperforms passive reading for low prior-knowledge learners. DOI: 10.1145/3605098.3636007

2. **Oli et al. (2023)** — "When is Reading More Effective than Tutoring?" CSEDM Workshop. Socratic guided self-explanation produced 59% learning gains vs 30% for free self-explanation.

3. **Yamaguchi et al. (2018)** — "Report on Practice of a Learning Support System for Reading Program Code Exercise." Springer. Code tracing exercises produced better coding test scores than writing-first approaches.

4. **Woerner & Socha (2024)** — "Addressing the Gap Between How Students and Professionals Read Code." JCSC. Students who received structured code reading training significantly improved comprehension.

5. **Azamnouri, Koch, Bogner & Wagner (2025)** — "How Do Computer Science Students Perceive Self-Study with Open-Source Repositories?" ICSE CSEE&T. n=112. 75% of students felt capable of building complex systems after studying OSS repos.

6. **Jarvie-Eggart et al. (2025)** — "Jiving with LLMs: 1st-Year Students' Computer Programming Self-Efficacy after Reading Code with LLMs." JTLT. Students who used LLMs to understand code shifted away from using LLMs to write code—a virtuous cycle.

7. **Tanaka, Horiuchi & Ueda (2024)** — "Evaluation of the Effectiveness of Reading Aloud a Program Code." IADIS CELDA. NIRS brain imaging showed significant brain activity increase during code reading aloud; test scores improved.

8. **Denny et al. (2024)** — "Explaining Code with a Purpose." ITiCSE. Explain in Plain English (EiPE) questions + LLM validation: 96-99% success rates; relational descriptions most effective.

### Industry Sources

9. **Brittany Ellich, GitHub Blog (2024)** — "How GitHub engineers learn new codebases." 14-strategy framework: hands-on exploration, Copilot-assisted learning, pair programming, personal documentation, diagramming, domain understanding, teaching-as-learning.

10. **Marit van Dijk, Dev2Next (2024)** — "Reading Code." Code Reading Club structured exercises. Three dimensions: Structure, Concepts, Context. Common novice anti-patterns identified.

11. **CSDN (2024)** — "如何通过开源项目学习编程？" Six-step methodology: project selection → documentation → reading → hands-on → community → iteration.

### Knowledge Management Patterns

12. **Developer Second Brain (dev.to)** — Four-database model: Code Snippets, TIL Journal, Resource Bookmarks, Technical Notes. Knowledge maturity pipeline (Seed→Growing→Evergreen). Spaced repetition integration.

---

## Key Cross-Source Agreements

All sources agree on these core principles:

1. **Structured > Passive**: Simply opening code and reading is ineffective. Guided, structured reading with self-explanation is effective.
2. **Self-explanation is critical**: Writing down what you think code does in your own words is the highest-leverage activity.
3. **Relational > Structural**: Explaining WHY code does something (relational) beats explaining WHAT each line does (structural).
4. **Novices need scaffolding**: Low-knowledge, low-efficacy learners benefit most from guided approaches.
5. **Read + Do > Read Only**: Reading must be followed by running, modifying, testing, or explaining.

## Key Cross-Source Tensions

- **AI assistance boundary**: Jarvie-Eggart (2025) finds AI-as-explainer leads to reduced AI-as-writer dependency (virtuous). Oli (2024) finds passive AI consumption reduces learning (vicious). Resolution: AI for explanation, not generation. The distinction matters.
- **Project scope for beginners**: GitHub Blog suggests "Good First Issues" on large projects. CSDN suggests standalone small projects. Resolution: depends on prior knowledge. Absolute beginners need small standalone projects first.

## Confidence Assessment

- **High confidence**: Code reading with structured self-explanation is effective for learning (multiple RCTs, converging evidence)
- **Medium confidence**: Beginner-specific adaptations (emerging evidence, smaller samples)
- **Low confidence**: Optimal sequence of project difficulty progression (no direct comparative studies found)
