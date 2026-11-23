#include "ThreatModule.hpp"
#include <string>
#include "GameInstanceConstants.hpp"
#include "IThreatLevel.hpp"
#include "InstallContext.hpp"
#include "TeamFirepower.hpp"
#include "ThreatLevel.hpp"
#include "ThreatRank.hpp"

namespace rtype::server {

std::string ThreatModule::name() const {
  return std::string{THREAT_MODULE_NAME};
}

void ThreatModule::install([[maybe_unused]] GameContext& game,
                           engine::InstallContext& install) {
  install.provide<IThreatLevel>(*this);
}

void ThreatModule::start() { rank_ = game::ThreatRank{}; }

void ThreatModule::stop() {}

void ThreatModule::observe(const game::TeamFirepower& team) {
  rank_.observe(team);
}

game::ThreatLevel ThreatModule::level() const { return rank_.level(); }

}  // namespace rtype::server
