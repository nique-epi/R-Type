#include "EntitySheets.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <span>
#include <string_view>
#include <utility>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BuiltInBehaviours.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntitySheetReader.hpp"
#include "EntityType.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"

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

/** @throws engine::UnknownDataFileChoiceException when the gun of a sheet of
 * @p sheets fires no sheet, and InconsistentEntitySheetException when it fires
 * a sheet of another side than projectileSideOf() its shooter. */
void checkProjectiles(std::span<const EntitySheet> sheets) {
  for (const EntitySheet& shooter : sheets) {
    if (!shooter.gun.has_value()) {
      continue;
    }
    const GunSheet& gun = *shooter.gun;
    const EntitySheet* const projectile = findNamed(sheets, gun.projectile);
    if (projectile == nullptr) {
      throw engine::UnknownDataFileChoiceException(
          gun.projectilePlace, gun.projectile, namesOf(sheets));
    }
    if (projectile->side != projectileSideOf(shooter.side)) {
      throw InconsistentEntitySheetException(gun.projectilePlace,
                                             PROJECTILE_SIDE_RULE);
    }
  }
}

/** @brief Writes in the gun of each sheet of @p sheets the number of the sheet
 * it fires, which checkProjectiles() found. */
void numberProjectiles(std::vector<EntitySheet>& sheets) {
  for (EntitySheet& shooter : sheets) {
    if (!shooter.gun.has_value()) {
      continue;
    }
    if (const EntitySheet* const projectile =
            findNamed(sheets, shooter.gun->projectile);
        projectile != nullptr) {
      shooter.gun->projectileNumber = projectile->number;
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
  if (number == 0 || number > sheets_.size()) {
    return nullptr;
  }
  return &sheets_[static_cast<std::size_t>(number) - 1];
}

std::span<const EntitySheet> EntitySheets::all() const { return sheets_; }

const EntitySheet& EntitySheets::projectileOf(const GunSheet& gun) const {
  return sheets_[static_cast<std::size_t>(gun.projectileNumber) - 1];
}

std::span<const SheetText> EntitySheets::texts() const { return texts_; }

EntitySheets::EntitySheets(std::vector<SheetText> texts,
                           BehaviourRegistry behaviours,
                           std::vector<EntitySheet> sheets)
    : texts_(std::move(texts)),
      behaviours_(std::move(behaviours)),
      sheets_(std::move(sheets)) {}

}  // namespace rtype::game
