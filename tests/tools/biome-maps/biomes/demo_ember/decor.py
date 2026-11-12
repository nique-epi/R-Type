"""Decor of the demonstration biome: only what file names cannot say, that is the families and lights of the sprites it borrows from the Pit."""
from .palette import EMBER, FLAME

SPRITES_FROM = "fosse"

FAMILIES = {
    "caps": {"large": ["shroom_shelf", "shroom_spotted", "shroom_slender"], "medium": ["shroom_trio", "shroom_slender", "shroom_spotted"], "small": ["shroom_tiny", "sprout", "pebbles"]},
    "rock": {"large": ["boulder_large", "rock_spur", "stalagmite"], "medium": ["boulder_mossy", "pebbles", "stalagmite"], "small": ["pebbles", "sprout", "shroom_tiny"]},
    "brush": {"large": ["reeds", "grass_tall", "fern_broad"], "medium": ["grass_short", "reeds", "fern_medium"], "small": ["sprout", "pebbles", "grass_short"]},
    "stal": {"large": ["stalactite_long", "stalactite_trio"], "medium": ["stalactite_wide", "stalactite_trio"], "small": ["vine_short"]},
}

EMITTERS = {
    "shroom_spotted": (EMBER, 66, 0.55), "shroom_shelf": (EMBER, 84, 0.6), "shroom_slender": (EMBER, 56, 0.5),
    "shroom_trio": (EMBER, 58, 0.5), "shroom_tiny": (FLAME, 34, 0.4),
}
