---
description: "Every HTTP route of the master is RESTful: resources as plural nouns, the method as the action, the caller from its credential"
trigger: always_on
---

# RULE: Every HTTP route of the master is RESTful, with the conventions of Yuno backend

Why: routes named after an action (`/register`, `/heartbeat` as a POST) and an attribute treated as a resource (`/server-keys`) reached two plans in a row; the user asked for the conventions of Yuno backend on every route.

## Rule

1. **A path names resources, never an action**: plural nouns (`/api/servers`), then the id or `me`, then a sub-resource (`/api/servers/me/heartbeat`). No verb in a path: no `/register`, `/toggle`, `/join`, `/login`, `/quick`.
2. **An attribute of a resource is not a resource.** A key, a status or a counter belongs to the resource it describes: it is created with it, read in its representation, or replaced as its sub-resource (`PUT /api/servers/me/key`). Never `/api/server-keys`.
3. **The method is the action**, with its status:

   | Method | Does | Answers |
   |---|---|---|
   | `GET` | reads; a list is paginated (`?limit&offset`) | 200 |
   | `POST` | creates a resource whose id the master chooses | 201 and `Location` of the new resource |
   | `PUT` | replaces the resource at a known path, created if absent; sending it twice gives the same result | 200, or 201 when it created it, or 204 with no body |
   | `PATCH` | changes some members only | 200 |
   | `DELETE` | removes; removing what is gone succeeds | 204 |

4. **The caller is named by its credential, never by its id in the path**: `/api/servers/me` for the server of the key, `/api/users/me` for the account of the session. A path that takes the caller's own id lets one credential reach another's resource (IDOR).
5. **No envelope**: the status says success or failure, never `{"success": true}`. An error is a problem document of RFC 9457 whose `detail` says what was refused.
6. **A member enters a representation only if it spares a heavier request**; a sub-resource with an access rule of its own stays a route of its own (Yuno: the shape of a payload follows the access rule, never a screen).
7. **Before writing a route, write its line**: method, path, who calls it, answer and errors, and check it against points 1 to 6. A plan that adds a route shows that table.

## Example

- ❌ **Before (wrong)**: `POST /api/game-servers/register`, `POST /api/game-servers/{id}/heartbeat` with the id compared to the key, then `POST /api/server-keys` to get a key.
- ✅ **After (right)**: `POST /api/servers` creates the server and answers 201 with its key, once; `PUT /api/servers/me` replaces the representation of the server of the key; `PUT /api/servers/me/heartbeat` replaces its last state.
