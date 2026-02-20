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

## Phase A: Core C99 programming language

**Scope:** Valid C99 per ISO C99 and nt1256 only. Do not apply 10rules or jpl-c-1.0 in this phase.

### A.1 LOAD_LANG_REF

- **INPUT:** Path to `refs/ISO_c_1999_definition.pdf`, path to `refs/nt1256.pdf`.
- **ACTION:** Load and parse (or index) language definitions for later steps. If tooling already embeds C99, treat as ready.
- **OUTPUT:** Internal language reference or "OK".
- **NEXT_STEP:** A.2.

### A.2 COMPILE_C99

- **INPUT:** Source file(s) under `src/`, include path `src/includes`, compiler config `-std=c99`. Default: compiled output to `bin/`.
- **ACTION:** Compile; capture all diagnostics (errors, warnings).
- **OUTPUT:** `language_report` per schema below. Write to `reports/<run_id>_language.json`.
- **NEXT_STEP:** A.3.

### A.3 EVALUATE_LANGUAGE

- **INPUT:** `language_report`.
- **ACTION:** Classify each finding as error or warning; set pass = (error_count == 0).
- **OUTPUT:** `language_result` per schema below.
- **DECISION:** IF pass THEN NEXT_STEP = B.1. ELSE NEXT_STEP = A.4.

### A.4 GENERATE_LANGUAGE_FIX

- **INPUT:** Source (default: under `src/`, headers under `src/includes/`), `language_result` (failed findings).
- **ACTION:** Generate edits so code conforms to C99. Cite ISO C99 or nt1256 clause for each edit.
- **OUTPUT:** Patched source (or patch file) + `remediation_log` entries (file, finding_id, reference as C99_clause, change_description).
- **NEXT_STEP:** A.2 (re-run compile).

---

## Phase B: Code Compliance

**Scope:** Rules from 10rules.pdf and jpl-c-1.0.pdf only. Run only after Phase A passes (A.3 pass).

### B.1 LOAD_RULES

- **INPUT:** Paths to `refs/10rules.pdf`, `refs/jpl-c-1.0.pdf`.
- **ACTION:** Load/index rules by rule_id and short description.
- **OUTPUT:** Rule index: list of { rule_id, source_doc, description }.
- **NEXT_STEP:** B.2.

### B.2 RUN_COMPLIANCE_CHECK

- **INPUT:** Source under `src/` (and headers under `src/includes/` if checked), rule index.
- **ACTION:** For each rule check the source. Record each violation with file, line, rule_id, message, severity.
- **OUTPUT:** `compliance_report` per schema below. Write to `reports/<run_id>_compliance.json`.
- **NEXT_STEP:** B.3.

### B.3 EVALUATE_COMPLIANCE

- **INPUT:** `compliance_report`.
- **ACTION:** Compute pass (e.g. no unresolved high/medium violations). Set pass boolean. Set counts by severity.
- **OUTPUT:** `compliance_result` per schema below.
- **DECISION:** IF pass THEN end (success). ELSE NEXT_STEP = B.4.

### B.4 GENERATE_COMPLIANCE_FIX

- **INPUT:** Source (default: under `src/`, headers under `src/includes/`), `compliance_report` (violations), rule index.
- **ACTION:** Generate edits per violation. Cite rule_id for each edit.
- **OUTPUT:** Patched source (or patch) + `remediation_log` (file, finding_id, reference as rule_id, change_description).
- **NEXT_STEP:** B.2 (re-run compliance check).

---

## Output schemas

MUST conform; use exact field names.

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
      "c99_clause": "string or null"
    }
  ]
}
```

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
      "severity": "high | medium | low"
    }
  ]
}
```

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
    "patch_or_snippet": "string or null"
  }
]
```

- **reference:** C99 clause (Phase A) or rule_id (Phase B).

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
[{"file": "src/foo.c", "finding_id": "L1", "reference": "C99 6.2.1", "change_description": "Declared identifier at block scope.", "patch_or_snippet": null}]
```
