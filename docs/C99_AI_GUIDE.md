# C99 Code Compliance and Compliant Code Generation — AI Procedure Guide

**Purpose:** Phase A (C99 language) then Phase B (rules); schema reports and remediation_log.

**AUDIENCE: AI agent.** Execute steps in order. Use schemas for all outputs. Do not skip or merge phases.

**REFERENCE DOCUMENTS (place in refs/):**

- `refs/ISO_c_1999_definition.pdf` — C99 language standard (Phase A)
- `refs/nt1256.pdf` — C99 language reference (Phase A)
- `refs/10rules.pdf` — Compliance rules (Phase B)
- `refs/jpl-c-1.0.pdf` — JPL C coding standard (Phase B)

---

## Template / How to use

1. Use as GitHub template (new repo) or copy this folder (e.g. `C_STANDARDS`) to repo root.
2. Copy `.cursor/rules/c99-compliance-codegen.mdc` to project `.cursor/rules/`.
3. Set rule guide path from workspace root: this repo as project → `docs/C99_AI_GUIDE.md`; standards folder in repo → `C_STANDARDS/docs/C99_AI_GUIDE.md` (update if folder renamed).

PDFs in `refs/`. To use other rule docs, add/replace PDFs and update REFERENCE DOCUMENTS below.

---

## Project layout (default)

Paths relative to workspace root. Override per project.

- **src/** — C source.
- **src/includes/** — Headers (`-I src/includes`).
- **bin/** — Compiled output.
- **reports/** — `reports/<run_id>_language.json`, `reports/<run_id>_compliance.json`. **run_id:** unique per run (e.g. ISO timestamp like `2025-02-19T12-00-00Z` or UUID); use the same value in report filenames and in the `run_id` field of each JSON.

---

## Step index

| STEP_ID | Phase | Action | Output schema |
| ------- | ----- | ------ | ------------- |
| A.1 | Core C99 | LOAD_LANG_REF | (internal/OK) |
| A.2 | Core C99 | COMPILE_C99 | language_report |
| A.3 | Core C99 | EVALUATE_LANGUAGE | language_result |
| A.4 | Core C99 | GENERATE_LANGUAGE_FIX | remediation_log |
| B.1 | Code Compliance | LOAD_RULES | rule index |
| B.2 | Code Compliance | RUN_COMPLIANCE_CHECK | compliance_report |
| B.3 | Code Compliance | EVALUATE_COMPLIANCE | compliance_result |
| B.4 | Code Compliance | GENERATE_COMPLIANCE_FIX | remediation_log |

---

## When generating new C code

This project targets **Unix-like systems only** (e.g. Linux, macOS). Generated code must use POSIX where applicable; never recommend or introduce Windows-specific APIs, conditional compilation for Windows, or Windows build instructions.

The same references used for Phase A (A.1) and Phase B (B.1) are the **single source of truth** for both checking and writing. When generating **new** C code (new file or new function), the agent **MUST** apply them as constraints so that the code conforms to C99 and to the rule index from B.1 (once Phase A passes). Do not rely on C11 or later; do not use constructs or style that violate the loaded rule set.

This reduces violations at A.2/B.2 and minimizes A.4/B.4 fix cycles. The agent does not modify the guide or the Cursor rule during generation; it only reads them.

### POSIX visibility (glibc/Linux)

- **When it applies:** Building with `-std=c99` on Linux/glibc. Any use of POSIX-only APIs that are not part of the C99 standard (e.g. `sigaction`, `sigemptyset`). Note: `pthread_*` from `<pthread.h>` is usually fine as it is a POSIX header; the issue is standard headers like `<signal.h>` that have both C99 and POSIX parts.
- **Rule:** Before including any header that provides POSIX-only symbols, ensure the translation unit requests POSIX. The standard way on glibc is to define a feature-test macro **before** any `#include` (e.g. at the very top of the file, or via compiler: `-D_POSIX_C_SOURCE=200809L`).
- **In source:** At the top of the file, first line(s), e.g. `#define _POSIX_C_SOURCE 200809L` then includes. Alternatively the build may define it (e.g. in `CFLAGS`); if so, generated code need not repeat it.
- **Common POSIX-only uses in standard headers:** `<signal.h>` → `struct sigaction`, `sigaction()`, `sigemptyset()`, `sigaddset()`; other headers may have similar C99 vs POSIX subsets on glibc.
- **Reference:** `man 7 feature_test_macros` (glibc); POSIX.1-2008.

### Generation constraints (checklist)

A fixed list the agent consults when writing C (derived from A.1 and B.1; update only when refs change):

- **C99 only:** Use only ISO C99 and nt1256; no C11+ language or library; ensure `-std=c99` compatibility.
- **Phase B rules:** When the rule index from B.1 is available, apply it while generating (e.g. naming, brace style, no disallowed file-scope objects, comment/style per rule_id). Same rule_ids as used in B.2/B.3.
- **Unix-like only; never recommend Windows:** Target POSIX/Unix-like systems only. Do not introduce or recommend `_WIN32`/`_WIN64`, Winsock, or Windows-only headers/libraries in review or generated code.
- **POSIX visibility on glibc:** When generating code that uses POSIX-only APIs from standard headers (e.g. `sigaction`/`sigemptyset` from `<signal.h>`), ensure the translation unit requests POSIX as per the "POSIX visibility (glibc/Linux)" section above (e.g. `#define _POSIX_C_SOURCE 200809L` at the very top of the file before any `#include`, or document that the build defines it). Generated code must compile with the project's existing flags (e.g. `-std=c99` without relying on default glibc extensions).
- **Inline citations:** Add comments per [docs/INLINE_TRACEABILITY_GUIDE.md](docs/INLINE_TRACEABILITY_GUIDE.md) (reference + rationale at key points).

Generated code **MUST** include inline traceability comments as defined in [docs/INLINE_TRACEABILITY_GUIDE.md](docs/INLINE_TRACEABILITY_GUIDE.md): at file header and at key constructs (e.g. function start, non-obvious C99 or rule-driven choices), cite the applicable C99 clause or rule_id and a short rationale. The agent follows that guide when generating new C code.

---

## Phase A: Core C99 programming language

**Scope:** Valid C99 per ISO C99 and nt1256 only. Do not apply 10rules or jpl-c-1.0 in this phase.

### A.1 LOAD_LANG_REF

- **INPUT:** Path to `refs/ISO_c_1999_definition.pdf`, path to `refs/nt1256.pdf`.
- **ACTION:** Load and parse (or index) language definitions for later steps. If tooling already embeds C99, treat as ready.
- **OUTPUT:** Internal language reference or "OK". Use this language reference when generating new C code (see "When generating new C code") so output conforms to C99.
- **NEXT_STEP:** A.2.

### A.2 COMPILE_C99

- **INPUT:** Source file(s) under `src/`, include path `src/includes`, compiler config `-std=c99`. Default: compiled output to `bin/`.
- **ACTION:** Compile; capture all diagnostics (errors, warnings).
- **OUTPUT:** `language_report` per schema below. Write to `reports/<run_id>_language.json`.
- **NEXT_STEP:** A.3.

### A.3 EVALUATE_LANGUAGE

- **INPUT:** `language_report`.
- **ACTION:** Classify each finding as error or warning; set pass = (error_count == 0). Include a short rationale per finding (why it violates the cited clause).
- **OUTPUT:** `language_result` per schema below.
- **DECISION:** IF pass THEN NEXT_STEP = B.1. ELSE NEXT_STEP = A.4.

### A.4 GENERATE_LANGUAGE_FIX

- **INPUT:** Source (default: under `src/`, headers under `src/includes/`), `language_result` (failed findings).
- **ACTION:** Address **all** current Phase A findings in one batch; then run A.2 once to verify. Do not fix one finding, re-run, then fix the next. Generate edits so code conforms to C99. Cite ISO C99 or nt1256 clause for each edit.
- **OUTPUT:** Patched source (or patch file) + `remediation_log` entries (file, finding_id, reference as C99_clause, change_description, remediation_rationale). Each entry must include remediation_rationale (why the change was required).
- **NEXT_STEP:** A.2 (re-run compile).

---

## Phase B: Code Compliance

**Scope:** Rules from 10rules.pdf and jpl-c-1.0.pdf only. Run only after Phase A passes (A.3 pass).

### B.1 LOAD_RULES

- **INPUT:** Paths to `refs/10rules.pdf`, `refs/jpl-c-1.0.pdf`.
- **ACTION:** Load/index rules by rule_id and short description.
- **OUTPUT:** Rule index: list of { rule_id, source_doc, description }. Use this rule index when generating new C code (see "When generating new C code") so output conforms to the rule set.
- **NEXT_STEP:** B.2.

### B.2 RUN_COMPLIANCE_CHECK

- **INPUT:** Source under `src/` (and headers under `src/includes/` if checked), rule index.
- **ACTION:** For each rule check the source. Record each violation with file, line, rule_id, message, severity.
- **OUTPUT:** `compliance_report` per schema below. Write to `reports/<run_id>_compliance.json`.
- **NEXT_STEP:** B.3.

### B.3 EVALUATE_COMPLIANCE

- **INPUT:** `compliance_report`.
- **ACTION:** Compute pass (e.g. no unresolved high/medium violations). Set pass boolean. Set counts by severity. Include a short rationale per finding (why it violates the rule_id).
- **OUTPUT:** `compliance_result` per schema below.
- **DECISION:** IF pass THEN end (success). ELSE NEXT_STEP = B.4.

### B.4 GENERATE_COMPLIANCE_FIX

- **INPUT:** Source (default: under `src/`, headers under `src/includes/`), `compliance_report` (violations), rule index.
- **ACTION:** Address **all** current Phase B findings in one batch; then run B.2 once to verify. Do not fix one finding, re-run, then fix the next. Generate edits per violation. Cite rule_id for each edit.
- **OUTPUT:** Patched source (or patch) + `remediation_log` (file, finding_id, reference as rule_id, change_description, remediation_rationale). Each entry must include remediation_rationale (why the change was required).
- **NEXT_STEP:** B.2 (re-run compliance check).

---

## Output schemas

MUST conform; use exact field names. **rationale** (in findings) and **remediation_rationale** (in remediation_log) are required for traceability.

### language_report

```json
{
  "run_id": "string",
  "files": ["string"],
  "findings": [
    {
      "file": "string",
      "line": "integer",
      "diagnostic_code": "string",
      "message": "string",
      "severity": "error | warning",
      "c99_clause": "string or null",
      "rationale": "string"
    }
  ]
}
```

- **rationale:** Required for every finding. Brief explanation of why this location violates the cited C99 clause.

### language_result

```json
{
  "pass": true,
  "error_count": 0,
  "warning_count": 0,
  "findings": []
}
```

File naming: `reports/<run_id>_language.json`.

### compliance_report

```json
{
  "project_id": "string",
  "run_id": "string",
  "files": ["string"],
  "findings": [
    {
      "file": "string",
      "line": "integer",
      "rule_id": "string",
      "message": "string",
      "severity": "high | medium | low",
      "rationale": "string"
    }
  ]
}
```

- **rationale:** Required for every finding. Brief explanation of why this location violates the cited rule_id.

### compliance_result

```json
{
  "pass": true,
  "run_id": "string",
  "counts": { "high": 0, "medium": 0, "low": 0 },
  "violation_ids": []
}
```

File naming: `reports/<run_id>_compliance.json`.

### remediation_log

Array (A.4 or B.4):

```json
[
  {
    "file": "string",
    "finding_id": "string",
    "reference": "string",
    "change_description": "string",
    "remediation_rationale": "string",
    "patch_or_snippet": "string or null"
  }
]
```

- **reference:** C99 clause (Phase A) or rule_id (Phase B).
- **change_description:** Required. What was changed.
- **remediation_rationale:** Required. Why this change was required by the reference.

### run_manifest

```json
{
  "run_id": "string",
  "timestamp": "ISO8601",
  "command": "string",
  "tool_versions": {}
}
```

---

## Minimal JSON examples

**language_result (pass):**

```json
{"pass": true, "error_count": 0, "warning_count": 0, "findings": []}
```

**compliance_result (pass):**

```json
{"pass": true, "run_id": "run1", "counts": {"high": 0, "medium": 0, "low": 0}, "violation_ids": []}
```

**remediation_log (one entry):**

```json
[{"file": "src/foo.c", "finding_id": "L1", "reference": "C99 6.2.1", "change_description": "Declared identifier at block scope.", "remediation_rationale": "C99 6.2.1 requires identifiers to be declared before use in block scope; the fix adds the declaration.", "patch_or_snippet": null}]
```

**language_report (one finding with rationale):**

```json
{"run_id": "r1", "files": ["src/foo.c"], "findings": [{"file": "src/foo.c", "line": 10, "diagnostic_code": "error", "message": "undeclared identifier", "severity": "error", "c99_clause": "C99 6.2.1", "rationale": "C99 6.2.1 requires identifiers to be declared before use; the identifier was used at block scope without declaration."}]}
```

**compliance_report (one finding with rationale):**

```json
{"project_id": "p1", "run_id": "r1", "files": ["src/foo.c"], "findings": [{"file": "src/foo.c", "line": 5, "rule_id": "JPL-C 1.2", "message": "Function length exceeds limit", "severity": "high", "rationale": "JPL-C 1.2 limits function length; this function exceeds the allowed line count."}]}
```
