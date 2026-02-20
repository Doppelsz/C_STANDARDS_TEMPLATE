# C_STANDARDS_TEMPLATE

A minimal, portable template for **C99 compliance checking** and **compliant code generation** with an AI agent in Cursor. Use it as a GitHub template or copy the folder into your C project.

## Features

- **Two-phase workflow:** Phase A (Core C99 per ISO C99 / nt1256) → Phase B (code rules: 10rules, JPL-C).
- **Structured outputs:** JSON reports and remediation logs with C99 clause or rule citations.
- **Agent-executable:** Step-by-step procedure in `docs/C99_AI_GUIDE.md`; one Cursor rule enforces phases and schemas.
- **Portable:** Default layout (`src/`, `bin/`, `reports/`, `refs/`); no build lock-in.

## Repository structure

| Path | Purpose |
|------|---------|
| `docs/C99_AI_GUIDE.md` | AI procedure: Phase A (A.1–A.4), Phase B (B.1–B.4), schemas, layout. |
| `docs/DESIGN.md` | Design goals, architecture, data flow. |
| `.cursor/rules/c99-compliance-codegen.mdc` | Cursor rule: when to apply; MUST follow guide and schema outputs. |
| `src/` | C source. |
| `src/includes/` | Headers (`-I src/includes`). |
| `bin/` | Compiled output. |
| `reports/` | `reports/<run_id>_language.json`, `reports/<run_id>_compliance.json`. |
| `refs/` | Reference PDFs (see guide; add your own). |

## Quick start

1. **Use this template:** Click **Use this template** on GitHub, or clone the repo.
2. **Copy the rule:** Copy `.cursor/rules/c99-compliance-codegen.mdc` into your project’s `.cursor/rules/`.
3. **Set guide path:** In the rule, set the procedure path from workspace root:
   - This repo as project → `docs/C99_AI_GUIDE.md`
   - Standards folder inside repo (e.g. `C_STANDARDS/`) → `C_STANDARDS/docs/C99_AI_GUIDE.md`
4. **Add reference PDFs** in `refs/` (see `docs/C99_AI_GUIDE.md` for required refs).
5. **Put C code** in `src/`, headers in `src/includes/`.

See **[USAGE.md](USAGE.md)** for setup, compliance runs, and fixes.

## Compliance and fixes

- **Check:** Open the project in Cursor and ask for a C99 compliance check. Follow Phase A then Phase B; reports go to `reports/`.
- **Fixes:** Ask the agent to fix reported issues; it will edit, log, and re-run the phase.

## License

Zero-Clause BSD (0BSD). See [LICENSE](LICENSE).
