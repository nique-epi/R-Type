#include "VolleySheetReading.hpp"
#include <cstdint>
#include <string_view>
#include <vector>
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"
#include "GameException.hpp"
#include "SheetActionReading.hpp"
#include "VolleySheet.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

namespace {

/** @returns The whole number @p key of @p table holds, 0 when it holds
 * none. */
std::int64_t wholeOrZeroAt(const engine::DataFileTable& table,
                           std::string_view key,
                           engine::WholeNumberRange range) {
  if (!table.table.get().contains(key)) {
    return 0;
  }
  return engine::wholeNumberAt(table, key, range);
}

/** @returns The value of @p key of @p table in logical units or degrees, 0
 * when it holds none. */
float floatOrZeroAt(const engine::DataFileTable& table, std::string_view key,
                    engine::WholeNumberRange range) {
  return static_cast<float>(wholeOrZeroAt(table, key, range));
}

/** @returns The chance of the volley of @p table, VOLLEY_SURE_CHANCE when
 * it gives none. */
std::uint8_t chanceAt(const engine::DataFileTable& table) {
  if (!table.table.get().contains(CHANCE_KEY)) {
    return VOLLEY_SURE_CHANCE;
  }
  return static_cast<std::uint8_t>(
      engine::wholeNumberAt(table, CHANCE_KEY, VOLLEY_CHANCE_RANGE));
}

/** @returns The volley one table of `[[gameplay.volley]]` describes.
 * @throws InconsistentEntitySheetException when its gap leaves no body. */
VolleySheet volleyFrom(const engine::DataFileTable& table) {
  engine::refuseUnknownKeys(table, VOLLEY_KEYS);
  VolleySheet volley{
      .on = actionKeyAt(table),
      .count = static_cast<std::uint8_t>(
          engine::wholeNumberAt(table, COUNT_KEY, VOLLEY_COUNT_RANGE)),
      .offsetX = static_cast<float>(
          engine::wholeNumberAt(table, OFFSET_X_KEY, VOLLEY_OFFSET_RANGE)),
      .offsetY = static_cast<float>(
          engine::wholeNumberAt(table, OFFSET_Y_KEY, VOLLEY_OFFSET_RANGE)),
      .stepX = floatOrZeroAt(table, STEP_X_KEY, VOLLEY_OFFSET_RANGE),
      .stepY = floatOrZeroAt(table, STEP_Y_KEY, VOLLEY_OFFSET_RANGE),
      .spreadDegrees = floatOrZeroAt(table, SPREAD_KEY, VOLLEY_SPREAD_RANGE),
      .distance = floatOrZeroAt(table, DISTANCE_KEY, VOLLEY_DISTANCE_RANGE),
      .headingDegrees = floatOrZeroAt(table, HEADING_KEY, VOLLEY_HEADING_RANGE),
      .jitterX = floatOrZeroAt(table, JITTER_X_KEY, VOLLEY_JITTER_RANGE),
      .jitterY = floatOrZeroAt(table, JITTER_Y_KEY, VOLLEY_JITTER_RANGE),
      .gapFrom = static_cast<std::uint8_t>(
          wholeOrZeroAt(table, GAP_FROM_KEY, VOLLEY_GAP_RANGE)),
      .gapCount = static_cast<std::uint8_t>(
          wholeOrZeroAt(table, GAP_COUNT_KEY, VOLLEY_GAP_RANGE)),
      .chancePercent = chanceAt(table),
      .body = engine::textAt(table, BODY_KEY),
      .bodyPlace = engine::placeOfKey(table, BODY_KEY)};
  if (volley.gapCount >= volley.count) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(table, GAP_COUNT_KEY), VOLLEY_GAP_RULE);
  }
  return volley;
}

}  // namespace

std::vector<VolleySheet> volleysFrom(const engine::DataFileTable& gameplay) {
  if (!gameplay.table.get().contains(VOLLEY_KEY)) {
    return {};
  }
  std::vector<VolleySheet> volleys;
  for (const engine::DataFileTable& table :
       engine::tablesAt(gameplay, VOLLEY_KEY)) {
    volleys.push_back(volleyFrom(table));
  }
  return volleys;
}

}  // namespace rtype::game
