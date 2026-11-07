#pragma once

#include <functional>
#include <optional>
#include <string>
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EventBus.hpp"
#include "World.hpp"

namespace rtype::server {

class PlayerRoster;

/** @brief What a BossStage is built from. */
struct BossStageParts {
  /** @brief The world the boss appears in; it must outlive the stage. */
  std::reference_wrapper<game::World> world;
  /** @brief The players told about the boss; it must outlive the stage. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  SendDatagram send;
  /** @brief Where the arrival is announced; it must outlive the stage. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The sheets the boss is named among; they must outlive the stage. */
  std::reference_wrapper<const game::EntitySheets> types;
  /** @brief The sheet of the boss of the difficulty; nothing for a level that
   * ends with its last wave. */
  std::optional<std::string> boss;
  /** @brief The health of the boss in percent of its sheet, for the
   * difficulty (WavePlan::bossHealthPercent). */
  int healthPercent{100};
};

}  // namespace rtype::server
