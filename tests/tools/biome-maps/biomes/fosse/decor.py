"""Decor of the Pit: families of sprites for the clusters, sprites that give light, silhouettes of the far plane."""
from .palette import CYAN, PINK, WARM, WHITE

FAMILIES = {
    "fern": {"large": ["fern_large", "fern_broad", "reeds", "grass_tall"], "medium": ["fern_medium", "grass_short", "reeds", "grass_tall"], "small": ["sprout", "flower_pink", "shroom_tiny", "grass_short"]},
    "shroom": {"large": ["shroom_shelf", "shroom_spotted", "shroom_slender"], "medium": ["shroom_trio", "shroom_slender", "shroom_spotted", "lamp_flower"], "small": ["shroom_tiny", "sprout", "shroom_trio"]},
    "pink": {"large": ["shroom_pink"], "medium": ["flower_pink", "shroom_pink_small", "fern_medium"], "small": ["shroom_pink_small", "flower_pink", "sprout"]},
    "rock": {"large": ["boulder_large", "rock_spur", "stalagmite"], "medium": ["boulder_mossy", "pebbles", "stalagmite"], "small": ["pebbles", "sprout", "shroom_tiny"]},
    "ruin": {"large": ["wall_chunk", "wall_corner", "steps", "arch"], "medium": ["brick_pile", "carved_block", "steps", "wall_corner"], "small": ["rubble", "pebbles", "sprout"]},
    "fallen": {"large": ["column_fallen", "wall_chunk", "wall_corner"], "medium": ["column_stump", "drum_fallen", "column_short", "brick_pile"], "small": ["rubble", "pebbles", "sprout"]},
    "crystal": {"large": ["crystals"], "medium": ["boulder_large", "rock_spur", "boulder_mossy"], "small": ["pebbles", "shroom_tiny", "sprout"]},
    "gate": {"large": ["arch"], "medium": ["lantern_lit"], "small": ["lantern_unlit", "rubble"]},
    "well": {"large": ["boulder_mossy"], "medium": ["fern_medium", "grass_tall", "reeds"], "small": ["sprout", "shroom_tiny", "flower_pink"]},
    "lantern": {"large": ["lantern_lit"], "medium": ["carved_block", "brick_pile", "drum_fallen"], "small": ["rubble", "sprout", "grass_short"]},
    "stal": {"large": ["stalactite_long", "stalactite_trio"], "medium": ["stalactite_wide", "stalactite_trio"], "small": ["vine_short"]},
    "hang": {"large": ["roots_a", "roots_b", "vine_long"], "medium": ["vine_long", "roots_b", "roots_a"], "small": ["vine_short"]},
    "vine": {"large": ["vine_long", "roots_a"], "medium": ["vine_short", "vine_long"], "small": ["vine_short"]},
}

EMITTERS = {
    "shroom_giant": (CYAN, 120, 0.85), "shroom_pink_giant": (PINK, 112, 0.80), "shroom_spotted": (CYAN, 66, 0.55),
    "shroom_shelf": (CYAN, 84, 0.60), "shroom_slender": (CYAN, 56, 0.50), "shroom_trio": (CYAN, 58, 0.50),
    "shroom_tiny": (CYAN, 34, 0.40), "shroom_pink": (PINK, 66, 0.55), "shroom_pink_small": (PINK, 38, 0.40),
    "lamp_flower": (CYAN, 74, 0.65), "crystals": (WHITE, 96, 0.75), "lantern_lit": (WARM, 150, 1.10),
}

FAR_SETS = {
    "columns": {"floor": ["column_short", "arch", "wall_chunk", "column_stump"], "ceil": ["vine_long", "roots_a"], "pillars": "colonnade"},
    "rocks": {"floor": ["stalagmite", "rock_spur", "boulder_large"], "ceil": ["stalactite_long", "stalactite_trio", "stalactite_wide"], "pillars": "pier"},
    "mushrooms": {"floor": ["shroom_giant", "shroom_pink_giant", "fern_large", "shroom_shelf"], "ceil": ["roots_a", "vine_long", "roots_b"], "pillars": None},
    "roots": {"floor": ["fern_broad", "shroom_slender", "reeds"], "ceil": ["roots_b", "roots_a", "vine_long"], "pillars": "pier"},
}

COLUMN_SPRITES = ("column_short", "column_stump")
