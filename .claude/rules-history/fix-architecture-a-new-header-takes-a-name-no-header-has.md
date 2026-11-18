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

## Update (2026-11-07) — a type name shared within a namespace

**Context.** The global leaderboard of the master added the messages of its two routes in `src/network/Master/Messages/`, one header per struct, among them `LeaderboardRow.hpp`.

**Mistake.** `git ls-files -- '*/LeaderboardRow.hpp'` printed nothing, so the name looked free. The struct `rtype::network::LeaderboardRow` already existed inside `src/network/Protocol/Messages/LeaderboardAnswer/LeaderboardAnswer.hpp`, with other members (a `uint32_t` rank, an `int64_t` date, no server). `r-type_client` and `r-type_server` link both `rtype_network_leaderboard_answer` and the master validators, so the programs held two classes of one name: a violation of the one-definition rule, which the build, the tests and the local clang-tidy all let through. A reuse review found it by reading the links.

**Root cause.** The rule searched file names, and a type is not always alone in a header of its name: a message often declares its row next to it.

**Rule added.** Search the type name itself in `src/` and `tests/`, and the name of a free function at namespace scope, before declaring them; choose a name that says what sets the new type apart.

**Revised the same day, after a review.** The first commands used `-P` (PCRE, missing from some builds of git), missed a type written with an attribute (`struct [[nodiscard]] Result`), left out untracked files, and searched free functions in headers only. The search is now portable, includes untracked files, reads attributes, says which lines are a clash (a forward declaration of the same type is not), and covers sources for free functions.

## Update (2026-11-18) — the searches run on the whole branch before the push

**Context.** A branch turned the background of the client into data. Its layers of particles keep their particles in a struct of `src/client/Rendering/Background/Layers/`, added with the classes that move and paint them.

**Mistake.** That struct was declared `rtype::client::Particle`, in `Particle.hpp`. `src/client/Rendering/Effects/Ground/Particle.hpp` already declared a `rtype::client::Particle`, the component of the ground effects, with other members and another size. Both searches of this rule would have printed it; neither was run on that name. Every test passed, and the client was started for a few seconds on its title, where no effect is painted. Built in Release and played, the client stopped on a segmentation fault 2.6 s into a game: `colorAt()`, called by `paintGroundEffects()`, read the colors of a particle of an effect as a null pointer and a size of 171 209 680. `nm` on the client showed one `ComponentStorage<rtype::client::Particle>` and one `std::vector<rtype::client::Particle>::emplace_back` for the two structs.

**Root cause.** The searches were steps to take when a header or a type is written, with nothing that runs them again once the branch is whole. No test could stand in for them: no test executable links the layers of a background and the effects of a game together, so only the program held both definitions.

**Rule added.** Before the first push of a branch, and before each push that adds a header or a type, both searches run on every header name and every type name the branch adds. The rule says why the suite cannot show a clash, and what a clash looks like when it crashes.

**Fix.** The particle of a layer is `LayerParticle`, in `LayerParticle.hpp`; `nm` then shows one `emplace_back` for each type.
