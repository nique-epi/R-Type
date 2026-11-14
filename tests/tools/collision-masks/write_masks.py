"""Writes the [gameplay.mask] table of every entity sheet from its sprite.

Usage: python3 write_masks.py <content folder>

The content folder is the assets folder or a resource: the one that holds
entities/ and the sprite sheets those sheets name. A pixel of a mask is drawn
when it is drawn (alpha above 0) in at least one frame of [appearance.idle] or
[appearance.enraged_idle], kept after peeling `erosion` pixels of outline (the
erosion the sheet already gives, 0 without one), and inside [gameplay.hitbox].
The test EntityAppearances.EveryShippedMaskIsWhatItsSpriteDraws checks the
same rule on the assets. Needs Pillow: pip install pillow.
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


def mask_block(sheet, root):
    gameplay, appearance = sheet["gameplay"], sheet["appearance"]
    if "box" in gameplay["hitbox"]:
        raise SystemExit("a sheet with several hitbox boxes takes no mask")
    erosion = gameplay.get("mask", {}).get("erosion", 0)
    image = Image.open(root / appearance["sheet"]).convert("RGBA")
    mask = peeled(drawn_union(image, appearance), erosion)
    (left, right), (top, bottom) = hitbox_pixels(
        gameplay["hitbox"], len(mask[0]), len(mask))
    rows = ["".join("#" if mask[y][x] and left <= x < right
                    and top <= y < bottom else "."
                    for x in range(len(mask[0])))
            for y in range(len(mask))]
    lines = ["[gameplay.mask]"]
    lines += [f"erosion = {erosion}"] if erosion else []
    lines += ["rows = ["] + [f'  "{row}",' for row in rows] + ["]", ""]
    return "\n".join(lines)


def rewrite(path, root):
    text = path.read_text(encoding="utf-8")
    try:
        block = mask_block(tomllib.loads(text), root)
    except SystemExit as stop:
        raise SystemExit(f"{path.name}: {stop}") from None
    if MASK_BLOCK.search(text):
        text = MASK_BLOCK.sub(lambda _: block + "\n", text, count=1)
    elif text.count(APPEARANCE_HEADER) == 1:
        text = text.replace(APPEARANCE_HEADER,
                            "\n" + block + APPEARANCE_HEADER, 1)
    else:
        raise SystemExit(f"{path.name}: no single [appearance] header")
    path.write_text(text, encoding="utf-8")


def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: python3 write_masks.py <content folder>")
    root = pathlib.Path(sys.argv[1])
    for path in sorted((root / "entities").glob("*.toml")):
        rewrite(path, root)


if __name__ == "__main__":
    main()
