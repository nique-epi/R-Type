#!/usr/bin/env python3
"""Adds the ground fields to an LDtk project exported before they existed.

Without the biome that generated it, the solid mask of the play plane is
lost: the tool rebuilds the picture of its `Near` layer from its tiles and
takes every drawn pixel as solid. Nothing is drawn in the play corridor, so
the floor is the first drawn pixel under it and the ceiling the last one above
it; a prop standing on the ground at the edge of the corridor reads as ground,
a few pixels higher than the terrain. A project exported by export_ldtk.py
already holds exact fields and needs none of this.

usage: add_ground.py <project.ldtk>
"""
import json
import os
import sys

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
from ground_fields import NEXT_UID, ground_definitions, ground_fields, profile_from_solid  # noqa: E402

NEAR_LAYER = "Near"


def near_picture(project, folder, level):
    """The alpha of the Near layer of a level, drawn from its tiles."""
    layer = next(layer for layer in level["layerInstances"] if layer["__identifier"] == NEAR_LAYER)
    tileset = next(tileset for tileset in project["defs"]["tilesets"] if tileset["uid"] == layer["__tilesetDefUid"])
    atlas = Image.open(os.path.join(folder, tileset["relPath"])).convert("RGBA")
    grid = layer["__gridSize"]
    picture = Image.new("RGBA", (level["pxWid"], level["pxHei"]))
    for tile in layer["gridTiles"]:
        source = atlas.crop((tile["src"][0], tile["src"][1], tile["src"][0] + grid, tile["src"][1] + grid))
        picture.alpha_composite(source, tuple(tile["px"]))
    return np.asarray(picture)[:, :, 3] > 0


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    path = sys.argv[1]
    with open(path, encoding="utf-8") as handle:
        project = json.load(handle)
    folder = os.path.dirname(os.path.abspath(path))
    for level in project["levels"]:
        floors, ceilings = profile_from_solid(near_picture(project, folder, level))
        level["fieldInstances"] = ground_fields(floors, ceilings)
        print(f"{level['identifier']}: floor rows {min(floors)} to {max(floors)}, ceiling rows {min(ceilings)} to {max(ceilings)}")
    project["defs"]["levelFields"] = ground_definitions()
    project["nextUid"] = max(project["nextUid"], NEXT_UID)
    with open(path, "w", encoding="utf-8") as handle:
        json.dump(project, handle, indent="\t")
    print(f"{path} written")


if __name__ == "__main__":
    main()
