"""Drawing helpers shared by the painters, the heroes and the map: index canvases, masks, stepped noise, stone blocks."""
import numpy as np

BAYER = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]) / 16.0


def stamp(canvas, sprite, x, y):
    """Draws the opaque pixels of an index sprite on an index canvas, clipped."""
    h, w = sprite.shape
    x0, y0, x1, y1 = max(x, 0), max(y, 0), min(x + w, canvas.shape[1]), min(y + h, canvas.shape[0])
    if x0 >= x1 or y0 >= y1:
        return
    part = sprite[y0 - y:y1 - y, x0 - x:x1 - x]
    region = canvas[y0:y1, x0:x1]
    region[part > 0] = part[part > 0]


def shift(mask, dy, dx, fill=True):
    """Mask moved by (dy, dx), the uncovered border taking the fill value."""
    out = np.full_like(mask, fill)
    h, w = mask.shape
    ys, yd = (slice(0, h - dy), slice(dy, h)) if dy >= 0 else (slice(-dy, h), slice(0, h + dy))
    xs, xd = (slice(0, w - dx), slice(dx, w)) if dx >= 0 else (slice(-dx, w), slice(0, w + dx))
    out[yd, xd] = mask[ys, xs]
    return out


def flood(mask, y, x):
    """Pixels of a mask reachable from one of its pixels through its pixels, side by side."""
    reach = np.zeros_like(mask)
    reach[y, x] = mask[y, x]
    count = 0
    while int(reach.sum()) != count:
        count = int(reach.sum())
        reach = (reach | shift(reach, 1, 0, False) | shift(reach, -1, 0, False) | shift(reach, 0, 1, False) | shift(reach, 0, -1, False)) & mask
    return reach


def erosion_depth(solid, limit):
    """Distance of each solid pixel to the nearest open pixel, capped at limit + 1."""
    depth = np.zeros(solid.shape, np.int16)
    current = solid.copy()
    for level in range(1, limit + 1):
        inner = current & shift(current, 1, 0) & shift(current, -1, 0) & shift(current, 0, 1) & shift(current, 0, -1)
        depth[current & ~inner] = level
        current = inner
    depth[current] = limit + 1
    return depth


def stepped_noise(length, rng, spans, values):
    """Random value held over random spans, so an edge moves in small steps and never along a curve."""
    out = np.zeros(length, int)
    x = 0
    while x < length:
        span = int(rng.integers(spans[0], spans[1] + 1))
        out[x:x + span] = int(rng.choice(values))
        x += span
    return out


def stepped_bit(x, seed):
    """Stable pseudo-random bit per group of six columns, used to break a cover into patches."""
    return ((x // 6) * 2654435761 + seed * 40503) % 7 < 4


def brick(biome, stone, width, height, rng, chips=True, bevel=2):
    """One stone block with a lit top-left edge, a dithered shadow and a few chips, in the letters of a stone; bevel 1 thins its edges."""
    rim, top, light, face, shade, dark = (stone[role] for role in ("rim", "top", "light", "face", "shade", "dark"))
    rows = []
    for y in range(height):
        if y == 0:
            row = [rim] + [top] * (width - 3) + [light, shade]
        elif y == height - 1:
            row = [shade] + [dark] * (width - 1)
        elif y == height - 2 and height >= 4 and bevel >= 2:
            row = [light, face] + [shade] * (width - 3) + [dark]
        elif y == 1 and height >= 5 and bevel >= 2:
            row = [top] + [light] * (width - 4) + [face, shade, dark]
        else:
            row = [top, light] + [face] * (width - 4) + [shade, dark]
        rows.append(row)
    depth = min(height // 2, 5)
    for y in range(2, height - 2):
        for x in range(2, width - 2):
            if (x - (width - 3)) + (y - (height - 3)) >= -depth and (x + y) % 2 == 0 and rows[y][x] == face:
                rows[y][x] = shade
    if chips and width >= 8 and height >= 8:
        for _ in range(max(width * height // 70, 1)):
            x, y = int(rng.integers(2, max(width * 2 // 3, 3))), int(rng.integers(2, max(height * 2 // 3, 3)))
            if rows[y][x] == face and rows[y][x + 1] == face:
                rows[y][x] = rows[y][x + 1] = light
        for _ in range(max(width * height // 90, 1)):
            x, y = int(rng.integers(3, width - 4)), int(rng.integers(3, height - 3))
            if rows[y][x] == face and rows[y][x + 1] == face:
                rows[y][x] = rows[y][x + 1] = shade
    return biome.grid(["".join(row) for row in rows])
