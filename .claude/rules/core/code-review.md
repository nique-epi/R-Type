---
description: Code review — how to launch it, severities, C++/network criteria, anti-false-positive rules, report format
trigger: always_on
---

# RULE: Code review — criteria, severities and launch procedure

## Goal

When the user asks for a **review** ("run a review", "review this PR"), the agent applies **this rule in full**. It defines what we look for, how we rank it, what we are allowed to report, and how the review is delegated.

A review that does not follow this rule is not a review: it is a read-through.

## 1. Launch procedure

1. **Delegate to a subagent**, never review in the main context: the review must be done by a reader who did not write the code and does not carry the author's intent bias.
2. **The agent's brief explicitly contains**:
   - the **exact diff** (`git diff origin/main...HEAD` or the list of files);
   - the **functional context** (what the PR is meant to do, and why);
   - the **path of this rule** + the `.claude/rules/` relevant to the diff;
   - the **gates already passed** (build, tests, `format-check`, `tidy`) with their real output;
   - the order to **verify every finding in the code before reporting it**.
3. **Never hand the agent an invariant "to assert" that you have not measured yourself** (see `fix-reasoning-delegated-invariant-must-be-computed.md`). Ask it to derive and measure, not to confirm a hypothesis.
4. **Several agents in parallel** if the diff spans disjoint areas (e.g. one for the network protocol, one for the ECS, one for build/CI). One agent for a coherent diff.
5. The report comes back to the user **with the findings, not with a summary of the diff**, separating what the agent verified from what it assumes (see `fix-process-relay-agent-report-separating-verified-facts.md`).

## 2. Severities

Every finding carries **exactly one** severity:

| Severity | Definition | Examples |
|---|---|---|
| **Critical** | Crash, undefined behavior, state corruption, remotely exploitable flaw | out-of-bounds read on a received packet, use-after-free, data race, server going down on a malformed packet |
| **Warning** | Bug, performance regression, structural anti-pattern, violation of a `.claude/rules/` | off-by-one, per-frame allocation in the game loop, logic in a `.hpp`, raw `throw std::runtime_error` |
| **Info** | Style, readability, minor suggestion | improvable naming, possible simplification |

**Tie-break**: when unsure between two levels, pick **the lower one**. A `Critical` that is not one destroys trust in all the others.

## 3. What we look for — general dimensions

In order of importance:

1. **Design** — is the code in the right place (engine vs game, client vs server, right ECS system)? Do dependencies point the right way (the server never depends on SFML)?
2. **Functionality** — does the code do what the author intended? Edge cases, concurrency, unhandled errors, behavior on Linux **and** Windows.
3. **Complexity** — can it be simpler? **Over-engineering** is a finding: code that solves a speculative future problem instead of the current need.
4. **Tests** — do they exist, and **would they actually fail** if the code broke? Traps to look for (see `fix-process-green-test-locking-a-defect-and-overly-deterministic-fake.md`): assertion copied from observed output, assertion that recognizes a shape instead of a value, test double tamer than the real dependency (network with no loss or reordering, frozen clock).
5. **Naming** — do names say what the thing is? (see `code-style-full-names-no-abbreviations.md`, `fix-format-name-after-data-not-process.md`)
6. **Comments** — compliant with `code-style-comments-english-outside-bodies-no-tickets.md`.
7. **Style / consistency** — compliant with the `code-style/` rules and neighboring conventions.
8. **Documentation** — README and repository docs updated if behavior, build or protocol changes.
9. **Every line** — the diff is read entirely. A line you do not understand is said, not skimmed.
10. **Context** — read around the diff. Does the change improve or degrade overall code health?
11. **Reuse** — does the diff invent a helper, type or component that already exists? A concept that gets a second shape is a finding.
12. **Cost per frame / per tick** — for any code in the game loop or network handling: allocations, copies, linear searches over the number of entities or clients.

## 4. What we look for — security and robustness (always)

The server receives data from **untrusted** clients over the network.

- **Network input**: every size, index or identifier read from a packet is bounded **before** use. A truncated, oversized or unknown-type packet is rejected without crashing.
- **Memory**: no out-of-bounds read/write, no pointer or reference outliving its owner, no `reinterpret_cast` on a network buffer without size and alignment checks.
- **Endianness and types**: explicit wire format (fixed sizes, defined byte order), never `sizeof` of an unpacked struct sent as is.
- **Concurrency**: any data shared between the network thread and the game loop is protected or handed over through a dedicated queue.
- **Denial of service**: a client cannot make the server allocate memory proportional to a value it controls.
- **Secrets**: no key, token or private address hard-coded.

## 5. Repository-specific gates

**Violating one of these = `Warning` at least, `Critical` if the impact is a crash or a flaw.**

| Gate | Source |
|---|---|
| No method body in a `.hpp` (except templates, `constexpr`, `= default`/`= delete`) | `architecture-hpp-declarations-cpp-definitions.md` |
| Interfaces: only `= 0` methods + `= default` virtual destructor | `architecture-pure-virtual-interfaces.md` |
| Domain errors as custom per-module exceptions, never raw `throw std::…` | `architecture-custom-exceptions-per-module.md` |
| Naming: no `k` prefix, no abbreviations, trailing `member_`, file = class | `code-style/` |
| Comments in English, outside function bodies, no ticket references | `code-style-comments-english-outside-bodies-no-tickets.md` |
| Magic values as named constants | `fix-architecture-magic-values-as-named-constants.md` |
| Conventional Commits, **no `Co-Authored-By` trailer** or AI mention | `commit.md`, `fix-process-no-co-author-trailer.md` |
| Pre-existing inconsistency touched: fixed or reported, never extended | `fix-process-rework-preexisting-inconsistencies.md` |

### Verification gates

| Gate | Command |
|---|---|
| Build and tests green, zero warnings, real output shown | `cmake --workflow --preset test` |
| Formatting | `cmake --build build --target format-check` |
| Lint (warnings = errors) | `cmake --build build --target tidy` |
| Linux (GCC) **and** Windows (MSVC) | the PR's `build-and-test` CI |

## 6. Reporting rules (anti false positives)

These rules take precedence over exhaustiveness. A noisy report does not get read.

1. **Every finding is verified in the code before being written.** Cite `file:line`. A finding without an exact location is not reported.
2. **Every finding has a concrete failure scenario**: inputs / state → wrong output or crash. If you cannot write it, the finding is not ready — downgrade it to `Info` or drop it.
3. **No finding on code the diff does not touch**, unless the diff worsens it or directly depends on it (context finding, stated as such).
4. **No personal preference disguised as a defect.** A matter of taste is an `Info` prefixed with `Nit:`.
5. **Never claim a gate result you did not run.** "Tests pass" requires the real output and a correctly read exit code (see `fix-execution-zsh-pipestatus.md`).
6. **Explicitly report what could not be verified** rather than implying full coverage.

## 7. Output format

```markdown
## Verdict

<GO | GO-with-changes | NO-GO> — <one sentence>

## Critical (n)

### <short title>

- **Where**: `src/server/Network/PacketReader.cpp:42`
- **Defect**: <one sentence>
- **Failure**: <inputs/state → consequence>
- **Fix**: <the direction, not an essay>

## Warning (n)

<same structure>

## Info (n)

<short list, one line each>

## Not verified

<what could not be, and why>
```

**Verdict**: `NO-GO` only if there is at least one `Critical`. `GO-with-changes` if `Warning`s must be addressed before merge.

## Example

- ❌ **Before (wrong)**: "I read the PR, looks clean, a few style remarks" — no severity, no location, no scenario, review done by the person who wrote the code.
- ✅ **After (right)**: subagent briefed with the diff + this rule + the real gates → `Critical` on `PacketReader.cpp:42` ("`payloadSize` is read from the header then used to copy from the buffer without comparing it to the number of bytes received → a client announcing 65535 bytes in a 12-byte datagram makes the server read out of bounds"), verdict `NO-GO`.
