#include "SheetVisualNames.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "ContentName.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "GameException.hpp"

namespace rtype::game {

namespace {

/** @brief Where the names of one kind of visual are read: the key of their
 * table in `[appearance]`, how many it may name, and the rule a sheet that
 * names more breaks. */
struct VisualTable {
  std::string_view key;
  std::size_t most{};
  std::string_view tooManyRule;
};

/** @throws InconsistentEntitySheetException when @p name, a key of
 * @p table, is not a name of the content. */
void checkVisualName(const engine::DataFileTable& table,
                     std::string_view name) {
  if (!identifiers::isContentName(name)) {
    throw InconsistentEntitySheetException(engine::placeOfKey(table, name),
                                           VISUAL_NAME_RULE);
  }
}

/** @returns The names of the tables @p kind names in @p appearance, in their
 * order, each checked; none when @p appearance has no such table. */
std::vector<std::string> namesIn(const engine::DataFileTable& appearance,
                                 const VisualTable& kind) {
  const std::optional<engine::DataFileTable> table =
      engine::optionalTableAt(appearance, kind.key);
  if (!table.has_value()) {
    return {};
  }
  std::vector<std::string> names;
  for (const auto& [key, node] : table->table.get()) {
    checkVisualName(*table, key.str());
    (void)engine::tableAt(*table, key.str());
    names.emplace_back(key.str());
  }
  if (names.size() > kind.most) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(appearance, kind.key), kind.tooManyRule);
  }
  std::ranges::sort(names);
  return names;
}

}  // namespace

SheetVisualNames visualNamesOf(const engine::DataFileTable& appearance) {
  return SheetVisualNames{
      .effects =
          namesIn(appearance, VisualTable{.key = EFFECTS_TABLE_KEY,
                                          .most = MOST_EFFECTS,
                                          .tooManyRule = MOST_EFFECTS_RULE}),
      .looks =
          namesIn(appearance, VisualTable{.key = LOOKS_TABLE_KEY,
                                          .most = MOST_LOOKS,
                                          .tooManyRule = MOST_LOOKS_RULE})};
}

bool namesEffect(const SheetVisualNames& names, std::string_view effect) {
  return std::ranges::binary_search(names.effects, effect);
}

std::optional<std::uint8_t> lookNumberOf(const SheetVisualNames& names,
                                         std::string_view look) {
  if (look.empty()) {
    return std::uint8_t{0};
  }
  const auto found = std::ranges::lower_bound(names.looks, look);
  if (found == names.looks.end() || *found != look) {
    return std::nullopt;
  }
  return static_cast<std::uint8_t>(found - names.looks.begin() + 1);
}

}  // namespace rtype::game
