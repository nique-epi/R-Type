"""The ground of a map, as two level fields of its LDtk project.

`Floor` holds, for each pixel column of a level, the row of the first solid
pixel of the play plane below the play corridor, and `Ceiling` the row just
under the last solid pixel above it; -1 where the column has none. The game
server reads them to lay decor and walking bodies on the terrain the client
draws (see the page "Generating the maps of a biome" of the docs).
"""
from frame import PLAY_BOTTOM, PLAY_TOP

FLOOR = "Floor"
CEILING = "Ceiling"
NONE = -1
UID = {FLOOR: 1000, CEILING: 1001}
NEXT_UID = max(UID.values()) + 1


def profile_from_solid(solid):
    """Floor and ceiling rows of each column of a solid mask (rows, columns),
    searched from the edges of the play corridor outwards."""
    rows, columns = solid.shape
    floors, ceilings = [], []
    for x in range(columns):
        column = solid[:, x]
        floor = next((y for y in range(PLAY_BOTTOM, rows) if column[y]), NONE)
        ceiling = next((y + 1 for y in range(PLAY_TOP - 1, -1, -1) if column[y]), NONE)
        floors.append(int(floor))
        ceilings.append(int(ceiling))
    return floors, ceilings


def field_definition(name):
    """A level field of LDtk 1.5.3 holding an array of integers, hidden in the
    editor."""
    return {
        "identifier": name, "doc": "Row of the " + name.lower() + " of the play plane, per pixel column; -1 for none",
        "__type": "Array<Int>", "uid": UID[name], "type": "F_Int", "isArray": True, "canBeNull": False,
        "arrayMinLength": None, "arrayMaxLength": None, "editorDisplayMode": "Hidden", "editorDisplayScale": 1,
        "editorDisplayPos": "Above", "editorLinkStyle": "StraightArrow", "editorDisplayColor": None,
        "editorAlwaysShow": False, "editorShowInWorld": False, "editorCutLongValues": True, "editorTextSuffix": None,
        "editorTextPrefix": None, "useForSmartColor": False, "exportToToc": False, "searchable": False,
        "min": None, "max": None, "regex": None, "acceptFileTypes": None, "defaultOverride": None,
        "textLanguageMode": None, "symmetricalRef": False, "autoChainRef": True, "allowOutOfLevelRef": True,
        "allowedRefs": "Any", "allowedRefsEntityUid": None, "allowedRefTags": [], "tilesetUid": None,
    }


def field_instance(name, values):
    """The value of the field `name` in a level."""
    return {
        "__identifier": name, "__type": "Array<Int>", "__value": list(values), "__tile": None, "defUid": UID[name],
        "realEditorValues": [{"id": "V_Int", "params": [value]} for value in values],
    }


def ground_fields(floors, ceilings):
    """The two field instances of a level."""
    return [field_instance(FLOOR, floors), field_instance(CEILING, ceilings)]


def ground_definitions():
    """The two field definitions of a project."""
    return [field_definition(FLOOR), field_definition(CEILING)]
