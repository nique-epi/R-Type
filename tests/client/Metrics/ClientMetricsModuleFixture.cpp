#include "ClientMetricsModuleFixture.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include "ClientConstants.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficFileTestConstants.hpp"

ClientMetricsModuleFixture::ClientMetricsModuleFixture()
    : stages_(rtype::client::CLIENT_LOOP_STAGES) {
  services_.provide(traffic_);
}

rtype::engine::InstallContext ClientMetricsModuleFixture::context() {
  return {std::string{rtype::client::METRICS_MODULE_NAME}, services_, stages_};
}

rtype::engine::InstallContext
ClientMetricsModuleFixture::contextWithoutServices() {
  return {std::string{rtype::client::METRICS_MODULE_NAME}, noServices_,
          stages_};
}

void ClientMetricsModuleFixture::runAFrameASecondLater() {
  clock_->advance(rtype::engine::testing::ONE_SECOND);
  stages_.run(rtype::engine::Duration::zero());
}

rtype::network::TrafficCounters& ClientMetricsModuleFixture::traffic() {
  return traffic_;
}

const std::shared_ptr<SimulatedClock>& ClientMetricsModuleFixture::clock()
    const {
  return clock_;
}

std::filesystem::path ClientMetricsModuleFixture::trafficFile() const {
  return folder_.path() / TRAFFIC_FILE_NAME;
}

const std::filesystem::path& ClientMetricsModuleFixture::folder() const {
  return folder_.path();
}
