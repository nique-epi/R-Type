"""Writes a starting biome that runs at once: a palette, one material that builds its own props, two templates of the layout asked for, and no sprite."""
import argparse
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT)
import biome_file  # noqa: E402

STONE = {"0": [14, 14, 22], "1": [26, 28, 44], "2": [40, 44, 66], "3": [58, 64, 92], "4": [82, 90, 122], "5": [112, 122, 156]}
STROKES = {"floor": {"blade": "b", "tip": "c"}, "ceil": {"young": "t", "old": "2", "root": "2"}}


def palette_of(extra, tints):
    """Palette of a starting biome: six tones of stone, the extra letters its layout needs, and its tints."""
    return {"near": dict(STONE, **extra), "mid_share": 0.5, "backdrop": [10, 10, 18], "far_a": [16, 17, 28], "far_b": [26, 29, 46], "far_lit_keys": "45tc", "tints": tints}


def corridor():
    """Starting biome of the corridor layout: courses of stone, a lamp, two zones around a cluster of props."""
    lamp = lambda dx: {"dx": dx, "plane": "near", "length": [10, 24], "lamp": "warm", "radius": 96, "power": 0.8}
    stone = {
        "painter": "bricks",
        "paint": {"stone": {"rim": "t", "top": "5", "light": "4", "face": "3", "shade": "2", "dark": "1"}, "fill": "1",
                  "darker": {"t": "4", "5": "4", "4": "3", "3": "2", "2": "1", "1": "1"},
                  "lighter": {"t": "t", "5": "5", "4": "5", "3": "4", "2": "3", "1": "2"}},
        "aligned": True, "props": True, "moss": None,
        "grass": {"heights": [2, 3, 4], "blades": [1, 4], "spacing": [30, 90], "keys": {"base": "a", "blade": "b", "tip": "c"}},
        "hanging": {"kind": "drips", "spacing": [40, 120], "keys": {"first": "t", "other": "2"}},
        "seam_floor": [1, ["gen:stone:pile", "gen:stone:drum"]], "seam_ceil": [1, []],
    }
    templates = {
        "hall": {
            "material": "stone", "left": "low", "right": "low", "ambient": 0.8, "mood": "@WARM",
            "hero": {"kind": "cluster", "x": 160, "family": "stone", "plane": "near", "surface": "floor", "lead": "gen:stone:pillar", "lamps": [lamp(60)]},
            "floor": [[0, 232], [120, 224], [280, 232]], "ceil": [[0, 32], [160, 40], [320, 32]],
            "mid_floor": [[0, 208], [200, 200], [360, 208]], "mid_ceil": [[0, 56], [240, 48]],
            "near_floor": [[380, "stone"]], "mid_floor_decor": [[90, "stone"], [300, "stone"]],
        },
        "gallery": {
            "material": "stone", "left": "low", "right": "low", "ambient": 0.9, "mood": "@COOL",
            "hero": {"kind": "cluster", "x": 320, "family": "stone", "plane": "near", "surface": "floor", "lead": "gen:stone:wall", "lamps": [lamp(-70)]},
            "floor": [[0, 232], [200, 224], [400, 232]], "ceil": [[0, 32], [120, 24], [300, 32]],
            "mid_floor": [[0, 208], [160, 200], [300, 208]], "mid_ceil": [[0, 56], [200, 64], [380, 56]],
            "near_floor": [[90, "stone"]], "mid_floor_decor": [[200, "stone"], [420, "stone"]],
        },
    }
    elements = {
        "lamps": {"warm": {"rows": [".11.", "1oo1", "1oo1", ".11."], "tint": "@WARM", "name": "lamp_warm", "cord": "3"}},
        "strokes": STROKES, "specks": {"keys": ["c", "t"], "count": 12, "rows": [40, 210], "gain": 0.9}, "wall_key": "c",
    }
    extra = {"t": [70, 96, 110], "a": [30, 70, 60], "b": [48, 110, 84], "c": [90, 170, 120], "o": [255, 196, 110]}
    return {"palette": palette_of(extra, {"WARM": [1.0, 0.75, 0.45], "COOL": [0.6, 0.8, 1.0]}), "materials": {"stone": stone}, "seam_material": "stone",
            "elements": elements, "decor": {}, "templates": templates}


def field():
    """Starting biome of the field layout: bodies of dark rock in open space, a few stars, two zones around a great body with a glowing vein."""
    rock = {
        "painter": "soil",
        "paint": {"core": "0", "shade": "1", "body": "2", "edge": "3", "lit": "4",
                  "blobs": [["00000000", "00110000", "01111000", "00110000", "00000000", "00000110", "00000100", "00000000"],
                            ["00000000", "00000010", "00000110", "00011100", "00110000", "00100000", "00000000", "00000000"],
                            ["00000000", "01100000", "01110000", "00000000", "00001100", "00011100", "00001000", "00000000"]]},
        "aligned": False, "props": ["pile", "wall", "pillar_broken", "drum", "slab"],
    }
    vein = lambda radius: {"keys": ["b", "c", "d"], "tint": "@GLOW", "radius": radius, "power": 0.85}
    templates = {
        "expanse": {
            "layout": "field", "material": "rock", "left": "open", "right": "open", "ambient": 1.0, "mood": "@GLOW",
            "hero": {"kind": "great_body", "x": 320, "y": 130, "size": [180, 104], "silhouette": "boulder", "material": "rock", "family": "rock", "vein": vein(110)},
            "mid_bodies": [[96, 80, "rock"], [130, 196, None]],
            "near_top": [[110, None], [400, None]], "near_bottom": [[214, "rock"], [436, None]],
            "far_bodies": [[60, 170], [236, 40], [430, 214]],
        },
        "shoal": {
            "layout": "field", "material": "rock", "left": "open", "right": "open", "ambient": 1.1, "mood": "@PALE",
            "hero": {"kind": "great_body", "x": 160, "y": 146, "size": [150, 88], "silhouette": "boulder", "material": "rock", "family": "rock", "vein": vein(120)},
            "mid_bodies": [[330, 92, "rock"], [396, 186, None], [104, 54, None]],
            "near_top": [[196, None], [430, None]], "near_bottom": [[300, "rock"], [70, None]],
            "far_bodies": [[46, 110], [250, 214], [364, 30]],
        },
    }
    elements = {
        "strokes": STROKES, "specks": {"keys": ["c", "3"], "count": 12, "rows": [30, 236], "gain": 0.9}, "wall_key": "c",
        "stars": {"cell": 16, "sparse": [0, 0, 0, 0, 0, 1, 1, 2], "crowded": [1, 2, 2, 3, 4, 6], "wander": [-24, -12, 0, 0, 12, 24], "band": [22, 30, 38, 46],
                  "tones": [[[40, 44, 70], 0.6], [[90, 98, 136], 0.31], [[200, 210, 236], 0.09]], "cross": 0.4},
        "haze": {"tones": [[14, 15, 28], [19, 21, 38]], "banks": [1, 3], "width": [150, 290], "height": [44, 96]},
    }
    extra = {"t": [70, 96, 110], "b": [40, 92, 120], "c": [80, 160, 190], "d": [190, 236, 244]}
    return {"palette": palette_of(extra, {"GLOW": [0.5, 0.85, 1.0], "PALE": [0.85, 0.92, 1.0]}), "materials": {"rock": rock}, "seam_material": "rock",
            "elements": elements, "decor": {}, "templates": templates}


STARTERS = {"corridor": corridor, "field": field}


def main():
    """Command line: creates the folder of a new biome with its data file and an empty sprites folder."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("name", help="name of the new biome: letters, digits and underscores")
    parser.add_argument("--biomes", required=True, help="folder that holds one folder per biome; it is created when missing")
    parser.add_argument("--layout", choices=sorted(STARTERS), default="corridor")
    arguments = parser.parse_args()
    if not arguments.name.replace("_", "").isalnum():
        parser.error("the name of a biome holds only letters, digits and underscores")
    folder = os.path.join(arguments.biomes, arguments.name)
    if os.path.exists(folder):
        biome_file.stop(f"{folder} already exists: choose another name, or remove it first")
    biome_file.write(arguments.biomes, arguments.name, STARTERS[arguments.layout]())
    os.makedirs(os.path.join(folder, "sprites"))
    print(f"{os.path.join(folder, biome_file.FILE)} written: a {arguments.layout} biome with two templates and no sprite")
    print(f"try it: python3 generate_maps.py --biomes {arguments.biomes} --biome {arguments.name} --count 2 --zones 2 --seed 1 --out out_{arguments.name}")


if __name__ == "__main__":
    main()
