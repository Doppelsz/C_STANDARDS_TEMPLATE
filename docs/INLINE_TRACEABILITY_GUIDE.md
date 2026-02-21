# Inline Traceability Guide — C99 and Rule Citations in Source

**Purpose:** Define when and how to add inline comments in generated C code that cite the applicable C99 clause or rule_id and a short rationale (why that constraint is followed). This gives traceability in the source itself, not only in reports and remediation_log.

**AUDIENCE: AI agent** (and humans reviewing generated code).

**Relationship:** This guide supplements [C99_AI_GUIDE.md](C99_AI_GUIDE.md) “When generating new C code”. It does not replace the remediation_log for A.4/B.4 fixes; inline comments are additive.

---

## 1. Purpose and scope

- **When this guide applies:**
  - All **new** C code generation (new file or new function) under the procedure in [C99_AI_GUIDE.md](C99_AI_GUIDE.md).
  - Optionally during A.4/B.4 fixes: the remediation_log is still required for each fix; inline comments may be added at the same time for clarity at the changed or surrounding code.
- **What it does not replace:** Remediation_log entries remain required for every fix in A.4 and B.4. Inline comments do not substitute for remediation_log; they provide in-source traceability for audits and future changes.

---

## 2. Comment format (required shape)

- **Single-line form:**  
  `/* <reference>: <short rationale>. */`
  - **Reference:** C99 clause (e.g. `C99 6.2.1`) or rule_id (e.g. `JPL-C 1.2`, `10rules R3`).
  - **Rationale:** One short phrase or sentence stating **why** this construct or style is used (e.g. “declarations at block scope before use”, “async-signal-safe type for handler”, “unused parameter cast”).
- **Multi-line:** If needed, use a block comment with a second line, e.g. `/* Why: ... */` or a second sentence in the same block. Keep comments concise.

---

## 3. Placement rules — where to add comments

- **File header:** One block comment at the top of each generated `.c` or `.h` file that:
  - Cites overall constraints (e.g. C99, target rule set, Unix-only).
  - Optionally points to this guide (e.g. “Inline citations per INLINE_TRACEABILITY_GUIDE.”).

- **Key constructs:** Add at least one citation comment at or near:
  - **Start of each function:** Main C99/rule choices for that function (e.g. block-scope declarations, brace style).
  - **Non-obvious language or rule-driven choices:** e.g. `volatile sig_atomic_t` (C99/signal safety), `(void)param` (unused parameter), brace style, file-scope vs block-scope, use of POSIX vs C99-only.

- **Avoid:** Do not require a comment on every line. Focus on “key points” where the reference and rationale aid audits and future fixes.

---

## 4. Examples

**File header:**

```c
/*
 * Module name. C99, POSIX. Target: Unix-like systems only.
 * Conforms to C99 (ISO C99, nt1256) and project rule set.
 * Inline citations per INLINE_TRACEABILITY_GUIDE.
 */
```

**Function-level (block-scope declarations, brace style):**

```c
/* C99 6.2.1: declarations at block scope before statements. JPL-C: brace style. */
static void *handle_connection(void *arg)
{
    int client_fd;
    int *pfd;
    /* ... */
}
```

**Inline at a specific construct:**

```c
/* C99 7.14.1.1: sig_atomic_t and volatile for async-signal-safe use in handler. */
static volatile sig_atomic_t g_running = 1;

static void on_signal(int signum)
{
    /* C99 / style: mark unused parameter to satisfy -Wunused-parameter. */
    (void)signum;
    g_running = 0;
}
```

---

## 5. Integration

- The agent **MUST** follow this guide when generating new C code under the procedure in [C99_AI_GUIDE.md](C99_AI_GUIDE.md).
- Remediation_log remains required for A.4 and B.4 fixes; inline comments are additive and may be added during fixes as well.
