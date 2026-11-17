#pragma once

#include <functional>
#include <optional>
#include <string>
#include "IEntityOrders.hpp"
#include "World.hpp"

namespace rtype::server {

/** @brief What a BossStage is built from. */
struct BossStageParts {
  /** @brief The world whose enemies say whether the level is cleared; it
   * must outlive the stage. */
  std::reference_wrapper<game::World> world;
  /** @brief What creates the boss, as it creates the bodies the scripts ask
   * for; it must outlive the stage. */
  std::reference_wrapper<game::IEntityOrders> orders;
  /** @brief The sheet of the boss of the difficulty; nothing for a level that
   * ends with its last wave. */
  std::optional<std::string> boss;
};

}  // namespace rtype::server
