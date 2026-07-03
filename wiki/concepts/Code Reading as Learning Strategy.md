---
type: concept
title: Code Reading as Learning Strategy
created: 2026-06-28
updated: 2026-06-28
tags:
  - learning-methodology
  - code-reading
  - programming-education
  - cognitive-science
status: evergreen
related:
  - "[[合意困难-Desirable-Difficulties]]"
  - "[[Research-GitHub-开源项目学习法可行性]]"
sources:
  - "[[github-codebase-learning-research]]"
---
 
# Code Reading as Learning Strategy

## Definition

Code reading as a learning strategy means using existing source code as the primary learning material for programming, rather than (or in addition to) tutorials, textbooks, or courses. The learner reads, traces, and explains existing code to build understanding of programming concepts, patterns, and practices.

## The Evidence Base

Multiple independent research lines converge on the same conclusion: reading code is not just a valid way to learn—it may be the most underutilized and effective method, especially for novices with proper scaffolding.

### Academic Evidence

| Study | Year | Design | Key Finding |
|-------|------|--------|-------------|
| Oli et al., ACM SAC | 2024 | RCT, n=90 | Scaffolded self-explanation of code significantly outperforms passive reading for low prior-knowledge students |
| Socratic vs Free Explanation | 2023 | Controlled | 59% learning gain (Socratic) vs 30% (free) — interactive guidance doubles effectiveness |
| Yamaguchi et al., Springer | 2018 | A/B test | Code tracing/tracing exercises produced better output prediction AND coding test scores than writing-first approaches |
| Woerner & Socha, JCSC | 2024 | Intervention | Teaching students to read code like professionals (cross-file tracing) significantly improved code comprehension |
| ICSE CSEE&T | 2025 | n=112 | 75% of CS students reported ability to build complex systems after studying open-source repos with documentation |

### Industry Consensus

- Professional developers spend **58-70%** of their time reading code (not writing it)
- GitHub engineers' onboarding process is fundamentally code-reading driven (Ellich, GitHub Blog 2024)
- Code review—essentially reading others' code critically—is a universal industry practice

## Why Passive Reading Fails

> [!warning] Simply opening a file and reading line by line is the least effective approach.

Three reasons novices fail at passive code reading:

1. **Lack of chunking**: Experts see patterns ("a loop with a filter"); novices see individual tokens. Without guidance on what to group together, working memory is overwhelmed.
2. **Linearity assumption**: Novices read code top-to-bottom like prose. Experts scan non-linearly—jumping to key functions, following call chains, ignoring irrelevant sections.
3. **Missing domain model**: Without understanding WHY the code exists (business context, design rationale), comprehension is shallow.

## The Structured Reading Framework

Based on the Code Reading Club approach (Van Dijk) and research evidence:

### Six-Step Structured Read

```
1. FIRST GLANCE (1 min)
   What catches your eye first? Second? Why those?

2. STRUCTURE MAP
   Identify: functions, classes, their relationships. Draw if needed.

3. CRITICAL LINES
   Find the 5 most important lines. Defend your choices.

4. SELF-EXPLANATION (THE KEY STEP)
   Explain the code in your own words. Use RELATIONAL language
   ("this validates input then transforms it")
   NOT line-by-line language ("line 3 declares a variable")

5. TRACE EXECUTION
   Pick a concrete input. Trace it through the code by hand.

6. SUMMARIZE
   What did you learn? What's still unclear?
```

### The Self-Explanation Rule

> **Relational > Structural.** "This function authenticates the user by checking the token against the database" is a relational explanation. "This function takes two parameters and calls three other functions" is structural. Research shows relational explanations correlate with higher learning gains and better code-writing ability. Practice relational explanations deliberately.

### For Beginners Specifically

Research (Oli et al., 2024) is clear: **low prior-knowledge learners benefit the most from structured scaffolding.**

- Start with **one function**, not one file
- Use **guided questions** (What does this function return? What are its side effects? What edge cases does it handle?)
- **AI as explainer, not doer**: Ask AI "explain this function to me like I know basic Python" — students who use AI to understand code later rely LESS on AI to write code (Jarvie-Eggart et al., 2025)
- **Draw it**: Hand-drawn diagrams of call relationships expose gaps that reading alone hides

## Three Dimensions of Code Comprehension

(Van Dijk, Dev2Next 2024)

| Dimension | What to Look For | Questions to Ask |
|-----------|-----------------|------------------|
| **Structure** | Organization, patterns, dependencies | How is the code organized? What design patterns? |
| **Concepts** | Programming concepts, domain concepts | What abstractions are used? What domain terms appear? |
| **Context** | Purpose, users, constraints | Why was this written? Who uses it? What are the constraints? |

Effective code reading requires all three. Most tutorials only teach Structure. Open source projects naturally expose all three.

## Integration with Knowledge Management

Code reading produces raw observations. Converting those into durable knowledge requires active synthesis:

1. **During reading**: Write self-explanations as Markdown notes
2. **After reading**: Extract concepts, file to wiki (see [[Developer Knowledge Base Pattern]])
3. **Cross-project**: Compare similar patterns across different codebases (this is where [[渐进摘要-Progressive-Summarization]] Layer 5 lives)
4. **Spaced review**: Revisit notes to reinforce (see [[合意困难-Desirable-Difficulties]] — retrieval practice)

## Practical Rule

> [!tip] The 20-minute rule: If you've been staring at the same code for 20 minutes without being able to explain what it does, your scope is too large. Zoom in. One function. One loop. One conditional. Explain that first.
