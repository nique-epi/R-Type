"""Registry of level layouts: each one builds the zones, the terrain and the decor of a map, and has its own list of checks in rules.py."""
import sys

LAYOUTS = {}
DEFAULT_LAYOUT = "corridor"


def layout_of(biome, sequence):
    """Layout shared by every template of a chain; a chain that mixes two layouts is refused."""
    found = sorted({biome.templates[name].get("layout", DEFAULT_LAYOUT) for name, _ in sequence})
    if len(found) > 1:
        sys.exit(f"the layouts {' and '.join(found)} do not mix in one map: give a chain of templates of one layout")
    if found[0] not in LAYOUTS:
        sys.exit(f"unknown layout '{found[0]}': the layouts are {', '.join(sorted(LAYOUTS))}")
    return found[0]
