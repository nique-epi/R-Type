---
description: A CMake loop over a list tests the length of the list, never the list as a condition
trigger: always_on
---

# RULE: A CMake loop over a list tests its length — `while(<list>)` stops on an entry that reads as false

Why: `rtype_forbid_reachable_links` ran `while(pending)`; the list ended with `interface_linked-NOTFOUND`, from a target without interface links, CMake read the whole list as false, and the walk stopped after ten targets: the check of the links of the master never reached the network library.

## Rule

1. **A loop that empties a list tests its length**: `list(LENGTH pending pending_count)` before the loop and after each `POP_FRONT` and `APPEND`, then `while(pending_count GREATER 0)`. `if(<variable>)` and `while(<variable>)` read the whole text of the list as one value, false when it ends in `-NOTFOUND` or is `0`, `OFF`, `NO`, `FALSE`, `N`, `IGNORE` or `NOTFOUND`.
2. **`get_target_property()` gives `<variable>-NOTFOUND` for a property the target lacks**: never append it to a list of names; leave it out (`if(NOT library MATCHES "-NOTFOUND$")`).
3. **A check written in CMake is calibrated on a configure it must refuse** before it is trusted: add the forbidden link on an isolated copy, run the configure, read the refusal, restore. A check that lets the faulty configure pass checks nothing.

## Example

- ❌ **Before (wrong)**: `while(pending)`, with `list(APPEND pending ${library})` for every value of `LINK_LIBRARIES` and `INTERFACE_LINK_LIBRARIES` → the walk from `rtype_master_modules` stopped at `rtype_engine_exceptions`, and a private link of `rtype_network_udp_socket` to `rtype_game_components` passed the configure.
- ✅ **After (right)**: `while(pending_count GREATER 0)`, the NOTFOUND values left out → the same link stops the configure with `rtype_master_modules must not reach rtype_game_components, linked by rtype_network_udp_socket`.
