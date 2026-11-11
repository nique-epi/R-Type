#!/usr/bin/env python3
"""Writes the decor of the biomes: one PNG per parallax layer.

Each biome has four layers, from the farthest to the nearest: a haze of
nebulae, small far bodies, middle rocks, and rare near silhouettes along the
top and the bottom of the sky. Every layer is as tall as the sky of the client
(270 pixels) and two skies wide (960 pixels), repeats side by side without a
seam, and is drawn with the dark palette of its biome, so the ships and the
missiles of the playfield stay the brightest things on screen. Seeded draws
make two runs write the same files.

Run it from the repository root, with Pillow installed:

    python3 tests/tools/biome-layers/make_biome_layers.py [output folder]
"""

import math
import random
import sys
from dataclasses import dataclass
from pathlib import Path

from PIL import Image

REPOSITORY = Path(__file__).resolve().parents[3]
OUTPUT = REPOSITORY / "assets" / "backgrounds"

WIDTH = 960
HEIGHT = 270

# The 2 x 2 Bayer matrix the planets of the client dither with.
BAYER = ((0, 2), (3, 1))

# The light falls from the top left, as on the planet of the sky.
LIGHT = (-0.55, -0.45, 0.7)


@dataclass(frozen=True)
class Biome:
    seed: int
    haze: tuple  # nebula shades, darkest first, with their alpha
    body: tuple  # far bodies, dark side to lit side
    rock: tuple  # middle rocks, dark side to lit side
    silhouette: tuple  # near silhouettes: fill, rim
    glow: tuple  # the few lit pixels of the near layer
    shape: str  # a key of EDGE_SHAPES


BIOMES = {
    "drift_fields": Biome(
        seed=2611,
        haze=((24, 36, 82, 70), (34, 52, 112, 105), (52, 76, 150, 135)),
        body=((22, 28, 60), (40, 52, 98), (70, 88, 140)),
        rock=((20, 24, 46), (36, 44, 76), (58, 70, 110)),
        silhouette=((12, 15, 32), (30, 38, 70)),
        glow=(96, 150, 210),
        shape="rocks",
    ),
    "ember_reach": Biome(
        seed=1987,
        haze=((60, 18, 30, 70), (92, 28, 34, 105), (132, 48, 36, 135)),
        body=((48, 20, 26), (88, 34, 34), (140, 64, 44)),
        rock=((34, 16, 20), (62, 28, 28), (98, 46, 36)),
        silhouette=((24, 10, 14), (60, 24, 22)),
        glow=(214, 104, 44),
        shape="spires",
    ),
    "hive_depths": Biome(
        seed=4421,
        haze=((16, 46, 38, 70), (26, 70, 50, 105), (44, 100, 62, 135)),
        body=((18, 40, 34), (32, 70, 52), (60, 108, 70)),
        rock=((16, 30, 26), (28, 52, 40), (46, 80, 54)),
        silhouette=((14, 34, 26), (44, 90, 58)),
        glow=(150, 214, 96),
        shape="pods",
    ),
}


def periodic_noise(draw, cells_across, cells_down):
    """Value noise that repeats every WIDTH pixels across."""
    grid = [[draw.random() for _ in range(cells_across)] for _ in range(cells_down + 1)]

    def value(x, y):
        fx = (x % WIDTH) / WIDTH * cells_across
        fy = y / HEIGHT * cells_down
        left, top = int(fx), min(int(fy), cells_down - 1)
        right = (left + 1) % cells_across
        tx, ty = fx - int(fx), fy - top
        tx, ty = tx * tx * (3 - 2 * tx), ty * ty * (3 - 2 * ty)
        upper = grid[top][left] * (1 - tx) + grid[top][right] * tx
        lower = grid[top + 1][left] * (1 - tx) + grid[top + 1][right] * tx
        return upper * (1 - ty) + lower * ty

    return value


def layered_noise(draw, octaves):
    noises = [(periodic_noise(draw, across, down), weight) for across, down, weight in octaves]
    total = sum(weight for _, _, weight in octaves)
    return lambda x, y: sum(noise(x, y) * weight for noise, weight in noises) / total


def dithered(level, shades_count, x, y):
    """Picks a shade for a level in [0, 1], mixing two neighbours in a checkerboard."""
    scaled = max(0.0, min(0.999, level)) * (shades_count - 1)
    lower = int(scaled)
    threshold = (BAYER[y % 2][x % 2] + 0.5) / 4
    return min(shades_count - 1, lower + (1 if scaled - lower > threshold else 0))


def blank_layer():
    picture = Image.new("RGBA", (WIDTH, HEIGHT), (0, 0, 0, 0))
    return picture, picture.load()


def circle(radius):
    return lambda angle: radius


def put(pixels, x, y, color):
    if 0 <= y < HEIGHT:
        pixels[x % WIDTH, y] = color if len(color) == 4 else (*color, 255)


def shaded_body(pixels, centre, radius_at, shades):
    """A lit body whose edge lies radius_at(angle) from its centre."""
    cx, cy = centre
    reach = int(max(radius_at(angle / 16 * math.tau) for angle in range(16))) + 2
    for y in range(cy - reach, cy + reach + 1):
        for x in range(cx - reach, cx + reach + 1):
            dx, dy = x - cx, y - cy
            distance = math.hypot(dx, dy)
            edge = radius_at(math.atan2(dy, dx))
            if distance > edge:
                continue
            nx, ny = dx / edge, dy / edge
            nz = math.sqrt(max(0.0, 1 - nx * nx - ny * ny))
            light = max(0.0, nx * LIGHT[0] + ny * LIGHT[1] + nz * LIGHT[2])
            put(pixels, x, y, shades[dithered(light, len(shades), x, y)])


# The haze blends its three colors into this many shades, so its steps stay soft.
HAZE_SHADES = 7


def blended(colors, count):
    """count colors evenly spread along the given ones, darkest first."""
    result = []
    for index in range(count):
        place = index / (count - 1) * (len(colors) - 1)
        lower = min(int(place), len(colors) - 2)
        share = place - lower
        result.append(tuple(round(a + (b - a) * share) for a, b in zip(colors[lower], colors[lower + 1])))
    return result


def haze_layer(biome, draw):
    picture, pixels = blank_layer()
    cloud = layered_noise(draw, ((6, 2, 4), (12, 4, 2), (24, 8, 1)))
    shades = blended(biome.haze, HAZE_SHADES)
    band = draw.uniform(0.3, 0.7)
    for y in range(HEIGHT):
        # The nebulae gather around a band of the sky and thin out above and below it.
        near_band = 1 - min(1.0, abs(y / HEIGHT - band) * 1.8)
        for x in range(WIDTH):
            level = cloud(x, y) * 0.75 + near_band * 0.45 - 0.42
            if level <= 0:
                continue
            shade = dithered(level * 1.6, len(shades) + 1, x, y)
            if shade > 0:
                put(pixels, x, y, shades[shade - 1])
    return picture


def far_layer(biome, draw):
    picture, pixels = blank_layer()
    for _ in range(14):
        radius = draw.choice((2, 3, 3, 4, 5, 7))
        centre = (draw.randrange(WIDTH), draw.randrange(12, HEIGHT - 12))
        shaded_body(pixels, centre, circle(radius), biome.body)
    return picture


def rock_outline(draw, radius, roughness):
    phases = [draw.uniform(0, math.tau) for _ in range(3)]
    return lambda angle: radius * (
        1
        + roughness * math.sin(2 * angle + phases[0])
        + roughness * 0.6 * math.sin(3 * angle + phases[1])
        + roughness * 0.3 * math.sin(5 * angle + phases[2])
    )


def middle_layer(biome, draw):
    picture, pixels = blank_layer()
    for _ in range(11):
        radius = draw.uniform(5, 12)
        centre = (draw.randrange(WIDTH), draw.randrange(16, HEIGHT - 16))
        shaded_body(pixels, centre, rock_outline(draw, radius, 0.22), biome.rock)
    return picture


# A shape of near silhouettes: how sharp its peaks are (the power the ground
# noise is raised to), and its tallest height at the top and at the bottom.
EDGE_SHAPES = {
    "rocks": (2.4, 34, 58),
    "spires": (3, 38, 70),
    "pods": (1.6, 40, 66),
}


def edge_profile(draw, shape):
    """Heights, in pixels, of the silhouette along the bottom and the top."""
    ground = layered_noise(draw, ((8, 1, 3), (32, 1, 1)))
    sharpness, top_height, bottom_height = EDGE_SHAPES[shape]
    return lambda x, top: (ground(x, 0) ** sharpness) * (top_height if top else bottom_height)


def edge_spot(draw, profile):
    """A column of the near layer, whether it is at the top, and the height of the silhouette there."""
    x = draw.randrange(WIDTH)
    top = draw.random() < 0.4
    return x, top, int(profile(x, top))


def near_layer(biome, draw):
    picture, pixels = blank_layer()
    fill, rim = biome.silhouette
    profile = edge_profile(draw, biome.shape)
    for top in (False, True):
        for x in range(WIDTH):
            height = int(profile(x, top))
            for depth in range(height):
                y = depth if top else HEIGHT - 1 - depth
                put(pixels, x, y, rim if depth == height - 1 else fill)
    # A few points of light, so the near layer reads as far off, not as a wall.
    for _ in range(18):
        x, top, height = edge_spot(draw, profile)
        if height > 6:
            depth = draw.randrange(2, height - 2)
            put(pixels, x, depth if top else HEIGHT - 1 - depth, biome.glow)
    if biome.shape == "pods":
        for _ in range(9):
            x, top, height = edge_spot(draw, profile)
            radius = draw.uniform(6, 11)
            centre = (x, height if top else HEIGHT - 1 - height)
            shaded_body(pixels, centre, circle(radius), (fill, rim, biome.rock[2]))
    return picture


LAYERS = (("haze", haze_layer), ("far", far_layer), ("middle", middle_layer), ("near", near_layer))


def main():
    output = Path(sys.argv[1]) if len(sys.argv) > 1 else OUTPUT
    output.mkdir(parents=True, exist_ok=True)
    for name, biome in BIOMES.items():
        for layer, paint in LAYERS:
            draw = random.Random(f"{biome.seed}-{layer}")
            paint(biome, draw).save(output / f"{name}_{layer}.png", optimize=True)


if __name__ == "__main__":
    main()
