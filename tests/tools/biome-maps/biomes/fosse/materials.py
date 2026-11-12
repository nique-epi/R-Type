"""Materials of the Pit: which painter draws each one, with which palette letters, and what grows on it."""
from .palette import COOL

ROCKS = ["boulder_large", "rock_spur", "stalagmite", "boulder_mossy"]
CREEPERS = ["vine_long", "roots_a"]
MOSS = {"top": "c", "body": "b", "deep": "a"}
BLADES = {"base": "b", "blade": "c", "tip": "d"}
STONE = {"rim": "t", "top": "4", "light": "3", "face": "2", "shade": "1", "dark": "0"}

MATERIALS = {
    "rock": {
        "painter": "cobble",
        "paint": {"texture": "texture_rock", "core": "A", "shade": "B", "lit": "E", "spark": "F",
                  "darker": {"F": "E", "E": "D", "D": "B", "B": "A", "A": "A", "0": "0"}},
        "aligned": False,
        "moss": {"cover": "patches", "keys": MOSS},
        "grass": {"heights": [2, 3, 3, 4, 6], "blades": (3, 8), "spacing": (28, 90), "keys": BLADES},
        "hanging": {"kind": "teeth", "spacing": (34, 110), "keys": {"body": "B", "edge": "D"}},
        "seam_floor": (2, ROCKS),
        "seam_ceil": (2, ["stalactite_trio", "stalactite_wide", "stalactite_long"]),
    },
    "earth": {
        "painter": "soil",
        "paint": {"core": "A", "shade": "B", "body": "D", "edge": "E", "lit": "F", "blobs": [
            ["AAAAAAAA", "AABBAAAA", "AABBBAAA", "AAABAAAA", "AAAAAAAA", "AAAAABBA", "AAAAABBA", "AAAAAAAA"],
            ["AAAAAAAA", "AAAAAAAA", "AAADDAAA", "AADDBBAA", "AABBBAAA", "AAAAAAAA", "ABAAAAAA", "AAAAAAAA"],
            ["AAAAAAAA", "AAAAAABA", "AAAAABBA", "AAABBBAA", "AABBAAAA", "AABAAAAA", "AAAAAAAA", "AAAAAAAA"],
        ]},
        "aligned": False,
        "moss": {"cover": "full", "keys": MOSS},
        "grass": {"heights": [3, 4, 5, 7, 9, 12], "blades": (3, 8), "spacing": (12, 50), "keys": BLADES},
        "hanging": {"kind": "strands", "spacing": (20, 70), "keys": {"root": "b", "young": "c", "bud": "d", "bloom": "K"}},
        "seam_floor": (2, ROCKS),
        "seam_ceil": (1, CREEPERS),
    },
    "masonry": {
        "painter": "bricks",
        "paint": {"stone": STONE, "fill": "1", "frame": {"bands": ((0, 136), (140, 268)), "slabs": ((136, 0), (268, 3))},
                  "darker": {"t": "1", "T": "t", "5": "4", "4": "3", "3": "2", "2": "1", "1": "1", "0": "0"},
                  "lighter": {"t": "T", "T": "T", "4": "5", "3": "4", "2": "3", "1": "2", "0": "0", "5": "5"}},
        "aligned": True,
        "broken_edges": {"floor": 0.10, "ceil": 0.22, "mid_floor": 0.08, "mid_ceil": 0.2},
        "bears_columns": True,
        "cracks": {"key": "z", "tint": COOL, "slopes": [-0.22, 0.18, 0.26], "reach": 36, "width": 5, "spread": 0.09, "power": 0.5},
        "moss": None,
        "grass": {"heights": [2, 3, 3, 4, 6], "blades": (1, 4), "spacing": (28, 90), "keys": BLADES},
        "hanging": {"kind": "drips", "spacing": (34, 110), "keys": {"first": "T", "other": "t"}},
        "seam_floor": (1, ["wall_chunk", "wall_corner", "steps"]),
        "seam_ceil": (1, CREEPERS),
    },
}
SEAM_MATERIAL = "rock"
