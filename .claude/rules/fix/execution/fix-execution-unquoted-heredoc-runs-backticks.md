---
description: An unquoted heredoc runs the backticks and $() in its text — every heredoc carrying prose or code is quoted (<<'EOF')
trigger: always_on
---

# RULE: An unquoted heredoc runs the backticks in its text — every heredoc carrying prose or code is quoted

## Rule

1. **Every heredoc is quoted by default**: `<<'EOF'`. The body then goes through as is, backticks and `$` included. This matters especially for a commit message or a PR description in Markdown, full of `` `code` ``.
2. **Shell values come in through arguments**, never through interpolation of the body: `python3 - "$file" <<'PY'` then `sys.argv[1]`.
3. **An unquoted heredoc only carries text reread character by character** — never Markdown, code, or prose quoting commands.
4. **A command quoted for the user must not be able to run by accident**: if it is destructive, describe it in words rather than writing it executable in a script.
5. **After an incident of this kind**, measure the effect before going on, say it at the top of the answer, then repair.

## Example

- ❌ **Before (wrong)**: `gh pr create --body "$(cat <<EOF … run \`cmake --build build --target format\` … EOF)"` → the shell runs the quoted command and the description loses the text.
- ✅ **After (right)**: `gh pr create --body-file /tmp/pr-body.md`, the file written without going through the shell, or `<<'EOF'`.
