# Design: C99 Template for Cursor Compliance and Code Generation

## 1. Goals

- **Compliance:** Language conformance (C99) and rule conformance (10rules, JPL-C); structured output.
- **Code generation:** Fix non-compliant source; each change cited (C99 clause or rule_id); re-run phase to verify.
- **Portable:** One folder, one rule; default layout (src/, src/includes/, bin/, reports/); no build lock-in.

---

## 2. Architecture

- **Procedure:** `docs/C99_AI_GUIDE.md`; rule enforces phase order, outputs, citations.
- **Phases:** A = Core C99 (ISO C99, nt1256). B = Code Compliance (10rules, jpl-c-1.0). B runs after A passes.
- **Agent-executable:** INPUT, ACTION, OUTPUT, NEXT_STEP per step; no user handoff.

**Flow:** src/ + refs/ → guide + rule → Phase A → Phase B → reports/, bin/, remediation_log.

---

## 3. Components

| Component | Role |
| --------- | ----- |
| **docs/C99_AI_GUIDE.md** | Procedure: step index, A.1–A.4, B.1–B.4, schemas, default layout. AI-oriented (INPUT/OUTPUT/NEXT). |
| **docs/INLINE_TRACEABILITY_GUIDE.md** | Inline citation format and placement for generated C code; reference + rationale at key points. |
| **.cursor/rules/c99-compliance-codegen.mdc** | Rule: when to apply; MUST follow guide, Phase A then B; schema outputs; remediation_log with clause/rule_id. |
| **refs/** | PDFs: Phase A (ISO C99, nt1256), Phase B (10rules, jpl-c-1.0). Add/replace and update guide list. |
| **README.md** | Use this template, layout, contents, 0BSD. |
| **USAGE.md** | Setup, compliance, fixes. |
| **LICENSE** | 0BSD. |

---

## 4. Data flow

- **Compliance:** src/ (+ src/includes/) → A.2 → language_report → A.3 → language_result; B.2 → compliance_report → B.3 → compliance_result. Output: reports/ (JSON) + run manifest.
- **Code gen:** A.3 fail → A.4 (edits + remediation_log) → re-run A.2. B.3 fail → B.4 → re-run B.2. Success when A.3 and B.3 pass.

---

## 5. Portability

- **Layout:** Default src/, src/includes/, bin/, reports/; agent may use project layout.
- **Deploy:** Use as GitHub template or copy folder + rule; guide path from workspace root (e.g. `docs/C99_AI_GUIDE.md` when repo is project; `C_STANDARDS/docs/C99_AI_GUIDE.md` when standards folder in repo). No env or config.
- **Refs:** Add/replace PDFs in refs/; update REFERENCE DOCUMENTS in guide.
- **Tools:** Guide specifies -std=c99, include path, bin/, schemas—no specific compiler.

---

## 6. Design decisions

- **Two phases:** Language (C99) vs rules (10rules, JPL-C); clause vs rule_id.
- **AI-oriented:** Step index, INPUT/ACTION/OUTPUT/NEXT, JSON schemas.
- **Rule required:** One procedure and outputs for compliance and code gen.
- **Default layout:** Single convention; override per project.
- **0BSD:** Reuse and GitHub "Use this template" with no license friction.
