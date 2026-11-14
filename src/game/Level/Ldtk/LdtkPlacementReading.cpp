#include "LdtkPlacementReading.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ContentName.hpp"
#include "DataFileReading.hpp"
#include "GameException.hpp"
#include "LdtkConstants.hpp"
#include "LdtkField.hpp"
#include "LdtkFieldValue.hpp"
#include "LdtkPlacement.hpp"
#include "LdtkRefusal.hpp"
#include "Position.hpp"

namespace rtype::game {

namespace {

/** @brief The box of an entity as the file writes it, in pixels of its level:
 * where its pivot is, its size, and where its pivot lies in its box, from 0
 * to 1 on each axis. */
struct LdtkEntityBox {
  float pivotX{};
  float pivotY{};
  float width{};
  float height{};
  float pivotShareX{};
  float pivotShareY{};
};

/** @brief Where a walk of the levels of a file stands: the placements found
 * so far, where the next level starts, and the file named in the errors. */
struct LdtkLevelWalk {
  std::vector<LdtkPlacement> placements;
  float levelStart{};
  std::string_view source;
};

/** @throws InvalidLdtkFileException about @p source, for @p reason followed
 * by @p found. */
[[noreturn]] void refuse(std::string_view source, std::string_view reason,
                         std::string_view found) {
  throw InvalidLdtkFileException(
      LdtkRefusal{.file = std::string{source},
                  .reason = std::string{reason} + std::string{found}});
}

/** @returns The member @p name of @p object.
 * @throws InvalidLdtkFileException when @p object is no object or has no
 * such member. */
const nlohmann::json& memberOf(const nlohmann::json& object,
                               std::string_view name, std::string_view source) {
  if (object.is_object()) {
    const auto found = object.find(name);
    if (found != object.end()) {
      return *found;
    }
  }
  refuse(source, MISSING_MEMBER_REASON, name);
}

/** @returns The number @p value holds, read as the member @p name.
 * @throws InvalidLdtkFileException when it holds anything else. */
float numberOf(const nlohmann::json& value, std::string_view name,
               std::string_view source) {
  if (!value.is_number()) {
    refuse(source, NOT_A_NUMBER_REASON, name);
  }
  return value.get<float>();
}

/** @returns The text @p value holds, read as the member @p name.
 * @throws InvalidLdtkFileException when it holds anything else. */
std::string textOf(const nlohmann::json& value, std::string_view name,
                   std::string_view source) {
  if (!value.is_string()) {
    refuse(source, NOT_A_TEXT_REASON, name);
  }
  return value.get<std::string>();
}

/** @returns The number the member @p name of @p object holds. */
float numberMemberOf(const nlohmann::json& object, std::string_view name,
                     std::string_view source) {
  return numberOf(memberOf(object, name, source), name, source);
}

/** @returns The text the member @p name of @p object holds. */
std::string textMemberOf(const nlohmann::json& object, std::string_view name,
                         std::string_view source) {
  return textOf(memberOf(object, name, source), name, source);
}

/** @returns The two numbers of the pair @p name of @p entity, x then y. */
std::array<float, PAIR_SIZE> pairOf(const nlohmann::json& entity,
                                    std::string_view name,
                                    std::string_view source) {
  const nlohmann::json& pair = memberOf(entity, name, source);
  if (!pair.is_array() || pair.size() != PAIR_SIZE) {
    refuse(source, NOT_A_NUMBER_REASON, name);
  }
  return std::array{numberOf(pair.at(HORIZONTAL_AXIS), name, source),
                    numberOf(pair.at(VERTICAL_AXIS), name, source)};
}

/** @returns The box of @p entity, as the file writes it. */
LdtkEntityBox boxOf(const nlohmann::json& entity, std::string_view source) {
  const std::array pixel = pairOf(entity, ENTITY_PIXEL_MEMBER, source);
  const std::array pivot = pairOf(entity, ENTITY_PIVOT_MEMBER, source);
  return LdtkEntityBox{
      .pivotX = pixel[HORIZONTAL_AXIS],
      .pivotY = pixel[VERTICAL_AXIS],
      .width = numberMemberOf(entity, ENTITY_WIDTH_MEMBER, source),
      .height = numberMemberOf(entity, ENTITY_HEIGHT_MEMBER, source),
      .pivotShareX = pivot[HORIZONTAL_AXIS],
      .pivotShareY = pivot[VERTICAL_AXIS]};
}

/** @returns The center of @p box in logical units, @p levelStart units after
 * the start of the file. */
Position centerOf(const LdtkEntityBox& box, float levelStart) {
  const float centerX =
      box.pivotX + ((LDTK_CENTER_PIVOT - box.pivotShareX) * box.width);
  const float centerY =
      box.pivotY + ((LDTK_CENTER_PIVOT - box.pivotShareY) * box.height);
  return Position{.x = levelStart + (centerX * LDTK_PIXEL),
                  .y = centerY * LDTK_PIXEL};
}

/** @returns The identifier @p identifier of an entity as the name of a sheet:
 * each capital letter after a letter or a digit starts a word, the words are
 * joined by SHEET_NAME_WORD_SEPARATOR, and every letter is lowercase. */
std::string sheetNameOf(std::string_view identifier) {
  std::string name;
  for (const char character : identifier) {
    const bool capital =
        std::isupper(static_cast<unsigned char>(character)) != 0;
    if (capital && !name.empty() && name.back() != SHEET_NAME_WORD_SEPARATOR) {
      name.push_back(SHEET_NAME_WORD_SEPARATOR);
    }
    name.push_back(
        static_cast<char>(std::tolower(static_cast<unsigned char>(character))));
  }
  return name;
}

/** @returns Whether a field of @p type is kept as a text. */
bool isTextFieldType(std::string_view type) {
  return type == STRING_FIELD_TYPE || type == MULTILINES_FIELD_TYPE ||
         type == COLOR_FIELD_TYPE || type.starts_with(ENUM_FIELD_TYPE_START);
}

/** @returns The value of the field @p field, read as its kind says; nothing
 * for a null value.
 * @throws InvalidLdtkFileException for a kind the game does not read. */
std::optional<LdtkFieldValue> fieldValueOf(const nlohmann::json& field,
                                           std::string_view source) {
  const std::string type = textMemberOf(field, FIELD_TYPE_MEMBER, source);
  const nlohmann::json& value = memberOf(field, FIELD_VALUE_MEMBER, source);
  if (value.is_null()) {
    return std::nullopt;
  }
  if (type == INT_FIELD_TYPE || type == FLOAT_FIELD_TYPE) {
    return LdtkFieldValue{double{numberOf(value, FIELD_VALUE_MEMBER, source)}};
  }
  if (type == BOOL_FIELD_TYPE && value.is_boolean()) {
    return LdtkFieldValue{value.get<bool>()};
  }
  if (isTextFieldType(type)) {
    return LdtkFieldValue{textOf(value, FIELD_VALUE_MEMBER, source)};
  }
  refuse(source, UNSUPPORTED_FIELD_REASON,
         textMemberOf(field, FIELD_IDENTIFIER_MEMBER, source));
}

/** @returns The fields of @p entity the game reads, in the order of the
 * file. */
std::vector<LdtkField> fieldsOf(const nlohmann::json& entity,
                                std::string_view source) {
  std::vector<LdtkField> fields;
  for (const nlohmann::json& field :
       memberOf(entity, FIELD_INSTANCES_MEMBER, source)) {
    std::optional<LdtkFieldValue> value = fieldValueOf(field, source);
    if (value.has_value()) {
      fields.push_back(LdtkField{
          .name = textMemberOf(field, FIELD_IDENTIFIER_MEMBER, source),
          .value = std::move(*value)});
    }
  }
  return fields;
}

/** @returns The placement of @p entity, in a level starting @p levelStart
 * units after the start of the file. */
LdtkPlacement placementOf(const nlohmann::json& entity, float levelStart,
                          std::string_view source) {
  std::string identifier =
      textMemberOf(entity, ENTITY_IDENTIFIER_MEMBER, source);
  std::string sheet = sheetNameOf(identifier);
  if (!identifiers::isContentName(sheet)) {
    refuse(source, NOT_A_SHEET_NAME_REASON, identifier);
  }
  const LdtkEntityBox box = boxOf(entity, source);
  return LdtkPlacement{.entity = std::move(identifier),
                       .sheet = std::move(sheet),
                       .position = centerOf(box, levelStart),
                       .width = box.width * LDTK_PIXEL,
                       .fields = fieldsOf(entity, source)};
}

/** @returns The layer of entities MOBS_LAYER of @p level.
 * @throws InvalidLdtkFileException when the level is saved separately or
 * has no such layer. */
const nlohmann::json& mobsLayerOf(const nlohmann::json& level,
                                  std::string_view source) {
  const nlohmann::json& layers =
      memberOf(level, LAYER_INSTANCES_MEMBER, source);
  const std::string identifier =
      textMemberOf(level, LEVEL_IDENTIFIER_MEMBER, source);
  if (!layers.is_array()) {
    refuse(source, LEVELS_SAVED_SEPARATELY_REASON, identifier);
  }
  for (const nlohmann::json& layer : layers) {
    if (memberOf(layer, LAYER_TYPE_MEMBER, source) == ENTITIES_LAYER_TYPE &&
        memberOf(layer, LAYER_IDENTIFIER_MEMBER, source) == MOBS_LAYER) {
      return layer;
    }
  }
  refuse(source, NO_MOBS_LAYER_REASON, identifier);
}

/** @brief Adds the enemies of @p level to the walk, then moves its start past
 * the level. */
void addLevel(const nlohmann::json& level, LdtkLevelWalk& walk) {
  const nlohmann::json& layer = mobsLayerOf(level, walk.source);
  for (const nlohmann::json& entity :
       memberOf(layer, ENTITY_INSTANCES_MEMBER, walk.source)) {
    walk.placements.push_back(
        placementOf(entity, walk.levelStart, walk.source));
  }
  walk.levelStart +=
      numberMemberOf(level, LEVEL_WIDTH_MEMBER, walk.source) * LDTK_PIXEL;
}

}  // namespace

std::vector<LdtkPlacement> ldtkPlacementsOf(const std::filesystem::path& file) {
  const std::string source = engine::shownPath(file);
  const nlohmann::json project =
      nlohmann::json::parse(engine::dataFileContent(file), nullptr, false);
  if (project.is_discarded() || !project.is_object()) {
    refuse(source, NOT_JSON_REASON, {});
  }
  LdtkLevelWalk walk{.placements = {}, .levelStart = 0.0F, .source = source};
  for (const nlohmann::json& level : memberOf(project, LEVELS_MEMBER, source)) {
    addLevel(level, walk);
  }
  std::ranges::stable_sort(walk.placements, {},
                           [](const LdtkPlacement& placement) noexcept {
                             return placement.position.x;
                           });
  return std::move(walk.placements);
}

}  // namespace rtype::game
