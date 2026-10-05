# History: do not over-engineer, step back

The rule was imported when R-Type started; its original incident belongs to another repository.

## Update (2026-10-05): every new library or binary names its constraint

### Context
The part 2 architecture (several game servers, a master service, a web client) was turned into a precise inventory of CMake libraries and binaries.

### Mistake
The inventory proposed eleven libraries and five binaries for a team of four: one library per role (serialization, master API, tickets, metrics, client network part, client view, server and master aggregates), a separate bot binary and a fuzzing binary. The user found it was too much and asked how other games do it.

### Root cause
Each library was justified by a role ("the bot needs the client without SFML", "tests need the server code") instead of a constraint no folder could meet. No reference project was checked before proposing the structure; ioq3, Teeworlds and the Source SDK all use shared code, a client and a server, and Teeworlds hosts its master server on the same shared library.

### Correction
Three libraries that already exist (engine, game, network) and three programs (client with a bot option, server, master).

## Update (2026-10-05): an optional feature never sits on the base path

### Mistake
In the same architecture, guests needed a ticket from the master to join a server, and the admin web page needed an admin account: accounts sat on the path of a plain game. The load-test client was first a separate binary, then an option of the player's client. The user stated that a player must be able to play without an account, that accounts are an add-on, and that the bot is instrumentation.

### Root cause
The base path (what a player needs to play) was never written down, so add-on issues ended up as blockers of base issues.

### Correction
Guests join with a nickname, no ticket; tickets come with the accounts add-on; the admin authenticates with a secret given at the master's launch; the load-test client is a test tool.

## Rule
See `.claude/rules/fix/process/fix-process-do-not-over-engineer-step-back.md`, section "Every new library or binary names the constraint that forces it".
