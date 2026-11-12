#!/usr/bin/env python3
"""Exports the generated maps as one LDtk project: a level per map, three tile layers cut from the lit planes, a wall grid, an empty entity layer."""
import argparse
import glob
import json
import math
import os
import sys
import uuid
import zlib

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import generate_maps  # noqa: E402
from biome_loader import Biome, add_options  # noqa: E402
from frame import H, ZONE  # noqa: E402

LDTK_VERSION = "1.5.3"
APP_BUILD_ID = 473703
GRID = 16
LEVEL_GAP = 32
WALL_SHARE = 0.5
ATLAS_COLUMNS = 32
PLANES = (("Near", "near"), ("Mid", "mid"), ("Far", "far"))
NAMESPACE = uuid.UUID("5f0e2c1a-7b3d-4f6e-9a55-0c6d1e2f3a4b")
UID = {"Mobs": 1, "Walls": 2, "Near": 3, "Mid": 4, "Far": 5, "Near_tiles": 6, "Mid_tiles": 7, "Far_tiles": 8, "Props": 9}
FIRST_LEVEL_UID = 10


def iid(biome, name):
    """Stable instance identifier of a named thing of the project of a biome."""
    return str(uuid.uuid5(NAMESPACE, f"{biome.name}_procgen/{name}"))


def hex_color(color):
    """Color as the #RRGGBB text LDtk stores."""
    return "#%02X%02X%02X" % tuple(int(v) for v in color)


def load_maps(biome, folder):
    """Rebuilds each map of a folder from its sidecar file and checks it still matches the picture on disk."""
    caves = []
    for file in sorted(glob.glob(os.path.join(folder, "map_*.json")), key=lambda path: int(os.path.basename(path)[4:-5])):
        with open(file) as handle:
            entry = json.load(handle)
        sequence = [(name.rstrip("*"), name.endswith("*")) for name in entry["sequence"]] if entry["sequence"] else None
        cave = generate_maps.CaveMap(biome, entry["seed"], entry["zones"], sequence)
        picture = np.asarray(Image.open(file[:-5] + ".png").convert("RGB"))
        if picture.shape != cave.image.shape or not np.array_equal(picture, cave.image):
            sys.exit(f"{file[:-5]}.png is not what its seed generates for the biome {biome.name}: run generate_maps.py again, or pass the right --biome")
        cave.label = entry["label"]
        caves.append(cave)
    if not caves:
        sys.exit(f"no map_<seed>.json in {folder}: run generate_maps.py first")
    return caves


def cut_tiles(caves, plane):
    """Unique 16 px tiles of one plane over every map, and where each map uses them."""
    tiles, index, uses = [], {}, []
    for cave in caves:
        layer = cave.layers[plane]
        placed = []
        for cy in range(layer.shape[0] // GRID):
            for cx in range(layer.shape[1] // GRID):
                tile = layer[cy * GRID:(cy + 1) * GRID, cx * GRID:(cx + 1) * GRID]
                if not tile[..., 3].any():
                    continue
                key = tile.tobytes()
                if key not in index:
                    index[key] = len(tiles)
                    tiles.append(tile)
                placed.append((cx, cy, index[key]))
        uses.append(placed)
    return tiles, uses


def build_atlas(tiles):
    """Atlas of tiles, without margin or spacing, close to square once past 32 columns."""
    columns = max(ATLAS_COLUMNS, math.ceil(math.sqrt(len(tiles))))
    rows = max(math.ceil(len(tiles) / columns), 1)
    atlas = np.zeros((rows * GRID, columns * GRID, 4), np.uint8)
    for position, tile in enumerate(tiles):
        cy, cx = divmod(position, columns)
        atlas[cy * GRID:(cy + 1) * GRID, cx * GRID:(cx + 1) * GRID] = tile
    return atlas, columns


def pixel_cache(atlas):
    """Opacity flag and average color of each tile, as LDtk caches them in a tileset definition."""
    rows, columns = atlas.shape[0] // GRID, atlas.shape[1] // GRID
    opaque, colors = [], []
    for cy in range(rows):
        for cx in range(columns):
            tile = atlas[cy * GRID:(cy + 1) * GRID, cx * GRID:(cx + 1) * GRID].astype(float) / 255.0
            alpha = tile[..., 3]
            opaque.append("1" if (alpha >= 1).all() else "0")
            weight = alpha.sum()
            if weight == 0:
                colors.append("0000")
                continue
            average = [math.sqrt((tile[..., channel] ** 2 * alpha).sum() / weight) for channel in range(3)]
            values = [alpha.mean()] + average
            colors.append("".join("0123456789abcdef"[int(round(int(min(max(v, 0), 1) * 255) / 255 * 15))] for v in values))
    return {"opaqueTiles": "".join(opaque), "averageColors": "".join(colors)}


def pack_props(biome):
    """Atlas of the kept sprites, each bottom-aligned and centered in a box whose sides are multiples of 16."""
    table = biome.lut("near")
    textures = {declared["paint"].get("texture") for declared in biome.materials.values()}
    names = sorted(name for name in biome.sprites if name not in textures)
    boxes, x, y, tallest = [], 0, 0, 0
    width = ATLAS_COLUMNS * GRID
    for name in names:
        h, w = biome.sprites[name].shape
        box_w, box_h = math.ceil(w / GRID) * GRID, math.ceil(h / GRID) * GRID
        if x + box_w > width:
            x, y, tallest = 0, y + tallest, 0
        boxes.append({"name": name, "x": x, "y": y, "width": box_w, "height": box_h})
        x, tallest = x + box_w, max(tallest, box_h)
    atlas = np.zeros((y + tallest, width, 4), np.uint8)
    for box in boxes:
        sprite = biome.sprites[box["name"]]
        h, w = sprite.shape
        left, top = box["x"] + (box["width"] - w) // 2, box["y"] + box["height"] - h
        region = atlas[top:top + h, left:left + w]
        region[sprite > 0, :3] = table[sprite[sprite > 0]]
        region[sprite > 0, 3] = 255
    return atlas, boxes


WALLS_OPACITY = 0.35


def layer_definition(biome, name, kind, tileset=None):
    """Layer definition with every field of the format, as LDtk 1.5.3 writes it."""
    return {
        "__type": kind, "identifier": name, "type": kind, "uid": UID[name], "doc": None, "uiColor": None,
        "gridSize": GRID, "guideGridWid": 0, "guideGridHei": 0, "displayOpacity": WALLS_OPACITY if kind == "IntGrid" else 1, "inactiveOpacity": 1,
        "hideInList": False, "hideFieldsWhenInactive": kind != "Entities", "canSelectWhenInactive": True, "renderInWorldView": True,
        "pxOffsetX": 0, "pxOffsetY": 0, "parallaxFactorX": 0, "parallaxFactorY": 0, "parallaxScaling": True,
        "requiredTags": [], "excludedTags": [], "autoTilesKilledByOtherLayerUid": None, "uiFilterTags": [], "useAsyncRender": False,
        "intGridValues": [{"value": 1, "identifier": "wall", "color": hex_color(biome.near[biome.wall_key]), "tile": None, "groupUid": 0}] if kind == "IntGrid" else [],
        "intGridValuesGroups": [], "autoRuleGroups": [], "autoSourceLayerDefUid": None,
        "tilesetDefUid": UID[tileset] if tileset else None, "tilePivotX": 0, "tilePivotY": 0, "biomeFieldUid": None,
    }


def tileset_definition(name, file, atlas):
    """Tileset definition of an atlas saved beside the project file."""
    return {
        "__cWid": atlas.shape[1] // GRID, "__cHei": atlas.shape[0] // GRID, "identifier": name, "uid": UID[name],
        "relPath": file, "embedAtlas": None, "pxWid": atlas.shape[1], "pxHei": atlas.shape[0],
        "tileGridSize": GRID, "spacing": 0, "padding": 0, "tags": [], "tagsSourceEnumUid": None, "enumTags": [],
        "customData": [], "savedSelections": [], "cachedPixelData": pixel_cache(atlas),
    }


def layer_instance(biome, name, kind, level, tileset=None, file=None):
    """Layer instance of a level, empty; the caller fills the field of its kind."""
    return {
        "__identifier": name, "__type": kind, "__cWid": level["pxWid"] // GRID, "__cHei": level["pxHei"] // GRID, "__gridSize": GRID,
        "__opacity": WALLS_OPACITY if kind == "IntGrid" else 1, "__pxTotalOffsetX": 0, "__pxTotalOffsetY": 0,
        "__tilesetDefUid": UID[tileset] if tileset else None, "__tilesetRelPath": file,
        "iid": iid(biome, f"{level['identifier']}/{name}"), "levelId": level["uid"], "layerDefUid": UID[name],
        "pxOffsetX": 0, "pxOffsetY": 0, "visible": kind != "IntGrid", "optionalRules": [], "intGridCsv": [], "autoLayerTiles": [],
        "seed": zlib.crc32(f"{level['identifier']}/{name}".encode()) % 9999999, "overrideTilesetUid": None,
        "gridTiles": [], "entityInstances": [],
    }


def wall_cells(cave):
    """1 for each 16 px cell at least half filled by the solid mask of the play plane, water left out; row by row."""
    solid = cave.solid["near"]
    rows, columns = solid.shape[0] // GRID, solid.shape[1] // GRID
    share = solid.reshape(rows, GRID, columns, GRID).mean(axis=(1, 3))
    return [int(value) for value in (share >= WALL_SHARE).reshape(-1)]


def build_project(biome, caves, atlases, uses):
    """The whole project as the dictionary LDtk saves."""
    background = hex_color(biome.backdrop)
    levels, world_x = [], 0
    for position, cave in enumerate(caves):
        level = {
            "identifier": f"{biome.title}_{cave.label}", "iid": iid(biome, f"{biome.title}_{cave.label}"), "uid": FIRST_LEVEL_UID + position,
            "worldX": world_x, "worldY": 0, "worldDepth": 0, "pxWid": cave.width, "pxHei": cave.image.shape[0],
            "__bgColor": background, "bgColor": None, "useAutoIdentifier": False, "bgRelPath": None, "bgPos": None,
            "bgPivotX": 0.5, "bgPivotY": 0.5, "__smartColor": background, "__bgPos": None, "externalRelPath": None,
            "fieldInstances": [], "layerInstances": [], "__neighbours": [],
        }
        mobs = layer_instance(biome, "Mobs", "Entities", level)
        walls = layer_instance(biome, "Walls", "IntGrid", level)
        walls["intGridCsv"] = wall_cells(cave)
        level["layerInstances"] += [mobs, walls]
        for name, plane in PLANES:
            atlas, columns, file = atlases[name]
            instance = layer_instance(biome, name, "Tiles", level, name + "_tiles", file)
            for cx, cy, tile in uses[name][position]:
                source_y, source_x = divmod(tile, columns)
                instance["gridTiles"].append({"px": [cx * GRID, cy * GRID], "src": [source_x * GRID, source_y * GRID], "f": 0, "t": tile, "d": [cx + cy * instance["__cWid"]], "a": 1})
            level["layerInstances"].append(instance)
        levels.append(level)
        world_x += cave.width + LEVEL_GAP
    layers = [layer_definition(biome, "Mobs", "Entities"), layer_definition(biome, "Walls", "IntGrid")] + [layer_definition(biome, name, "Tiles", name + "_tiles") for name, _ in PLANES]
    tilesets = [tileset_definition(name + "_tiles", atlases[name][2], atlases[name][0]) for name, _ in PLANES] + [tileset_definition("Props", atlases["Props"][2], atlases["Props"][0])]
    return {
        "__header__": {"fileType": "LDtk Project JSON", "app": "LDtk", "doc": "https://ldtk.io/json", "schema": "https://ldtk.io/files/JSON_SCHEMA.json",
                       "appAuthor": "Sebastien 'deepnight' Benard", "appVersion": LDTK_VERSION, "url": "https://ldtk.io"},
        "iid": iid(biome, "project"), "jsonVersion": LDTK_VERSION, "appBuildId": APP_BUILD_ID, "nextUid": FIRST_LEVEL_UID + len(levels),
        "identifierStyle": "Capitalize", "toc": [], "worldLayout": "Free", "worldGridWidth": GRID, "worldGridHeight": GRID,
        "defaultLevelWidth": ZONE, "defaultLevelHeight": H, "defaultPivotX": 0, "defaultPivotY": 0,
        "defaultGridSize": GRID, "defaultEntityWidth": GRID, "defaultEntityHeight": GRID,
        "bgColor": hex_color((4, 4, 9)), "defaultLevelBgColor": background, "minifyJson": False, "externalLevels": False,
        "exportTiled": False, "simplifiedExport": False, "imageExportMode": "None", "exportLevelBg": True, "pngFilePattern": None,
        "backupOnSave": False, "backupLimit": 10, "backupRelPath": None, "levelNamePattern": f"{biome.title}_%idx", "tutorialDesc": None,
        "customCommands": [], "flags": [],
        "defs": {"layers": layers, "entities": [], "tilesets": tilesets, "enums": [], "externalEnums": [], "levelFields": []},
        "levels": levels, "worlds": [], "dummyWorldIid": iid(biome, "world"),
    }


def main():
    """Command line: reads the maps of a folder, writes the project, its three atlases and the prop atlas."""
    parser = argparse.ArgumentParser(description=__doc__)
    add_options(parser)
    parser.add_argument("--maps", default="out", help="folder holding map_<seed>.png and map_<seed>.json")
    parser.add_argument("--out", default="ldtk")
    arguments = parser.parse_args()
    biome = Biome(arguments.biome, arguments.biomes)
    os.makedirs(arguments.out, exist_ok=True)
    caves = load_maps(biome, arguments.maps)
    atlases, uses = {}, {}
    for name, plane in PLANES:
        tiles, uses[name] = cut_tiles(caves, plane)
        atlas, columns = build_atlas(tiles)
        file = f"{biome.name}_{plane}.png"
        Image.fromarray(atlas).save(os.path.join(arguments.out, file))
        atlases[name] = (atlas, columns, file)
        placed = sum(len(level) for level in uses[name])
        print(f"{name}: {len(tiles)} unique tiles for {placed} placed, atlas {atlas.shape[1]} x {atlas.shape[0]} px ({file})")
    props, boxes = pack_props(biome)
    Image.fromarray(props).save(os.path.join(arguments.out, "props_16.png"))
    with open(os.path.join(arguments.out, "props_16.json"), "w") as handle:
        json.dump(boxes, handle, indent=2)
    atlases["Props"] = (props, ATLAS_COLUMNS, "props_16.png")
    print(f"Props: {len(boxes)} sprites, atlas {props.shape[1]} x {props.shape[0]} px (props_16.png, props_16.json)")
    project = build_project(biome, caves, atlases, uses)
    target = os.path.join(arguments.out, f"{biome.name}_procgen.ldtk")
    with open(target, "w") as handle:
        json.dump(project, handle, indent="\t")
    for level in project["levels"]:
        walls = sum(level["layerInstances"][1]["intGridCsv"])
        print(f"{level['identifier']}: {level['pxWid']} x {level['pxHei']} px at world x {level['worldX']}, {walls} wall cells")
    print(f"{target} written ({os.path.getsize(target)} bytes)")


if __name__ == "__main__":
    main()
