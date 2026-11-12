"""Registries of what paints terrain: material painters, moss covers, hanging growth and far structures, each under a name a biome refers to."""
import numpy as np

from drawing import BAYER, brick, erosion_depth, shift, stamp, stepped_bit, stepped_noise
from frame import H, HANG_LIMIT

PAINTERS, COVERS, HANGERS, FAR_STRUCTURES = {}, {}, {}, {}


def register(registry, name):
    """Decorator that records a function in a registry under a name."""
    def record(function):
        """Stores the function and gives it back unchanged."""
        registry[name] = function
        return function
    return record


def shared_of(solid):
    """What every painter needs to know about a solid mask: depth of each pixel, exposed sides, dither patterns."""
    ys, xs = np.mgrid[0:solid.shape[0], 0:solid.shape[1]]
    return {
        "solid": solid, "depth": erosion_depth(solid, 28), "ys": ys, "xs": xs,
        "air_above": ~shift(solid, 1, 0), "air_left": ~shift(solid, 0, 1), "air_below": ~shift(solid, -1, 0), "air_right": ~shift(solid, 0, -1),
        "checker": (xs + ys) % 2 == 0, "dither": BAYER[ys % 4, xs % 4],
    }


@register(PAINTERS, "cobble")
def paint_cobble(scene, canvas, mask, shared, options):
    """Packed stones from a tiling texture near the surface, fading to a dark core, with a lit rim on the top and left."""
    biome, rng = scene.biome, scene.rng
    index, depth, dither = biome.index, shared["depth"], shared["dither"]
    texture = biome.sprites[options["texture"]]
    offset = rng.integers(0, 100, 2)
    cobble = texture[(shared["ys"] + offset[0]) % texture.shape[0], (shared["xs"] + offset[1]) % texture.shape[1]]
    dark_once, dark_twice = biome.remap(cobble, options["darker"]), biome.remap(biome.remap(cobble, options["darker"]), options["darker"])
    fade = stepped_noise(mask.shape[1], rng, (6, 18), [-3, -1, 0, 0, 2, 4])[None, :]
    layer = np.full(mask.shape, index[options["core"]], np.uint8)
    layer = np.where(depth <= 24 + fade, np.where(dither < 0.5, dark_twice, index[options["core"]]), layer)
    layer = np.where(depth <= 17 + fade, dark_once, layer)
    layer = np.where(depth <= 10 + fade, cobble, layer)
    lit = (depth == 1) & (shared["air_above"] | shared["air_left"])
    layer[(depth == 1) & ~lit & (shared["air_below"] | shared["air_right"])] = index[options["shade"]]
    layer[lit] = index[options["lit"]]
    layer[lit & (rng.random(mask.shape) < 0.08)] = index[options["spark"]]
    canvas[mask] = layer[mask]


@register(PAINTERS, "soil")
def paint_soil(scene, canvas, mask, shared, options):
    """Dark soil with small blobs inside and a banded, dithered edge that gets lighter towards the open air."""
    biome, rng = scene.biome, scene.rng
    index, depth, dither, checker = biome.index, shared["depth"], shared["dither"], shared["checker"]
    blobs = [biome.grid(rows) for rows in options["blobs"]]
    canvas[mask] = index[options["core"]]
    for y in range(0, mask.shape[0], 8):
        for x in range(0, mask.shape[1], 8):
            if mask[y:y + 8, x:x + 8].all() and (depth[y:y + 8, x:x + 8] > 6).all() and rng.random() < 0.4:
                stamp(canvas, blobs[int(rng.integers(0, 3))], x, y)
    band = depth - stepped_noise(mask.shape[1], rng, (5, 16), [-2, -1, 0, 0, 1, 3])[None, :]
    lit = (depth == 1) & (shared["air_above"] | shared["air_left"])
    canvas[mask & (band <= 18) & (dither < 0.3)] = index[options["shade"]]
    canvas[mask & (band <= 13)] = index[options["shade"]]
    canvas[mask & (band <= 9) & checker] = index[options["body"]]
    canvas[mask & (band <= 6)] = index[options["body"]]
    canvas[mask & (band <= 6) & (band > 2) & (rng.random(mask.shape) < 0.05)] = index[options["edge"]]
    canvas[mask & (band <= 4) & checker] = index[options["edge"]]
    canvas[mask & (depth <= 2)] = index[options["edge"]]
    canvas[mask & lit] = index[options["lit"]]


@register(PAINTERS, "bricks")
def paint_bricks(scene, canvas, mask, shared, options):
    """Courses of stone blocks of uneven tints, darker with depth; cell size, block length, bond, bevel and chips are settings."""
    biome, rng, solid = scene.biome, scene.rng, shared["solid"]
    cell_w, cell_h = options.get("cell", (8, 8))
    span, stacked = options.get("span", 2), options.get("bond", "running") == "stack"
    dark_share, light_share = options.get("tints", (0.24, 0.86))
    bevel, chips = options.get("bevel", 2), options.get("chips", True)
    frame = options.get("frame") or {"bands": ((0, mask.shape[0] // cell_h * cell_h),), "slabs": ()}
    canvas[mask] = biome.index[options["fill"]]
    columns = mask.shape[1] // cell_w
    for top, bottom in frame["bands"]:
        rows = (bottom - top) // cell_h
        cells = mask[top:bottom, :columns * cell_w].reshape(rows, cell_h, columns, cell_w).all(axis=(1, 3))
        cell_depth = erosion_depth(solid[top:bottom, :columns * cell_w].reshape(rows, cell_h, columns, cell_w).all(axis=(1, 3)), 4)
        used = np.zeros_like(cells)
        for r in range(rows):
            for c in range(columns):
                if not cells[r, c] or used[r, c]:
                    continue
                length = 1
                if (c if stacked else c + r * (span // 2)) % span == 0:
                    while length < span and c + length < columns and cells[r, c + length] and not used[r, c + length]:
                        length += 1
                tall = 2 if length == span and r + 1 < rows and cells[r + 1, c:c + length].all() and rng.random() < options.get("tall_chance", 0.14) else 1
                used[r:r + tall, c:c + length] = True
                block = brick(biome, options["stone"], cell_w * length, cell_h * tall, rng, chips, bevel)
                tint = rng.random()
                if tint < dark_share:
                    block = biome.remap(block, options["darker"])
                elif tint > light_share:
                    block = biome.remap(block, options["lighter"])
                for _ in range(max(int(cell_depth[r, c]) - 1, 0) if shared.get("shade_depth", True) else 0):
                    block = biome.remap(block, options["darker"])
                stamp(canvas, block, c * cell_w, top + r * cell_h)
    for top, shade in frame["slabs"]:
        for x in range(0, mask.shape[1], 16):
            if mask[top:top + 4, x:x + 16].all():
                slab = brick(biome, options["stone"], 16, 4, rng, False)
                for _ in range(shade):
                    slab = biome.remap(slab, options["darker"])
                stamp(canvas, slab, x, top)


@register(PAINTERS, "plates")
def paint_plates(scene, canvas, mask, shared, options):
    """Large riveted plates with a bevelled edge, uneven tints and a few streaks, cut by the shape they fill."""
    biome, rng = scene.biome, scene.rng
    index, keys = biome.index, options["keys"]
    width, height = options.get("plate", (32, 24))
    rows, columns = mask.shape
    layer = np.full(mask.shape, index[keys["face"]], np.uint8)
    for py in range(0, rows, height):
        start = -(width // 2) if options.get("stagger", True) and (py // height) % 2 else 0
        for px in range(start, columns, width):
            tone = rng.random()
            face = keys["shade"] if tone < 0.25 else (keys["light"] if tone > 0.88 else keys["face"])
            plate = np.full((height, width), index[face], np.uint8)
            plate[0, :], plate[:, 0] = index[keys["light"]], index[keys["light"]]
            plate[1, 1:-1] = index[keys["face"] if face == keys["shade"] else keys["light"]]
            plate[-1, :], plate[:, -1] = index[keys["dark"]], index[keys["dark"]]
            plate[-2, 2:-1] = index[keys["shade"]]
            for y in range(height // 2, height - 2):
                for x in range(width // 2, width - 1):
                    if (x - width // 2) + (y - height // 2) >= height // 2 and (x + y) % 2 == 0:
                        plate[y, x] = index[keys["shade"]]
            for ry, rx in ((3, 3), (3, width - 5), (height - 5, 3), (height - 5, width - 5)):
                plate[ry:ry + 2, rx:rx + 2] = index[keys["rivet"]]
                plate[ry + 1, rx + 1] = index[keys["dark"]]
            for _ in range(int(rng.integers(0, 3))):
                sx, sy = int(rng.integers(3, width - 3)), int(rng.integers(2, height - 10))
                plate[sy:sy + int(rng.integers(3, 9)), sx] = index[keys["rust"]]
            stamp(layer, plate, px, py)
    lit = (shared["depth"] == 1) & (shared["air_above"] | shared["air_left"])
    layer[(shared["depth"] == 1) & ~lit & (shared["air_below"] | shared["air_right"])] = index[keys["dark"]]
    layer[lit] = index[keys["rim"]]
    canvas[mask] = layer[mask]


@register(COVERS, "full")
def cover_full(x, bare, seed):
    """Moss everywhere but for a few bare stretches, which keep one column in five."""
    return not (bare and x % 5)


@register(COVERS, "patches")
def cover_patches(x, bare, seed):
    """Moss in patches only."""
    return not (bare or stepped_bit(x, seed))


@register(HANGERS, "strands")
def hang_strand(cave, plane, canvas, solid, spot, keys):
    """Root strand with small side shoots, sometimes ending in a bud; the first one of a large tuft is the long one."""
    rng, index = cave.rng, cave.biome.index
    sx, y0, position, size = spot
    length = int(rng.integers(12, 34)) if position == 0 and size > 1 else int(rng.integers(3, 8 + size * 4))
    length = min(length, HANG_LIMIT - y0) if plane == "near" else length
    canvas[y0, sx - 1:sx + 2] = index[keys["root"]]
    for k in range(max(length, 0)):
        y = y0 + k
        if solid[y, sx] and k > 2:
            break
        canvas[y, sx] = index[keys["young"] if k < 3 else keys["root"]]
        if k % 5 == 3:
            side = 1 if (k // 5) % 2 else -1
            canvas[y, sx + side] = index[keys["young"]]
            canvas[y + 1, sx + side] = index[keys["root"]]
    if position == 0 and size > 2 and length > 14:
        canvas[y0 + length:y0 + length + 2, sx - 1:sx + 1] = index[keys["bloom"] if rng.random() < 0.3 else keys["bud"]]


@register(HANGERS, "teeth")
def hang_tooth(cave, plane, canvas, solid, spot, keys):
    """Short tooth of rock under a ceiling."""
    rng, index = cave.rng, cave.biome.index
    sx, y0, _, size = spot
    length = int(rng.integers(2, 5 + size * 2))
    for k in range(length):
        half = max((length - k) // 3, 0)
        canvas[y0 + k, sx - half:sx + half + 1] = index[keys["body"]]
        canvas[y0 + k, sx - half] = index[keys["edge"]]


@register(HANGERS, "drips")
def hang_drip(cave, plane, canvas, solid, spot, keys):
    """Thin streak under a course of stone."""
    rng, index = cave.rng, cave.biome.index
    sx, y0, position, _ = spot
    length = int(rng.integers(2, 6))
    canvas[y0:y0 + length, sx] = index[keys["other"] if position else keys["first"]]


def far_column(cave, x):
    """One far column, whole or broken, recorded so the rule on rows of columns sees it."""
    biome = cave.biome
    top, ground = int(cave.far_ceil[x]), int(cave.far_floor[x])
    if cave.rng.random() < 0.3:
        top = ground - int(cave.rng.integers(18, 40))
    if top <= 0:
        top = ground - 44
    cave.columns.append({"x": x, "base": ground, "top": top, "plane": "far", "broken": False})
    cave.far[top:ground, x - 5:x + 5] = biome.far_a
    cave.far[top:ground, x - 5:x - 1] = biome.far_b
    for y in range(top, ground):
        cave.far[y, x - 1 + (y % 2):x + 2:2] = biome.far_b
    cave.far[top:top + 4, x - 7:x + 7] = biome.far_b
    cave.far[ground - 4:ground, x - 7:x + 7] = biome.far_b


@register(FAR_STRUCTURES, "colonnade")
def far_colonnade(cave, x):
    """Three far columns on a levelled terrace."""
    terrace = int(cave.far_floor[x - 42:x + 42].min())
    cave.far[terrace:, x - 42:x + 42] = cave.biome.far_a
    cave.far[terrace:terrace + 2, x - 42:x + 42] = cave.biome.far_b
    cave.far_floor[x - 42:x + 42] = terrace
    for cx in (x - 30, x, x + 30):
        far_column(cave, cx)


@register(FAR_STRUCTURES, "pier")
def far_pier(cave, x):
    """Far pier of rough rock joining the far ceiling to the far ground, or a spire under an open sky."""
    biome = cave.biome
    top, ground = int(cave.far_ceil[x]), int(cave.far_floor[x])
    left = stepped_noise(H, cave.rng, (4, 10), [0, 1, 2, 3])
    right = stepped_noise(H, cave.rng, (4, 10), [0, 1, 2, 3])
    if top <= 0:
        top = ground - 60
    for y in range(top, ground):
        flare = max(0, 6 - (y - top) // 2, 6 - (ground - y) // 2)
        a, b = x - 8 + left[y] - flare, x + 8 - right[y] + flare
        cave.far[y, a:b] = biome.far_a
        cave.far[y, a:a + 2] = biome.far_b
        if y % 2:
            cave.far[y, a + 2:a + 5:2] = biome.far_b
