"""Materials of the demonstration biome: slag by the cobble painter, riveted iron by the plates painter, firebrick by the bricks painter with its own settings."""

ROCKS = ["boulder_large", "rock_spur", "stalagmite"]
BLADES = {"base": "b", "blade": "c", "tip": "d"}
DARKER = {"t": "1", "T": "t", "5": "4", "4": "3", "3": "2", "2": "1", "1": "1", "0": "0"}
LIGHTER = {"t": "T", "T": "T", "4": "5", "3": "4", "2": "3", "1": "2", "0": "0", "5": "5"}

MATERIALS = {
    "slag": {
        "painter": "cobble",
        "paint": {"texture": "texture_rock", "core": "A", "shade": "B", "lit": "E", "spark": "F",
                  "darker": {"F": "E", "E": "D", "D": "B", "B": "A", "A": "A", "0": "0"}},
        "aligned": False,
        "moss": None,
        "grass": {"heights": [2, 3, 3], "blades": (1, 4), "spacing": (40, 120), "keys": BLADES},
        "hanging": {"kind": "teeth", "spacing": (30, 90), "keys": {"body": "B", "edge": "D"}},
        "seam_floor": (2, ROCKS),
        "seam_ceil": (2, ["stalactite_trio", "stalactite_wide", "stalactite_long"]),
    },
    "iron": {
        "painter": "plates",
        "paint": {"plate": (32, 24), "stagger": True, "keys": {"face": "2", "light": "4", "shade": "1", "dark": "0", "rim": "T", "rivet": "5", "rust": "b"}},
        "aligned": True,
        "props": True,
        "moss": None,
        "grass": {"heights": [1, 2], "blades": (1, 2), "spacing": (70, 180), "keys": BLADES},
        "hanging": {"kind": "drips", "spacing": (40, 120), "keys": {"first": "b", "other": "a"}},
        "seam_floor": (1, ["gen:iron:pile", "gen:iron:drum"]),
        "seam_ceil": (1, ["chain_hook", "chain_long"]),
    },
    "firebrick": {
        "painter": "bricks",
        "paint": {"stone": {"rim": "T", "top": "5", "light": "4", "face": "3", "shade": "2", "dark": "1"}, "fill": "1",
                  "cell": (8, 8), "span": 3, "bond": "stack", "bevel": 1, "chips": False, "tall_chance": 0.0, "tints": (0.3, 0.8),
                  "frame": {"bands": ((0, 136), (144, 272)), "slabs": ((136, 0), (140, 0))},
                  "darker": DARKER, "lighter": LIGHTER},
        "aligned": True,
        "props": True,
        "broken_edges": {"floor": 0.06, "ceil": 0.0, "mid_floor": 0.1, "mid_ceil": 0.0},
        "moss": None,
        "grass": {"heights": [2, 3], "blades": (1, 3), "spacing": (50, 140), "keys": BLADES},
        "hanging": {"kind": "drips", "spacing": (40, 120), "keys": {"first": "T", "other": "t"}},
        "seam_floor": (1, ["gen:firebrick:pile", "gen:firebrick:drum"]),
        "seam_ceil": (1, ["chain_hook", "chain_long"]),
    },
}
SEAM_MATERIAL = "slag"
