"""Sheets of the Pit as received from SpriteCook: how each one is cut into sprites and which palette letters each sprite may use."""

STONE_KEYS = "012345tT"
ROCK_KEYS = "ABDEF0"
PLANT_KEYS = "abcdepqrskK0123tTABD"
WATER_KEYS = "uvwxyzgh"
ROCK_NAMES = ("stalactite_long", "stalactite_wide", "stalactite_trio", "stalagmite", "boulder_large", "boulder_mossy", "pebbles", "rock_spur")

SHEETS = {
    "sheet_plants": {
        "asset_id": "341763ad-3398-4111-a07f-7af47c700461",
        "label": "plants and glowing mushrooms, 4x4 sheet",
        "keys": PLANT_KEYS,
        "cells": [
            ["fern_large", "fern_medium", "sprout", "grass_tall"],
            ["grass_short", "vine_long", "vine_short", "roots_a"],
            ["shroom_tiny", "shroom_spotted", "shroom_giant", "shroom_trio"],
            ["shroom_pink_small", "shroom_pink", "lamp_flower", "flower_pink"],
        ],
    },
    "sheet_ruins": {
        "asset_id": "e7130faf-c022-451c-86cb-ed3ffb3908dc",
        "label": "ruin props, 4x4 sheet",
        "keys": STONE_KEYS + "abcon" + "pq",
        "cells": [
            ["column_short", "column_stump", "drum_fallen", "column_fallen"],
            ["brick_pile", "-slab", "lantern_lit", "lantern_unlit"],
            ["arch", "-urn", "steps", "carved_block"],
            ["rubble", "wall_chunk", "-pedestal", "wall_corner"],
        ],
    },
    "sheet_rocks": {
        "asset_id": "e8a2f308-007a-42c7-b142-225864617193",
        "label": "rocks, stalactites and more plants, 4x4 sheet",
        "keys": PLANT_KEYS,
        "keys_for": dict({name: ROCK_KEYS for name in ROCK_NAMES}, boulder_mossy=ROCK_KEYS + "abc"),
        "cells": [
            ["stalactite_long", "stalactite_wide", "stalactite_trio", "stalagmite"],
            ["boulder_large", "boulder_mossy", "pebbles", "rock_spur"],
            ["fern_broad", "reeds", "shroom_slender", "shroom_shelf"],
            ["shroom_pink_giant", "-lamp_flower_b", "crystals", "roots_b"],
        ],
    },
    "sheet_water": {
        "asset_id": "d1be9979-61b7-4397-89a8-34297150dfc4",
        "label": "water pieces, 3x3 sheet",
        "keys": WATER_KEYS + "0",
        "keep_outline": True,
        "cells": [
            ["-wave_strip", "-water_block", "foam"],
            ["-fall_band", "spout", "splash"],
            ["drip", "-ripple", "-ledge_flow"],
        ],
    },
}
TEXTURES = {
    "texture_rock": {"asset_id": "cafaf857-01d6-4c36-af35-d4c6d8bed785", "label": "cave rock wall texture", "keys": "0ABDE"},
}
REFERENCES = {
    "style_reference": {"asset_id": "6813ae81-f886-4c7b-a71a-dc500a7224c4", "label": "style reference (room_lane.png, uploaded)", "sha12": "6a3a6da14d38"},
}
