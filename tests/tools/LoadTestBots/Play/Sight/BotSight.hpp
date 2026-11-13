#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "EntityMessage.hpp"
#include "IEntityTypeTable.hpp"
#include "SeenEntity.hpp"
#include "SightFigures.hpp"
#include "WorldState.hpp"

namespace rtype::tools {

/**
 * @brief What a bot knows of its game, read from the messages it receives:
 * its ship, its run and the enemies, their missiles and the items around it.
 *
 * The parts of one tick add up. Comparing the tick with the newest one is
 * enough, as in BotTally: the session never delivers a part of an older tick
 * after a newer one.
 */
class BotSight {
 public:
  /** @param types The table of the session; it must outlive the sight. */
  explicit BotSight(const client::IEntityTypeTable& types);

  /** @brief Reads @p message as the player @p playerNumber sees it. */
  void read(const client::EntityMessage& message, std::uint8_t playerNumber);

  [[nodiscard]] const SightFigures& figures() const;

  /** @returns What the states of the newest tick list. */
  [[nodiscard]] std::span<const SeenEntity> entities() const;

 private:
  /** @brief Keeps the entities of the tick before when @p tick is a new one. */
  void startTick(std::uint32_t tick);
  void readPlayer(const network::WorldState& state, std::uint8_t playerNumber);
  void addEntities(const network::WorldState& state);
  void endGame();

  const client::IEntityTypeTable* types_;
  SightFigures figures_;
  std::vector<SeenEntity> entities_;
  std::vector<SeenEntity> previous_;
  std::uint32_t previousTick_{0};
  std::uint64_t scoreOfEndedGames_{0};
};

}  // namespace rtype::tools
