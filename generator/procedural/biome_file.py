"""Reads and writes the one data file of a biome, biome.json, and stops with a plain message when a folder, a file or a key is missing."""
import json
import os
import sys

FILE = "biome.json"
WIDTH = 150
TINT_MARK = "@"
REQUIRED = {
    (): ("palette", "materials", "seam_material", "elements", "templates"),
    ("palette",): ("near", "backdrop", "far_a", "far_b", "far_lit_keys"),
    ("elements",): ("strokes", "specks", "wall_key"),
}
REQUIRED_IN_MATERIAL = ("painter", "paint", "aligned")
REQUIRED_IN_TEMPLATE = ("material", "left", "right", "ambient", "mood", "hero")


def stop(message):
    """Ends the command with a message about the data of a biome."""
    sys.exit(f"biome data: {message}")


def names_in(folder):
    """Names of the biomes of a folder: its sub-folders that hold a data file."""
    return sorted(name for name in os.listdir(folder) if os.path.isfile(os.path.join(folder, name, FILE)))


def path_of(folder, name):
    """Data file of a biome, after checking that the folder of biomes and the biome exist."""
    if not folder or not os.path.isdir(folder):
        stop(f"the folder of biomes '{folder}' does not exist: pass --biomes <folder>, the folder that holds one folder per biome")
    file = os.path.join(folder, name, FILE)
    if not os.path.isfile(file):
        stop(f"no biome '{name}' in {folder}: it holds {', '.join(names_in(folder)) or 'no biome'}; pass --biome <name>, or create one with tools/new_biome.py")
    return file


def need(data, keys, where):
    """Stops when a part of the data lacks one of the keys it must have."""
    if not isinstance(data, dict):
        stop(f"{where} must be an object")
    missing = [key for key in keys if key not in data]
    if missing:
        stop(f"{where} lacks the key{'s' if len(missing) > 1 else ''} {', '.join(missing)}")


def read(folder, name):
    """Data of a biome as written in its file, checked for the keys every biome must have."""
    file = path_of(folder, name)
    try:
        with open(file, encoding="utf-8") as handle:
            data = json.load(handle)
    except json.JSONDecodeError as error:
        stop(f"{file} is not valid JSON: line {error.lineno}, column {error.colno}: {error.msg}")
    for path, keys in REQUIRED.items():
        part = data
        for key in path:
            part = part[key]
        need(part, keys, f"{file}: {'.'.join(path) or 'the file'}")
    for material, declared in data["materials"].items():
        need(declared, REQUIRED_IN_MATERIAL, f"{file}: materials.{material}")
    for template, declared in data["templates"].items():
        need(declared, REQUIRED_IN_TEMPLATE, f"{file}: templates.{template}")
    if not data["materials"] or not data["templates"]:
        stop(f"{file}: a biome needs at least one material and one template")
    return data


def resolve(value, tints):
    """Data as the engine reads it: every list a tuple, every string that names a tint replaced by that tint."""
    if isinstance(value, dict):
        return {key: resolve(item, tints) for key, item in value.items()}
    if isinstance(value, list):
        return tuple(resolve(item, tints) for item in value)
    if isinstance(value, str) and value.startswith(TINT_MARK):
        if value[1:] not in tints:
            stop(f"'{value}' names a tint that palette.tints does not hold: it holds {', '.join(tints) or 'none'}")
        return tints[value[1:]]
    return value


def render(value, indent=0):
    """JSON text of a value: on one line when it fits or holds only plain values, one entry per line otherwise."""
    flat = json.dumps(value, ensure_ascii=False)
    plain = isinstance(value, list) and not any(isinstance(item, (dict, list)) for item in value)
    if not isinstance(value, (dict, list)) or plain or len(flat) + indent <= WIDTH:
        return flat
    pad = " " * (indent + 2)
    if isinstance(value, dict):
        entries = [f"{pad}{json.dumps(key)}: {render(item, indent + 2)}" for key, item in value.items()]
        return "{\n" + ",\n".join(entries) + "\n" + " " * indent + "}"
    return "[\n" + ",\n".join(pad + render(item, indent + 2) for item in value) + "\n" + " " * indent + "]"


def write(folder, name, data):
    """Writes the data file of a biome, creating its folder when needed."""
    os.makedirs(os.path.join(folder, name), exist_ok=True)
    with open(os.path.join(folder, name, FILE), "w", encoding="utf-8") as handle:
        handle.write(render(data) + "\n")
