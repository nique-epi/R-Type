"""Writes the collision mask of every entity sheet from its sprite.

Usage: python3 write_masks.py <content folder>

The content folder is the assets folder or a resource: the one that holds
entities/ and the sprite sheets those sheets name. For each sheet, the mask is
written to entities/masks/<name of the sheet>.txt, one line of # (drawn) and .
(clear) per row of pixels, and the sheet names it in its [gameplay.mask] table.
A pixel of a mask is drawn when it is drawn (alpha above 0) in at least one
frame of [appearance.idle] or [appearance.enraged_idle], kept after peeling
`erosion` pixels of outline (the erosion the sheet already gives, 0 without
one), and inside [gameplay.hitbox].

A look item, which only makes a pilot's ship look different, takes no mask: it
keeps its hitbox, and the tool removes any mask it had. A mask file whose sheet
takes no mask is removed. The test
EntityAppearances.EveryShippedMaskIsWhatItsSpriteDraws checks the same rule on
the assets. Needs Pillow: pip install pillow.
"""
import pathlib
import re
import sys
import tomllib

from PIL import Image

PIXEL_UNITS = 4
NEIGHBOURS = ((1, 0), (-1, 0), (0, 1), (0, -1))
MASK_BLOCK = re.compile(r"^\[gameplay\.mask\]\n(?:(?!\[).*\n)*", re.MULTILINE)
APPEARANCE_HEADER = "\n[appearance]\n"
MASK_FOLDER = "masks"
MASK_EXTENSION = ".txt"
LOOK_ITEM_SIDE = "look_item"


def frames_of(animation):
    step = animation["width"] + animation.get("spacing", 0)
    return [(animation["left"] + index * step, animation["top"])
            for index in range(animation.get("frames", 1))]


def drawn_union(image, appearance):
    idle = appearance["idle"]
    if "anchor_x" in idle or "anchor_y" in idle:
        raise SystemExit("an idle frame with an anchor is not supported")
    frames = frames_of(idle)
    if "enraged_idle" in appearance:
        frames += frames_of(appearance["enraged_idle"])
    pixels = image.load()
    return [[any(pixels[left + x, top + y][3] > 0 for left, top in frames)
             for x in range(idle["width"])] for y in range(idle["height"])]


def peeled(mask, times):
    for _ in range(times):
        height, width = len(mask), len(mask[0])
        mask = [[mask[y][x] and all(
            0 <= x + dx < width and 0 <= y + dy < height
            and mask[y + dy][x + dx] for dx, dy in NEIGHBOURS)
            for x in range(width)] for y in range(height)]
    return mask


def hitbox_pixels(hitbox, width, height):
    edges = []
    for size, offset, side in (
            (hitbox["width"], hitbox.get("offset_x", 0), width),
            (hitbox["height"], hitbox.get("offset_y", 0), height)):
        low = side * PIXEL_UNITS / 2 + offset - size / 2
        high = low + size
        if low % PIXEL_UNITS or high % PIXEL_UNITS:
            raise SystemExit("the hitbox does not fall on whole pixels")
        edges.append((int(low) // PIXEL_UNITS, int(high) // PIXEL_UNITS))
    return edges


def mask_rows(sheet, root, erosion):
    gameplay, appearance = sheet["gameplay"], sheet["appearance"]
    if "box" in gameplay["hitbox"]:
        raise SystemExit("a sheet with several hitbox boxes takes no mask")
    image = Image.open(root / appearance["sheet"]).convert("RGBA")
    mask = peeled(drawn_union(image, appearance), erosion)
    (left, right), (top, bottom) = hitbox_pixels(
        gameplay["hitbox"], len(mask[0]), len(mask))
    return ["".join("#" if mask[y][x] and left <= x < right
                    and top <= y < bottom else "."
                    for x in range(len(mask[0])))
            for y in range(len(mask))]


def mask_block(name, erosion):
    lines = ["[gameplay.mask]",
             f'file = "{MASK_FOLDER}/{name}{MASK_EXTENSION}"']
    lines += [f"erosion = {erosion}"] if erosion else []
    return "\n".join(lines + [""])


def with_block(text, block):
    if MASK_BLOCK.search(text):
        return MASK_BLOCK.sub(lambda _: block + ("\n" if block else ""),
                              text, count=1)
    if not block:
        return text
    if text.count(APPEARANCE_HEADER) != 1:
        raise SystemExit("no single [appearance] header")
    return text.replace(APPEARANCE_HEADER, "\n" + block + APPEARANCE_HEADER, 1)


def rewrite(path, root):
    """Writes the mask of the sheet at path; returns its file, or None."""
    text = path.read_text(encoding="utf-8")
    sheet = tomllib.loads(text)
    try:
        if sheet["gameplay"].get("side") == LOOK_ITEM_SIDE:
            path.write_text(with_block(text, ""), encoding="utf-8")
            return None
        erosion = sheet["gameplay"].get("mask", {}).get("erosion", 0)
        rows = mask_rows(sheet, root, erosion)
        text = with_block(text, mask_block(path.stem, erosion))
    except SystemExit as stop:
        raise SystemExit(f"{path.name}: {stop}") from None
    mask_file = path.parent / MASK_FOLDER / (path.stem + MASK_EXTENSION)
    mask_file.parent.mkdir(exist_ok=True)
    mask_file.write_text("\n".join(rows) + "\n", encoding="utf-8")
    path.write_text(text, encoding="utf-8")
    return mask_file


def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: python3 write_masks.py <content folder>")
    sheets = pathlib.Path(sys.argv[1]) / "entities"
    written = {rewrite(path, sheets.parent)
               for path in sorted(sheets.glob("*.toml"))}
    for mask_file in (sheets / MASK_FOLDER).glob("*" + MASK_EXTENSION):
        if mask_file not in written:
            mask_file.unlink()


if __name__ == "__main__":
    main()
