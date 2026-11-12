"""Colors of the demonstration biome: the letters of the sprites it borrows from the Pit, in the warm tones of embers and ash."""

NEAR = {
    "0": (14, 10, 10), "1": (26, 18, 18), "2": (40, 28, 27), "3": (58, 40, 36), "4": (80, 56, 48), "5": (104, 74, 60),
    "t": (86, 40, 22), "T": (120, 58, 26),
    "a": (70, 22, 14), "b": (120, 36, 16), "c": (176, 62, 18), "d": (240, 130, 36), "e": (255, 214, 120),
    "p": (28, 16, 16), "q": (52, 30, 26), "r": (70, 34, 24), "s": (120, 70, 50),
    "k": (230, 170, 60), "K": (255, 236, 170), "o": (255, 190, 90), "n": (64, 30, 20),
    "u": (40, 8, 6), "v": (80, 16, 8), "w": (140, 34, 10), "x": (200, 70, 14), "y": (240, 130, 30), "z": (255, 210, 110),
    "g": (255, 170, 60), "h": (170, 60, 20),
    "A": (16, 11, 12), "B": (26, 18, 18), "D": (42, 28, 26), "E": (66, 44, 38), "F": (92, 62, 50),
}
BACKDROP = (16, 9, 9)
MID = {key: tuple(int(round(BACKDROP[channel] + (color[channel] - BACKDROP[channel]) * 0.5)) for channel in range(3)) for key, color in NEAR.items()}
FAR_A, FAR_B = (24, 13, 12), (40, 20, 16)
FAR_LIT_KEYS = "45tTbcdeszyoKFE"
OUTLINE = {"dark_through": "1", "rim": "t"}

EMBER, FLAME, HOT = (1.0, 0.55, 0.25), (1.0, 0.8, 0.4), (1.0, 0.95, 0.75)
