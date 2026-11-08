#include "EntitySheets.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BuiltInBehaviours.hpp"
#include "BurstSheet.hpp"
#include "DataFilePlace.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntitySheetReader.hpp"
#include "EntityType.hpp"
#include "EntityTypeConstants.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"
#include "VolleySheet.hpp"

namespace rtype::game {

namespace {

/** @returns @p texts in the order of their names.
 * @throws DuplicateEntityTypeException when two share a name. */
std::vector<SheetText> sortedByName(std::span<const SheetText> texts) {
  std::vector<SheetText> sorted{texts.begin(), texts.end()};
  std::ranges::sort(sorted, std::ranges::less{}, &SheetText::name);
  const auto twin = std::ranges::adjacent_find(sorted, std::ranges::equal_to{},
                                               &SheetText::name);
  if (twin != sorted.end()) {
    throw DuplicateEntityTypeException(twin->name);
  }
  return sorted;
}

/** @returns The gameplay of each of @p sorted, numbered from 1 in that
 * order. */
std::vector<EntitySheet> numberedSheets(std::span<const SheetText> sorted,
                                        const BehaviourRegistry& behaviours) {
  std::vector<EntitySheet> sheets;
  sheets.reserve(sorted.size());
  for (const SheetText& text : sorted) {
    EntitySheet sheet = gameplayOf(text, behaviours);
    sheet.number = static_cast<std::uint8_t>(sheets.size() + 1);
    sheets.push_back(std::move(sheet));
  }
  return sheets;
}

/** @returns The sheet of @p sheets named @p name, nullptr when none is. */
const EntitySheet* findNamed(std::span<const EntitySheet> sheets,
                             std::string_view name) {
  const auto found = std::ranges::find(sheets, name, &EntitySheet::name);
  return found == sheets.end() ? nullptr : &*found;
}

/** @returns The names of @p sheets, in their order. */
std::vector<std::string_view> namesOf(std::span<const EntitySheet> sheets) {
  std::vector<std::string_view> names;
  names.reserve(sheets.size());
  for (const EntitySheet& sheet : sheets) {
    names.emplace_back(sheet.name);
  }
  return names;
}

/** @returns The side of what a body of side @p shooter fires: the missiles
 * of a player for a player, those of an enemy for any other side. */
EntityType projectileSideOf(EntityType shooter) {
  return shooter == EntityType::Player ? EntityType::PlayerMissile
                                       : EntityType::EnemyMissile;
}

/** @returns The sheet of @p sheets named @p name, which a sheet names at
 * @p place.
 * @throws engine::UnknownDataFileChoiceException when no sheet has that
 * name. */
const EntitySheet& referencedSheet(std::span<const EntitySheet> sheets,
                                   const std::string& name,
                                   const engine::DataFilePlace& place) {
  const EntitySheet* const found = findNamed(sheets, name);
  if (found == nullptr) {
    throw engine::UnknownDataFileChoiceException(place, name, namesOf(sheets));
  }
  return *found;
}

/** @throws engine::UnknownDataFileChoiceException when the gun of a sheet of
 * @p sheets fires no sheet, or a missile bursts into no sheet, and
 * InconsistentEntitySheetException when a gun fires a sheet of another side
 * than projectileSideOf() its shooter, or a missile bursts into missiles of
 * another side than its own. */
void checkProjectiles(std::span<const EntitySheet> sheets) {
  for (const EntitySheet& sheet : sheets) {
    if (sheet.gun.has_value()) {
      const GunSheet& gun = *sheet.gun;
      if (referencedSheet(sheets, gun.projectile, gun.projectilePlace).side !=
          projectileSideOf(sheet.side)) {
        throw InconsistentEntitySheetException(gun.projectilePlace,
                                               PROJECTILE_SIDE_RULE);
      }
    }
    if (sheet.burst.has_value()) {
      const BurstSheet& burst = *sheet.burst;
      if (referencedSheet(sheets, burst.shard, burst.shardPlace).side !=
          sheet.side) {
        throw InconsistentEntitySheetException(burst.shardPlace,
                                               SHARD_SIDE_RULE);
      }
    }
  }
}

/** @throws engine::UnknownDataFileChoiceException when a volley of a sheet of
 * @p sheets names no sheet, and InconsistentEntitySheetException when it
 * names one that is neither an enemy nor a missile of an enemy. */
void checkVolleys(std::span<const EntitySheet> sheets) {
  for (const EntitySheet& sheet : sheets) {
    for (const VolleySheet& volley : sheet.volleys) {
      const EntityType side =
          referencedSheet(sheets, volley.body, volley.bodyPlace).side;
      if (side != EntityType::Enemy && side != EntityType::EnemyMissile) {
        throw InconsistentEntitySheetException(volley.bodyPlace,
                                               VOLLEY_BODY_SIDE_RULE);
      }
    }
  }
}

/** @brief Writes in the gun of each sheet of @p sheets the number of the sheet
 * it fires, in its burst the number of the sheet of its shards, and in each
 * of its volleys the number of the sheet of its bodies. checkProjectiles()
 * and checkVolleys() ran first, so every name is a sheet of @p sheets. */
void numberProjectiles(std::vector<EntitySheet>& sheets) {
  for (EntitySheet& sheet : sheets) {
    if (sheet.gun.has_value()) {
      sheet.gun->projectileNumber =
          findNamed(sheets, sheet.gun->projectile)->number;
    }
    if (sheet.burst.has_value()) {
      sheet.burst->shardNumber = findNamed(sheets, sheet.burst->shard)->number;
    }
    for (VolleySheet& volley : sheet.volleys) {
      volley.bodyNumber = findNamed(sheets, volley.body)->number;
    }
  }
}

}  // namespace

EntitySheets EntitySheets::fromFolder(const std::filesystem::path& folder) {
  return fromTexts(sheetTextsIn(folder));
}

EntitySheets EntitySheets::fromTexts(std::span<const SheetText> texts) {
  checkSheetNames(texts);
  std::vector<SheetText> sorted = sortedByName(texts);
  BehaviourRegistry behaviours = builtInBehaviours();
  std::vector<EntitySheet> sheets = numberedSheets(sorted, behaviours);
  checkProjectiles(sheets);
  checkVolleys(sheets);
  numberProjectiles(sheets);
  return EntitySheets{std::move(sorted), std::move(behaviours),
                      std::move(sheets)};
}

const EntitySheet& EntitySheets::named(std::string_view name) const {
  const EntitySheet* const sheet = findNamed(sheets_, name);
  if (sheet == nullptr) {
    throw UnknownEntityTypeException(name, namesOf(sheets_));
  }
  return *sheet;
}

const EntitySheet* EntitySheets::numbered(std::uint8_t number) const {
  if (number == identifiers::NO_ENTITY_TYPE || number > sheets_.size()) {
    return nullptr;
  }
  return &sheets_[static_cast<std::size_t>(number) - 1];
}

std::span<const EntitySheet> EntitySheets::all() const { return sheets_; }

std::span<const SheetText> EntitySheets::texts() const { return texts_; }

EntitySheets::EntitySheets(std::vector<SheetText> texts,
                           BehaviourRegistry behaviours,
                           std::vector<EntitySheet> sheets)
    : texts_(std::move(texts)),
      behaviours_(std::move(behaviours)),
      sheets_(std::move(sheets)) {}

}  // namespace rtype::game
