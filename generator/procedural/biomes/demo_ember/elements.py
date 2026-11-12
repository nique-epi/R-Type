"""Elements of the demonstration biome: lava in place of water, a lava fall, one ember lamp, no column."""
from .palette import FLAME

WET_CONNECTORS = ()

WATER = {
    "level": 238,
    "depth_tint": (0.6, (30, 6, 2)),
    "tiles": {
        "top": [
            "....yyyyyy......", "..yyxxxxxxyy....", "yyxxwwwwwwxxyyyy", "xxwwwwwwwwwwxxxx",
            "wwwwwwwwwwwwwwww", "wwwwwwwwwwwwwwww", "wwvwwwvwwwvwwwvw", "vwvvvwvvvwvvvwvv"],
        "wave": [
            "vvvvvvvvvvvvvvvv", "vvvvxxxxxxvvvvvv", "vvxxxxxxxxxxvvvv", "xxxxvvvvvvxxxxxx",
            "xxvvvvvvvvvvxxxx", "vvvvvvvvvvvvvvvv", "vvvvvvvvvvvvvvvv", "vvvvvvvvvvvvvvvv"],
        "wave_dim": ("wave", {"x": "w"}),
        "plain": ["vvvvvvvvvvvvvvvv"] * 8,
    },
    "rows": [
        {"dy": 0, "dx": 0, "tile": "top"},
        {"dy": 8, "dx": -8, "tile": "wave", "chance": 0.6, "else": "wave_dim"},
        {"dy": 16, "dx": 0, "tile": "wave_dim", "chance": 0.6, "else": "plain"},
        {"dy": 24, "dx": 0, "tile": "plain"},
        {"dy": 32, "dx": 0, "tile": "plain"},
        {"dy": 40, "dx": 0, "tile": "plain"},
    ],
}

FALL_COLUMNS = [
    ".x.x.x.x.x.x.x.x", "xwxwxwxwxwxwxwxw", "yyyyxxyyyyyyxxyy", "zzzzzzyyzzzzzzyy", "yyxxyyyyyyxxyyyy", "xxxxyyxxxxxxyyxx",
    "zzzzzzzzyyzzzzzz", "yyzzzzyyzzyyzzzz", "yyyyxxyyyyyyxxyy", "xxyyxxxxxxyyxxxx", "wxwxwxwxwxwxwxwx", "x.x.x.x.x.x.x.x."]
FALL = ["".join(column[row] for column in FALL_COLUMNS) for row in range(16)]

LAMPS = {
    "ember": {"rows": ["..11..", ".1331.", "13oo31", "1oooo1", "1oooo1", "13oo31", ".1111."], "tint": FLAME, "name": "lamp_ember", "cord": "3"},
}

STROKES = {"floor": {"blade": "c", "tip": "d"}, "ceil": {"young": "s", "old": "q", "root": "q"}}
SPECKS = {"keys": ["d", "k", "c", "b"], "count": 22, "rows": (40, 210), "gain": 0.9}
WALL_KEY = "d"
