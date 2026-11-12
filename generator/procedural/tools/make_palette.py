"""Builds the palette of a biome from pictures: finds their color ramps and fills the standard letters every biome file refers to."""
import argparse
import colorsys
import os
import sys

import numpy as np
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT)
import biome_file  # noqa: E402
from biome_loader import add_options  # noqa: E402

ROLES = ("stone", "rock", "plant", "liquid", "stem", "accent")
OUTLINE = {"dark_through": "1", "rim": "t"}
MID_SHARE = 0.5
FAR_SHARE = 0.3
FAR_LIT_KEYS = "45tTbcdeszyoKFE"


def luma(color):
    """Perceived brightness of a color."""
    return float(np.dot(color, (0.30, 0.59, 0.11)))


def collect(paths):
    """Up to 48 colors that stand for the opaque pixels of the pictures, each with its share of them."""
    pixels = []
    for path in paths:
        files = sorted(os.path.join(path, name) for name in os.listdir(path) if name.lower().endswith(".png")) if os.path.isdir(path) else [path]
        for file in files:
            rgba = np.asarray(Image.open(file).convert("RGBA"))
            pixels.append(rgba[rgba[..., 3] > 0][:, :3])
    pixels = np.concatenate(pixels)
    strip = Image.fromarray(np.ascontiguousarray(pixels[None, :, :])).quantize(colors=48, method=Image.Quantize.MEDIANCUT)
    table = np.array(strip.getpalette()[:48 * 3], float).reshape(-1, 3)
    counts = np.bincount(np.asarray(strip).ravel(), minlength=48)[:len(table)]
    return [(table[position], counts[position] / counts.sum()) for position in np.argsort(-counts) if counts[position]]


def find_ramps(colors):
    """Colors grouped into ramps: one of the near-gray ones, then one per stretch of hue, heaviest first, each sorted dark to light."""
    gray, tinted = [], []
    for color, share in colors:
        hue, saturation, _ = colorsys.rgb_to_hsv(*(color / 255.0))
        (gray if saturation < 0.22 else tinted).append((hue * 360.0, color, share))
    groups = [[(color, share) for _, color, share in gray]] if gray else []
    tinted.sort(key=lambda entry: entry[0])
    if tinted:
        gaps = [(tinted[(i + 1) % len(tinted)][0] - tinted[i][0]) % 360.0 for i in range(len(tinted))]
        start = (int(np.argmax(gaps)) + 1) % len(tinted)
        ordered = tinted[start:] + tinted[:start]
        group = [ordered[0]]
        for previous, entry in zip(ordered, ordered[1:]):
            if (entry[0] - previous[0]) % 360.0 > 25.0:
                groups.append([(color, share) for _, color, share in group])
                group = []
            group.append(entry)
        groups.append([(color, share) for _, color, share in group])
    groups.sort(key=lambda group: -sum(share for _, share in group))
    return [sorted(group, key=lambda entry: luma(entry[0])) for group in groups]


def tones(ramp, count, low=0.0, high=1.0):
    """A number of tones of a ramp, evenly spread between two points of its span from dark to light."""
    colors = np.array([color for color, _ in ramp])
    if len(colors) == 1:
        colors = np.array([colors[0] * 0.35, colors[0], np.minimum(colors[0] * 1.3, 255)])
    positions = np.linspace(0.0, 1.0, len(colors))
    wanted = np.linspace(low, high, count)
    return [tuple(int(round(float(np.interp(point, positions, colors[:, channel])))) for channel in range(3)) for point in wanted]


def vividness(ramp):
    """How much a ramp glows: its share of the pixels weighted by how saturated and bright its colors are."""
    return sum(share * colorsys.rgb_to_hsv(*(color / 255.0))[1] * luma(color) for color, share in ramp)


def assign(ramps, forced):
    """Which ramp feeds which role: the heaviest for stone, the next one for rock when it holds a tenth of the pixels, the most vivid for plants and liquid, unless told otherwise."""
    by_glow = sorted(range(len(ramps)), key=lambda position: -vividness(ramps[position]))
    pick = lambda order, rank: order[min(rank, len(order) - 1)]
    heavy = [position for position, ramp in enumerate(ramps) if position and sum(share for _, share in ramp) >= 0.1]
    roles = {"stone": 0, "rock": heavy[0] if heavy else 0, "plant": by_glow[0], "liquid": pick(by_glow, 1), "stem": pick(by_glow, 2), "accent": pick(by_glow, 1)}
    roles.update(forced)
    return roles


def build(ramps, roles):
    """The standard letters of a palette, filled from the ramps of the roles."""
    scale = lambda color, share: tuple(int(round(channel * share)) for channel in color)
    stone, rock = tones(ramps[roles["stone"]], 6), tones(ramps[roles["rock"]], 5, 0.0, 0.8)
    plant, liquid = tones(ramps[roles["plant"]], 5, 0.25, 1.0), tones(ramps[roles["liquid"]], 6)
    stem, accent = tones(ramps[roles["stem"]], 4, 0.0, 0.7), tones(ramps[roles["accent"]], 2, 0.8, 1.0)
    brightest = max((color for ramp in ramps for color, _ in ramp), key=luma)
    near = dict(zip("012345", stone))
    near.update({"t": scale(plant[0], 0.55), "T": scale(plant[0], 0.8)})
    near.update(zip("abcde", plant))
    near.update(zip("pqrs", stem))
    near.update({"k": accent[0], "K": accent[1], "o": tuple(int(round(v)) for v in brightest), "n": scale(brightest, 0.25)})
    near.update(zip("uvwxyz", liquid))
    near.update({"g": tuple((a + b) // 2 for a, b in zip(liquid[4], liquid[5])), "h": tuple((a + b) // 2 for a, b in zip(liquid[2], liquid[3]))})
    near.update(zip("ABDEF", rock))
    return near


def palette_of(near, outline, tints):
    """The palette part of a data file: the play plane, the share that darkens the second plane, the backdrop and the far tones derived from them, and the tints kept."""
    backdrop = [int(round(channel * 0.9)) for channel in near["0"]]
    toward = lambda color, share: [int(round(backdrop[c] + (color[c] - backdrop[c]) * share)) for c in range(3)]
    far_b = toward(near["3"], MID_SHARE)
    return {"near": {key: list(color) for key, color in near.items()}, "mid_share": MID_SHARE, "backdrop": backdrop, "far_a": toward(far_b, FAR_SHARE), "far_b": far_b,
            "far_lit_keys": FAR_LIT_KEYS, "outline": outline, "tints": tints}


def main():
    """Command line: reads pictures, prints the ramps it found and the role each one got, and writes the palette of the biome."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("paths", nargs="+", help="pictures, or folders of pictures, whose colors the biome takes")
    add_options(parser)
    parser.add_argument("--assign", default="", help="comma-separated role=ramp pairs that overrule the choice, for example stone=1,liquid=2; roles: " + ", ".join(ROLES))
    parser.add_argument("--out", help="file to write the palette alone into, instead of the data file of the biome")
    arguments = parser.parse_args()
    data = biome_file.read(arguments.biomes, arguments.biome)
    ramps = find_ramps(collect(arguments.paths))
    forced = {role: int(position) for role, position in (pair.split("=") for pair in arguments.assign.split(",") if pair)}
    unknown = [role for role, position in forced.items() if role not in ROLES or not 0 <= position < len(ramps)]
    if unknown:
        parser.error(f"--assign names unknown roles or ramps: {unknown}; {len(ramps)} ramp(s) were found")
    roles = assign(ramps, forced)
    for position, ramp in enumerate(ramps):
        fed = [role for role in ROLES if roles[role] == position]
        dark, light = (tuple(int(v) for v in ramp[end][0]) for end in (0, -1))
        print(f"ramp {position}: {sum(share for _, share in ramp):5.1%} of the pixels, {len(ramp):2d} colors from {dark} to {light}  ->  {', '.join(fed) or 'unused'}")
    old = data["palette"]
    palette = palette_of(build(ramps, roles), old.get("outline", OUTLINE), old.get("tints", {}))
    if arguments.out:
        with open(arguments.out, "w", encoding="utf-8") as handle:
            handle.write(biome_file.render(palette) + "\n")
    else:
        biome_file.write(arguments.biomes, arguments.biome, dict(data, palette=palette))
    target = arguments.out or os.path.join(arguments.biomes, arguments.biome, biome_file.FILE)
    print(f"palette written in {target}; tints kept from the old one: {', '.join(palette['tints']) or 'none'}")


if __name__ == "__main__":
    main()
