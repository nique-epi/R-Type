#include "ServerLoggerModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "ModuleConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

ServerLoggerModule::ServerLoggerModule(logging::LogLaunchOptions options)
    : options_(std::move(options)), logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerLoggerModule::name() const {
  return std::string{LOGGER_MODULE_NAME};
}

std::vector<std::string> ServerLoggerModule::dependencies() const { return {}; }

void ServerLoggerModule::install(
    [[maybe_unused]] engine::InstallContext& context) {}

void ServerLoggerModule::start() {
  options_.apply();
  logger_.info("server starting");
}

void ServerLoggerModule::stop() { logger_.info("server stopped"); }

}  // namespace rtype::server
