# History: a bounded contract is checked on the empty state and at both bounds

The rule was imported from another project when R-Type started; its original incident belongs there. This file records the updates made on R-Type.

## Update (2026-10-07) — A range written in words copies the computed endpoints

### Context

Version 0 of the network protocol drops a datagram whose 16-bit `sequence` is not newer than the newest one accepted, compared with serial number arithmetic (RFC 1982). The comparison was tested at its bounds before writing: 1 and 32767 ahead are newer, 0 and 32768 are not. The protocol page says "ahead by 1 to 32767".

### Mistake

The rule written from that incident (`fix-reasoning-last-received-is-not-newest-over-udp.md`) said "newer means ahead by 1 to half the range". Half the range of a 16-bit counter is 32768, the very value RFC 1982 leaves undefined and the test treats as not newer. The pull request review flagged it.

### Root cause

The endpoints were computed and tested, then paraphrased in words when the sentence was written a second time, and the paraphrase moved the upper bound by one.

### Rule

When a range goes into prose, write its endpoints as numbers taken from the computed or tested values, name the excluded edge, and reread the sentence against the boundary test.

### Example

- ❌ **Before (wrong)**: "newer means ahead by 1 to half the range".
- ✅ **After (right)**: "newer means ahead by 1 to 32767; exactly half the range (32768) is never newer".
