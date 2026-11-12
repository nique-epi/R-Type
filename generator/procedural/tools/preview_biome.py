"""Draws a sheet of what a biome builds by rule: a swatch of each material and every generated prop of each material."""
import argparse
import os
import sys
import types

import numpy as np
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT)
from biome_loader import Biome  # noqa: E402
from painters import PAINTERS, shared_of  # noqa: E402
from props import PREFIX, SHAPES  # noqa: E402

SWATCH = (64, 96)
ROW = 72


def swatch(biome, declared, seed):
    """Block of a material with a stepped top, painted by its painter with its settings."""
    solid = np.zeros((SWATCH[0] + 8, SWATCH[1] + 16), bool)
    solid[8:, 8:8 + SWATCH[1]] = True
    solid[8:24, 8 + SWATCH[1] // 2:8 + SWATCH[1]] = False
    canvas = np.zeros(solid.shape, np.uint8)
    options = {key: value for key, value in declared["paint"].items() if key != "frame"}
    PAINTERS[declared["painter"]](types.SimpleNamespace(biome=biome, rng=np.random.default_rng(seed)), canvas, solid, shared_of(solid), options)
    canvas[~solid] = 0
    return canvas


def main():
    """Command line: writes the preview sheet of a biome, three times enlarged."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--biome", default="fosse")
    parser.add_argument("--out", default="preview.png")
    arguments = parser.parse_args()
    biome = Biome(arguments.biome)
    table = biome.lut("near")
    rows = []
    for position, (material, declared) in enumerate(biome.materials.items()):
        rows.append([swatch(biome, declared, position)] + [biome.sprites[f"{PREFIX}{material}:{shape}"] for shape in SHAPES])
    width = max(sum(sprite.shape[1] + 6 for sprite in row) for row in rows) + 6
    sheet = np.zeros((ROW * len(rows) + 6, width, 3), np.uint8)
    sheet[:] = biome.backdrop
    for position, row in enumerate(rows):
        x = 6
        for sprite in row:
            h, w = sprite.shape
            region = sheet[(position + 1) * ROW - h:(position + 1) * ROW, x:x + w]
            region[sprite > 0] = table[sprite[sprite > 0]]
            x += w + 6
    Image.fromarray(sheet).resize((width * 3, sheet.shape[0] * 3), Image.NEAREST).save(arguments.out)
    print(f"{arguments.out}: one row per material ({', '.join(biome.materials)}), a swatch then the props {', '.join(SHAPES)}")


if __name__ == "__main__":
    main()
