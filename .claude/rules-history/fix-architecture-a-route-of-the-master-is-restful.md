---
description: "History of the rule: every HTTP route of the master is RESTful"
---

# History: every HTTP route of the master is RESTful

## Context

The master of R-Type exposes an HTTP API that the game servers and the web site call. The routes of the registration of game servers were planned and then rewritten twice in one session.

## Mistake

1. The first plan named its routes after actions: `POST /api/game-servers/register`, and `POST /api/game-servers/{id}/heartbeat` with the id of the server in the path, compared to its key.
2. The user asked for the conventions of Yuno backend. The routes became `PUT /api/servers/me` and `PUT /api/servers/me/heartbeat`.
3. A later plan treated the key of a server as a resource of its own, `POST /api/server-keys`.

## Root cause

The conventions lived in the plans and refactors of another repository (the RESTful refactor of the favorite module of Yuno backend, its rule on the shape of a payload) and in the conversation, never in a rule of this repository, so each new route was designed from habit.

## Rule

See `.claude/rules/fix/architecture/fix-architecture-a-route-of-the-master-is-restful.md`.

## Example

- ❌ `POST /api/server-keys` to get the key of a server.
- ✅ `POST /api/servers` creates the server and answers its key once; the key is replaced later as `PUT /api/servers/me/key`.

## Update (5 November 2026) — the history of the statuses of a server

The plan of RTY-171 listed the statuses a server took at `GET /api/servers/{id}/status-changes`, answering `{"statusChanges":[{"changedAt":…,"status":…}]}`, and checked it against points 1 to 6 as a plural noun. A review answered "not RESTful": "changes" reads as the action of changing, and the changes of the status, an attribute of the server that point 2 names, became a resource of their own. The user chose `GET /api/servers/{id}/statuses`, each item the status and the second it began (`since`); the table `status_changes` keeps its name.
