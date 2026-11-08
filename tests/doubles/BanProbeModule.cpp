#include "BanProbeModule.hpp"
#include <string>
#include <vector>
#include "BanProbeModuleConstants.hpp"
#include "InstallContext.hpp"
#include "LatestBans.hpp"

std::string BanProbeModule::name() const {
  return std::string{BAN_PROBE_MODULE_NAME};
}

std::vector<std::string> BanProbeModule::dependencies() const { return {}; }

void BanProbeModule::install(rtype::engine::InstallContext& context) {
  bans_ = context.find<rtype::server::LatestBans>();
}

void BanProbeModule::start() {}

void BanProbeModule::stop() {}

rtype::server::LatestBans* BanProbeModule::bans() const { return bans_; }
