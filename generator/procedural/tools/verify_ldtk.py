"""Checks an exported LDtk project: the official JSON schema, the pixel round trip, the wall grid, and optionally the field names of a sample project."""
import argparse
import json
import os
import sys

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, ROOT)
import biome_file  # noqa: E402
from biome_loader import add_options  # noqa: E402
from frame import PLAY_BOTTOM, PLAY_TOP  # noqa: E402

SCHEMA_URL = "https://ldtk.io/files/JSON_SCHEMA.json"
GRID = 16


def keys_differ(label, mine, theirs):
    """Prints the fields one side has and the other lacks; returns how many."""
    missing, extra = sorted(set(theirs) - set(mine)), sorted(set(mine) - set(theirs))
    print(f"   {label}: {len(mine)} fields, missing {missing}, extra {extra}")
    return len(missing) + len(extra)


def check_schema(project, file):
    """Check 1: the project against the official JSON schema, given as a downloaded file."""
    if not file or not os.path.exists(file):
        print(f"1. NOT validated against the schema: download {SCHEMA_URL} and pass the file with --schema")
        return False
    try:
        import jsonschema
        from importlib.metadata import version
    except ImportError:
        print("1. NOT validated against the schema: the jsonschema package is missing (pip install jsonschema)")
        return False
    with open(file) as handle:
        schema = json.load(handle)
    validator = jsonschema.Draft7Validator(schema)
    errors = sorted(validator.iter_errors(project), key=lambda error: [str(part) for part in error.absolute_path])
    print(f"1. schema '{schema.get('title')}', declared as {schema.get('$schema')} (jsonschema {version('jsonschema')}, {validator.__class__.__name__}): {len(errors)} error(s)")
    for error in errors[:10]:
        print("   ", "/".join(str(part) for part in error.absolute_path), ":", error.message[:160])
    return not errors


def check_sample(project, file):
    """Optional check: same field names as a sample project saved by the same version of LDtk."""
    with open(file) as handle:
        sample = json.load(handle)
    print(f"1b. field names against the sample (jsonVersion {sample['jsonVersion']}, ours {project['jsonVersion']})")
    differences = keys_differ("project", project, sample) + keys_differ("defs", project["defs"], sample["defs"])
    differences += keys_differ("tileset definition", project["defs"]["tilesets"][0], sample["defs"]["tilesets"][0])
    differences += keys_differ("level", project["levels"][0], sample["levels"][0])
    for kind in ("Entities", "IntGrid", "Tiles"):
        mine = next(layer for layer in project["defs"]["layers"] if layer["type"] == kind)
        theirs = next(layer for layer in sample["defs"]["layers"] if layer["type"] == kind)
        differences += keys_differ(f"{kind} layer definition", mine, theirs)
        mine = next(layer for layer in project["levels"][0]["layerInstances"] if layer["__type"] == kind)
        theirs = next(layer for level in sample["levels"] for layer in level["layerInstances"] if layer["__type"] == kind)
        differences += keys_differ(f"{kind} layer instance", mine, theirs)
    mine = next(layer for layer in project["levels"][0]["layerInstances"] if layer["gridTiles"])["gridTiles"][0]
    theirs = next(layer for level in sample["levels"] for layer in level["layerInstances"] if layer["gridTiles"])["gridTiles"][0]
    differences += keys_differ("tile", mine, theirs)
    return differences == 0 and sample["jsonVersion"] == project["jsonVersion"]


def check_round_trip(project, folder, maps):
    """Check 2: each level rebuilt from its tiles equals the picture of its map."""
    atlases = {tileset["uid"]: np.asarray(Image.open(os.path.join(folder, tileset["relPath"])).convert("RGBA")) for tileset in project["defs"]["tilesets"]}
    total = 0
    print(f"2. round trip, pixels that differ from {maps}/map_<seed>.png")
    for level in project["levels"]:
        canvas = np.zeros((level["pxHei"], level["pxWid"], 3), np.uint8)
        covered = np.zeros((level["pxHei"], level["pxWid"]), bool)
        tiles = 0
        for layer in reversed(level["layerInstances"]):
            if layer["__type"] != "Tiles":
                continue
            atlas = atlases[layer["__tilesetDefUid"]]
            for tile in layer["gridTiles"]:
                (x, y), (sx, sy) = tile["px"], tile["src"]
                source = atlas[sy:sy + GRID, sx:sx + GRID]
                opaque = source[..., 3] > 0
                canvas[y:y + GRID, x:x + GRID][opaque] = source[..., :3][opaque]
                covered[y:y + GRID, x:x + GRID] |= opaque
                tiles += 1
        seed = level["identifier"].rsplit("_", 1)[1]
        picture = np.asarray(Image.open(os.path.join(maps, f"map_{seed}.png")).convert("RGB"))
        different = int((canvas != picture).any(axis=2).sum()) if canvas.shape == picture.shape else -1
        total += abs(different)
        print(f"   {level['identifier']}: {different} of {picture.shape[0] * picture.shape[1]} pixels differ, {int((~covered).sum())} pixels not covered, {tiles} tiles")
    return total == 0


def check_walls(project):
    """Check 3: wall cells per level, none across the corridor kept clear for the game."""
    passed = True
    first, last = PLAY_TOP // GRID, PLAY_BOTTOM // GRID
    print(f"3. Walls: cells at 1, and cells at 1 in rows {first} to {last} (y {first * GRID} to {last * GRID + GRID - 1}, the rows that touch y {PLAY_TOP}-{PLAY_BOTTOM})")
    for level in project["levels"]:
        layer = next(layer for layer in level["layerInstances"] if layer["__identifier"] == "Walls")
        cells = np.array(layer["intGridCsv"]).reshape(layer["__cHei"], layer["__cWid"])
        inside = int(cells[first:last + 1].sum())
        other = sorted(set(int(v) for v in np.unique(cells)) - {0, 1})
        passed &= inside == 0 and not other and cells.size == layer["__cWid"] * layer["__cHei"]
        print(f"   {level['identifier']}: {int(cells.sum())} of {cells.size} cells at 1, {inside} in the corridor, other values {other}")
    return passed


def check_layout(project):
    """Parameters asked for: grid, level size, layer order, levels side by side, unique identifiers."""
    faults = []
    order = [layer["identifier"] for layer in project["defs"]["layers"]]
    if order != ["Mobs", "Walls", "Near", "Mid", "Far"]:
        faults.append(f"layer order {order}")
    spans = []
    for level in project["levels"]:
        if level["pxHei"] != 272 or level["pxWid"] % 480 or any(layer["__gridSize"] != GRID for layer in level["layerInstances"]):
            faults.append(f"{level['identifier']} size or grid")
        if [layer["__identifier"] for layer in level["layerInstances"]] != order:
            faults.append(f"{level['identifier']} layer order")
        if next(layer for layer in level["layerInstances"] if layer["__identifier"] == "Mobs")["entityInstances"]:
            faults.append(f"{level['identifier']} Mobs is not empty")
        spans.append((level["worldX"], level["worldX"] + level["pxWid"]))
    if any(b[0] < a[1] for a, b in zip(spans, spans[1:])):
        faults.append("levels overlap")
    uids = [item["uid"] for item in project["defs"]["layers"] + project["defs"]["tilesets"] + project["levels"]]
    iids = [project["iid"]] + [level["iid"] for level in project["levels"]] + [layer["iid"] for level in project["levels"] for layer in level["layerInstances"]]
    if len(set(uids)) != len(uids) or len(set(iids)) != len(iids) or project["nextUid"] <= max(uids):
        faults.append("identifiers are not unique")
    for tileset in project["defs"]["tilesets"]:
        if tileset["padding"] or tileset["spacing"] or tileset["tileGridSize"] != GRID or tileset["pxWid"] % GRID or os.path.isabs(tileset["relPath"]):
            faults.append(f"tileset {tileset['identifier']}")
    print(f"4. parameters: layers {order}, {len(project['levels'])} levels at world x {[span[0] for span in spans]}, {len(uids)} uids and {len(iids)} iids unique, pivot ({project['defaultPivotX']}, {project['defaultPivotY']}): {'ok' if not faults else faults}")
    return not faults


def main():
    """Runs every check on the project of a biome and exits with an error when one fails."""
    parser = argparse.ArgumentParser(description=__doc__)
    add_options(parser)
    parser.add_argument("--maps", default="out", help="folder of the maps the project was exported from")
    parser.add_argument("--project", default="ldtk", help="folder that holds <biome>_procgen.ldtk")
    parser.add_argument("--schema", help=f"the official JSON schema, downloaded from {SCHEMA_URL}")
    parser.add_argument("--sample", help="a project saved by LDtk, to compare field names with (optional)")
    arguments = parser.parse_args()
    biome_file.path_of(arguments.biomes, arguments.biome)
    file = os.path.join(arguments.project, f"{arguments.biome}_procgen.ldtk")
    if not os.path.isfile(file):
        sys.exit(f"{file} does not exist: export the maps of {arguments.biome} with export_ldtk.py first, or name another folder with --project")
    print(f"checking {file}")
    with open(file) as handle:
        project = json.load(handle)
    results = [check_schema(project, arguments.schema), check_round_trip(project, arguments.project, arguments.maps), check_walls(project), check_layout(project)]
    if arguments.sample:
        results.append(check_sample(project, arguments.sample))
    print("ALL CHECKS PASSED" if all(results) else f"FAILED: {results}")
    sys.exit(0 if all(results) else 1)


if __name__ == "__main__":
    main()
