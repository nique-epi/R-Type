#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::game {

/** @brief How many logical units one LDtk pixel is: a pixel of a 480 by 270
 * level is a pixel of the canvas of the client, 4 units of the playfield. */
constexpr float LDTK_PIXEL = 4.0F;

/** @brief The pivot of the center of an entity, on each axis: `__pivot` goes
 * from 0, the left or the top, to 1. */
constexpr float LDTK_CENTER_PIVOT = 0.5F;

/** @brief The layer of entities a level places its enemies on. */
constexpr std::string_view MOBS_LAYER = "Mobs";

/** @brief The members of a project read, as the JSON schema of LDtk 1.5.3
 * names them. */
constexpr std::string_view LEVELS_MEMBER = "levels";
constexpr std::string_view LEVEL_IDENTIFIER_MEMBER = "identifier";
constexpr std::string_view LEVEL_WIDTH_MEMBER = "pxWid";
constexpr std::string_view LAYER_INSTANCES_MEMBER = "layerInstances";
constexpr std::string_view LAYER_TYPE_MEMBER = "__type";
constexpr std::string_view LAYER_IDENTIFIER_MEMBER = "__identifier";
constexpr std::string_view ENTITIES_LAYER_TYPE = "Entities";
constexpr std::string_view ENTITY_INSTANCES_MEMBER = "entityInstances";
constexpr std::string_view ENTITY_IDENTIFIER_MEMBER = "__identifier";
constexpr std::string_view ENTITY_PIXEL_MEMBER = "px";
constexpr std::string_view ENTITY_PIVOT_MEMBER = "__pivot";
constexpr std::string_view ENTITY_WIDTH_MEMBER = "width";
constexpr std::string_view ENTITY_HEIGHT_MEMBER = "height";
constexpr std::string_view FIELD_INSTANCES_MEMBER = "fieldInstances";
constexpr std::string_view FIELD_IDENTIFIER_MEMBER = "__identifier";
constexpr std::string_view FIELD_TYPE_MEMBER = "__type";
constexpr std::string_view FIELD_VALUE_MEMBER = "__value";

/** @brief The kinds of field a placement keeps: the numbers, the flags and
 * the texts; an enum is `Enum(name)`. */
constexpr std::string_view INT_FIELD_TYPE = "Int";
constexpr std::string_view FLOAT_FIELD_TYPE = "Float";
constexpr std::string_view BOOL_FIELD_TYPE = "Bool";
constexpr std::string_view STRING_FIELD_TYPE = "String";
constexpr std::string_view MULTILINES_FIELD_TYPE = "Multilines";
constexpr std::string_view COLOR_FIELD_TYPE = "Color";
constexpr std::string_view ENUM_FIELD_TYPE_START = "Enum(";

/** @brief A pair `[x, y]` of the file: its size and its axes. */
constexpr std::size_t PAIR_SIZE = 2;
constexpr std::size_t HORIZONTAL_AXIS = 0;
constexpr std::size_t VERTICAL_AXIS = 1;

/** @brief What joins the words of the identifier of an entity in the name of
 * its sheet: `BydoViolet` names `bydo_violet`. */
constexpr char SHEET_NAME_WORD_SEPARATOR = '_';

/** @brief Why a file is refused; each is followed by what was found. */
constexpr std::string_view NOT_JSON_REASON = "the file is not a JSON object";
constexpr std::string_view MISSING_MEMBER_REASON = "a member is missing: ";
constexpr std::string_view NOT_A_NUMBER_REASON = "a number is expected at ";
constexpr std::string_view NOT_A_TEXT_REASON = "a text is expected at ";
constexpr std::string_view LEVELS_SAVED_SEPARATELY_REASON =
    "the levels are saved in separate files, which the game does not read; "
    "turn the option off in LDtk for the level ";
constexpr std::string_view NO_MOBS_LAYER_REASON =
    "no layer of entities is named Mobs in the level ";
constexpr std::string_view NOT_A_SHEET_NAME_REASON =
    "the identifier names no sheet, since it is not 1 to 24 letters and "
    "digits: ";
constexpr std::string_view UNSUPPORTED_FIELD_REASON =
    "a field of a kind the game does not read (a point, a tile, a reference "
    "or an array): ";

}  // namespace rtype::game
