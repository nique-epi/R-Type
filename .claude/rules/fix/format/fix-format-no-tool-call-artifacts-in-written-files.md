---
description: Content written to a file never contains fragments of tool-call syntax; a large file is checked at the end of writing
trigger: always_on
---

# RULE: Never leave a tool-call artifact in the CONTENT written to a file

## Rule

1. **The content passed to the write tool never contains fragments of the tool-call syntax** (invocation closing tag, parameter tag, truncated call JSON). These are artifacts of the tool mechanics, never content.
2. **The content is complete and consistent from the first to the last character**: every opened code block is closed, every opened tag has its closing tag.
3. **For a long file**, write a complete and valid base, then extend it with targeted edits — never a partial write that leaves a block open.
4. **After writing large content, check the end of the file** (last lines, `grep` for an invocation tag name) before moving on.

## Example

- ❌ **Before (wrong)**: a rule `.md` that stops in the middle of a never-closed ```` ```cpp ```` block, followed by a fragment of a tool-call tag.
- ✅ **After (right)**: complete content, closed blocks, then `tail -5` of the file to check it.
