"""Level design checks run on every generated map; a map that fails one is never written."""
import math

import numpy as np

from drawing import flood, shift
from frame import CEIL_LIMIT, DECOR_CLEAR, FLOOR_LIMIT, H, HANG_LIMIT, HUD_Y, MID_CEIL_LIMIT, MID_FLOOR_LIMIT, PLAY_BOTTOM, PLAY_TOP, ZONE

NOT_APPLICABLE = "not applicable to this layout"
LEVEL_SPREAD = 48
TELLING_AREA = 48
DRAWN_BY_RULE = ("strokes",)


def attached(solid):
    """Solid pixels reachable from the borders of the picture through solid pixels."""
    reach = np.zeros_like(solid)
    reach[0], reach[-1], reach[:, 0], reach[:, -1] = solid[0], solid[-1], solid[:, 0], solid[:, -1]
    height, width = solid.shape
    while True:
        before = int(reach.sum())
        for y in range(1, height):
            reach[y] |= reach[y - 1] & solid[y]
        for y in range(height - 2, -1, -1):
            reach[y] |= reach[y + 1] & solid[y]
        for x in range(1, width):
            reach[:, x] |= reach[:, x - 1] & solid[:, x]
        for x in range(width - 2, -1, -1):
            reach[:, x] |= reach[:, x + 1] & solid[:, x]
        if int(reach.sum()) == before:
            return reach


def groups_of(cave, plane, surface):
    """Extents of the groups of decor of one surface: clusters, heroes and seam covers."""
    found = {}
    for position, record in enumerate(cave.decor):
        if record["plane"] != plane or record["surface"] != surface or record["role"] in ("lamp", "silhouette", "structure"):
            continue
        key = record["cluster"] if record["cluster"] is not None else f"single-{position}"
        x0, x1 = found.get(key, (record["x0"], record["x1"]))
        found[key] = (min(x0, record["x0"]), max(x1, record["x1"]))
    return sorted(found.values())


def unsupported(cave):
    """Names of the props that have nothing solid under them, or above them when they hang."""
    unheld = []
    for record in cave.decor:
        if record["plane"] == "far" or record.get("structure"):
            continue
        solid = cave.solid[record["plane"]]
        row = record["y1"] - 1 if record["surface"] == "floor" else max(record["y0"] - (1 if record["role"] == "lamp" else 0), 0)
        if not solid[min(row, H - 1), record["x0"] + 1:record["x1"] - 1].any():
            unheld.append(record["name"])
    return unheld


def nothing_floats(cave):
    """Rule 3: every solid pixel reaches a border of the picture and every prop rests on something."""
    loose = sum(int((cave.solid[plane] & ~attached(cave.solid[plane])).sum()) for plane in ("near", "mid"))
    unheld = unsupported(cave)
    return loose == 0 and not unheld, f"{loose} loose terrain pixels, {len(unheld)} props without support {unheld[:3]}"


def columns_grouped(cave):
    """Rule 4: columns stand on a material that bears them, out of the water, in rows."""
    faults = []
    wet = np.zeros(cave.width, bool)
    for x0, x1 in cave.water_runs:
        wet[x0:x1] = True
    materials, sprites = cave.biome.materials, cave.biome.column_sprites
    bearing = [cave.biome.material_id[name] for name, declared in materials.items() if declared.get("bears_columns")]
    for column in cave.columns:
        base = min(column["base"], H - 1)
        on_stone = column["plane"] == "far" or (cave.solid["mid"][base, column["x"]] and cave.material["mid"][base, column["x"]] in bearing)
        mates = sum(1 for other in cave.columns if other is not column and other["plane"] == column["plane"] and abs(other["x"] - column["x"]) <= 110 and abs(other["base"] - column["base"]) <= 12)
        if not on_stone or column["base"] >= cave.water_y or mates < 2:
            faults.append(column["x"])
    for record in cave.decor:
        if record["name"] in sprites and record["plane"] != "far":
            near_columns = [c for c in cave.columns if abs(c["x"] - record["x"]) <= 200]
            near_sprites = [r for r in cave.decor if r is not record and r["name"] in sprites and abs(r["x"] - record["x"]) <= 200]
            stone = materials[cave.zone_of(record["x"])["material"]].get("bears_columns", False)
            if not stone or not (near_columns or near_sprites):
                faults.append(record["x"])
    return not faults, f"{len(cave.columns)} standing columns in rows, {len(faults)} isolated or off stone"


def water_closed(cave):
    """Rule 5: water lies in basins closed by terrain whose banks go down in steps."""
    floor = cave.profile["floor"]
    faults = []
    inside = np.zeros(cave.width, bool)
    for x0, x1 in cave.water_runs:
        inside[x0:x1] = True
        if x0 == 0 or x1 >= cave.width:
            faults.append(f"open basin {x0}-{x1}")
            continue
        deepest = int(floor[x0:x1].max())
        for edge in (floor[x0:x0 + 12], floor[x1 - 12:x1]):
            if int(edge.min()) - cave.water_y > 14 or deepest - int(edge.min()) < 4:
                faults.append(f"bank of {x0}-{x1} drops straight down")
    if (cave.water & ~inside[None, :]).any() or (cave.water & cave.solid["near"]).any():
        faults.append("water outside a basin")
    return not faults, f"{len(cave.water_runs)} closed basin(s) with stepped banks" if not faults else "; ".join(faults)


def waterfalls_fed(cave):
    """Rule 6: each waterfall has a source and a pool, and they stay rare."""
    faults = []
    for fall in cave.waterfalls:
        lands = any(x0 <= fall["x"] < x1 for x0, x1 in cave.water_runs)
        fed = cave.solid["mid"][max(fall["top"] - 1, 0), fall["x"]]
        if not lands or not fed or not fall["splash"]:
            faults.append(fall["x"])
    limit = math.ceil(cave.zone_count / min([fall["one_per"] for fall in cave.waterfalls] + [cave.zone_count]))
    return not faults and len(cave.waterfalls) <= limit, f"{len(cave.waterfalls)} waterfall(s) (at most {limit}), each with a spout and a pool"


def one_material(cave):
    """Rule 7: one material makes most of the visible terrain of each zone."""
    shares = []
    for zone in cave.zones:
        columns = slice(zone["x0"], zone["x0"] + ZONE)
        near, mid = cave.solid["near"][:, columns], cave.solid["mid"][:, columns]
        kind = np.where(near, cave.material["near"][:, columns], cave.material["mid"][:, columns])
        visible = near | mid
        shares.append(float((kind[visible] == cave.biome.material_id[zone["material"]]).mean()))
    return min(shares) >= 0.6, "share of the main material per zone: " + ", ".join(f"{share:.0%}" for share in shares)


def play_area_clear(cave):
    """Rule 8: the corridor the game is played in stays clear on the play plane and open on the second plane."""
    blocked = int(cave.solid["near"][PLAY_TOP:PLAY_BOTTOM + 1].sum())
    intruders = [r["name"] for r in cave.decor if r["plane"] == "near" and not r.get("structure") and r["y0"] < DECOR_CLEAR[1] and r["y1"] > DECOR_CLEAR[0]]
    low_roof = int((cave.profile["ceil"] > CEIL_LIMIT).sum())
    long_hangers = [r["name"] for r in cave.decor if r["plane"] == "near" and r["surface"] == "ceil" and not r.get("structure") and r["y1"] > HANG_LIMIT]
    behind = int((cave.profile["mid_ceil"] > MID_CEIL_LIMIT).sum()) + int(((cave.profile["mid_floor"] < MID_FLOOR_LIMIT) & ~cave.mid_exempt).sum())
    passed = blocked == 0 and not intruders and low_roof == 0 and not long_hangers and behind == 0
    return passed, (f"{blocked} solid pixels between y {PLAY_TOP} and {PLAY_BOTTOM}, {len(intruders)} props across y {DECOR_CLEAR[0]}-{DECOR_CLEAR[1]}, "
                    f"{low_roof} ceiling columns below y {CEIL_LIMIT}, {len(long_hangers)} hanging pieces below y {HANG_LIMIT}, {behind} second-plane columns inside y {MID_CEIL_LIMIT}-{MID_FLOOR_LIMIT}")


def decor_in_clusters(cave):
    """Rule 9: decor comes in clusters with space between them and no even rhythm."""
    faults, sizes = [], []
    clusters = {}
    for record in cave.decor:
        if record["cluster"] is not None:
            clusters.setdefault(record["cluster"], []).append(record)
    for members in clusters.values():
        sizes.append(len(members))
        lead = members[0]
        if len(members) < 2:
            faults.append(f"lone {lead['name']}")
            continue
        gaps = sorted(max(m["x0"] - lead["x1"], lead["x0"] - m["x1"]) for m in members[1:])
        if gaps[0] > 0:
            faults.append(f"nothing touches {lead['name']}")
        if len(gaps) > 1 and not 3 <= gaps[1] <= 24:
            faults.append(f"small piece {gaps[1]} px from {lead['name']}")
    if sizes and sum(size >= 3 for size in sizes) < 0.5 * len(sizes):
        faults.append("fewer than half of the clusters have three pieces")
    for plane in ("near", "mid"):
        for surface in ("floor", "ceil"):
            groups = groups_of(cave, plane, surface)
            spaces = [b[0] - a[1] for a, b in zip(groups, groups[1:])]
            if any(space < 20 for space in spaces):
                faults.append(f"groups closer than 20 px on {plane} {surface}")
            free = {}
            for record in cave.decor:
                if record["plane"] == plane and record["surface"] == surface and record["cluster"] is not None:
                    free.setdefault(record["cluster"], []).append(record)
            centers = sorted((min(m["x0"] for m in members) + max(m["x1"] for m in members)) / 2 for members in free.values() if members[0]["role"] == "cluster")
            steps = np.diff(centers)
            if any(abs(steps[i] - steps[i + 1]) < 6 and max(steps[i], steps[i + 1]) < 240 for i in range(len(steps) - 1)):
                faults.append(f"even spacing on {plane} {surface}")
    return not faults, f"{len(sizes)} clusters (sizes {sorted(sizes, reverse=True)}), uneven spacing" if not faults else "; ".join(faults[:4])


def no_close_repeat(cave):
    """Rule 10: no sprite repeats close by, and no grass pattern twice in a row."""
    faults = []
    for plane in ("far", "mid", "near"):
        seen = {}
        for record in cave.decor:
            if record["plane"] == plane and not record.get("structure") and record["name"] not in DRAWN_BY_RULE:
                seen.setdefault(record["name"], []).append(record["x"])
        for name, places in seen.items():
            places.sort()
            if any(b - a < 160 for a, b in zip(places, places[1:])):
                faults.append(f"{name} twice within 160 px on {plane}")
    for first in cave.decor:
        for second in cave.decor:
            if first["name"] == second["name"] and first["plane"] == "mid" and second["plane"] == "near" and not first.get("structure") and first["name"] not in DRAWN_BY_RULE and abs(first["x"] - second["x"]) < 96:
                faults.append(f"{first['name']} on both planes within 96 px")
    for plane, heights in cave.grass.items():
        for start in range(0, cave.width - 24, 12):
            first, second = heights[start:start + 12], heights[start + 12:start + 24]
            if first.any() and (first == second).all():
                faults.append(f"same grass twice in a row at x {start} on {plane}")
    return not faults, "no sprite repeated within 160 px on a plane, no grass pattern twice in a row" if not faults else "; ".join(faults[:4])


def one_hero(cave):
    """Rule 11: each zone has one hero, on a third of the screen."""
    faults, names = [], []
    for zone in cave.zones:
        heroes = [r for r in cave.decor if r["role"] == "hero" and zone["x0"] <= r["x"] < zone["x0"] + ZONE]
        if len(heroes) != 1 or zone.get("hero_record") is None:
            faults.append(f"{zone['name']}: {len(heroes)} hero(es)")
            continue
        local = heroes[0]["x"] - zone["x0"]
        if min(abs(local - ZONE / 3), abs(local - 2 * ZONE / 3)) > 24:
            faults.append(f"{zone['name']}: hero at x {local}, off the thirds")
        names.append(f"{heroes[0]['name']}@{local}")
    return not faults, "one hero per zone on a third: " + ", ".join(names) if not faults else "; ".join(faults)


def seams_hidden(cave):
    """Rule 12: a dry change of material hides behind a cover on the ground and a pier behind."""
    faults = []
    for seam in cave.seams:
        if seam["wet"] or not seam["change"]:
            continue
        covered = any(r["surface"] == "floor" and r["x0"] - 4 <= seam["x"] <= r["x1"] + 4 for r in cave.seam_covers)
        if not covered or "pillar" not in seam:
            faults.append(seam["x"])
    return not faults, f"{len(cave.seams)} seam(s), every change of material behind a rock and a pier" if not faults else f"bare seams at {faults}"


def colors_quiet(cave):
    """Rule 13: vivid pixels stay a small share of the picture."""
    image = cave.image.astype(float)
    top, low = image.max(axis=2), image.min(axis=2)
    vivid = ((top - low) / np.maximum(top, 1) > 0.6) & (top > 120)
    share = float(vivid.mean())
    return share <= 0.06, f"{share:.1%} of the pixels are vivid (limit 6%)"


def three_planes(cave):
    """Rule 14: each zone has decor on the far, second and play planes."""
    faults, counts = [], []
    for zone in cave.zones:
        per_plane = {plane: sum(1 for r in cave.decor if r["plane"] == plane and zone["x0"] <= r["x"] < zone["x0"] + ZONE) for plane in ("far", "mid", "near")}
        counts.append("/".join(str(per_plane[plane]) for plane in ("far", "mid", "near")))
        if min(per_plane.values()) < 1:
            faults.append(zone["name"])
    return not faults, "decor per zone far/second/play: " + ", ".join(counts) if not faults else f"a plane is empty in {faults}"


def every_zone_lit(cave):
    """Rule 15: each zone has a light source and the zones do not all share one mood."""
    faults, counts = [], []
    for zone in cave.zones:
        sources = [light for light in cave.lights if zone["x0"] <= light[0] < zone["x0"] + ZONE] + [beam for beam in cave.beams if zone["x0"] <= beam[0] < zone["x0"] + ZONE]
        counts.append(len(sources))
        if not sources:
            faults.append(zone["name"])
    moods = {(zone["ambient"], zone["mood"]) for zone in cave.zones}
    if cave.zone_count > 1 and len(moods) < 2:
        faults.append("every zone has the same mood")
    return not faults, f"light sources per zone: {counts}, {len(moods)} moods" if not faults else f"unlit or uniform: {faults}"


def hud_band_empty(cave):
    """Rule 17: no hero, light source or foam lies under the status band of the HUD."""
    heroes = [r["name"] for r in cave.decor if r["role"] == "hero" and (r["y0"] + r["y1"]) / 2 >= HUD_Y]
    sources = [light for light in cave.lights if light[1] >= HUD_Y] + [beam for beam in cave.beams if beam[1] >= HUD_Y]
    foam = [mark for fall in cave.waterfalls for mark in fall["marks"] if mark >= HUD_Y]
    lowest = max([light[1] for light in cave.lights] + [0])
    return not heroes and not sources and not foam, f"{len(heroes)} heroes, {len(sources)} light sources, {len(foam)} foam centers under y {HUD_Y} (lowest light at y {lowest})"


def body_piece(cave, body):
    """Pixels a plane records for one body, inside the box of that body."""
    return cave.body_id[body["plane"]][body["y0"]:body["y1"], body["x0"]:body["x1"]] == body["id"]


def box_gap(first, second):
    """Empty pixels between two boxes, along the axis that separates them most; zero when they meet."""
    return max(first["x0"] - second["x1"], second["x0"] - first["x1"], first["y0"] - second["y1"], second["y0"] - first["y1"], 0)


def bodies_whole(cave):
    """Field rule 3: each floating body is in one piece."""
    broken = []
    for body in cave.bodies:
        piece = body_piece(cave, body)
        rows, columns = np.where(piece)
        if not len(rows) or int(flood(piece, rows[0], columns[0]).sum()) != len(rows):
            broken.append(body["id"])
    return not broken, f"{len(cave.bodies)} bodies, {len(broken)} in several pieces {broken[:3]}"


def bodies_apart(cave):
    """Field rule 3: two bodies of a plane neither overlap nor touch, and nothing solid lies outside a body."""
    touching, stray = 0, 0
    for plane, labels in cave.body_id.items():
        for dy, dx in ((1, 0), (0, 1)):
            other = shift(labels, dy, dx, 0)
            touching += int(((labels > 0) & (other > 0) & (labels != other)).sum())
        stray += int(((labels > 0) != cave.solid[plane]).sum())
    covered = [body["id"] for body in cave.bodies if int(body_piece(cave, body).sum()) != body["area"]]
    return touching == 0 and stray == 0 and not covered, f"{touching} pixels where two bodies touch, {len(covered)} bodies covered by another {covered[:3]}, {stray} solid pixels outside a body"


def bodies_not_cut(cave):
    """Field rule 3: a body of the play plane reaches the top or the bottom of the screen and is never cut by a zone edge."""
    solid = cave.solid["near"]
    cut, crossing = [], 0
    if solid[:, 0].any():
        cut.append(0)
    if solid[:, -1].any():
        cut.append(cave.width)
    for position in range(1, cave.zone_count):
        x = position * ZONE
        for rows in (slice(0, H // 2), slice(H // 2, H)):
            left, right = int(solid[rows, x - 1].sum()), int(solid[rows, x].sum())
            if (left >= 4 and right == 0) or (right >= 4 and left == 0):
                cut.append(x)
            crossing += left > 0 and right > 0
    unheld = [body["id"] for body in cave.bodies if body["plane"] == "near" and body["y0"] > 0 and body["y1"] < H]
    return not cut and not unheld, f"{crossing} bodies go on across a zone edge, {len(cut)} cut at an edge {cut[:3]}, {len(unheld)} away from the top and bottom of the screen"


def decor_on_bodies(cave):
    """Field rule 3: every prop rests on a body, or hangs from one."""
    unheld = unsupported(cave)
    count = sum(1 for record in cave.decor if record["plane"] != "far" and not record.get("structure"))
    return not unheld, f"{count} props, {len(unheld)} without a body under them {unheld[:3]}"


def corridor_free(cave):
    """Field rule 8: the play plane holds bodies only above the corridor and below it, and no prop reaches into it."""
    blocked = int(cave.solid["near"][PLAY_TOP:PLAY_BOTTOM + 1].sum())
    intruders = [r["name"] for r in cave.decor if r["plane"] == "near" and not r.get("structure") and r["y0"] < DECOR_CLEAR[1] and r["y1"] > DECOR_CLEAR[0]]
    low = int((cave.profile["ceil"] > CEIL_LIMIT).sum())
    high = int((cave.profile["floor"] < FLOOR_LIMIT).sum())
    long_hangers = [r["name"] for r in cave.decor if r["plane"] == "near" and r["surface"] == "ceil" and not r.get("structure") and r["y1"] > HANG_LIMIT]
    passed = blocked == 0 and not intruders and low == 0 and high == 0 and not long_hangers
    return passed, (f"{blocked} solid pixels between y {PLAY_TOP} and {PLAY_BOTTOM}, {len(intruders)} props across y {DECOR_CLEAR[0]}-{DECOR_CLEAR[1]}, "
                    f"{low} columns of an upper body below y {CEIL_LIMIT}, {high} columns of a lower body above y {FLOOR_LIMIT}, {len(long_hangers)} hanging pieces below y {HANG_LIMIT}")


def bodies_in_clusters(cave):
    """Field rule 9: bodies come as a large one, a medium one close to it and a small one apart, with empty space between the groups and no even spacing along a row."""
    faults, sizes = [], []
    clusters = {}
    for body in cave.bodies:
        if body["cluster"] is not None:
            clusters.setdefault(body["cluster"], []).append(body)
    for members in clusters.values():
        sizes.append(len(members))
        by_size = {body["size"]: body for body in members}
        if "large" not in by_size or "medium" not in by_size:
            faults.append(f"group at x {members[0]['x0']} lacks a large or a medium body")
            continue
        close = box_gap(by_size["large"], by_size["medium"])
        if close > 12:
            faults.append(f"medium body {close} px from its large one")
        if "small" in by_size:
            apart = min(box_gap(by_size["small"], other) for other in members if other is not by_size["small"])
            if not 12 <= apart <= 40:
                faults.append(f"small body {apart} px from its group")
    if sizes and sum(size >= 3 for size in sizes) < 0.5 * len(sizes):
        faults.append("fewer than half of the groups have three bodies")
    for plane in cave.body_id:
        boxes, centers = [], {}
        for key, members in clusters.items():
            if members[0]["plane"] == plane:
                box = {"x0": min(b["x0"] for b in members), "x1": max(b["x1"] for b in members), "y0": min(b["y0"] for b in members), "y1": max(b["y1"] for b in members)}
                boxes.append(box)
                centers.setdefault(members[0]["edge"], []).append(((box["x0"] + box["x1"]) / 2, (box["y0"] + box["y1"]) / 2))
        boxes += [body for body in cave.bodies if body["plane"] == plane and body["cluster"] is None]
        if any(box_gap(a, b) < 20 for position, a in enumerate(boxes) for b in boxes[position + 1:]):
            faults.append(f"groups closer than 20 px on {plane}")
        for edge, places in centers.items():
            places.sort()
            steps = np.diff([x for x, _ in places])
            for i in range(len(steps) - 1):
                heights = [y for _, y in places[i:i + 3]]
                if abs(steps[i] - steps[i + 1]) < 6 and max(steps[i], steps[i + 1]) < 240 and max(heights) - min(heights) < LEVEL_SPREAD:
                    faults.append(f"even spacing on {plane} {edge}")
    return not faults, f"{len(sizes)} groups of bodies (sizes {sorted(sizes, reverse=True)}), uneven spacing" if not faults else "; ".join(faults[:4])


def no_body_twice(cave):
    """Field rule 10: no body silhouette large enough to be told apart is drawn twice within 160 px on a plane."""
    twins = []
    for position, first in enumerate(cave.bodies):
        for second in cave.bodies[position + 1:]:
            near_by = first["plane"] == second["plane"] and abs((first["x0"] + first["x1"]) - (second["x0"] + second["x1"])) / 2 < 160
            if near_by and first["area"] >= TELLING_AREA and first["mask"].shape == second["mask"].shape and np.array_equal(first["mask"], second["mask"]):
                twins.append((first["id"], second["id"]))
    return not twins, f"{len(cave.bodies)} silhouettes, {len(twins)} drawn twice within 160 px {twins[:3]}"


def bodies_on_three_planes(cave):
    """Field rule 14: each zone has bodies on the far, second and play planes."""
    faults, counts = [], []
    for zone in cave.zones:
        per_plane = {plane: sum(1 for b in cave.bodies if b["plane"] == plane and zone["x0"] <= (b["x0"] + b["x1"]) // 2 < zone["x0"] + ZONE) for plane in ("far", "mid", "near")}
        counts.append("/".join(str(per_plane[plane]) for plane in ("far", "mid", "near")))
        if min(per_plane.values()) < 1:
            faults.append(zone["name"])
    return not faults, "bodies per zone far/second/play: " + ", ".join(counts) if not faults else f"a plane is empty in {faults}"


CORRIDOR_CHECKS = [
    ("R3 nothing floats", nothing_floats),
    ("R4 columns in rows on stone", columns_grouped),
    ("R5 water in closed basins", water_closed),
    ("R6 waterfall fed and received", waterfalls_fed),
    ("R7 one material per zone", one_material),
    ("R8 play area clear", play_area_clear),
    ("R9 decor in clusters", decor_in_clusters),
    ("R10 varied repetition", no_close_repeat),
    ("R11 one hero per zone", one_hero),
    ("R12 seams hidden", seams_hidden),
    ("R13 quiet colors", colors_quiet),
    ("R14 three planes", three_planes),
    ("R15 light per zone", every_zone_lit),
    ("R17 nothing important under the HUD", hud_band_empty),
]

FIELD_CHECKS = [
    ("R3 bodies in one piece", bodies_whole),
    ("R3 bodies apart", bodies_apart),
    ("R3 no body cut at a zone edge", bodies_not_cut),
    ("R3 decor rests on a body", decor_on_bodies),
    ("R4 columns in rows on stone", None),
    ("R5 water in closed basins", None),
    ("R6 waterfall fed and received", None),
    ("R7 one material per zone", one_material),
    ("R8 play area clear", corridor_free),
    ("R9 decor in clusters", decor_in_clusters),
    ("R9 bodies in clusters", bodies_in_clusters),
    ("R10 varied repetition", no_close_repeat),
    ("R10 no body silhouette twice", no_body_twice),
    ("R11 one hero per zone", one_hero),
    ("R12 seams hidden", None),
    ("R13 quiet colors", colors_quiet),
    ("R14 three planes", bodies_on_three_planes),
    ("R15 light per zone", every_zone_lit),
    ("R17 nothing important under the HUD", hud_band_empty),
]

CHECKS = {"corridor": CORRIDOR_CHECKS, "field": FIELD_CHECKS}


def check(cave):
    """Runs the checks of the layout of a map; returns (name, passed, detail) per rule, passed being None for a rule that does not apply."""
    return [(name, *function(cave)) if function else (name, None, NOT_APPLICABLE) for name, function in CHECKS[cave.layout]]
