#!/usr/bin/env python3
"""Writes the stand-in decor of the biomes: one PNG per parallax layer.

The layers are made of the planets of assets/sprites/sector_map.png, placed
by a seeded draw, so two runs write the same files. Each layer is as tall as
the sky of the client (270 pixels) and two skies wide (960 pixels), and is
repeated side by side as the level scrolls. They stand in for the decor drawn
for each biome, and are exported the way a level editor exports its layers:
one image per layer, back to front.

Run it from the repository root, with Pillow installed:

    python3 tests/tools/biome-layers/make_biome_layers.py
"""

import random
from pathlib import Path

from PIL import Image

REPOSITORY = Path(__file__).resolve().parents[3]
SHEET = REPOSITORY / "assets" / "sprites" / "sector_map.png"
OUTPUT = REPOSITORY / "assets" / "backgrounds"

LAYER_SIZE = (960, 270)
SPRITE_SPACING = 2
FRAMES_PER_PLANET = 8

# The rows of the sheet: the top and the side of a planet, in pixels.
SMALL_PLANET = (0, 5)
MEDIUM_PLANET = (7, 7)
LARGE_PLANET = (16, 9)

# A layer: the planet it is made of, how many times it is enlarged, how many
# it holds, and the share of its light it keeps, in percent.
LAYERS = {
    "far": (SMALL_PLANET, 1, 90, 45),
    "middle": (MEDIUM_PLANET, 2, 26, 70),
    "near": (LARGE_PLANET, 5, 7, 100),
}

# A biome: the seed of its draws, and whether its planets swap red and blue,
# which turns the blue planets of the sheet into embers.
BIOMES = {
    "drift_fields": (2611, False),
    "ember_reach": (1987, True),
}


def planet_frames(sheet, row):
    top, side = row
    step = side + SPRITE_SPACING
    return [
        sheet.crop((index * step, top, index * step + side, top + side))
        for index in range(FRAMES_PER_PLANET)
    ]


def dimmed(image, percent):
    red, green, blue, alpha = image.split()
    scale = lambda value: value * percent // 100
    return Image.merge(
        "RGBA", (red.point(scale), green.point(scale), blue.point(scale), alpha)
    )


def embers(image):
    red, green, blue, alpha = image.split()
    return Image.merge("RGBA", (blue, green.point(lambda value: value // 2), red, alpha))


def layer(sheet, description, draw, as_embers):
    row, enlarged, count, light = description
    picture = Image.new("RGBA", LAYER_SIZE, (0, 0, 0, 0))
    frames = planet_frames(sheet, row)
    side = row[1] * enlarged
    for _ in range(count):
        planet = draw.choice(frames).resize((side, side), Image.NEAREST)
        planet = dimmed(planet, light)
        if as_embers:
            planet = embers(planet)
        place = (
            draw.randrange(0, LAYER_SIZE[0] - side),
            draw.randrange(0, LAYER_SIZE[1] - side),
        )
        picture.alpha_composite(planet, place)
    return picture


def main():
    sheet = Image.open(SHEET).convert("RGBA")
    OUTPUT.mkdir(parents=True, exist_ok=True)
    for biome, (seed, as_embers) in BIOMES.items():
        draw = random.Random(seed)
        for name, description in LAYERS.items():
            layer(sheet, description, draw, as_embers).save(
                OUTPUT / f"{biome}_{name}.png", optimize=True
            )


if __name__ == "__main__":
    main()
