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

### A heredoc inside a heredoc takes another terminator

1. **A heredoc whose body holds another heredoc uses a different terminator for each**: the outer one `<<'EDIT'`, the inner one `<<'PY'`. The outer heredoc ends at the first line that is exactly its word, even inside the text it carries.
2. **Better, keep the inner script in its own file** (in the scratchpad) and call it: nothing then needs nesting.
3. **`unmatched "` or `parse error` from zsh means the whole command line was refused**: nothing ran, nothing was written. Fix it and send it again; never read the empty output as a result.

### A quoted heredoc carries the script's own escapes, never the shell's

1. **In a `<<'PY'` heredoc the body reaches the interpreter byte for byte**: write the escapes of the target language, as in a file. A `\\'` written for the shell arrives as a backslash and ends the Python string.
2. **A string holding the quote that delimits it switches quotes**: `"Nolan's review"` in Python, never `'Nolan\\'s review'`.
3. **A script that carries prose with apostrophes or quotes goes in its own file** (in the scratchpad), written with the write tool, then called: nothing is escaped twice.
4. **`SyntaxError` from such a script means nothing ran**: check the target file is unchanged (`wc -l`, its headings) before writing again.

## Example

- ❌ **Before (wrong)**: `gh pr create --body "$(cat <<EOF … run \`cmake --build build --target format\` … EOF)"` → the shell runs the quoted command and the description loses the text.
- ✅ **After (right)**: `gh pr create --body-file /tmp/pr-body.md`, the file written without going through the shell, or `<<'EOF'`.
- ❌ **Before (wrong)**: `python3 - script.sh <<'PY'` whose replacement text holds a `<<'PY' … PY` block → the outer heredoc ends at the inner `PY`, zsh answers `unmatched "`, the edit is not made.
- ✅ **After (right)**: the parser written to `scratchpad/flagged_parameters.py`, and the edit sent with `<<'EDIT'`.
- ❌ **Before (wrong)**: `python3 - .context/context.md <<'PY'` with `'… Nolan\\'s parameter review …'` → `SyntaxError: invalid syntax`, the context file left as it was.
- ✅ **After (right)**: the script written to `scratchpad/tools/compact_context.py`, the string in double quotes, then `python3 -I scratchpad/tools/compact_context.py .context/context.md`.
