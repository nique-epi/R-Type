"""Level design checks run on every generated map; a map that fails one is never written."""
import math

import numpy as np

from frame import CEIL_LIMIT, DECOR_CLEAR, H, HANG_LIMIT, HUD_Y, MID_CEIL_LIMIT, MID_FLOOR_LIMIT, PLAY_BOTTOM, PLAY_TOP, ZONE

STRUCTURES = ("far_pillar", "colonnade", "waterfall", "light_well")
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


def nothing_floats(cave):
    """Rule 3: every solid pixel reaches a border of the picture and every prop rests on something."""
    loose = sum(int((cave.solid[plane] & ~attached(cave.solid[plane])).sum()) for plane in ("near", "mid"))
    adrift = []
    for record in cave.decor:
        if record["plane"] == "far" or record["name"] in STRUCTURES:
            continue
        solid = cave.solid[record["plane"]]
        row = record["y1"] - 1 if record["surface"] == "floor" else max(record["y0"] - (1 if record["role"] == "lamp" else 0), 0)
        if not solid[min(row, H - 1), record["x0"] + 1:record["x1"] - 1].any():
            adrift.append(record["name"])
    return loose == 0 and not adrift, f"{loose} loose terrain pixels, {len(adrift)} props without support {adrift[:3]}"


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
    intruders = [r["name"] for r in cave.decor if r["plane"] == "near" and r["name"] not in STRUCTURES and r["y0"] < DECOR_CLEAR[1] and r["y1"] > DECOR_CLEAR[0]]
    low_roof = int((cave.profile["ceil"] > CEIL_LIMIT).sum())
    long_hangers = [r["name"] for r in cave.decor if r["plane"] == "near" and r["surface"] == "ceil" and r["name"] not in STRUCTURES and r["y1"] > HANG_LIMIT]
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
            if record["plane"] == plane and record["name"] not in STRUCTURES + DRAWN_BY_RULE:
                seen.setdefault(record["name"], []).append(record["x"])
        for name, places in seen.items():
            places.sort()
            if any(b - a < 160 for a, b in zip(places, places[1:])):
                faults.append(f"{name} twice within 160 px on {plane}")
    for first in cave.decor:
        for second in cave.decor:
            if first["name"] == second["name"] and first["plane"] == "mid" and second["plane"] == "near" and first["name"] not in STRUCTURES + DRAWN_BY_RULE and abs(first["x"] - second["x"]) < 96:
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


CHECKS = [
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


def check(cave):
    """Runs every check; returns (name, passed, detail) per rule."""
    return [(name, *function(cave)) for name, function in CHECKS]
