#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include "BossStage.hpp"
#include "CollisionRules.hpp"
#include "DamageRules.hpp"
#include "EntityOrders.hpp"
#include "GameContext.hpp"
#include "IHitArbitration.hpp"
#include "LevelDirector.hpp"
#include "LifeKeeper.hpp"
#include "LookItemKeeper.hpp"
#include "MatchSystemsParts.hpp"
#include "VolleySystem.hpp"
#include "WaveSpawner.hpp"
#include "WeaponSystem.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief The rules and spawners of one world of a game, and the tick of a
 * game in progress they make up: the look items, the views of the hit
 * arbitration, the level a script directs when the game plays one, the
 * systems of the world, the weapons, the volleys, the bodies the scripts
 * asked for, the waves and the boss when the game plays no level, then the
 * lives.
 *
 * The game builds one with each new world. They are built in the order they
 * subscribe to the bus of the game, which is the order its events reach them.
 * Neither copied nor moved: its parts point into each other.
 */
class MatchSystems {
 public:
  explicit MatchSystems(const MatchSystemsParts& parts);

  MatchSystems(const MatchSystems&) = delete;
  MatchSystems& operator=(const MatchSystems&) = delete;
  MatchSystems(MatchSystems&&) = delete;
  MatchSystems& operator=(MatchSystems&&) = delete;
  ~MatchSystems() = default;

  /** @brief Runs tick @p tick of the game in progress, the hit arbitration
   * @p arbitration seeing the world as each player saw it. */
  void onTick(IHitArbitration& arbitration, std::uint32_t tick);

  /** @returns Why the game is won now, as its journal says: the level a
   * script directs is cleared, or, in a game that plays its waves, the last
   * wave is over and its boss, if any, is gone with every other enemy (see
   * BossStage). Nothing while the game goes on. */
  [[nodiscard]] std::optional<std::string_view> victoryReason() const;

  /** @returns Whether the script of the level the game plays failed. */
  [[nodiscard]] bool levelFailed() const;

  /**
   * @brief Puts a team that lost every life back at the last checkpoint of
   * its level: the level restarts there (see LevelDirector), and every
   * player gets the lives of the rules and a new ship.
   * @returns Whether the game plays a level that had reached a checkpoint;
   * nothing changes otherwise.
   */
  bool restartFromCheckpoint();

 private:
  std::reference_wrapper<GameContext> game_;
  std::reference_wrapper<game::World> world_;
  std::unique_ptr<game::CollisionRules> collisionRules_;
  std::unique_ptr<game::DamageRules> damageRules_;
  std::unique_ptr<WaveSpawner> waveSpawner_;
  std::unique_ptr<WeaponSystem> weaponSystem_;
  std::unique_ptr<VolleySystem> volleySystem_;
  std::unique_ptr<EntityOrders> entityOrders_;
  /** nullptr in a game that plays the waves of its difficulty. */
  std::unique_ptr<LevelDirector> levelDirector_;
  std::unique_ptr<BossStage> bossStage_;
  std::unique_ptr<LifeKeeper> lifeKeeper_;
  std::unique_ptr<LookItemKeeper> lookItemKeeper_;
};

}  // namespace rtype::server
