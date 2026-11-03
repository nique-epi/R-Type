# fix-architecture-a-new-header-takes-a-name-no-header-has

## Context

The plan of the guaranteed messages (protocol version 2) moved the struct "a message ready to send" from the server to the network library: `src/network/Protocol/EncodedMessage.hpp`, `{type, payload}`, added by the first commit; the server's `src/server/Replication/EncodedMessage.hpp`, `{type, payload, copies}`, was to be replaced by `EventMessage.hpp` in the third commit.

## Mistake

The new header was written into the worktree while a build of `main` ran in it. The server targets reach both `src/network/Protocol` and `src/server/Replication`; `#include "EncodedMessage.hpp"` in `GameEventEncoder.hpp` took the network header, and the build failed with `unknown type name 'EncodedMessage'; did you mean 'network::EncodedMessage'?` and `no member named 'copies' in 'rtype::network::EncodedMessage'`. The same failure was in the plan itself: its first commit, built alone, could not compile the server.

## Root cause

Every include of the repository is a bare file name found through the include folders the targets expose, and nothing checked that the new name was free. The plan placed the file by its role ("a struct of data next to `MessageFrame.hpp`") and never looked for another header of that name.

## Rule

1. Before creating a header, look for its file name in the whole repository: `git ls-files -- '*/<Name>.hpp'` must print nothing.
2. When the name is taken, choose another one, or rename the existing header first, in a commit of its own before the commit that adds the new one.
3. A plan writes, next to each new header of its tree, that the name is free, with the command it ran.

Calibration: on `origin/main` at `edf3388`, `git ls-files -- '*/EncodedMessage.hpp'` prints `src/server/Replication/EncodedMessage.hpp`.

## Example

- ❌ Before (wrong): `src/network/Protocol/EncodedMessage.hpp` created in the first commit, the server's `EncodedMessage.hpp` removed only in the third → the server and its tests stop building from the first commit on.
- ✅ After (right): a `refactor` commit renames the server's struct `EventMessage` in `EventMessage.hpp` first; `git ls-files -- '*/EncodedMessage.hpp'` prints nothing; then the network `EncodedMessage.hpp` is added.
