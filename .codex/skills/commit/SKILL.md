---
name: commit
description: Prepare and create high-quality git commits for JzRE. Use when the user asks to commit changes, draft commit messages, split commits, or verify commit readiness.
metadata:
  short-description: Create disciplined commits for JzRE
---

# Commit Skill (JzRE)

Create small, reviewable, and reversible commits using Conventional Commits and JzRE project rules.

## When To Use

Use this skill when the user asks to:
- commit current changes
- write or refine a commit message
- split changes into multiple commits
- review commit readiness before pushing

## Repository Rules

- Never commit directly on `main` or `master`.
- Prefer branch names with `codex/` prefix (for example: `codex/fix-shader-cache`).
- Keep one logical change per commit.
- Do not include unrelated formatting or refactors in the same commit.
- For engine architecture or module changes, ensure docs under `docs/architecture/` are updated in the same commit.

## Workflow

1. Inspect workspace state.
```bash
git branch --show-current
git status --short
git diff --name-only
git diff --cached --name-only
```

2. Validate branch safety.
- If branch is `main` or `master`, create a new branch before committing:
```bash
git checkout -b codex/<short-description>
```

3. Review changes before staging.
- Read diffs and group files by one logical intent.
- If multiple intents exist, split into separate commits.

4. Run relevant checks before commit.
- Prefer targeted checks first (changed module tests).
- For broad/runtime changes, at least run:
```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

5. Stage precisely.
```bash
git add <files>
git status --short
git diff --cached
```

6. Write commit message using format below.

7. Commit non-interactively.
```bash
git commit -m "<header>" -m "<body>" -m "<footer>"
```
- If body/footer is unnecessary, use a single `-m "<header>"`.

8. Report outcome.
- Provide commit hash, header, staged files, and checks run.

## Commit Message Format

```text
<type>(<scope>): <subject>

<body>

<footer>
```

Header is required. Scope is optional. Keep lines under 100 chars.

## Allowed Types

- `feat`: new feature
- `fix`: bug fix
- `refactor`: code restructure without behavior change
- `perf`: performance improvement
- `docs`: documentation only
- `test`: tests only
- `build`: build system, dependencies, toolchain
- `ci`: CI workflow/config
- `chore`: maintenance tasks
- `style`: formatting only
- `revert`: revert a previous commit

## Subject Rules

- Use imperative present tense (for example: `Add`, `Fix`, `Refactor`)
- Capitalize first letter
- No trailing period
- Target 70 chars or fewer

## Body Guidelines

- Explain what changed and why
- Mention behavior impact and risk when relevant
- Avoid low-level implementation noise

## Footer Guidelines

- Link issues when available:
  - `Fixes #123`
  - `Refs #123`
  - `Fixes GH-123`
- Breaking changes:
  - `BREAKING CHANGE: <what changed and migration hint>`

Only include AI attribution if the project or user explicitly requires it.

## Commit Readiness Checklist

- [ ] Not on `main`/`master`
- [ ] Commit is single-purpose
- [ ] Relevant tests/build checks passed (or explicitly skipped with reason)
- [ ] Architecture docs updated when code change requires it
- [ ] Commit message matches format and intent

## Examples

```text
fix(resource): Handle null shader include dependency

Prevent shader asset load from crashing when an optional include file
is missing. Return a structured error and keep the asset in failed state.

Fixes #142
```

```text
docs(architecture): Update resource loading flow for shader cache

Document cache hit/miss behavior and fallback path in asset system docs.
```
