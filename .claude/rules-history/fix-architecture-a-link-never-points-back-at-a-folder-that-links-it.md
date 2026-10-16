# RULE: A new link never points back at a folder that already links the folder of the new target

## Context

Options menu of the client. The DISPLAY tab shows a chart of every window size to scale, painted by a free function, `paintWindowSizeChart()`, that outlines each size with the `Outline` widget piece. The plan placed it in `src/client/Rendering/`, because the map puts the title squadron, another picture painted in a menu, in `Rendering/`. The plan named the link to `Widgets/` as a risk and left it to the placement review.

## Mistake

`rtype_client_window_size_chart` linked `rtype_client_outline` from `Widgets/`, while `rtype_client_widgets`, the interface target every widget links, already links `rtype_client_rendering_interfaces`. `Rendering/` and `Widgets/` depended on each other; the fresh placement review reported it after the code, its tests and its documentation were written.

## Root cause

The folder was chosen by what the file shows (a picture in a menu, like the squadron), not by what it links. The squadron links nothing from `Widgets/`, so the analogy did not hold, and reading the two `CMakeLists.txt` would have shown it before a line was written.

## Rule

1. **Before writing `target_link_libraries` for a new target, list the folders of the targets it links**, and for each one check whether that folder already links the folder of the new target: read the `INTERFACE` target of the folder (`rtype_client_widgets` links `rtype_client_rendering_interfaces`) and `grep -rn '<target>' src/client/<other folder>` for the targets of the new folder.
2. **If it does, the new file goes in the folder that already depends on the other**, or the piece it needs moves to the folder below; never a link that reverses the direction.
3. **An analogy with a neighbour ("the squadron lives in `Rendering/`") is checked against the links**: the neighbour's links decide whether the analogy holds, not its role on screen.
4. **A risk of this kind written in a plan is settled before the code**, by reading the links, not left to a later review.

## Example

- ❌ **Before (wrong)**: `paintWindowSizeChart()` in `src/client/Rendering/`, linking `rtype_client_outline` from `Widgets/`, while `rtype_client_widgets` already links `rtype_client_rendering_interfaces`.
- ✅ **After (right)**: the chart and its constants header in `src/client/Widgets/`, its target declared by `Widgets/CMakeLists.txt`; `Rendering/` links nothing from `Widgets/`.
