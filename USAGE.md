# How to use this template

1. Put C source in `src/`, headers in `src/includes/`.
2. Copy `.cursor/rules/c99-compliance-codegen.mdc` to your project’s `.cursor/rules/`.
3. Put reference PDFs in `refs/` (see `docs/C99_AI_GUIDE.md`).

**Compliance:** Open the project in Cursor; ask for a C99 compliance check. Follow Phase A then Phase B in the guide. Reports → `reports/`.

**Fixes:** Ask the agent to fix reported issues; it will edit, log, and re-run the phase.

**Guide path in rule:** From workspace root. If this repo is your project: `docs/C99_AI_GUIDE.md`. If the standards folder lives in the repo (e.g. `C_STANDARDS`): `C_STANDARDS/docs/C99_AI_GUIDE.md`. Update if you rename the folder.
