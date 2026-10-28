#include "ServiceProbeModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "ServerConstants.hpp"
#include "StageWork.hpp"
#include "TickDatagrams.hpp"

ServiceProbeModule::ServiceProbeModule(std::string name,
                                       std::vector<std::string> dependencies,
                                       rtype::engine::StageWork work)
    : name_(std::move(name)),
      dependencies_(std::move(dependencies)),
      work_(std::move(work)) {}

std::string ServiceProbeModule::name() const { return name_; }

std::vector<std::string> ServiceProbeModule::dependencies() const {
  return dependencies_;
}

void ServiceProbeModule::install(rtype::engine::InstallContext& context) {
  sender_ = context.find<rtype::server::IDatagramSender>();
  admission_ = context.find<rtype::server::IPlayerAdmission>();
  tickDatagrams_ = context.find<rtype::server::TickDatagrams>();
  context.addWork(rtype::server::SIMULATION_STAGE, work_);
}

void ServiceProbeModule::start() {}

void ServiceProbeModule::stop() {}

rtype::server::IDatagramSender* ServiceProbeModule::sender() const {
  return sender_;
}

rtype::server::IPlayerAdmission* ServiceProbeModule::admission() const {
  return admission_;
}

rtype::server::TickDatagrams* ServiceProbeModule::tickDatagrams() const {
  return tickDatagrams_;
}
