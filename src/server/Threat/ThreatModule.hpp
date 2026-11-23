#pragma once

#include <string>
#include "IGameModule.hpp"
#include "IThreatLevel.hpp"
#include "InstallContext.hpp"
#include "TeamFirepower.hpp"
#include "ThreatLevel.hpp"
#include "ThreatRank.hpp"

namespace rtype::server {

struct GameContext;

/**
 * @brief The module of a game that keeps its threat, which grows with the
 * firepower of its team so its enemies answer the weapons the players pick
 * up (see IThreatLevel).
 *
 * Optional: nothing includes it but the list of the modules of a game, which
 * installs it before the rules so they find its service. A game without it
 * makes its enemies at the strength of its difficulty and its players alone.
 */
class ThreatModule final : public IGameModule, public IThreatLevel {
 public:
  [[nodiscard]] std::string name() const override;
  void install(GameContext& game, engine::InstallContext& install) override;
  void start() override;
  void stop() override;

  void observe(const game::TeamFirepower& team) override;
  [[nodiscard]] game::ThreatLevel level() const override;

 private:
  game::ThreatRank rank_;
};

}  // namespace rtype::server
