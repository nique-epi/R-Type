"""Registry of hero kinds: each builds the one subject of a zone from the hero entry of its template."""
import numpy as np

from drawing import stamp
from frame import ZONE
from painters import register

HEROES = {}
HERO_MOVES = (0, 6, -6, 12, -12, 18, -18)


@register(HEROES, "colonnade")
def colonnade(cave, zone, x, hero):
    """Row of columns on the second plane, some of them broken, with a lamp hung between two of them."""
    rng = cave.rng
    columns = [zone["x0"] + cx for cx in hero["columns"]]
    broken = set(int(v) for v in rng.choice(len(columns), size=hero.get("broken", 1), replace=False))
    tops = []
    for position, cx in enumerate(columns):
        ground = int(cave.profile["mid_floor"][cx])
        top = ground - hero["height"] if "height" in hero else int(cave.profile["mid_ceil"][cx - 12:cx + 12].min())
        tops.append(ground - int(rng.integers(34, 78)) if position in broken else top)
        cave.pillar(cx, tops[-1], ground, position in broken)
    gap = int(rng.integers(0, len(columns) - 1))
    lamp = hero.get("lamp")
    if lamp:
        cave.hang_lamp(columns[gap] + 24, "mid", int(rng.integers(*lamp["length"])), lamp["lamp"], lamp["radius"], lamp["power"])
    ground = int(cave.profile["mid_floor"][columns[0]:columns[-1] + 1].max())
    record = {"name": "colonnade", "plane": "mid", "surface": "floor", "x": x, "x0": columns[0] - 12, "x1": columns[-1] + 12, "y0": min(tops), "y1": ground, "role": "hero", "cluster": None, "structure": True}
    cave.decor.append(record)
    cave.reserve("mid", "floor", columns[0] - 12, columns[-1] + 12)
    cave.reserve("mid", "ceil", columns[0] - 12, columns[-1] + 12)
    return record


@register(HEROES, "cluster")
def lead_cluster(cave, zone, x, hero):
    """Cluster led by a named sprite, with optional hanging lamps and an echo of it on another plane."""
    rng = cave.rng
    lead = hero.get("lead")
    if isinstance(lead, (list, tuple)):
        lead = lead[0] if rng.random() < hero["lead_chance"] else lead[1]
    marked = cave.biome.families[hero["family"]].get("hero", [])
    if lead is None and marked:
        lead = marked[int(rng.integers(0, len(marked)))]
    members = cave.cluster(x, hero["family"], hero["plane"], hero["surface"], lead, "hero", HERO_MOVES)
    for lamp in hero.get("lamps", ()):
        cave.hang_lamp(x + lamp["dx"], lamp["plane"], int(rng.integers(*lamp["length"])), lamp["lamp"], lamp["radius"], lamp["power"])
    echo = hero.get("echo")
    if echo:
        cave.cluster(x + echo["dx"], echo["family"], echo["plane"], echo["surface"], echo["lead"])
    return members[0] if members else None


@register(HEROES, "waterfall")
def waterfall(cave, zone, x, hero):
    """Fall of the second plane: a spout under the ceiling, a band that may pass behind a shelf, foam where it lands."""
    biome, level = cave.biome, cave.water_y
    top = int(cave.profile["mid_ceil"][x])
    spout, band = biome.sprites[hero["spout"]], biome.fall
    stamp(cave.fall, spout, x - spout.shape[1] // 2 - 2, top - 6)
    for y in range(top + 12, level + 3, band.shape[0]):
        stamp(cave.fall, band[:min(band.shape[0], level + 3 - y)], x - band.shape[1] // 2, y)
    splash, foam = biome.sprites[hero["splash"]], biome.sprites[hero["foam"]]
    stamp(cave.front, foam, x + 8, level - 5)
    stamp(cave.front, splash, x - splash.shape[1] // 2, level - splash.shape[0] + 7)
    marks = [level - 5 + foam.shape[0] // 2, level - splash.shape[0] + 7 + splash.shape[0] // 2]
    cave.waterfalls.append({"x": x, "top": top, "bottom": level, "zone": zone["name"], "splash": True, "marks": marks, "one_per": hero.get("one_per", 1)})
    cave.beams.append((x - 2, top, 0.04, 12, 0.10, 0.7, hero["tint"]))
    cave.lights.append((x, level - 10, 96, hero["tint"], 0.8))
    record = {"name": "waterfall", "plane": "mid", "surface": "ceil", "x": x, "x0": x - 8, "x1": x + 8, "y0": top, "y1": level, "role": "hero", "cluster": None, "structure": True}
    cave.decor.append(record)
    cave.reserve("mid", "ceil", x - 8, x + 8)
    return record


@register(HEROES, "light_well")
def light_well(cave, zone, x, hero):
    """Shaft of light falling from a crack of the ceiling onto a cluster on the ground."""
    index = cave.biome.index
    top = int(cave.profile["ceil"][x])
    cave.near[top - 2:top + 1, x - 3:x + 4] = index[hero["crack"][0]]
    cave.near[top - 4:top - 2, x - 2:x + 2] = index[hero["crack"][1]]
    cave.beams.append((x, top, 0.12, 9, 0.15, 1.0, hero["tint"]))
    cave.lights.append((x + 20, int(cave.profile["floor"][x + 20]) - 10, 100, hero["tint"], 0.7))
    members = cave.cluster(x + 22, hero["family"], "near", "floor", hero["lead"], "member")
    record = {"name": "light_well", "plane": "near", "surface": "ceil", "x": x, "x0": x - 6, "x1": x + 6, "y0": 0, "y1": top + 2, "role": "hero", "cluster": None, "structure": True}
    cave.decor.append(record)
    cave.reserve("near", "ceil", x - 6, x + 6)
    return record if members else None


def crack_of_light(cave, zone, cracks):
    """Thin shaft of light through a crack of the ceiling, on the third of the zone the hero does not hold."""
    rng = cave.rng
    bx = zone["x0"] + ZONE - zone["hero"]["x"] + int(rng.integers(-cracks["reach"], cracks["reach"] + 1))
    top = int(cave.profile["ceil"][bx])
    slope = float(rng.choice(cracks["slopes"]))
    if top <= 0:
        return
    cave.near[top - 1:top + 1, bx - 2:bx + 3] = cave.biome.index[cracks["key"]]
    cave.beams.append((bx, top, slope, cracks["width"], cracks["spread"], cracks["power"], cracks["tint"]))


def place_heroes(cave):
    """The one subject of each zone, set on a third of the screen, then the cracks of light of the materials that have them."""
    for zone in cave.zones:
        hero = zone["hero"]
        record = HEROES[hero["kind"]](cave, zone, zone["x0"] + hero["x"], hero)
        cracks = cave.biome.materials[zone["material"]].get("cracks")
        if cracks:
            crack_of_light(cave, zone, cracks)
        zone["hero_record"] = record
