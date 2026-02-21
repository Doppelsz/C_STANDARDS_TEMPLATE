# Code Review — Guide Path and Response Format

**Purpose:** Single entry point for C code review: procedure path (which steps, in what order) and how the agent responds to the user (structure, tone).

**AUDIENCE: AI agent.** When performing a code review, follow this guide then [C99_AI_GUIDE.md](C99_AI_GUIDE.md) for step details. Use the response format below for the reply to the user.

**Path from workspace root:** `docs/CODE_REVIEW_GUIDE.md`. If standards folder in repo: `C_STANDARDS/docs/CODE_REVIEW_GUIDE.md`.

---

## 1. Guide path (procedure)

1. Follow [docs/C99_AI_GUIDE.md](C99_AI_GUIDE.md): run **A.2** (COMPILE_C99) → **A.3** (EVALUATE_LANGUAGE). Write `reports/<run_id>_language.json`.
2. If A.3 pass: run **B.2** (RUN_COMPLIANCE_CHECK) → **B.3** (EVALUATE_COMPLIANCE). Write `reports/<run_id>_compliance.json`.
3. Use one `run_id` per run (e.g. ISO timestamp or UUID). Do not run Phase B before Phase A passes.

Schema fields, report filenames, and phase rules are defined in C99_AI_GUIDE; do not omit or alter them.

---

## 2. Review output format (agent response to user)

The reply to the user **MUST** use this structure and tone.

**Tone:** Professional, programmer-to-programmer. Concise. No filler, no marketing language.

**Required structure:**

- **Summary** — One sentence: scope (e.g. "Code review: `src/`") and result (pass/fail).
- **Verdict** — Pass or Fail. If Fail: state phase (A.3 or B.3) and counts (e.g. error_count, or high/medium/low).
- **Findings** — If any: short list (bullets or table). Per finding: file, line, reference (C99 clause or rule_id), message. One line per finding; avoid long prose.
- **Reports** — Paths to `reports/<run_id>_language.json` and, if run, `reports/<run_id>_compliance.json`.
- **Next steps** — Only if Fail: e.g. "Run A.4 (language) or B.4 (compliance) to generate fixes; re-run A.2/B.2 to verify."

**MUST NOT:** Long intro paragraphs; repeated disclaimers; suggesting merge when fail; verbose rationale in the narrative (rationale stays in JSON).
