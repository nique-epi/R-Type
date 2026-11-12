"""Templates of the demonstration biome: a lava fall in slag, a foundry of riveted iron, and a kiln yard of firebrick under an open sky."""
from .palette import EMBER, FLAME, HOT

TEMPLATES = {
    "lava_fall": {
        "material": "slag", "left": "low", "right": "low", "ambient": 0.78, "mood": EMBER, "fixed": ["mid_floor"],
        "hero": {"kind": "waterfall", "x": 320, "ledge": (298, 356, 168, 182), "spout": "spout", "splash": "splash", "foam": "foam", "tint": FLAME, "one_per": 3},
        "mid_exempt": (296, 376),
        "floor": [(0, 232), (80, 234), (104, 243), (128, 250), (152, 257), (184, 265), (336, 261), (360, 254), (384, 246), (408, 235), (432, 232)],
        "ceil": [(0, 32), (112, 36), (200, 40), (264, 36), (296, 40), (352, 34), (424, 32)],
        "mid_floor": [(0, 210), (88, 214), (112, 252), (340, 168), (372, 198), (400, 204), (440, 210)],
        "mid_ceil": [(0, 56), (96, 60), (288, 64), (352, 60), (432, 56)],
        "near_floor": [(40, "rock"), (452, "caps")],
        "near_ceil": [(56, "stal"), (412, "stal")],
        "mid_floor_decor": [(48, "brush"), (410, "caps")],
        "mid_ceil_decor": [(176, "stal")],
    },
    "foundry": {
        "material": "iron", "left": "low", "right": "low", "ambient": 0.74, "mood": FLAME,
        "hero": {"kind": "cluster", "x": 160, "family": "forge", "plane": "near", "surface": "floor",
                 "lamps": [{"dx": 120, "plane": "near", "length": (10, 24), "lamp": "ember", "radius": 96, "power": 0.8}]},
        "floor": [(0, 232), (120, 224), (264, 232), (376, 224), (432, 232)],
        "ceil": [(0, 32), (88, 40), (200, 24), (312, 40), (408, 32)],
        "mid_floor": [(0, 208), (160, 200), (320, 208)],
        "mid_ceil": [(0, 56), (120, 48), (360, 56)],
        "near_floor": [(330, "iron"), (444, "forge")],
        "near_ceil": [(90, "chains"), (380, "chains")],
        "mid_floor_decor": [(70, "iron"), (390, "iron")],
        "mid_ceil_decor": [(250, "chains")],
    },
    "kiln_yard": {
        "material": "firebrick", "left": "low", "right": "low", "ambient": 0.86, "mood": HOT, "sky": True,
        "hero": {"kind": "cluster", "x": 320, "family": "firebrick", "plane": "near", "surface": "floor", "lead": "gen:firebrick:pillar",
                 "echo": {"dx": -140, "family": "firebrick", "plane": "mid", "surface": "floor", "lead": "gen:firebrick:wall"}},
        "floor": [(0, 232), (104, 224), (216, 232), (328, 224), (424, 232)],
        "mid_floor": [(0, 208), (96, 200), (256, 208), (368, 200), (440, 208)],
        "near_floor": [(70, "firebrick"), (200, "brush"), (440, "forge")],
        "mid_floor_decor": [(80, "firebrick"), (250, "caps"), (410, "firebrick")],
    },
}
