"""Loads a biome: a folder of data kept outside this tool, read from its biome.json and its sprites, turned into what the engine draws with."""
import os
import sys

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
if HERE not in sys.path:
    sys.path.insert(0, HERE)
import biome_file  # noqa: E402
from props import PREFIX, SpriteStore  # noqa: E402

SIZES = ("large", "medium", "small")
MARKS = ("glow", "hang", "hero")
PROP_SIZES = {"large": ["wall", "pillar", "steps", "lintel"], "medium": ["pile", "pillar_broken", "drum"], "small": ["slab"]}
MID_SHARE = 0.5


def add_options(parser):
    """Gives a command line the two options that name a biome: the folder of biomes and the biome in it."""
    parser.add_argument("--biomes", required=True, help="folder that holds one folder per biome; biomes are kept outside this tool")
    parser.add_argument("--biome", required=True, help="name of a biome: a folder of the folder of biomes")


class Biome:
    """One biome: its palette, sprites, materials, elements, decor and templates."""

    def __init__(self, name, store):
        """Reads the data file of the biome in a folder of biomes and prepares its sprites, families, lights and elements."""
        raw = biome_file.read(store, name)
        self.tints = {key: tuple(value) for key, value in raw["palette"].get("tints", {}).items()}
        data = biome_file.resolve(raw, self.tints)
        self.name, self.store = name, store
        self.title = name[:1].upper() + name[1:]
        self.folder = os.path.join(store, name)
        palette, elements, decor = data["palette"], data["elements"], data.get("decor", {})
        self.near, self.backdrop = palette["near"], palette["backdrop"]
        share = palette.get("mid_share", MID_SHARE)
        self.mid = palette.get("mid") or {key: tuple(int(round(self.backdrop[c] + (color[c] - self.backdrop[c]) * share)) for c in range(3)) for key, color in self.near.items()}
        self.far_a, self.far_b, self.far_lit_keys = palette["far_a"], palette["far_b"], palette["far_lit_keys"]
        self.outline = palette.get("outline")
        self.drawn, self.sheets = data.get("drawn", {}), data.get("sheets")
        self.keys = "." + "".join(self.near)
        self.index = {key: position for position, key in enumerate(self.keys)}
        self.materials = data["materials"]
        self.material_id = {material: position + 1 for position, material in enumerate(self.materials)}
        self.material_name = {position: material for material, position in self.material_id.items()}
        self.seam_material = data["seam_material"]
        self.roles = {}
        self.sprites = SpriteStore(self, self.load_sprites(decor.get("sprites_from")))
        self.families = self.deduce_families()
        self.families.update(decor.get("families", {}))
        self.emitters = self.deduce_emitters()
        self.emitters.update(decor.get("emitters", {}))
        self.far_sets = {"default": self.default_far_set()}
        self.far_sets.update(decor.get("far_sets", {}))
        self.column_sprites = tuple(decor.get("column_sprites", ()))
        self.templates = data["templates"]
        self.wet_connectors = tuple(elements.get("wet_connectors", ()))
        self.water = self.compile_water(elements.get("water"))
        self.fall = self.grid(elements["fall"]) if elements.get("fall") else None
        self.lamps = {lamp: dict(spec, sprite=self.grid(spec["rows"])) for lamp, spec in elements.get("lamps", {}).items()}
        column = elements.get("column")
        self.column = dict(column, sprite=self.grid(column["shaft"])) if column else None
        self.strokes, self.specks, self.wall_key = elements["strokes"], elements["specks"], elements["wall_key"]
        self.stars, self.haze = elements.get("stars"), elements.get("haze")
        self.marks = {material: [self.grid(rows) for rows in stamps] for material, stamps in elements.get("marks", {}).items()}

    def lut(self, plane):
        """Color table of a plane, indexed like the letters of the palette."""
        table = np.zeros((len(self.keys), 3))
        for key, color in {"near": self.near, "mid": self.mid}[plane].items():
            table[self.index[key]] = color
        return table

    def grid(self, rows):
        """Index sprite of a sprite drawn as rows of palette letters."""
        assert len({len(row) for row in rows}) == 1
        return np.array([[self.index[ch] for ch in row] for row in rows], np.uint8)

    def remap(self, sprite, mapping):
        """Sprite with some palette letters replaced by others."""
        table = np.arange(len(self.keys), dtype=np.uint8)
        for source, target in mapping.items():
            table[self.index[source]] = self.index[target]
        return table[sprite]

    def read_folder(self, folder, source):
        """Index sprites of one sprites folder, decoded with the palette they were snapped to; a dotted file name gives a sprite its role."""
        sprites = {}
        for file in sorted(os.listdir(folder)) if os.path.isdir(folder) else []:
            if not file.endswith(".png"):
                continue
            rgba = np.asarray(Image.open(os.path.join(folder, file)).convert("RGBA"))
            sprite = np.zeros(rgba.shape[:2], np.uint8)
            for key, color in source.items():
                sprite[(rgba[..., :3] == np.array(color)).all(axis=2) & (rgba[..., 3] > 0)] = self.index[key]
            assert ((sprite > 0) == (rgba[..., 3] > 0)).all(), f"{file} holds a color outside the palette"
            parts = file[:-4].split(".")
            if len(parts) >= 3 and parts[1] in SIZES:
                self.roles[parts[2]] = {"family": parts[0], "size": parts[1], "marks": set(parts[3:]) & set(MARKS)}
                sprites[parts[2]] = sprite
            else:
                sprites[file[:-4]] = sprite
        return sprites

    def load_sprites(self, borrowed):
        """Sprites of the biome: those it borrows from another biome, read letter for letter, then its own, which win."""
        sprites = {}
        if borrowed:
            source = biome_file.read(self.store, borrowed)["palette"]["near"]
            missing = sorted(set(source) - set(self.near))
            assert not missing, f"the palette of {self.name} lacks the letters {missing} of the sprites it borrows"
            sprites.update(self.read_folder(os.path.join(self.store, borrowed, "sprites"), source))
            self.roles.clear()
        sprites.update(self.read_folder(os.path.join(self.folder, "sprites"), self.near))
        return sprites

    def deduce_families(self):
        """Families read from the file names of the sprites, plus one family of generated props per material that asks for it."""
        families = {}
        for name, role in self.roles.items():
            family = families.setdefault(role["family"], {"large": [], "medium": [], "small": [], "hero": []})
            family["hero" if "hero" in role["marks"] else role["size"]].append(name)
        for material, declared in self.materials.items():
            if declared.get("props"):
                families[material] = {size: self.prop_names(material, shapes) for size, shapes in PROP_SIZES.items()}
        return families

    def deduce_emitters(self):
        """Light of each sprite marked as glowing: its tint from its brightest pixels, its reach and power from its size."""
        emitters = {}
        table = self.lut("near")
        for name, role in self.roles.items():
            if "glow" not in role["marks"]:
                continue
            sprite = self.sprites[name]
            colors = table[sprite[sprite > 0]]
            luma = colors @ np.array([0.30, 0.59, 0.11])
            bright = colors[luma >= np.quantile(luma, 0.8)].mean(axis=0)
            size = max(sprite.shape)
            emitters[name] = (tuple(round(float(v), 2) for v in bright / bright.max()), int(4 + 2.6 * size), round(min(0.9, 0.35 + size / 90.0), 2))
        return emitters

    def prop_names(self, material, shapes):
        """Names of the generated props a material builds among some shapes; a material may list its shapes or take them all."""
        wanted = self.materials[material]["props"]
        return [f"{PREFIX}{material}:{shape}" for shape in shapes if wanted is True or shape in wanted]

    def default_far_set(self):
        """Far silhouettes used by a template that names no far set: the large pieces of the families, hanging ones under the ceiling."""
        floor, ceil = [], []
        for name, role in self.roles.items():
            if role["size"] == "large" and "hero" not in role["marks"]:
                (ceil if "hang" in role["marks"] else floor).append(name)
        for material, declared in self.materials.items():
            if declared.get("props"):
                floor += self.prop_names(material, PROP_SIZES["large"])
        return {"floor": floor, "ceil": ceil, "pillars": "pier"}

    def compile_water(self, water):
        """Water of the biome with its tiles as index sprites; a tile may be another tile with letters swapped."""
        if not water:
            return None
        tiles = {}
        copies = {name: rows for name, rows in water["tiles"].items() if len(rows) == 2 and isinstance(rows[1], dict)}
        for name, rows in water["tiles"].items():
            if name not in copies:
                tiles[name] = self.grid(rows)
        for name, rows in copies.items():
            tiles[name] = self.remap(tiles[rows[0]], rows[1])
        return dict(water, tiles=tiles)
