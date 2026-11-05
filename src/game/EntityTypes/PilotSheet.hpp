#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include "IBehaviour.hpp"

namespace rtype::game {

/**
 * @brief The behaviour a sheet names as its pilot, with the values its table
 * `[gameplay.behaviour]` gives the settings of that behaviour, in the order
 * IBehaviour::settings() declares them.
 *
 * The behaviour belongs to the BehaviourRegistry of the EntitySheets that read
 * the sheet, which keeps it at the same address for as long as it lives.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct PilotSheet {
  std::reference_wrapper<const IBehaviour> behaviour;
  std::vector<std::int64_t> settings;
};

}  // namespace rtype::game
