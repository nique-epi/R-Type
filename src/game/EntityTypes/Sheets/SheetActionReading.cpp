#include "SheetActionReading.hpp"
#include <cstdint>
#include "ActionKey.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheetConstants.hpp"

namespace rtype::game {

ActionKey actionKeyAt(const engine::DataFileTable& table) {
  return ActionKey{
      .attack = static_cast<std::uint8_t>(
          engine::wholeNumberAt(table, ATTACK_KEY, ATTACK_NUMBER_RANGE)),
      .phase = PHASES.at(engine::choiceAt(table, PHASE_KEY, PHASE_NAMES))};
}

}  // namespace rtype::game
