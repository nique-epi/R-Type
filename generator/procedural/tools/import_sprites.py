"""Brings pictures into a biome as sprites: crops them, snaps every pixel to the palette of the biome, softens the lit outline, updates the index."""
import argparse
import hashlib
import importlib
import json
import os
import sys

import numpy as np
from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT)


def runs(flags):
    """Stretches of consecutive true values, as (start, end) pairs."""
    out, start = [], None
    for index, flag in enumerate(flags):
        if flag and start is None:
            start = index
        if not flag and start is not None:
            out.append((start, index))
            start = None
    if start is not None:
        out.append((start, len(flags)))
    return out


def snap(rgb, palette, keys):
    """Each pixel moved to the nearest color among the letters allowed for it, by weighted distance."""
    colors = np.array([palette[k] for k in keys], float)
    weights = np.array([0.30, 0.59, 0.11]) ** 0.5
    flat = rgb.reshape(-1, 3).astype(float)
    distance = (((flat[:, None, :] - colors[None, :, :]) * weights) ** 2).sum(axis=2)
    nearest = distance.argmin(axis=1)
    return colors[nearest].reshape(rgb.shape).astype(np.uint8)


def soften_outline(rgba, palette, outline):
    """Replaces the near-black rim on the lit side of a sprite by the rim color of the biome."""
    alpha = rgba[..., 3] > 0
    rgb = rgba[..., :3]
    dark = alpha & (rgb.astype(int).sum(axis=2) <= sum(palette[outline["dark_through"]]) + 6)
    above = np.zeros_like(alpha)
    above[1:] = alpha[:-1]
    left = np.zeros_like(alpha)
    left[:, 1:] = alpha[:, :-1]
    below = np.zeros_like(alpha)
    below[:-1] = alpha[1:]
    lit_rim = dark & (~above | ~left) & below
    rgb[lit_rim] = palette[outline["rim"]]
    return rgba


def to_sprite(cell, palette, keys, outline):
    """One picture cut to its opaque box, snapped to the palette, its lit outline softened when the biome says how."""
    ys, xs = np.where(cell[..., 3] > 0)
    cell = cell[ys.min():ys.max() + 1, xs.min():xs.max() + 1].copy()
    cell[..., :3] = snap(cell[..., :3], palette, keys)
    cell[cell[..., 3] == 0] = 0
    return soften_outline(cell, palette, outline) if outline else cell


def cells_of(rgba, names):
    """Named cells of a sheet, found by its empty gutters; a name that starts with - is left out."""
    alpha = rgba[..., 3] > 0
    row_runs, col_runs = runs(alpha.any(axis=1)), runs(alpha.any(axis=0))
    assert len(row_runs) == len(names) and len(col_runs) == len(names[0]), f"the sheet has {len(row_runs)} x {len(col_runs)} cells, {len(names)} x {len(names[0])} names were given"
    for (y0, y1), row in zip(row_runs, names):
        for (x0, x1), name in zip(col_runs, row):
            yield name, rgba[y0:y1, x0:x1]


def preview(sprites, backdrop, file):
    """Every sprite on two rows, standing on a common base line, three times enlarged."""
    names = sorted(sprites)
    half = (len(names) + 1) // 2
    rows = [names[:half], names[half:]]
    sheet_w = max(sum(sprites[n].shape[1] + 4 for n in row) for row in rows) + 4
    sheet_h = sum(max(sprites[n].shape[0] for n in row) + 6 for row in rows)
    board = np.zeros((sheet_h, sheet_w, 4), np.uint8)
    board[..., :3] = backdrop
    board[..., 3] = 255
    y = 3
    for row in rows:
        tall = max(sprites[n].shape[0] for n in row)
        x = 4
        for n in row:
            s = sprites[n]
            h, w = s.shape[:2]
            region = board[y + tall - h:y + tall, x:x + w]
            m = s[..., 3] > 0
            region[m] = s[m]
            x += w + 4
        y += tall + 6
    Image.fromarray(board).resize((sheet_w * 3, sheet_h * 3), Image.NEAREST).save(file)


def sha12(file):
    """First twelve characters of the SHA-256 of a file."""
    with open(file, "rb") as handle:
        return hashlib.sha256(handle.read()).hexdigest()[:12]


def import_sheets(biome, folder, target, palette, outline):
    """Cuts every sheet the biome describes in sheets.py, and rewrites its index, its manifest and its preview."""
    sheets = importlib.import_module(f"biomes.{biome}.sheets")
    manifest, kept, rejected, sprites = [], [], [], {}
    for sheet, spec in sheets.SHEETS.items():
        file = os.path.join(folder, "raw", sheet + ".png")
        manifest.append({"asset_id": spec["asset_id"], "sha12": sha12(file), "label": spec["label"], "file": f"raw/{sheet}.png"})
        for name, cell in cells_of(np.asarray(Image.open(file).convert("RGBA")).copy(), spec["cells"]):
            if name.startswith("-"):
                rejected.append(name[1:])
                continue
            sprites[name] = to_sprite(cell, palette, spec.get("keys_for", {}).get(name, spec["keys"]), None if spec.get("keep_outline") else outline)
            Image.fromarray(sprites[name]).save(os.path.join(target, "sprites", name + ".png"))
            kept.append(name)
    for name, spec in sheets.TEXTURES.items():
        file = os.path.join(folder, "raw", name + ".png")
        manifest.append({"asset_id": spec["asset_id"], "label": spec["label"], "sha12": sha12(file), "file": f"raw/{name}.png"})
        Image.fromarray(snap(np.asarray(Image.open(file).convert("RGB")), palette, spec["keys"])).save(os.path.join(target, "sprites", name + ".png"))
        kept.append(name)
    for spec in sheets.REFERENCES.values():
        manifest.append({"asset_id": spec["asset_id"], "label": spec["label"], "sha12": spec["sha12"]})
    with open(os.path.join(target, "spritecook-assets.json"), "w") as handle:
        json.dump(manifest, handle, indent=2)
    with open(os.path.join(target, "sprites.json"), "w") as handle:
        json.dump({"kept": kept, "rejected": rejected}, handle, indent=2)
    return sprites


def render_drawn(biome, folder, palette):
    """Writes the sprites a biome draws in palette letters in drawn.py as pictures, and returns their files."""
    drawn = importlib.import_module(f"biomes.{biome}.drawn").DRAWN
    os.makedirs(os.path.join(folder, "raw", "drawn"), exist_ok=True)
    files = []
    for stem, rows in drawn.items():
        picture = np.zeros((len(rows), len(rows[0]), 4), np.uint8)
        for y, row in enumerate(rows):
            for x, letter in enumerate(row):
                if letter != ".":
                    picture[y, x] = (*palette[letter], 255)
        files.append(os.path.join(folder, "raw", "drawn", stem + ".png"))
        Image.fromarray(picture).save(files[-1])
    return files


def import_files(paths, arguments, target, palette, outline):
    """Imports single pictures, or the named cells of one sheet, and adds them to the index."""
    files = []
    for path in paths:
        files += sorted(os.path.join(path, name) for name in os.listdir(path) if name.endswith(".png")) if os.path.isdir(path) else [path]
    keys = arguments.keys or "".join(palette)
    sprites = {}
    for file in files:
        rgba = np.asarray(Image.open(file).convert("RGBA")).copy()
        if arguments.grid:
            columns, rows = (int(v) for v in arguments.grid.split("x"))
            names = arguments.names.split(",")
            assert len(names) == columns * rows, f"--grid {arguments.grid} needs {columns * rows} names, {len(names)} were given"
            cells = cells_of(rgba, [names[row * columns:(row + 1) * columns] for row in range(rows)])
        else:
            cells = [(os.path.basename(file)[:-4], rgba)]
        for name, cell in cells:
            if not name.startswith("-"):
                sprites[name] = to_sprite(cell, palette, keys, None if arguments.keep_outline else outline)
                Image.fromarray(sprites[name]).save(os.path.join(target, "sprites", name + ".png"))
    index_file = os.path.join(target, "sprites.json")
    index = {"kept": [], "rejected": []}
    if os.path.exists(index_file):
        with open(index_file) as handle:
            index = json.load(handle)
    index["kept"] += [name for name in sprites if name not in index["kept"]]
    with open(index_file, "w") as handle:
        json.dump(index, handle, indent=2)
    return sprites


def main():
    """Command line: imports pictures, a sheet in a grid, the sheets of sheets.py or the letter drawings of drawn.py into a biome."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("paths", nargs="*", help="pictures with a transparent background, or folders of them; the file name becomes the sprite name")
    parser.add_argument("--biome", default="fosse")
    parser.add_argument("--grid", help="the picture is a sheet of COLUMNSxROWS cells separated by empty gutters, for example 4x4")
    parser.add_argument("--names", help="comma-separated names of the cells of the sheet, row by row; a name that starts with - is left out")
    parser.add_argument("--keys", help="palette letters the pixels may snap to (default: every letter)")
    parser.add_argument("--keep-outline", action="store_true", help="leave the dark outline as it is")
    parser.add_argument("--sheets", action="store_true", help="cut the sheets the biome describes in sheets.py")
    parser.add_argument("--drawn", action="store_true", help="render the sprites the biome draws in letters in drawn.py, then import them")
    parser.add_argument("--out", help="folder to write into instead of the folder of the biome")
    arguments = parser.parse_args()
    folder = os.path.join(ROOT, "biomes", arguments.biome)
    target = arguments.out or folder
    palette_module = importlib.import_module(f"biomes.{arguments.biome}.palette")
    palette, outline = palette_module.NEAR, getattr(palette_module, "OUTLINE", None)
    os.makedirs(os.path.join(target, "sprites"), exist_ok=True)
    if arguments.sheets:
        sprites = import_sheets(arguments.biome, folder, target, palette, outline)
        preview(sprites, palette_module.BACKDROP, os.path.join(target, "_preview_sprites.png"))
    else:
        paths = list(arguments.paths) + (render_drawn(arguments.biome, folder, palette) if arguments.drawn else [])
        if not paths:
            parser.error("give pictures or folders, or --sheets, or --drawn")
        sprites = import_files(paths, arguments, target, palette, outline)
    for name in sorted(sprites):
        print(f"{name:40s} {sprites[name].shape[1]:3d} x {sprites[name].shape[0]:3d}")
    print(f"{len(sprites)} sprite(s) written in {os.path.join(target, 'sprites')}")


if __name__ == "__main__":
    main()
