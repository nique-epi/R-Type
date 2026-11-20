#include "GroundReading.hpp"
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <nlohmann/json.hpp>  // IWYU pragma: keep
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "BiomeGround.hpp"
#include "DataFileReading.hpp"
#include "GroundConstants.hpp"
#include "LdtkConstants.hpp"
#include "LdtkJson.hpp"

namespace rtype::game {

namespace {

/** @returns The field @p name of @p level; nullptr when it has none. */
const nlohmann::json* fieldNamed(const nlohmann::json& level,
                                 std::string_view name,
                                 std::string_view source) {
  for (const nlohmann::json& field :
       ldtkMemberOf(level, FIELD_INSTANCES_MEMBER, source)) {
    if (ldtkTextMemberOf(field, FIELD_IDENTIFIER_MEMBER, source) == name) {
      return &field;
    }
  }
  return nullptr;
}

/** @returns Whether @p value holds a whole number. */
bool isWholeNumber(const nlohmann::json& value) {
  return value.is_number_integer() ||
         (value.is_number() &&
          std::trunc(value.get<double>()) == value.get<double>());
}

/** @returns The rows the field @p name of @p level holds, one for each of
 * its pixel columns.
 * @throws InvalidLdtkFileException when the level lacks the field or the
 * field holds anything else. */
const nlohmann::json& groundRowsOf(const nlohmann::json& level,
                                   std::string_view name,
                                   std::string_view source) {
  const std::string identifier =
      ldtkTextMemberOf(level, LEVEL_IDENTIFIER_MEMBER, source);
  const nlohmann::json* const field = fieldNamed(level, name, source);
  if (field == nullptr) {
    refuseLdtkFile(source, GROUND_FIELD_MISSING_REASON, identifier);
  }
  const nlohmann::json& values =
      ldtkMemberOf(*field, FIELD_VALUE_MEMBER, source);
  const auto width = static_cast<std::size_t>(
      ldtkNumberMemberOf(level, LEVEL_WIDTH_MEMBER, source));
  if (!values.is_array() || values.size() != width) {
    refuseLdtkFile(source, GROUND_FIELD_SIZE_REASON, identifier);
  }
  for (const nlohmann::json& value : values) {
    if (!isWholeNumber(value)) {
      refuseLdtkFile(source, GROUND_FIELD_SIZE_REASON, identifier);
    }
  }
  return values;
}

/** @brief Adds @p values, rows groundRowsOf() checked, to @p rows. */
void addRows(std::vector<int>& rows, const nlohmann::json& values) {
  for (const nlohmann::json& value : values) {
    rows.push_back(value.get<int>());
  }
}

}  // namespace

std::optional<BiomeGround> ldtkGroundOf(const std::filesystem::path& file) {
  const std::string source = engine::shownPath(file);
  const nlohmann::json project = ldtkProjectOf(file, source);
  const nlohmann::json& levels = ldtkMemberOf(project, LEVELS_MEMBER, source);
  if (!levels.is_array() || levels.empty() ||
      fieldNamed(levels.front(), FLOOR_FIELD, source) == nullptr) {
    return std::nullopt;
  }
  BiomeGround ground;
  for (const nlohmann::json& level : levels) {
    addRows(ground.floorRows, groundRowsOf(level, FLOOR_FIELD, source));
    addRows(ground.ceilingRows, groundRowsOf(level, CEILING_FIELD, source));
  }
  return ground;
}

}  // namespace rtype::game
