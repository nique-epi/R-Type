#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ServerConstants.hpp"

namespace rtype::server::testing {

/**
 * @brief The level a game of the flow tests plays: its name, one of
 * LEVELS_OF_THE_FLOW_TESTS, the continues of its team and the stage the game
 * starts at, from FIRST_LEVEL_STAGE.
 */
struct PlayedLevel {
  std::string_view name;
  std::uint8_t continues{DEFAULT_CONTINUES};
  std::size_t startingStage{FIRST_LEVEL_STAGE};
};

}  // namespace rtype::server::testing
