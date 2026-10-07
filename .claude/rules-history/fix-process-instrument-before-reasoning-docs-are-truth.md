# History: fix-process-instrument-before-reasoning-docs-are-truth

The rule was imported from another project when R-Type started; its original incident belongs there. This file records the updates made on R-Type.

## Update (2026-10-07) — A guarantee taken from a source keeps the source's scope

### Context

Writing version 0 of the network protocol page. The maximum datagram size (1200 bytes of UDP payload) was justified with RFC 8200 (IPv6 minimum MTU of 1280 bytes) and RFC 9000, section 14 (QUIC uses the same figure). Both sections were read in their raw text.

### Mistake

The page concluded: "So IP never fragments a datagram of this protocol, on any path." RFC 9000 itself only says that "modern IPv4 and all IPv6 network paths are expected to be able to support QUIC". An IPv4 path or a tunnel with a smaller MTU can still fragment or drop the datagram. The pull request review flagged the overstatement.

### Root cause

I read the section for its figure and kept the figure, then turned the source's "expected" on a named set of paths into "never" on every path. The qualifier was in the very paragraph I quoted.

### Rule

Copy the qualifiers with the claim; before writing "never", "always" or "on any path", find the sentence of the source that says it, or drop the word and write down the case the source leaves open; reread the cited section for its limits.

### Example

- ❌ **Before (wrong)**: "IP never fragments a datagram of this protocol, on any path."
- ✅ **After (right)**: "QUIC relies on the same figure and expects it to pass on all IPv6 paths and on modern IPv4 paths; a path with a smaller MTU may still fragment or drop a datagram."
