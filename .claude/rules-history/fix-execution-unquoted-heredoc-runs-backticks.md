# RULE: An unquoted heredoc runs the backticks in its text — every heredoc carrying prose or code is quoted

## Context

The rule was imported from an earlier project and had no history here. This history starts with its first extension in R-Type (19 October), while measuring the parameters of the functions a pull request changed.

## Mistake

A `python3 - <<'PY'` heredoc carried a replacement text that itself held a `<<'PY' … PY` block. The outer heredoc ended at the inner `PY` line, zsh refused the whole line with `unmatched "`, and the edit was not made. Nothing ran.

## Root cause

The terminator was chosen out of habit (`PY` for every Python heredoc), without checking that the text inside never holds a line that is exactly that word.

## Rule

1. **Every heredoc is quoted by default**: `<<'EOF'`. The body then goes through as is, backticks and `$` included. This matters especially for a commit message or a PR description in Markdown, full of `` `code` ``.
2. **Shell values come in through arguments**, never through interpolation of the body: `python3 - "$file" <<'PY'` then `sys.argv[1]`.
3. **An unquoted heredoc only carries text reread character by character** — never Markdown, code, or prose quoting commands.
4. **A command quoted for the user must not be able to run by accident**: if it is destructive, describe it in words rather than writing it executable in a script.
5. **After an incident of this kind**, measure the effect before going on, say it at the top of the answer, then repair.

### A heredoc inside a heredoc takes another terminator

1. **A heredoc whose body holds another heredoc uses a different terminator for each**: the outer one `<<'EDIT'`, the inner one `<<'PY'`. The outer heredoc ends at the first line that is exactly its word, even inside the text it carries.
2. **Better, keep the inner script in its own file** (in the scratchpad) and call it: nothing then needs nesting.
3. **`unmatched "` or `parse error` from zsh means the whole command line was refused**: nothing ran, nothing was written. Fix it and send it again; never read the empty output as a result.

## Example

- ❌ **Before (wrong)**: `gh pr create --body "$(cat <<EOF … run \`cmake --build build --target format\` … EOF)"` → the shell runs the quoted command and the description loses the text.
- ✅ **After (right)**: `gh pr create --body-file /tmp/pr-body.md`, the file written without going through the shell, or `<<'EOF'`.
- ❌ **Before (wrong)**: `python3 - script.sh <<'PY'` whose replacement text holds a `<<'PY' … PY` block → the outer heredoc ends at the inner `PY`, zsh answers `unmatched "`, the edit is not made.
- ✅ **After (right)**: the parser written to `scratchpad/flagged_parameters.py`, and the edit sent with `<<'EDIT'`.
