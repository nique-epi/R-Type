#include "EntityAppearances.hpp"
#include <cstddef>
#include <filesystem>
#include <functional>
#include <map>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearanceConstants.hpp"
#include "EntitySheetConstants.hpp"
#include "SheetRow.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"
#include "SpriteFrame.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::client {

namespace {

using AppearancesByName = std::map<std::string, EntityAppearance, std::less<>>;

/** @returns The integer @p key of @p table holds, as an int; throws as
 * engine::wholeNumberAt(). The ranges of the keys keep it within an int. */
int pixelsAt(const engine::DataFileTable& table, std::string_view key,
             engine::WholeNumberRange range) {
  return static_cast<int>(engine::wholeNumberAt(table, key, range));
}

/** @returns The first frame of the idle animation @p idle describes. */
game::SpriteFrame firstFrameOf(const engine::DataFileTable& idle) {
  return game::SpriteFrame{
      .left = pixelsAt(idle, FRAME_LEFT_KEY, FRAME_CORNER_RANGE),
      .top = pixelsAt(idle, FRAME_TOP_KEY, FRAME_CORNER_RANGE),
      .width = pixelsAt(idle, FRAME_WIDTH_KEY, FRAME_SIDE_RANGE),
      .height = pixelsAt(idle, FRAME_HEIGHT_KEY, FRAME_SIDE_RANGE)};
}

/** @returns Every frame of the idle animation @p idle describes, in its
 * row. */
std::vector<game::SpriteFrame> idleFramesOf(const engine::DataFileTable& idle) {
  const game::SpriteFrame first = firstFrameOf(idle);
  const auto count = static_cast<std::size_t>(
      engine::wholeNumberAt(idle, FRAME_COUNT_KEY, FRAME_COUNT_RANGE));
  const int spacing = pixelsAt(idle, FRAME_SPACING_KEY, FRAME_SPACING_RANGE);
  std::vector<game::SpriteFrame> frames;
  frames.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    frames.push_back(frameInRow(first, index, spacing));
  }
  return frames;
}

/** @returns The appearance the `[appearance]` table @p appearance
 * describes, every key checked. */
EntityAppearance appearanceFrom(const engine::DataFileTable& appearance) {
  engine::refuseUnknownKeys(appearance, APPEARANCE_KEYS);
  const engine::DataFileTable idle =
      engine::tableAt(appearance, IDLE_TABLE_KEY);
  engine::refuseUnknownKeys(idle, IDLE_KEYS);
  return EntityAppearance{.sheet = engine::textAt(appearance, SHEET_KEY),
                          .idleFrames = idleFramesOf(idle),
                          .idleFrameDuration = engine::millisecondsAt(
                              idle, FRAME_TIME_KEY, FRAME_TIME_RANGE)};
}

/** @returns The appearance of the sheet @p text holds, which the errors call
 * @p source; `[gameplay]` must be there, and is not read. */
EntityAppearance appearanceIn(std::string_view text, std::string_view source) {
  const toml::table root = engine::parseDataFile(text, source);
  return appearanceFrom(
      game::sheetTableOf(engine::rootOf(root, source), game::APPEARANCE_KEY));
}

}  // namespace

EntityAppearances EntityAppearances::fromFolder(
    const std::filesystem::path& folder) {
  return fromTexts(game::sheetTextsIn(folder));
}

EntityAppearances EntityAppearances::fromTexts(
    std::span<const game::SheetText> sheets) {
  AppearancesByName appearances;
  for (const game::SheetText& sheet : sheets) {
    appearances.emplace(sheet.name, appearanceIn(sheet.text, sheet.source));
  }
  return EntityAppearances{std::move(appearances)};
}

const EntityAppearance* EntityAppearances::named(std::string_view name) const {
  const auto found = appearances_.find(name);
  return found == appearances_.end() ? nullptr : &found->second;
}

std::vector<std::string> EntityAppearances::sheets() const {
  std::set<std::string> distinct;
  for (const EntityAppearance& appearance : std::views::values(appearances_)) {
    distinct.insert(appearance.sheet);
  }
  return {distinct.begin(), distinct.end()};
}

EntityAppearances::EntityAppearances(AppearancesByName appearances)
    : appearances_(std::move(appearances)) {}

}  // namespace rtype::client
