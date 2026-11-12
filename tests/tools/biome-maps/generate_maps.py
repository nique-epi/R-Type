#!/usr/bin/env python3
"""Generates maps of a biome from its hand-written zone templates and keeps only the maps that pass the level design checks."""
import argparse
import json
import math
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rules  # noqa: E402
from biome_loader import Biome  # noqa: E402
from drawing import BAYER, brick, stamp, stepped_noise  # noqa: E402
from frame import (CEIL_LIMIT, DECOR_CLEAR, FLOOR_BOTTOM, FLOOR_LIMIT, H, HANG_LIMIT, MID_CEIL_LIMIT, MID_FLOOR_LIMIT,  # noqa: E402
                   SPIRE_HEIGHT, ZONE)
from heroes import place_heroes  # noqa: E402
from painters import COVERS, FAR_STRUCTURES, HANGERS, PAINTERS, shared_of  # noqa: E402

LIGHT_STEP = 0.13
PROFILE_OF = {("near", "floor"): "floor", ("near", "ceil"): "ceil", ("mid", "floor"): "mid_floor", ("mid", "ceil"): "mid_ceil"}


def profile_of(steps, width=ZONE):
    """Height per column of a list of (x, y) steps, each held until the next one."""
    out = np.zeros(width, int)
    for position, (x, y) in enumerate(steps):
        end = steps[position + 1][0] if position + 1 < len(steps) else width
        out[x:end] = y
    return out


def jitter_steps(steps, rng, move_x, step_y, limits, water_y):
    """Template steps moved a little, the two connector heights kept as written."""
    low, high = limits
    points = [list(point) for point in steps]
    for position in range(1, len(points)):
        if move_x:
            moved = points[position][0] + 8 * int(rng.integers(-1, 2))
            if moved >= points[position - 1][0] + 16 and (position + 1 == len(points) or moved <= steps[position + 1][0] - 24) and moved <= ZONE - 24:
                points[position][0] = moved
        if step_y and position + 1 < len(points):
            y = points[position][1]
            wet = y > water_y
            y += step_y * int(rng.integers(-1, 2))
            points[position][1] = int(np.clip(y, water_y + 4, FLOOR_BOTTOM)) if wet else int(np.clip(y, low, min(high, water_y - 6)))
    return [tuple(point) for point in points]


def roughen(profile, rng):
    """Natural edge: each cliff becomes two or three small steps, and the surface moves by a pixel or two."""
    out = profile.copy()
    for x in np.where(np.diff(profile) != 0)[0] + 1:
        jump = profile[x] - profile[x - 1]
        count = min(3, abs(jump) // 4)
        run = int(rng.integers(2, 5))
        for step in range(count):
            out[x + step * run:x + step * run + run] = profile[x - 1] + int(round(jump * (step + 1) / (count + 1)))
    return out + stepped_noise(len(out), rng, (3, 9), [-1, 0, 0, 0, 1, 1, 2])


def instantiate(biome, name, mirrored, rng, water_y):
    """One zone: a template with its heights jittered, mirrored when asked; a template under an open sky has no ceiling."""
    template = biome.templates[name]
    aligned = biome.materials[template["material"]]["aligned"]
    fixed = template.get("fixed", [])
    sky = bool(template.get("sky"))
    zone = {"name": name, "mirrored": mirrored, "material": template["material"], "ambient": template["ambient"], "mood": template["mood"], "far": template.get("far", "default"), "sky": sky}
    limits = {"floor": (FLOOR_LIMIT, FLOOR_BOTTOM), "ceil": (8, CEIL_LIMIT), "mid_floor": (MID_FLOOR_LIMIT, FLOOR_BOTTOM), "mid_ceil": (CEIL_LIMIT, MID_CEIL_LIMIT)}
    for key, bounds in limits.items():
        if sky and "ceil" in key:
            zone[key] = np.zeros(ZONE, int)
            continue
        locked = key in fixed
        step_y = 0 if locked or (aligned and "floor" in key) else (8 if aligned else 4)
        steps = jitter_steps(template[key], rng, not aligned and not locked, step_y, bounds, water_y)
        profile = profile_of(steps)
        if not aligned:
            profile = roughen(profile, rng)
        zone[key] = profile[::-1].copy() if mirrored else profile
    flip = (lambda x: ZONE - x) if mirrored else (lambda x: x)
    for key in ("near_floor", "near_ceil", "mid_floor_decor", "mid_ceil_decor"):
        zone[key + "_slots"] = [(flip(x), family) for x, family in template.get(key, [])]
    zone["pillars"] = [tuple(sorted((flip(a), flip(b)))) for a, b in template.get("mid_pillars", [])]
    hero = dict(template["hero"])
    hero["x"] = flip(hero["x"])
    if "columns" in hero:
        hero["columns"] = sorted(flip(x) for x in hero["columns"])
    if "ledge" in hero:
        x0, x1, y0, y1 = hero["ledge"]
        hero["ledge"] = (*sorted((flip(x0), flip(x1))), y0, y1)
    zone["hero"] = hero
    if "mid_exempt" in template:
        zone["mid_exempt"] = tuple(sorted((flip(template["mid_exempt"][0]), flip(template["mid_exempt"][1]))))
    zone["left"], zone["right"] = (template["right"], template["left"]) if mirrored else (template["left"], template["right"])
    return zone


def choose_sequence(biome, rng, count):
    """Chain of templates whose connectors match, closed on dry ground at both ends."""
    templates, wet = biome.templates, biome.wet_connectors
    options = [(name, mirrored) for name in templates for mirrored in (False, True)]

    def ends(option):
        """Left and right connectors of a template, swapped when it is mirrored."""
        template = templates[option[0]]
        return (template["right"], template["left"]) if option[1] else (template["left"], template["right"])

    def is_wet(option):
        """True when one end of a template lets the water through."""
        return any(end in wet for end in ends(option))

    def extend(chain):
        """Chain completed by a depth-first search over the templates, in a weighted random order."""
        if len(chain) == count:
            return chain if ends(chain[-1])[1] not in wet else None
        keys = [rng.random() ** (1.0 / templates[name].get("weight", 1)) for name, _ in options]
        order = [options[position] for position in np.argsort(keys)[::-1]]
        for option in order:
            left, _ = ends(option)
            names = [name for name, _ in chain]
            if option in chain or (option[0] in names and not is_wet(option) and len(names) < len(templates)):
                continue
            if not chain and left in wet:
                continue
            if chain and ends(chain[-1])[1] != left:
                continue
            hero = templates[option[0]]["hero"]
            if "one_per" in hero:
                same = sum(templates[name]["hero"]["kind"] == hero["kind"] for name in names + [option[0]])
                if same > math.ceil(count / hero["one_per"]):
                    continue
            found = extend(chain + [option])
            if found:
                return found
        return None

    return extend([])


class CaveMap:
    """One generated map: its terrain masks, its layers, and the record of everything placed on it."""

    def __init__(self, biome, seed, zone_count, sequence=None):
        """Builds the whole map from a seed: chain of zones, terrain, decor, light."""
        self.biome = biome
        self.seed = seed
        self.forced = ["%s%s" % (name, "*" if mirrored else "") for name, mirrored in sequence] if sequence else None
        self.rng = np.random.default_rng(seed)
        self.zone_count = len(sequence) if sequence else zone_count
        zone_count = self.zone_count
        self.width = ZONE * zone_count
        self.water_y = biome.water["level"] if biome.water else H + 1
        self.decor, self.columns, self.waterfalls, self.lights, self.beams, self.seam_covers = [], [], [], [], [], []
        self.extents, self.rhythm = {}, {}
        self.cluster_count = 0
        sequence = sequence or choose_sequence(biome, self.rng, zone_count)
        if not sequence:
            sys.exit(f"the templates of {biome.name} cannot be chained into {zone_count} zones: check their connectors")
        self.zones = [instantiate(biome, name, mirrored, self.rng, self.water_y) for name, mirrored in sequence]
        for position, zone in enumerate(self.zones):
            zone["x0"] = position * ZONE
        self.build_profiles()
        self.build_masks()
        self.far = np.zeros((H, self.width, 3))
        self.far[:] = biome.backdrop
        self.fall = np.zeros((H, self.width), np.uint8)
        self.mid = np.zeros((H, self.width), np.uint8)
        self.near = np.zeros((H, self.width), np.uint8)
        self.front = np.zeros((H, self.width), np.uint8)
        self.grass = {"near": np.zeros(self.width, int), "mid": np.zeros(self.width, int)}
        self.paint_far()
        for plane in ("mid", "near"):
            self.paint_terrain(plane)
            self.grow(plane)
        self.place_seam_covers()
        place_heroes(self)
        self.place_clusters()
        self.collect_lights()
        self.image = self.compose()

    def zone_of(self, x):
        """Zone that holds a column."""
        return self.zones[min(int(x) // ZONE, self.zone_count - 1)]

    def is_wet(self, connector):
        """True when a connector lets the water through."""
        return connector in self.biome.wet_connectors

    def build_profiles(self):
        """Joins the zone profiles, breaks a few blocks out of the edges of the materials that ask for it, then keeps the corridor clear."""
        self.profile = {key: np.concatenate([zone[key] for zone in self.zones]) for key in ("floor", "ceil", "mid_floor", "mid_ceil")}
        self.sky = np.concatenate([np.full(ZONE, zone["sky"]) for zone in self.zones])
        rng, water_y = self.rng, self.water_y
        for zone in self.zones:
            broken = self.biome.materials[zone["material"]].get("broken_edges")
            if not broken:
                continue
            kept = [(zone["x0"] + x - 20, zone["x0"] + x + 20) for x in zone["hero"].get("columns", [])]
            for key, sign in (("floor", 1), ("ceil", -1), ("mid_floor", 1), ("mid_ceil", -1)):
                profile, chance = self.profile[key], broken[key]
                for cell in range(zone["x0"] + 24, zone["x0"] + ZONE - 24, 8):
                    if any(a <= cell <= b for a, b in kept) and "mid" in key:
                        continue
                    if rng.random() < chance and (sign < 0 or profile[cell] < water_y - 12) and (sign > 0 or profile[cell] > 16):
                        profile[cell:cell + 8] += 8 * sign
        floor = np.maximum(self.profile["floor"], FLOOR_LIMIT)
        floor[(floor >= water_y - 1) & (floor <= water_y)] = water_y - 2
        edges = np.diff(np.concatenate([[0], (floor > water_y).astype(int), [0]]))
        for x0, x1 in zip(np.where(edges == 1)[0], np.where(edges == -1)[0]):
            if x1 - x0 < 24:
                floor[x0:x1] = water_y - 2
        self.profile["floor"] = floor
        self.profile["ceil"] = np.where(self.sky, 0, np.clip(self.profile["ceil"], 6, CEIL_LIMIT))
        self.mid_exempt = np.zeros(self.width, bool)
        for zone in self.zones:
            if "mid_exempt" in zone:
                self.mid_exempt[zone["x0"] + zone["mid_exempt"][0]:zone["x0"] + zone["mid_exempt"][1]] = True
        self.profile["mid_ceil"] = np.where(self.sky, 0, np.minimum(self.profile["mid_ceil"], MID_CEIL_LIMIT))
        self.profile["mid_floor"] = np.where(self.mid_exempt, self.profile["mid_floor"], np.maximum(self.profile["mid_floor"], MID_FLOOR_LIMIT))
        for x in np.where(np.diff(self.sky.astype(int)) != 0)[0] + 1:
            self.taper_ceiling(int(x), -1 if self.sky[x] else 1)

    def taper_ceiling(self, x, side):
        """Ends a ceiling in three thinner steps where it meets an open sky, instead of a straight cut."""
        reach = 0
        for share in (0.25, 0.5, 0.75):
            width = int(self.rng.integers(6, 13))
            start = x - reach - width if side < 0 else x + reach
            for key in ("ceil", "mid_ceil"):
                self.profile[key][start:start + width] = (self.profile[key][start:start + width] * share).astype(int)
            reach += width

    def build_masks(self):
        """Solid masks and material maps of the two terrain planes, with the piers of the second plane."""
        rows = np.arange(H)[:, None]
        rng, biome, identifier = self.rng, self.biome, self.biome.material_id
        self.solid = {
            "near": (rows < self.profile["ceil"][None, :]) | (rows >= self.profile["floor"][None, :]),
            "mid": (rows < self.profile["mid_ceil"][None, :]) | (rows >= self.profile["mid_floor"][None, :]),
        }
        material = np.zeros(self.width, np.uint8)
        for zone in self.zones:
            material[zone["x0"]:zone["x0"] + ZONE] = identifier[zone["material"]]
        self.material = {plane: np.tile(material, (H, 1)) for plane in ("near", "mid")}
        for position in range(1, self.zone_count):
            left, right, x = self.zones[position - 1], self.zones[position], position * ZONE
            if left["material"] == right["material"] or self.is_wet(left["right"]):
                continue
            for plane in ("near", "mid"):
                for y in range(0, H, 8):
                    reach = 8 * int(rng.integers(-3, 4))
                    if reach > 0:
                        self.material[plane][y:y + 8, x:x + reach] = identifier[left["material"]]
                    elif reach < 0:
                        self.material[plane][y:y + 8, x + reach:x] = identifier[right["material"]]
        self.seams = []
        piers = []
        for zone in self.zones:
            piers += [(zone["x0"] + a, zone["x0"] + b, zone["material"]) for a, b in zone["pillars"]]
            if "ledge" in zone["hero"]:
                x0, x1, y0, y1 = zone["hero"]["ledge"]
                under = y1 + stepped_noise(x1 - x0, rng, (4, 9), [-2, -1, 0, 0, 1])
                over = y0 + stepped_noise(x1 - x0, rng, (5, 11), [0, 0, 1, 2])
                span = np.arange(H)[:, None]
                self.solid["mid"][:, zone["x0"] + x0:zone["x0"] + x1] |= (span >= over[None, :]) & (span < under[None, :])
        for position in range(1, self.zone_count):
            left, right = self.zones[position - 1], self.zones[position]
            seam = {"x": position * ZONE, "change": left["material"] != right["material"], "wet": self.is_wet(left["right"])}
            self.seams.append(seam)
            if seam["wet"]:
                continue
            shared = not seam["change"] and biome.materials[left["material"]]["aligned"]
            kind = left["material"] if shared else biome.seam_material
            if biome.materials[kind]["aligned"]:
                piers.append((seam["x"] - 16, seam["x"] + 16, kind))
            else:
                offset, half = int(rng.integers(-8, 9)), int(rng.integers(15, 22))
                piers.append((seam["x"] + offset - half, seam["x"] + offset + half, kind))
            seam["pillar"] = piers[-1][:2]
        for x0, x1, kind in piers:
            ceiling = int(self.profile["mid_ceil"][x0:x1].max())
            ground = int(self.profile["mid_floor"][x0:x1].min())
            if biome.materials[kind]["aligned"]:
                top = 0 if ceiling > 0 else (ground - SPIRE_HEIGHT) // 8 * 8
                self.solid["mid"][top:, x0:x1] = True
                self.material["mid"][top:, x0:x1] = identifier[kind]
                continue
            left = stepped_noise(H, rng, (5, 12), [0, 1, 2, 3, 4])
            right = stepped_noise(H, rng, (5, 12), [0, 1, 2, 3, 4])
            first = max(ceiling - 6, 0) if ceiling > 0 else ground - SPIRE_HEIGHT
            for y in range(first, min(ground + 6, H)):
                under_roof = 10 - (y - ceiling) // 2 if ceiling > 0 else 0
                flare = min(max(0, under_roof, 10 - (ground - y) // 2), 10)
                a, b = max(x0 + left[y] - flare, 0), min(x1 - right[y] + flare, self.width)
                self.solid["mid"][y, a:b] = True
                self.material["mid"][y, a:b] = identifier[kind]
        floor = self.profile["floor"]
        wet = floor > self.water_y
        edges = np.diff(np.concatenate([[0], wet.astype(int), [0]]))
        self.water_runs = list(zip(np.where(edges == 1)[0].tolist(), np.where(edges == -1)[0].tolist()))
        self.water = np.zeros((H, self.width), bool)
        for x0, x1 in self.water_runs:
            self.water[self.water_y:, x0:x1] = ~self.solid["near"][self.water_y:, x0:x1]

    def paint_far(self):
        """Far plane: flat two-tone ground and ceiling, the far structure of each zone and sprite silhouettes."""
        rng, biome = self.rng, self.biome
        far_a, far_b = biome.far_a, biome.far_b
        self.far_floor, self.far_ceil = np.zeros(self.width, int), np.zeros(self.width, int)
        x = 0
        while x < self.width:
            span = int(rng.integers(3, 10)) * 8
            ground = min(int(self.profile["mid_floor"][x:x + span].min()), 200) - 4 * int(rng.integers(3, 10))
            roof = min(int(self.profile["mid_ceil"][x:x + span].max()) + 4 * int(rng.integers(2, 8)), ground - 28)
            self.far_floor[x:x + span], self.far_ceil[x:x + span] = ground, roof
            x += span
        self.far_floor, self.far_ceil = roughen(self.far_floor, rng), roughen(self.far_ceil, rng)
        self.far_ceil = np.where(self.sky, 0, self.far_ceil)
        for x in range(self.width):
            ground, roof = int(self.far_floor[x]), int(self.far_ceil[x])
            self.far[0:roof, x] = far_a
            self.far[ground:, x] = far_a
            self.far[ground:ground + 2, x] = far_b
            for y in range(ground + 2, ground + 6):
                if (x + y) % 2 == 0:
                    self.far[y, x] = far_b
            for y in range(max(roof - 4, 0), roof):
                if (x + y) % 2 == 0 or y == roof - 1:
                    self.far[y, x] = far_b
            if x and self.far_floor[x - 1] > ground:
                self.far[ground:self.far_floor[x - 1], x] = far_b
        used = {}
        lit_keys = [biome.index[key] for key in biome.far_lit_keys]
        for zone in self.zones:
            choice = biome.far_sets[zone["far"]]
            spots = sorted(int(v) for v in rng.choice(np.arange(40, ZONE - 40, 8), size=5, replace=False))
            spots = [x for position, x in enumerate(spots) if position == 0 or x - spots[position - 1] >= 56]
            for position, local in enumerate(spots):
                surface = "ceil" if position % 3 == 1 else "floor"
                x = zone["x0"] + local
                if choice["pillars"] and position == len(spots) - 1:
                    FAR_STRUCTURES[choice["pillars"]](self, x)
                    self.decor.append({"name": "far_pillar", "plane": "far", "surface": "floor", "x": x, "x0": x - 6, "x1": x + 6, "y0": 0, "y1": H, "role": "structure", "cluster": None})
                    continue
                names = [name for name in choice[surface] if all(abs(x - other) >= 160 for other in used.get(name, []))]
                if not names:
                    continue
                name = names[int(rng.integers(0, len(names)))]
                if surface == "ceil" and self.far_ceil[x] <= 0:
                    continue
                sprite = biome.sprites[name]
                h, w = sprite.shape
                y = int(self.far_floor[x]) + 2 - h if surface == "floor" else int(self.far_ceil[x]) - 2
                lit = np.isin(sprite, lit_keys)
                block = self.far[max(y, 0):y + h, x - w // 2:x - w // 2 + w]
                part = sprite[max(-y, 0):, :block.shape[1]][:block.shape[0]]
                block[part > 0] = far_a
                block[(part > 0) & lit[max(-y, 0):, :block.shape[1]][:block.shape[0]]] = far_b
                used.setdefault(name, []).append(x)
                self.decor.append({"name": name, "plane": "far", "surface": surface, "x": x, "x0": x - w // 2, "x1": x - w // 2 + w, "y0": y, "y1": y + h, "role": "silhouette", "cluster": None})

    def paint_terrain(self, plane):
        """Paints each material of one plane with the painter its biome names for it."""
        solid, material, canvas, biome = self.solid[plane], self.material[plane], getattr(self, plane), self.biome
        shared = shared_of(solid)
        for name, declared in biome.materials.items():
            mask = solid & (material == biome.material_id[name])
            if mask.any():
                PAINTERS[declared["painter"]](self, canvas, mask, shared, declared["paint"])

    def grow(self, plane):
        """Moss, grass and hanging growth of each material, drawn stroke by stroke from rules, never from a tile."""
        rng, canvas, solid, material = self.rng, getattr(self, plane), self.solid[plane], self.material[plane]
        biome, index, water_y = self.biome, self.biome.index, self.water_y
        declared = {identifier: biome.materials[name] for name, identifier in biome.material_id.items()}
        floor = self.profile[PROFILE_OF[(plane, "floor")]]
        ceiling = self.profile[PROFILE_OF[(plane, "ceil")]]
        thickness = 2 + stepped_noise(self.width, rng, (3, 11), [0, 0, 1, 1, 2])
        bare = stepped_noise(self.width, rng, (14, 46), [0, 0, 0, 1]).astype(bool)
        for x in range(self.width):
            y = int(floor[x])
            if y >= water_y - 1 or y >= H or not solid[y, x]:
                continue
            moss = declared[material[min(y, H - 1), x]]["moss"]
            if not moss or not COVERS[moss["cover"]](x, bare[x], self.seed):
                continue
            keys = moss["keys"]
            for d in range(-1, thickness[x] + 1):
                key = keys["top"] if d <= 0 else (keys["body"] if d == 1 or (x + y + d) % 2 == 0 else keys["deep"])
                if 0 <= y + d < H:
                    canvas[y + d, x] = index[key]
        x = int(rng.integers(2, 16))
        while x < self.width - 8:
            grass = declared[material[min(int(floor[x]), H - 1), x]]["grass"]
            keys = grass["keys"]
            tallest = int(rng.choice(grass["heights"]))
            for _ in range(int(rng.integers(*grass["blades"]))):
                offset = int(rng.integers(-7, 8))
                bx = x + offset
                if not 2 <= bx < self.width - 2 or floor[bx] >= water_y - 1:
                    continue
                height = 1 + int(tallest * (1 - abs(offset) / 9.0)) + int(rng.integers(0, 2))
                lean = int(rng.choice([-1, 0, 0, 1]))
                self.grass[plane][bx] = max(self.grass[plane][bx], height)
                for k in range(height):
                    px = bx + (lean if k > height * 0.6 else 0)
                    key = keys["tip"] if k >= height - 1 and height > 3 else (keys["blade"] if k >= height // 2 else keys["base"])
                    canvas[int(floor[bx]) - 1 - k, px] = index[key]
            x += int(rng.integers(*grass["spacing"]))
        heights = self.grass[plane]
        for start in range(0, self.width - 24, 12):
            first, second = heights[start:start + 12], heights[start + 12:start + 24]
            if first.any() and (first == second).all():
                bx = start + 12 + int(np.argmax(second))
                blade = declared[material[min(int(floor[bx]), H - 1), bx]]["grass"]["keys"]["blade"]
                for k in range(heights[bx], heights[bx] + 2):
                    canvas[int(floor[bx]) - 1 - k, bx] = index[blade]
                heights[bx] += 2
        x = int(rng.integers(6, 30))
        while x < self.width - 14:
            hanging = declared[material[max(int(ceiling[x]) - 1, 0), x]]["hanging"]
            size = int(rng.choice([1, 1, 2, 2, 3]))
            for position, offset in enumerate([0] + [int(v) for v in rng.integers(-12, 13, size * 2)]):
                sx = x + offset
                if not 3 <= sx < self.width - 3 or ceiling[sx] <= 0:
                    continue
                HANGERS[hanging["kind"]](self, plane, canvas, solid, (sx, int(ceiling[sx]), position, size), hanging["keys"])
            x += int(rng.integers(*hanging["spacing"]))

    def reserve(self, plane, surface, x0, x1):
        """Books a stretch of a surface for one group of decor."""
        self.extents.setdefault((plane, surface), []).append((x0, x1))

    def fits(self, plane, surface, x0, x1, rhythm=False, margin=24):
        """True when a new group keeps clear of the booked ones and, for a free cluster, does not fall into an even rhythm."""
        booked = self.extents.get((plane, surface), [])
        if any(x1 + margin > a and x0 - margin < b for a, b in booked):
            return False
        if not rhythm:
            return True
        centers = sorted(self.rhythm.get((plane, surface), []) + [(x0 + x1) / 2])
        steps = np.diff(centers)
        return not any(abs(steps[i] - steps[i + 1]) < 10 and max(steps[i], steps[i + 1]) < 240 for i in range(len(steps) - 1))

    def plan(self, name, x, plane, surface, search=14, planned=()):
        """Finds where a sprite can stand on the ground or hang from the ceiling of a plane, without drawing it."""
        sprite = self.biome.sprites[name]
        h, w = sprite.shape
        role = self.biome.roles.get(name)
        if role and ("hang" in role["marks"]) != (surface == "ceil"):
            return None
        for other in list(self.decor) + list(planned):
            if other["plane"] == "far":
                continue
            if other["name"] == name and abs(other["x"] - x) < (162 if other["plane"] == plane else 98) + search:
                return None
        profile = self.profile[PROFILE_OF[(plane, surface)]]
        crop = 0
        for move in sorted(range(-search, search + 1), key=abs):
            x0 = int(x) + move - w // 2
            if x0 < 2 or x0 + w > self.width - 2:
                continue
            base = profile[x0 + w // 4:x0 + w - w // 4]
            if base.max() - base.min() > 4:
                continue
            if surface == "floor":
                if base.max() > self.water_y - 3:
                    continue
                y0 = int(base.max()) + 2 - h
                if plane == "near" and y0 < DECOR_CLEAR[1]:
                    continue
            else:
                if base.min() <= 0:
                    continue
                y0 = int(base.min()) - 2
                if plane == "near":
                    crop = max(y0 + h - HANG_LIMIT, 0)
                    if h - crop < 16:
                        continue
            return {"name": name, "plane": plane, "surface": surface, "x": x0 + w // 2, "x0": x0, "x1": x0 + w, "y0": y0, "y1": y0 + h - crop, "crop": crop}
        return None

    def plan_strokes(self, x, plane, surface):
        """A small piece drawn by rule where no small sprite fits: a few grass blades on the ground, a few strands under a ceiling."""
        profile, rng = self.profile[PROFILE_OF[(plane, surface)]], self.rng
        x = int(x)
        if not 6 <= x < self.width - 6 or (surface == "floor" and profile[x - 4:x + 5].max() > self.water_y - 3) or (surface == "ceil" and profile[x - 4:x + 5].min() <= 0):
            return None
        strokes = [(dx, int(rng.integers(4, 11) if surface == "floor" else rng.integers(6, 19))) for dx in sorted(int(v) for v in rng.choice(np.arange(-4, 5), size=int(rng.integers(3, 5)), replace=False))]
        base = int(profile[x])
        if surface == "ceil" and plane == "near":
            strokes = [(dx, min(length, HANG_LIMIT - base)) for dx, length in strokes]
        tallest = max(length for _, length in strokes)
        if tallest < 4:
            return None
        y0, y1 = (base - tallest, base + 1) if surface == "floor" else (base - 2, base + tallest)
        return {"name": "strokes", "plane": plane, "surface": surface, "x": x, "x0": x - 4, "x1": x + 5, "y0": y0, "y1": y1, "strokes": strokes}

    def commit(self, record, role, cluster):
        """Draws a planned sprite, or a planned group of strokes, and records it."""
        biome, index = self.biome, self.biome.index
        if record["name"] == "strokes":
            canvas, keys = getattr(self, record["plane"]), biome.strokes[record["surface"]]
            profile = self.profile[PROFILE_OF[(record["plane"], record["surface"])]]
            for dx, length in record.pop("strokes"):
                x = record["x"] + dx
                start = int(profile[x])
                for k in range(length):
                    y = start - 1 - k if record["surface"] == "floor" else start + k
                    tip = k >= length - 2
                    if record["surface"] == "floor":
                        canvas[y, x] = index[keys["tip"] if tip and length > 5 else keys["blade"]]
                    else:
                        canvas[y, x] = index[keys["young"] if k < 3 else keys["old"]]
                    if record["surface"] == "ceil" and k % 5 == 3:
                        canvas[y, x + (1 if dx % 2 else -1)] = index[keys["young"]]
                if record["surface"] == "ceil":
                    canvas[start - 1, x] = index[keys["root"]]
            record.update({"role": role, "cluster": cluster})
            self.decor.append(record)
            return record
        stamp(getattr(self, record["plane"]), biome.sprites[record["name"]][record.pop("crop", 0):], record["x0"], record["y0"])
        record.update({"role": role, "cluster": cluster})
        self.decor.append(record)
        return record

    def place_any(self, names, x, plane, surface, role, cluster, search=14):
        """Places the first sprite of a shuffled list that fits, as a group of its own."""
        for position in self.rng.permutation(len(names)):
            record = self.plan(names[position], x, plane, surface, search)
            if record and self.fits(plane, surface, record["x0"], record["x1"]):
                self.reserve(plane, surface, record["x0"], record["x1"])
                return self.commit(record, role, cluster)
        return None

    def cluster(self, x, family, plane, surface, lead=None, role="cluster", moves=(0, 14, -14, 28, -28, 44, -44)):
        """One group of related decor: a large piece, a medium one touching it, a small one a little apart."""
        rng, names, sprites = self.rng, self.biome.families[family], self.biome.sprites
        leads = [lead] if lead else [names["large"][position] for position in rng.permutation(len(names["large"]))]
        first_side = int(rng.choice([-1, 1]))
        for move in moves:
            for lead_name in leads:
                large = self.plan(lead_name, x + move, plane, surface, 10)
                if not large:
                    continue
                half = (large["x1"] - large["x0"]) // 2
                for side in (first_side, -first_side):
                    medium = None
                    for position in rng.permutation(len(names["medium"])):
                        name = names["medium"][position]
                        if name == large["name"]:
                            continue
                        reach = half + sprites[name].shape[1] // 2
                        medium = self.plan(name, large["x"] + side * (reach - int(rng.integers(4, 9))), plane, surface, 3, [large])
                        if medium and max(medium["x0"] - large["x1"], large["x0"] - medium["x1"]) <= 0:
                            break
                        medium = None
                    if not medium:
                        continue
                    members = [large, medium]
                    for position in rng.permutation(len(names["small"])):
                        name = names["small"][position]
                        if name in (large["name"], medium["name"]):
                            continue
                        reach = half + sprites[name].shape[1] // 2 + int(rng.integers(6, 15))
                        small = self.plan(name, large["x"] - side * reach, plane, surface, 2, members)
                        if small and 3 <= max(small["x0"] - large["x1"], large["x0"] - small["x1"]) <= 24:
                            members.append(small)
                            break
                    if len(members) == 2:
                        strokes = self.plan_strokes(large["x"] - side * (half + 5 + int(rng.integers(6, 13))), plane, surface)
                        if strokes:
                            members.append(strokes)
                    x0, x1 = min(m["x0"] for m in members), max(m["x1"] for m in members)
                    if not self.fits(plane, surface, x0, x1, role == "cluster"):
                        continue
                    self.cluster_count += 1
                    self.reserve(plane, surface, x0, x1)
                    if role == "cluster":
                        self.rhythm.setdefault((plane, surface), []).append((x0 + x1) / 2)
                    return [self.commit(member, role if position == 0 else "member", self.cluster_count) for position, member in enumerate(members)]
        return []

    def pillar(self, x, top, ground, broken):
        """Column of the biome on the second plane, standing on the floor, whole or broken."""
        rng, biome, spec = self.rng, self.biome, self.biome.column
        index, shaft, steps = biome.index, spec["sprite"], spec["steps"]
        wide = steps[0][0]
        column = np.zeros((ground - top, wide), np.uint8)
        height = column.shape[0]
        for y in range(0, height, shaft.shape[0]):
            stamp(column, shaft[:min(shaft.shape[0], height - y)], (wide - shaft.shape[1]) // 2, y)
        stacked = 0
        for width, tall in steps:
            stamp(column, brick(biome, spec["stone"], width, tall, rng, False), wide // 2 - width // 2, height - stacked - tall)
            if not broken:
                stamp(column, brick(biome, spec["stone"], width, tall, rng, False), wide // 2 - width // 2, stacked)
            stacked += tall
        if broken:
            edge = stepped_noise(wide, rng, (2, 4), [0, 1, 2, 3, 5]) + np.arange(wide) // 3
            for cx in range(wide):
                column[:edge[cx], cx] = 0
        for _ in range(int(rng.integers(2, 6))):
            y = int(rng.integers(12, max(height - 14, 13)))
            column[y:y + int(rng.integers(2, 5)), int(rng.integers(6, 15))] = index[spec["scar"]]
        if rng.random() < 0.6:
            y = int(rng.integers(height // 3, height - 14))
            column[y:y + 2, 5:19][column[y:y + 2, 5:19] > 0] = index[spec["moss"][0]]
            column[y + 2, 6:12][column[y + 2, 6:12] > 0] = index[spec["moss"][1]]
        stamp(self.mid, column, x - wide // 2, top)
        self.columns.append({"x": x, "base": ground, "top": top, "plane": "mid", "broken": broken})

    def hang_lamp(self, x, plane, length, lamp, radius, power):
        """Lamp of the biome hung by a cord from the ceiling of a plane, with its light."""
        spec = self.biome.lamps[lamp]
        sprite = spec["sprite"]
        y0 = int(self.profile[PROFILE_OF[(plane, "ceil")]][x])
        if plane == "near":
            length = min(length, HANG_LIMIT - sprite.shape[0] - y0)
        if length < 4 or y0 <= 0:
            return
        canvas = getattr(self, plane)
        canvas[y0:y0 + length:2, x] = self.biome.index[spec["cord"]]
        stamp(canvas, sprite, x - sprite.shape[1] // 2, y0 + length)
        scale = 1.0 if plane == "near" else 0.8
        self.lights.append((x, y0 + length + 3, int(radius * scale), spec["tint"], power * scale))
        self.decor.append({"name": spec["name"], "plane": plane, "surface": "ceil", "x": x, "x0": x - 5, "x1": x + 5, "y0": y0, "y1": y0 + length + sprite.shape[0], "role": "lamp", "cluster": None})

    def place_seam_covers(self):
        """Hides each seam between two zones behind a piece of the material that ranks first there, on the ground and under the ceiling."""
        materials = self.biome.materials
        for seam in self.seams:
            x = seam["x"]
            sides = [self.zone_of(x - 1)["material"], self.zone_of(x)["material"]]
            if not seam["wet"]:
                names = max((materials[side]["seam_floor"] for side in sides), key=lambda entry: entry[0])[1]
                record = self.place_any(names, x + int(self.rng.integers(-5, 6)), "near", "floor", "seam", None, 10)
                if record:
                    self.seam_covers.append(record)
            names = max((materials[side]["seam_ceil"] for side in sides), key=lambda entry: entry[0])[1]
            record = self.place_any(names, x + int(self.rng.integers(-5, 6)), "near", "ceil", "seam", None, 6)
            if record:
                self.seam_covers.append(record)

    def place_clusters(self):
        """The groups of decor each template asks for, on the ground and the ceiling of both planes."""
        for zone in self.zones:
            for key, plane, surface in (("mid_floor_decor", "mid", "floor"), ("mid_ceil_decor", "mid", "ceil"), ("near_floor", "near", "floor"), ("near_ceil", "near", "ceil")):
                for local, family in zone[key + "_slots"]:
                    self.cluster(zone["x0"] + local + int(self.rng.integers(-8, 9)), family, plane, surface)

    def collect_lights(self):
        """One light per luminous sprite, dimmer on the second plane."""
        emitters = self.biome.emitters
        for record in self.decor:
            if record["name"] in emitters and record["plane"] in ("near", "mid"):
                tint, radius, power = emitters[record["name"]]
                scale = 1.0 if record["plane"] == "near" else 0.8
                y = record["y0"] + int((record["y1"] - record["y0"]) * 0.3)
                self.lights.append((record["x"], y, int(radius * scale), tint, power * scale))

    def compose(self):
        """Lights each plane with the same field, pours the water in front, and stacks the planes into the picture."""
        rng, biome, water_y = self.rng, self.biome, self.water_y
        near_table = biome.lut("near")
        far = self.far.copy()
        far[self.fall > 0] = near_table[self.fall[self.fall > 0]]
        mid, mid_on = biome.lut("mid")[self.mid], self.mid > 0
        near, near_on = near_table[self.near], self.near > 0
        wet = np.zeros((H, self.width), np.uint8)
        for x0, x1 in self.water_runs:
            for x in range(x0 // 16 * 16, x1, 16):
                for row in biome.water["rows"]:
                    tile = row["tile"] if "chance" not in row or rng.random() < row["chance"] else row["else"]
                    stamp(wet, biome.water["tiles"][tile], x + row["dx"], water_y + row["dy"])
        wet[~self.water] = 0
        near[wet > 0] = near_table[wet[wet > 0]]
        near_on |= wet > 0
        sunk = self.solid["near"].copy()
        sunk[:min(water_y + 2, H)] = False
        for x0, x1 in self.water_runs:
            columns = np.zeros(self.width, bool)
            columns[max(x0 - 3, 0):x1 + 3] = True
            under = sunk & columns[None, :]
            gain, veil = biome.water["depth_tint"]
            near[under] = near[under] * gain + np.array(veil)
        near[self.front > 0] = near_table[self.front[self.front > 0]]
        near_on |= self.front > 0
        gain, haze = self.light_field()
        far, mid, near = (plane * gain + haze for plane in (far, mid, near))
        for lx, ly, radius, color, power in self.lights:
            if ly >= water_y or not any(a - 8 <= lx < b + 8 for a, b in self.water_runs):
                continue
            rows = 5
            for row in range(rows):
                y = water_y + 5 + row * 5 + int(rng.integers(0, 2))
                half = int(radius * 0.30 * (1 - row / (rows + 1.0)))
                for x in range(lx - half + int(rng.integers(-2, 3)), lx + half):
                    if 0 <= x < self.width and y < H and self.water[y, x] and (x + row) % 4 != 0 and abs(x - lx) % 7 != 5:
                        near[y, x] = near[y, x] * 0.45 + np.array(color) * (95 - row * 12) * power
        air = ~(self.solid["near"] | self.solid["mid"] | self.water)
        specks = biome.specks
        for zone in self.zones:
            for _ in range(specks["count"]):
                x, y = zone["x0"] + int(rng.integers(8, ZONE - 8)), int(rng.integers(*specks["rows"]))
                if air[y, x]:
                    near[y, x] = np.array(biome.near[str(rng.choice(specks["keys"]))]) * specks["gain"]
                    near_on[y, x] = True
        self.layers = {}
        for name, plane, opaque in (("far", far, np.ones_like(near_on)), ("mid", mid, mid_on), ("near", near, near_on)):
            rgba = np.zeros((H, self.width, 4), np.uint8)
            rgba[..., :3] = np.clip(plane, 0, 255).astype(np.uint8)
            rgba[..., 3] = np.where(opaque, 255, 0)
            rgba[~opaque] = 0
            self.layers[name] = rgba
        image = self.layers["far"][..., :3].copy()
        for name in ("mid", "near"):
            opaque = self.layers[name][..., 3] > 0
            image[opaque] = self.layers[name][..., :3][opaque]
        return image

    def light_field(self):
        """Dark ambience, then pools of light in quantized, dithered bands, one tint per source: a gain and a haze per pixel."""
        ys, xs = np.mgrid[0:H, 0:self.width]
        points, values = [], []
        for zone in self.zones:
            points += [zone["x0"] + 90, zone["x0"] + ZONE - 90]
            values += [zone["ambient"], zone["ambient"]]
        level = np.tile(np.interp(np.arange(self.width), points, values), (H, 1))
        tint = np.zeros((H, self.width, 3))
        weight = np.zeros((H, self.width))
        for lx, ly, radius, color, power in self.lights:
            x0, x1, y0, y1 = max(lx - radius, 0), min(lx + radius + 1, self.width), max(ly - radius, 0), min(ly + radius + 1, H)
            if x0 >= x1 or y0 >= y1:
                continue
            glow = np.clip(1 - np.hypot(xs[y0:y1, x0:x1] - lx, (ys[y0:y1, x0:x1] - ly) * 1.15) / radius, 0, 1) ** 1.25 * power
            level[y0:y1, x0:x1] += glow
            tint[y0:y1, x0:x1] += glow[..., None] * np.array(color)
            weight[y0:y1, x0:x1] += glow
        for bx, by, slope, width, spread, power, color in self.beams:
            reach = np.clip(1 - np.abs(xs - bx - (ys - by) * slope) / (width + (ys - by) * spread), 0, 1)
            beam = reach * np.clip(1.15 - (ys - by) / 240.0, 0, 1) * (ys >= by) * power
            level += beam
            tint += beam[..., None] * np.array(color)
            weight += beam
        dither = BAYER[ys % 4, xs % 4]
        quantized = np.clip(np.floor(level / LIGHT_STEP + dither) * LIGHT_STEP, 0.5, 2.1)
        hue = tint / np.maximum(weight, 1e-6)[..., None]
        share = np.clip(weight, 0, 1)[..., None] * 0.48
        steps = np.floor(np.clip(weight, 0, 1) * 3 + dither) / 3.0
        return quantized[..., None] * (1 - share + share * hue * 1.25), steps[..., None] * hue * np.array([21.0, 21.0, 25.0])


def generate(biome, seed, zones, attempts=20, sequence=None):
    """First map that passes every check, trying new seeds derived from the first one."""
    failures = []
    for attempt in range(attempts):
        current = seed + attempt * 7919
        cave = CaveMap(biome, current, zones, sequence)
        report = rules.check(cave)
        if all(passed for _, passed, _ in report):
            return cave, report, attempt + 1, failures
        failures.append((current, [f"{name}: {detail}" for name, passed, detail in report if not passed]))
    return None, None, attempts, failures


def contact_sheet(caves, path):
    """All the maps one under the other, each with its seed and its zones."""
    width = max(cave.width for cave in caves)
    sheet = Image.new("RGB", (width, (H + 14) * len(caves)), (4, 4, 9))
    draw = ImageDraw.Draw(sheet)
    for position, cave in enumerate(caves):
        y = position * (H + 14)
        label = f"seed {cave.label}   " + "  |  ".join(zone["name"] + ("*" if zone["mirrored"] else "") for zone in cave.zones)
        draw.text((4, y + 2), label, fill=(120, 150, 170))
        sheet.paste(Image.fromarray(cave.image), (0, y + 14))
    sheet.save(path)


def main():
    """Command line: writes the maps, the contact sheet, and prints the checks of each map."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--biome", default="fosse", help="name of a folder of biomes/")
    parser.add_argument("--count", type=int, default=6)
    parser.add_argument("--zones", type=int, default=3)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--out", default="out")
    parser.add_argument("--layers", action="store_true", help="also write the far, second and play planes of each map")
    parser.add_argument("--sequence", help="comma-separated template names to use instead of a drawn chain; a trailing * mirrors one")
    arguments = parser.parse_args()
    biome = Biome(arguments.biome)
    os.makedirs(arguments.out, exist_ok=True)
    kept = []
    for number in range(arguments.count):
        seed = arguments.seed + number
        sequence = [(name.rstrip("*"), name.endswith("*")) for name in arguments.sequence.split(",")] if arguments.sequence else None
        cave, report, attempts, failures = generate(biome, seed, arguments.zones, sequence=sequence)
        for failed_seed, reasons in failures:
            print(f"  rejected (seed {failed_seed}): " + "; ".join(reasons))
        if cave is None:
            print(f"map_{seed}: no valid map in {attempts} attempts, nothing written")
            continue
        cave.label = seed
        Image.fromarray(cave.image).save(os.path.join(arguments.out, f"map_{seed}.png"))
        with open(os.path.join(arguments.out, f"map_{seed}.json"), "w") as sidecar:
            json.dump({"label": seed, "seed": cave.seed, "zones": cave.zone_count, "sequence": cave.forced}, sidecar, indent=2)
        if arguments.layers:
            for name, layer in cave.layers.items():
                Image.fromarray(layer).save(os.path.join(arguments.out, f"map_{seed}_{name}.png"))
        zones = " -> ".join(zone["name"] + ("*" if zone["mirrored"] else "") for zone in cave.zones)
        print(f"map_{seed}.png  attempt {attempts}  [{zones}]")
        for name, _, detail in report:
            print(f"    ok  {name}: {detail}")
        kept.append(cave)
    if kept:
        contact_sheet(kept, os.path.join(arguments.out, "contact.png"))
        print(f"{len(kept)} map(s) and contact.png written in {arguments.out}/")


if __name__ == "__main__":
    main()
