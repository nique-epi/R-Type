#include "MasterLoggerModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "MasterConstants.hpp"
#include "MasterModuleConstants.hpp"

namespace rtype::master {

MasterLoggerModule::MasterLoggerModule(logging::LogLaunchOptions options)
    : options_(std::move(options)), logger_(std::string{MASTER_LOGGER_NAME}) {}

std::string MasterLoggerModule::name() const {
  return std::string{LOGGER_MODULE_NAME};
}

std::vector<std::string> MasterLoggerModule::dependencies() const { return {}; }

void MasterLoggerModule::install(
    [[maybe_unused]] engine::InstallContext& context) {}

void MasterLoggerModule::start() {
  options_.apply();
  logger_.info("master starting, version ", MASTER_VERSION);
}

void MasterLoggerModule::stop() { logger_.info("master stopped"); }

}  // namespace rtype::master
