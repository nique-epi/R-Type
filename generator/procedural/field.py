"""The field layout: open space without floor or ceiling, where floating bodies painted with the materials of the biome carry the decor."""
import numpy as np

from drawing import BAYER, erosion_depth, flood, shift, stamp, stepped_noise
from frame import H, ZONE
from heroes import HERO_MOVES, HEROES, place_heroes
from layouts import LAYOUTS
from painters import PAINTERS, register, shared_of

SILHOUETTES = {}
PLANES = ("far", "mid", "near")
SIZES = {
    "far": {"large": ((26, 46), (18, 30)), "medium": ((13, 22), (9, 15)), "small": ((5, 10), (4, 7))},
    "mid": {"large": ((58, 96), (38, 62)), "medium": ((28, 46), (20, 32)), "small": ((11, 20), (8, 14))},
    "near": {"large": ((116, 180), (60, 78)), "medium": ((62, 92), (54, 66)), "small": ((34, 52), (46, 56))},
}
SHOWN = {
    "top": {"large": (27, 38), "medium": (14, 24), "small": (7, 12)},
    "bottom": {"large": (219, 227), "medium": (227, 235), "small": (235, 242)},
}
FREE_ROWS = (12, 238)
MAP_MARGIN = 6
BODY_GAP = 3
CLUSTER_SPACE = 22
CLUSTER_MOVES = ((0, 0), (18, 0), (-18, 0), (0, 14), (0, -14), (36, 10), (-36, -10), (54, 0), (-54, 0))
SHADE_RANGE = 26
MARK_COUNT = {"hero": (4, 8), "large": (1, 4), "medium": (0, 2), "small": (0, 1)}
STAR_BLOCK = 48


def staircase(rng, length, drop):
    """Offsets that go from a drop down to zero over a length, in steps of uneven width and height."""
    out = np.zeros(length, int)
    steps = max(length // 5, 1)
    x, level = 0, drop
    while x < length and level > 0:
        span = int(rng.integers(2, 9))
        out[x:x + span] = level
        x += span
        level = max(level - int(rng.integers(0, 2 * drop // steps + 2)), 0)
    return out


def edge_profile(rng, width, height):
    """Inset of one edge of a body per column: deep at both ends, zero along a plateau set off-center, with a notch or two."""
    peak = int(width * rng.uniform(0.28, 0.72))
    plateau = int(width * rng.uniform(0.12, 0.3))
    left = staircase(rng, max(peak - plateau // 2, 1), int(height * rng.uniform(0.3, 0.5)))
    right = staircase(rng, max(width - peak - plateau // 2, 1), int(height * rng.uniform(0.3, 0.5)))[::-1]
    profile = np.zeros(width, int)
    profile[:len(left)] = left
    profile[width - len(right):] = right
    profile += np.minimum(stepped_noise(width, rng, (3, 9), [0, 0, 0, 1, 1, 2]), height // 5)
    for _ in range(int(rng.integers(0, 3)) if width >= 28 else 0):
        wide = int(rng.integers(4, max(width // 6, 5) + 1))
        x = int(rng.integers(width // 6, width - width // 6 - wide))
        deep = int(rng.integers(2, max(height // 7, 3) + 1))
        profile[x:x + wide] += deep
        profile[x - 2:x] += deep // 2
        profile[x + wide:x + wide + 2] += deep // 2
    return profile


def whole(mask):
    """Largest piece of a mask, its one-pixel spurs removed, cut to its box."""
    for _ in range(2 if mask.shape[1] >= 12 else 0):
        sides = sum(shift(mask, dy, dx, False).astype(int) for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)))
        mask = mask & (sides >= 2)
    if not mask.any():
        return np.ones((2, 3), bool)
    best, left = None, mask.copy()
    while left.any():
        rows, columns = np.where(left)
        piece = flood(left, rows[0], columns[0])
        if best is None or piece.sum() > best.sum():
            best = piece
        left &= ~piece
    rows, columns = np.where(best)
    return best[rows.min():rows.max() + 1, columns.min():columns.max() + 1]


@register(SILHOUETTES, "boulder")
def boulder(rng, width, height):
    """Lump of rock: a stepped top edge and a stepped bottom edge, each with its own plateau, so no two are alike and none is round."""
    top, bottom = edge_profile(rng, width, height), edge_profile(rng, width, height)
    rows = np.arange(height)[:, None]
    return whole((rows >= top[None, :]) & (rows < height - bottom[None, :]))


@register(SILHOUETTES, "hull")
def hull(rng, width, height):
    """Wreck laid on an 8 px grid: a long hull under a shorter upper deck and a tower, a heavier stern with its nozzle, a keel fin, a bow torn in steps and a hole or two."""
    unit = 8
    columns, rows = width // unit, height // unit
    cells = np.zeros((rows, columns), bool)
    cells[3:rows - 2] = True
    deck = int(rng.integers(columns // 4, columns // 3))
    cells[2, deck:columns - 2] = True
    tower = int(rng.integers(deck + 2, columns // 2 + 2))
    cells[0:2, tower:tower + 3] = True
    cells[1, tower - 1:tower + 5] = True
    cells[0, columns - 3] = True
    cells[1:rows - 1, columns - 5:] = True
    cells[rows - 1, columns - 4:columns - 1] = True
    cells[rows // 2, columns - 1] = False
    fin = int(rng.integers(columns // 4, columns // 2))
    cells[rows - 2, fin:fin + int(rng.integers(3, 7))] = True
    cells[rows - 1, fin + 1:fin + 3] = True
    first, kept = 3, rows - 5
    for column in range(min(kept, 5) - 1, -1, -1):
        span = min(column + 1, kept)
        first += int(rng.integers(0, kept - span + 1))
        cells[:, column] = False
        cells[first:first + span, column] = True
        kept = span
    for _ in range(int(rng.integers(1, 4))):
        row, column = int(rng.integers(4, rows - 3)), int(rng.integers(6, columns - 7))
        before = cells.copy()
        cells[row, column:column + int(rng.integers(1, 3))] = False
        if int(flood(cells, rows - 2, columns - 1).sum()) != int(cells.sum()):
            cells = before
    mask = np.kron(cells, np.ones((unit, unit), bool))
    rows, columns = np.where(mask)
    mask = mask[rows.min():rows.max() + 1, columns.min():columns.max() + 1]
    return mask if rng.random() < 0.5 else mask[:, ::-1]


def lit_depth(mask):
    """Steps from each pixel of a body to the open air above it or on its left: small on the lit side, large on the far side."""
    depth = np.zeros(mask.shape, int)
    current, level = mask.copy(), 0
    while current.any():
        level += 1
        inner = current & shift(current, 1, 0, False) & shift(current, 0, 1, False)
        depth[current & ~inner] = level
        current = inner
    return depth


def box_gap(first, second):
    """Empty pixels between two boxes, along the axis that separates them most; zero when they meet."""
    return max(first["x0"] - second["x1"], second["x0"] - first["x1"], first["y0"] - second["y1"], second["y0"] - first["y1"], 0)


def add_body(cave, plane, mask, x, y, entry):
    """Records a body whose box starts at (x, y) when it stays inside the map and clear of the other bodies of its plane."""
    height, width = mask.shape
    labels = cave.body_id[plane]
    x0, y0, x1, y1 = x, max(y, 0), x + width, min(y + height, H)
    if x0 < MAP_MARGIN or x1 > cave.width - MAP_MARGIN or y0 >= y1:
        return None
    part = mask[y0 - y:y1 - y]
    rows, columns = np.where(part)
    if not len(rows) or int(flood(part, rows[0], columns[0]).sum()) != len(rows):
        return None
    top, bottom = max(y0 - BODY_GAP, 0), min(y1 + BODY_GAP, H)
    around = np.zeros((bottom - top, width + 2 * BODY_GAP), bool)
    around[y0 - top:y1 - top, BODY_GAP:BODY_GAP + width] = part
    for _ in range(BODY_GAP):
        around = around | shift(around, 1, 0, False) | shift(around, -1, 0, False) | shift(around, 0, 1, False) | shift(around, 0, -1, False)
    if (labels[top:bottom, x0 - BODY_GAP:x1 + BODY_GAP] > 0)[around].any():
        return None
    cave.body_count += 1
    labels[y0:y1, x0:x1][part] = cave.body_count
    cave.solid[plane][y0:y1, x0:x1] |= part
    if entry.get("material") and plane in cave.material:
        cave.material[plane][y0:y1, x0:x1][part] = cave.biome.material_id[entry["material"]]
    record = dict(entry, id=cave.body_count, plane=plane, x0=x0, x1=x1, y0=y0, y1=y1, left=x, top=y, area=int(part.sum()), mask=mask, cluster=None)
    cave.bodies.append(record)
    return record


def drop_body(cave, record):
    """Takes a body back out of its plane."""
    region = (slice(record["y0"], record["y1"]), slice(record["x0"], record["x1"]))
    piece = cave.body_id[record["plane"]][region] == record["id"]
    cave.solid[record["plane"]][region][piece] = False
    cave.body_id[record["plane"]][region][piece] = 0
    cave.bodies.remove(record)


def body_cluster(cave, plane, edge, x, y, material, silhouette="boulder"):
    """One group of bodies: a large one, a medium one close to it, a small one apart, and empty space around the group."""
    rng, sizes = cave.rng, SIZES[plane]

    def shape(size):
        """New silhouette of a size."""
        return SILHOUETTES[silhouette](rng, int(rng.integers(*sizes[size][0])), int(rng.integers(*sizes[size][1])))

    def top_of(size, mask, middle):
        """Row a body starts at: centered on a row in open space, or set by how far it shows past the top or the bottom of the screen."""
        if edge == "top":
            return int(rng.integers(*SHOWN["top"][size])) - mask.shape[0]
        if edge == "bottom":
            return int(rng.integers(*SHOWN["bottom"][size]))
        return int(np.clip(middle - mask.shape[0] // 2, FREE_ROWS[0], FREE_ROWS[1] - mask.shape[0]))

    entry = {"edge": edge, "material": material}
    for move_x, move_y in CLUSTER_MOVES:
        mask = shape("large")
        large = add_body(cave, plane, mask, x + move_x - mask.shape[1] // 2, top_of("large", mask, y + move_y), dict(entry, size="large"))
        if not large:
            continue
        members = [large]
        middle, third = (large["y0"] + large["y1"]) // 2, max((large["y1"] - large["y0"]) // 3, 1)
        side = int(rng.choice([-1, 1]))
        for turn in (side, -side):
            mask = shape("medium")
            gap = int(rng.integers(4, 10))
            left = large["x1"] + gap if turn > 0 else large["x0"] - gap - mask.shape[1]
            medium = add_body(cave, plane, mask, left, top_of("medium", mask, middle + int(rng.integers(-third, third + 1))), dict(entry, size="medium"))
            if medium:
                members.append(medium)
                side = turn
                break
        if len(members) == 1:
            drop_body(cave, large)
            continue
        for turn in (-side, side):
            mask = shape("small")
            gap = int(rng.integers(16, 33))
            left = max(body["x1"] for body in members) + gap if turn > 0 else min(body["x0"] for body in members) - gap - mask.shape[1]
            small = add_body(cave, plane, mask, left, top_of("small", mask, middle + int(rng.integers(-third, third + 1))), dict(entry, size="small"))
            if small:
                members.append(small)
                break
        box = {"x0": min(b["x0"] for b in members), "x1": max(b["x1"] for b in members), "y0": min(b["y0"] for b in members), "y1": max(b["y1"] for b in members)}
        if any(box_gap(box, other) < CLUSTER_SPACE for other in cave.body_boxes[plane]):
            for body in members:
                drop_body(cave, body)
            continue
        cave.body_boxes[plane].append(box)
        cave.body_clusters += 1
        for body in members:
            body["cluster"] = cave.body_clusters
        return members
    return []


def hero_body(cave, zone):
    """The body the hero of a zone is made of, set on the second plane before any other body."""
    hero = zone["hero"]
    wide, tall = hero["size"]
    mask = SILHOUETTES[hero["silhouette"]](cave.rng, wide, tall)
    x, y = zone["x0"] + hero["x"] - mask.shape[1] // 2, hero["y"] - mask.shape[0] // 2
    record = add_body(cave, "mid", mask, x, y, {"edge": "free", "material": hero["material"], "size": "hero"})
    assert record, f"the hero body of {zone['name']} does not fit: check its size and its place"
    cave.body_boxes["mid"].append(record)
    zone["hero_body"] = record


def tint_patch(canvas, mask, x, y, color):
    """Flat patch of one color on a picture, its rim broken by an ordered dither instead of a gradient."""
    height, width = mask.shape
    x0, y0, x1, y1 = max(x, 0), max(y, 0), min(x + width, canvas.shape[1]), min(y + height, canvas.shape[0])
    if x0 >= x1 or y0 >= y1:
        return
    part = mask[y0 - y:y1 - y, x0 - x:x1 - x]
    depth = erosion_depth(mask, 6)[y0 - y:y1 - y, x0 - x:x1 - x]
    rows, columns = np.mgrid[y0:y1, x0:x1]
    canvas[y0:y1, x0:x1][part & (depth / 7.0 > BAYER[rows % 4, columns % 4])] = color


def paint_backdrop(cave):
    """Far backdrop: flat banks of haze with dithered rims, then stars of a few intensities, crowded along a wandering band and sparse elsewhere."""
    rng, biome, far = cave.rng, cave.biome, cave.far
    haze, stars = biome.haze, biome.stars
    for zone in cave.zones if haze else []:
        for _ in range(int(rng.integers(*haze["banks"]))):
            bank = boulder(rng, int(rng.integers(*haze["width"])), int(rng.integers(*haze["height"])))
            x, y = zone["x0"] + int(rng.integers(-40, ZONE - 120)), int(rng.integers(8, H - 60))
            tint_patch(far, bank, x, y, haze["tones"][0])
            core = boulder(rng, bank.shape[1] // 2, bank.shape[0] // 2)
            tint_patch(far, core, x + int(rng.integers(8, bank.shape[1] // 2 - 8)), y + int(rng.integers(4, bank.shape[0] // 2 - 4)), haze["tones"][1])
    if not stars:
        return
    cell = stars["cell"]
    tones = [np.array(color) for color, _ in stars["tones"]]
    shares = [share for _, share in stars["tones"]]
    middle, track = int(rng.integers(70, 200)), []
    for _ in range(cave.width // STAR_BLOCK + 1):
        middle = int(np.clip(middle + int(rng.choice(stars["wander"])), 40, 230))
        track.append((middle, int(rng.choice(stars["band"]))))
    for top in range(0, H, cell):
        for left in range(0, cave.width, cell):
            middle, half = track[left // STAR_BLOCK]
            crowded = abs(top + cell // 2 - middle) < half
            for _ in range(int(rng.choice(stars["crowded" if crowded else "sparse"]))):
                x, y = left + int(rng.integers(0, cell)), top + int(rng.integers(0, cell))
                if x >= cave.width - 1 or y >= H - 1 or x < 1 or y < 1:
                    continue
                tone = int(rng.choice(len(tones), p=shares))
                far[y, x] = tones[tone]
                if tone == len(tones) - 1 and rng.random() < stars["cross"]:
                    for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                        far[y + dy, x + dx] = tones[0]


def paint_far_bodies(cave):
    """Far bodies as flat two-tone silhouettes: one tone, and the other along the edge that faces the light."""
    biome = cave.biome
    for body in cave.bodies:
        if body["plane"] != "far":
            continue
        lit = lit_depth(body["mask"])
        rows, columns = np.mgrid[0:lit.shape[0], 0:lit.shape[1]]
        picture = np.zeros(lit.shape + (3,))
        picture[:] = biome.far_a
        picture[(lit <= 2) | ((lit == 3) & ((rows + columns) % 2 == 0))] = biome.far_b
        region = cave.far[body["y0"]:body["y1"], body["x0"]:body["x1"]]
        part = body["mask"][body["y0"] - body["top"]:body["y1"] - body["top"]]
        region[part] = picture[body["y0"] - body["top"]:body["y1"] - body["top"]][part]


def paint_bodies(cave, plane):
    """Paints the bodies of one plane with the painter of their material, lit from the top left: depth is counted from the lit side of each body."""
    solid, material, canvas, biome = cave.solid[plane], cave.material[plane], getattr(cave, plane), cave.biome
    shared = shared_of(solid)
    depth = shared["depth"]
    for body in cave.bodies:
        if body["plane"] != plane:
            continue
        lit = lit_depth(body["mask"])
        scaled = 2 + lit * SHADE_RANGE // max(int(lit.max()), 1)
        clip = slice(body["y0"] - body["top"], body["y1"] - body["top"])
        inside = body["mask"][clip]
        region = depth[body["y0"]:body["y1"], body["x0"]:body["x1"]]
        inner = inside & (region > 1)
        region[inner] = scaled[clip][inner]
    cave.shade[plane] = depth
    for name, declared in biome.materials.items():
        mask = solid & (material == biome.material_id[name])
        if mask.any():
            options = {key: value for key, value in declared["paint"].items() if key != "frame"}
            PAINTERS[declared["painter"]](cave, canvas, mask, shared, options)


def mark_bodies(cave, plane):
    """Stamps the marks a biome draws for a material on the lit face of its bodies: a few, set apart, never in a row."""
    rng, canvas, labels, depth = cave.rng, getattr(cave, plane), cave.body_id[plane], cave.shade[plane]
    for body in cave.bodies:
        stamps = cave.biome.marks.get(body.get("material"))
        if body["plane"] != plane or not stamps:
            continue
        placed, wanted = [], int(rng.integers(*MARK_COUNT[body["size"]]))
        for _ in range(wanted * 6):
            if len(placed) >= wanted:
                break
            mark = stamps[int(rng.integers(0, len(stamps)))]
            tall, wide = mark.shape
            if body["x1"] - body["x0"] <= wide + 6 or body["y1"] - body["y0"] <= tall + 6:
                continue
            x, y = int(rng.integers(body["x0"] + 3, body["x1"] - wide - 3)), int(rng.integers(body["y0"] + 3, body["y1"] - tall - 3))
            window = (slice(y - 1, y + tall + 1), slice(x - 1, x + wide + 1))
            on_face = (labels[window] == body["id"]).all() and depth[window].min() >= 3 and depth[window].max() <= 18
            if on_face and all(abs(x - px) + abs(y - py) >= 14 for px, py in placed):
                stamp(canvas, mark, x, y)
                placed.append((x, y))


def find_surfaces(cave):
    """Per column, the rows decor stands on and hangs from: the tops and undersides of the bodies of each plane."""
    near, mid = cave.solid["near"], cave.solid["mid"]
    upper, lower, half = near[:H // 2], near[H // 2:], H // 2
    cave.profile = {
        "ceil": np.where(upper.any(axis=0), half - np.argmax(upper[::-1], axis=0), 0),
        "floor": np.where(lower.any(axis=0), half + np.argmax(lower, axis=0), H + 8),
        "mid_floor": np.where(mid.any(axis=0), np.argmax(mid, axis=0), H + 8),
        "mid_ceil": np.where(mid.any(axis=0), H - np.argmax(mid[::-1], axis=0), 0),
    }


def field_zone(biome, name, mirrored):
    """One zone of open space: where its hero body and its groups of bodies go, mirrored when asked."""
    template = biome.templates[name]
    flip = (lambda x: ZONE - x) if mirrored else (lambda x: x)
    zone = {"name": name, "mirrored": mirrored, "material": template["material"], "ambient": template["ambient"], "mood": template["mood"], "sky": True}
    zone["hero"] = dict(template["hero"], x=flip(template["hero"]["x"]))
    zone["mid_bodies"] = [(flip(x), y, family) for x, y, family in template.get("mid_bodies", [])]
    zone["near_top"] = [(flip(x), family) for x, family in template.get("near_top", [])]
    zone["near_bottom"] = [(flip(x), family) for x, family in template.get("near_bottom", [])]
    zone["far_bodies"] = [(flip(x), y) for x, y in template.get("far_bodies", [])]
    zone["left"], zone["right"] = (template["right"], template["left"]) if mirrored else (template["left"], template["right"])
    return zone


def vein(cave, body, keys):
    """Glowing crack across the lit face of a body, in short level runs and small drops; returns its middle."""
    rng, index, labels, canvas, depth = cave.rng, cave.biome.index, cave.body_id["mid"], cave.mid, cave.shade["mid"]
    x = body["x0"] + int((body["x1"] - body["x0"]) * rng.uniform(0.12, 0.25))
    column = np.where(labels[:, x] == body["id"])[0]
    y = int(column[0] + len(column) * rng.uniform(0.3, 0.5))
    points = []
    for _ in range(int(rng.integers(7, 12))):
        for _ in range(int(rng.integers(3, 11))):
            if x < body["x1"] and labels[y, x] == body["id"] and 2 < depth[y, x] <= 20:
                canvas[y, x] = index[keys[1]]
                if labels[y + 1, x] == body["id"]:
                    canvas[y + 1, x] = index[keys[0]]
                points.append((x, y))
            x += 1
        y = int(np.clip(y + int(rng.choice([-2, -1, 1, 1, 2, 3])), body["y0"], body["y1"] - 1))
    for x, y in points[2::7]:
        canvas[y, x - 1:x + 1] = index[keys[2]]
    return points[len(points) // 2] if len(points) >= 12 else None


def hero_record(cave, name, x, body):
    """Records the hero of a zone as the body it is made of."""
    record = {"name": name, "plane": "mid", "surface": "face", "x": x, "x0": body["x0"], "x1": body["x1"], "y0": body["y0"], "y1": body["y1"], "role": "hero", "cluster": None, "structure": True}
    cave.decor.append(record)
    return record


@register(HEROES, "great_body")
def great_body(cave, zone, x, hero):
    """The largest body of a zone, on the second plane: a glowing vein across its face and a cluster of decor on its top."""
    body, glow = zone["hero_body"], hero["vein"]
    spot = vein(cave, body, glow["keys"])
    if spot:
        cave.lights.append((spot[0], spot[1], glow["radius"], glow["tint"], glow["power"]))
    if hero.get("family"):
        cave.cluster(x + hero.get("dx", 0), hero["family"], "mid", "floor", hero.get("lead"), "member", HERO_MOVES)
    record = hero_record(cave, "great_body", x, body)
    return record if spot else None


@register(HEROES, "wreck")
def wreck(cave, zone, x, hero):
    """Wreck built with a material of the biome, on the second plane: a row of portholes, some of them lit, a beacon on a mast and debris on its deck."""
    rng, index, labels, canvas = cave.rng, cave.biome.index, cave.body_id["mid"], cave.mid
    body, ports, beacon = zone["hero_body"], hero["portholes"], hero["beacon"]
    row = body["y0"] + int((body["y1"] - body["y0"]) * 0.5) // 8 * 8 + 3
    lit = []
    for left in range(body["x0"] + 12, body["x1"] - 12, 8):
        if (labels[row - 1:row + 3, left - 1:left + 3] == body["id"]).all():
            on = rng.random() < ports["share"]
            canvas[row:row + 2, left:left + 2] = index[ports["keys"][0] if on else ports["dark"]]
            if on:
                canvas[row, left] = index[ports["keys"][1]]
                lit.append(left)
    if lit:
        cave.lights.append((lit[len(lit) // 2], row, ports["radius"], ports["tint"], ports["power"]))
    mast = x + beacon["dx"] * (-1 if zone["mirrored"] else 1)
    deck = np.where(labels[:, mast] == body["id"])[0]
    if len(deck) and lit:
        top, length = int(deck[0]), int(rng.integers(*beacon["length"]))
        canvas[top - length:top, mast] = index[beacon["keys"][0]]
        canvas[top - length - 2:top - length, mast - 1:mast + 1] = index[beacon["keys"][1]]
        cave.lights.append((mast, top - length - 1, beacon["radius"], beacon["tint"], beacon["power"]))
        cave.decor.append({"name": "beacon", "plane": "mid", "surface": "floor", "x": mast, "x0": mast - 2, "x1": mast + 3, "y0": top - length - 2, "y1": top + 1, "role": "lamp", "cluster": None})
        cave.reserve("mid", "floor", mast - 2, mast + 3)
    if hero.get("family"):
        cave.cluster(x + hero.get("dx", 0), hero["family"], "mid", "floor", hero.get("lead"), "member", HERO_MOVES)
    record = hero_record(cave, "wreck", x, body)
    return record if lit else None


@register(LAYOUTS, "field")
def build_field(cave, sequence):
    """The field layout: a backdrop of stars, far silhouettes, bodies anywhere on the second plane, and on the play plane only bodies that show past the top and the bottom of the screen."""
    rng = cave.rng
    cave.zones = [field_zone(cave.biome, name, mirrored) for name, mirrored in sequence]
    for position, zone in enumerate(cave.zones):
        zone["x0"] = position * ZONE
    shape = (H, cave.width)
    cave.solid = {plane: np.zeros(shape, bool) for plane in PLANES}
    cave.material = {plane: np.zeros(shape, np.uint8) for plane in ("near", "mid")}
    cave.body_id = {plane: np.zeros(shape, np.int16) for plane in PLANES}
    cave.bodies, cave.body_boxes, cave.body_count, cave.body_clusters, cave.shade = [], {plane: [] for plane in PLANES}, 0, 0, {}
    cave.seams, cave.water_runs, cave.water = [], [], np.zeros(shape, bool)
    paint_backdrop(cave)
    for zone in cave.zones:
        hero_body(cave, zone)
    dressed = []
    for zone in cave.zones:
        material = zone["material"]
        for x, y, family in zone["mid_bodies"]:
            members = body_cluster(cave, "mid", "free", zone["x0"] + x + int(rng.integers(-10, 11)), y + int(rng.integers(-8, 9)), material)
            dressed.append((members, family, "mid", "floor"))
        for x, family in zone["near_top"]:
            dressed.append((body_cluster(cave, "near", "top", zone["x0"] + x + int(rng.integers(-10, 11)), 0, material), family, "near", "ceil"))
        for x, family in zone["near_bottom"]:
            dressed.append((body_cluster(cave, "near", "bottom", zone["x0"] + x + int(rng.integers(-10, 11)), 0, material), family, "near", "floor"))
        for x, y in zone["far_bodies"]:
            body_cluster(cave, "far", "free", zone["x0"] + x + int(rng.integers(-14, 15)), y + int(rng.integers(-12, 13)), None)
    paint_far_bodies(cave)
    for plane in ("mid", "near"):
        paint_bodies(cave, plane)
        mark_bodies(cave, plane)
    find_surfaces(cave)
    place_heroes(cave)
    for members, family, plane, surface in dressed:
        if members and family:
            cave.cluster((members[0]["x0"] + members[0]["x1"]) // 2 + int(rng.integers(-8, 9)), family, plane, surface)
