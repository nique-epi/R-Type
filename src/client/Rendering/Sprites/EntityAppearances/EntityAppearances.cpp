#include "EntityAppearances.hpp"
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>
#include "ActionAnimation.hpp"
#include "ActionKey.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearanceConstants.hpp"
#include "EntitySheetConstants.hpp"
#include "FrameAnchor.hpp"
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

/** @returns Every frame of the animation @p table describes, in its row. */
std::vector<game::SpriteFrame> framesOf(const engine::DataFileTable& table) {
  const game::SpriteFrame first = firstFrameOf(table);
  const auto count = static_cast<std::size_t>(
      engine::wholeNumberAt(table, FRAME_COUNT_KEY, FRAME_COUNT_RANGE));
  const int spacing = pixelsAt(table, FRAME_SPACING_KEY, FRAME_SPACING_RANGE);
  std::vector<game::SpriteFrame> frames;
  frames.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    frames.push_back(frameInRow(first, index, spacing));
  }
  return frames;
}

/** @returns The pixels the integer @p key of @p table holds, or 0 when it
 * holds nothing: a key the table may leave out. */
int optionalPixelsAt(const engine::DataFileTable& table, std::string_view key,
                     engine::WholeNumberRange range) {
  return table.table.get().contains(key) ? pixelsAt(table, key, range) : 0;
}

/** @returns The animation of the attack phase @p table describes, every key
 * checked, which plays for @p key. */
ActionAnimation actionAnimationOf(const engine::DataFileTable& table,
                                  game::ActionKey key) {
  engine::refuseUnknownKeys(table, ACTION_KEYS);
  const bool leavesAfterwards = !table.table.get().contains(AFTERWARDS_KEY);
  return ActionAnimation{
      .key = key,
      .frames = framesOf(table),
      .frameDuration =
          engine::millisecondsAt(table, FRAME_TIME_KEY, FRAME_TIME_RANGE),
      .playback =
          PLAYBACKS[engine::choiceAt(table, PLAYBACK_KEY, PLAYBACK_NAMES)],
      .returnsToIdle =
          leavesAfterwards ||
          engine::choiceAt(table, AFTERWARDS_KEY, AFTERWARDS_NAMES) == 0,
      .anchor = FrameAnchor{
          .x = optionalPixelsAt(table, ANCHOR_X_KEY, ANCHOR_RANGE),
          .y = optionalPixelsAt(table, ANCHOR_Y_KEY, ANCHOR_RANGE)}};
}

/** @returns The attack number the key @p key of `[appearance]` names
 * (`attack_3` names 3), or nothing when it is not an attack key. */
std::optional<std::uint8_t> attackNumberOf(std::string_view key) {
  if (!key.starts_with(ATTACK_TABLE_PREFIX)) {
    return std::nullopt;
  }
  const std::string_view digits = key.substr(ATTACK_TABLE_PREFIX.size());
  std::uint8_t number = 0;
  const auto [end, error] =
      std::from_chars(digits.data(), digits.data() + digits.size(), number);
  if (error != std::errc{} || end != digits.data() + digits.size()) {
    return std::nullopt;
  }
  return number;
}

/** @brief A key of `[appearance]` that names an attack, and its number. */
struct AttackKey {
  std::string_view key;
  std::uint8_t number{};
};

/** @returns The keys of `[appearance]` that name an attack, in the order the
 * table lists them; they view the keys of @p appearance. */
std::vector<AttackKey> attackKeysOf(const engine::DataFileTable& appearance) {
  std::vector<AttackKey> keys;
  for (const auto& [key, node] : appearance.table.get()) {
    if (const std::optional<std::uint8_t> number = attackNumberOf(key.str())) {
      keys.push_back(AttackKey{.key = key.str(), .number = *number});
    }
  }
  return keys;
}

/** @brief Adds to @p actions the animation of every phase the table
 * @p attack, the attack numbered @p number, describes. */
void addAttackPhases(const engine::DataFileTable& attack, std::uint8_t number,
                     std::vector<ActionAnimation>& actions) {
  engine::refuseUnknownKeys(attack, game::PHASE_NAMES);
  for (std::size_t phase = 0; phase < game::PHASES.size(); ++phase) {
    const std::optional<engine::DataFileTable> table =
        engine::optionalTableAt(attack, game::PHASE_NAMES[phase]);
    if (table.has_value()) {
      actions.push_back(actionAnimationOf(
          *table,
          game::ActionKey{.attack = number, .phase = game::PHASES[phase]}));
    }
  }
}

/** @returns The animation of every attack phase of the `[appearance]` table
 * @p appearance, whose keys are checked. */
std::vector<ActionAnimation> actionsOf(
    const engine::DataFileTable& appearance) {
  const std::vector<AttackKey> attacks = attackKeysOf(appearance);
  std::vector<std::string_view> known{APPEARANCE_KEYS.begin(),
                                      APPEARANCE_KEYS.end()};
  for (const AttackKey& attack : attacks) {
    known.push_back(attack.key);
  }
  engine::refuseUnknownKeys(appearance, known);
  std::vector<ActionAnimation> actions;
  for (const AttackKey& attack : attacks) {
    addAttackPhases(engine::tableAt(appearance, attack.key), attack.number,
                    actions);
  }
  return actions;
}

/** @returns The appearance the `[appearance]` table @p appearance
 * describes, every key checked. */
EntityAppearance appearanceFrom(const engine::DataFileTable& appearance) {
  std::vector<ActionAnimation> actions = actionsOf(appearance);
  const engine::DataFileTable idle =
      engine::tableAt(appearance, IDLE_TABLE_KEY);
  engine::refuseUnknownKeys(idle, IDLE_KEYS);
  return EntityAppearance{.sheet = engine::textAt(appearance, SHEET_KEY),
                          .idleFrames = framesOf(idle),
                          .idleFrameDuration = engine::millisecondsAt(
                              idle, FRAME_TIME_KEY, FRAME_TIME_RANGE),
                          .actions = std::move(actions)};
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
