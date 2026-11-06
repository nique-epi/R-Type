---
description: A CMake loop over a list tests the length of the list, never the list as a condition
trigger: always_on
---

# RULE: A CMake loop over a list tests its length — `while(<list>)` stops on an entry that reads as false

## Context

The master must never link SFML, Asio, the game or the code of the server. `rtype_forbid_reachable_links`, in the root `CMakeLists.txt`, walks every link the libraries of the master reach, `PRIVATE` ones included, and stops the configure on a forbidden one. The probe of the master (a UDP socket in a module `probe`) needed the master to reach `rtype_network_udp_socket`, which links Asio privately, so the plan added an exception for the network targets that hide Asio behind their headers, and predicted that without it the configure would fail.

## Mistake

The configure passed before the exception existed, and still passed when the list of exceptions was emptied on an isolated copy. A trace of the walk showed it visited ten targets and stopped at `rtype_engine_exceptions`, never reaching `rtype_master_probe_module`'s links. A private link of `rtype_network_udp_socket` to `rtype_game_components` also passed the configure. The check had been checking a small part of what the master reaches since it was written.

## Root cause

The walk ran `while(pending)` and appended every value of `LINK_LIBRARIES` and `INTERFACE_LINK_LIBRARIES` to `pending`. For a target without interface links, `get_target_property()` gives `interface_linked-NOTFOUND`, which the walk appended too. `while(<variable>)` reads the whole text of the list as one value, and a value ending in `-NOTFOUND` is false: as soon as such an entry was the last of the list, the loop ended, whatever came before it. Nothing had ever run the check on a configure it must refuse, so nobody saw it pass.

## Rule

1. **A loop that empties a list tests its length**: `list(LENGTH pending pending_count)` before the loop and after each `POP_FRONT` and `APPEND`, then `while(pending_count GREATER 0)`. `if(<variable>)` and `while(<variable>)` read the whole text of the list as one value, false when it ends in `-NOTFOUND` or is `0`, `OFF`, `NO`, `FALSE`, `N`, `IGNORE` or `NOTFOUND`.
2. **`get_target_property()` gives `<variable>-NOTFOUND` for a property the target lacks**: never append it to a list of names; leave it out (`if(NOT library MATCHES "-NOTFOUND$")`).
3. **A check written in CMake is calibrated on a configure it must refuse** before it is trusted: add the forbidden link on an isolated copy, run the configure, read the refusal, restore. A check that lets the faulty configure pass checks nothing.

## Example

- ❌ **Before (wrong)**: `while(pending)`, with `list(APPEND pending ${library})` for every value of `LINK_LIBRARIES` and `INTERFACE_LINK_LIBRARIES` → the walk from `rtype_master_modules` stopped at `rtype_engine_exceptions`, and a private link of `rtype_network_udp_socket` to `rtype_game_components` passed the configure.
- ✅ **After (right)**: `while(pending_count GREATER 0)`, the NOTFOUND values left out → the same link stops the configure with `rtype_master_modules must not reach rtype_game_components, linked by rtype_network_udp_socket`.
