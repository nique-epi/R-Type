"""Elements of the Pit drawn from rows of palette letters: water, waterfall band, lamps, column, strokes and specks."""
from .materials import STONE
from .palette import CYAN, PINK, WARM

WET_CONNECTORS = ("water",)

WATER = {
    "level": 238,
    "depth_tint": (0.55, (4, 8, 24)),
    "tiles": {
        "top": [
            "....yyyyyy......", "..yyxxxxxxyy....", "yyxxvvvvvvxxyyyy", "xxvvvvvvvvvvxxxx",
            "vvvvvvvvvvvvvvvv", "vvvvvvvvvvvvvvvv", "vvuvvvuvvvuvvvuv", "uvuuuvuuuvuuuvuu"],
        "top_dim": ("top", {"y": "x", "x": "w"}),
        "wave": [
            "uuuuuuuuuuuuuuuu", "uuuuwwwwwwuuuuuu", "uuwwwwwwwwwwuuuu", "wwwwuuuuuuwwwwww",
            "wwuuuuuuuuuuwwww", "uuuuuuuuuuuuuuuu", "uuuuuuuuuuuuuuuu", "uuuuuuuuuuuuuuuu"],
        "wave_dim": ("wave", {"w": "v"}),
        "glint": [
            "uuuuuuuuuuuuuuuu", "uuugggguuuuuuuuu", "uugggggguuuhhuuu", "uuuhhhhuuuhhhhuu",
            "uuuuuuuuuuuuuuuu", "uuuuuuuuuuuuuuuu", "uuuuuuuuuuuuuuuu", "uuuuuuuuuuuuuuuu"],
        "plain": ["uuuuuuuuuuuuuuuu"] * 8,
    },
    "rows": [
        {"dy": 0, "dx": 0, "tile": "top", "chance": 0.65, "else": "top_dim"},
        {"dy": 8, "dx": 0, "tile": "glint", "chance": 0.5, "else": "plain"},
        {"dy": 16, "dx": -8, "tile": "wave", "chance": 0.7, "else": "wave_dim"},
        {"dy": 24, "dx": 0, "tile": "wave_dim", "chance": 0.75, "else": "plain"},
        {"dy": 32, "dx": -8, "tile": "wave_dim", "chance": 0.4, "else": "plain"},
        {"dy": 40, "dx": 0, "tile": "plain"},
    ],
}

FALL_COLUMNS = [
    ".w.w.w.w.w.w.w.w", "wvwvwvwvwvwvwvwv", "xxxxwwxxxxxxwwxx", "yyyyyyxxyyyyyyxx", "xxwwxxxxxxwwxxxx", "wwwwxxwwwwwwxxww",
    "yyzzyyyyxxyyzzyy", "xxyyyyxxyyxxyyyy", "xxxxwwxxxxxxwwxx", "wwxxwwwwwwxxwwww", "vwvwvwvwvwvwvwvw", "w.w.w.w.w.w.w.w."]
FALL = ["".join(column[row] for column in FALL_COLUMNS) for row in range(16)]

LAMPS = {
    "cyan": {"rows": ["...0110...", "..012210..", ".deeedddc.", "deeedddddc", "dddddddccc", ".cdcdcccb.", "..bcbcbb.."], "tint": CYAN, "name": "lamp_cyan", "cord": "3"},
    "pink": {"rows": ["...0110...", "..012210..", ".KKKKKKKk.", "KKKKKKKKKk", "KKKKKKKkkk", ".kKkKkkkq.", "..qkqkqq.."], "tint": PINK, "name": "lamp_pink", "cord": "3"},
    "warm": {"rows": ["..11..", ".1331.", "13oo31", "1oooo1", "1oooo1", "13oo31", ".1111."], "tint": WARM, "name": "lamp_warm", "cord": "3"},
}

COLUMN = {
    "shaft": ["t4332212232110" if row % 2 == 0 else "t4332212231210" for row in range(32)],
    "steps": [(24, 5), (20, 4), (16, 3)],
    "stone": STONE,
    "scar": "1",
    "moss": ("T", "t"),
}

STROKES = {"floor": {"blade": "c", "tip": "d"}, "ceil": {"young": "c", "old": "b", "root": "b"}}
SPECKS = {"keys": ["d", "s", "T", "b"], "count": 14, "rows": (70, 200), "gain": 0.8}
WALL_KEY = "d"
