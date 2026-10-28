#include "LoggingModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

LoggingModule::LoggingModule(logging::LogLaunchOptions options)
    : options_(std::move(options)), logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string LoggingModule::name() const {
  return std::string{JOURNAL_MODULE_NAME};
}

std::vector<std::string> LoggingModule::dependencies() const { return {}; }

void LoggingModule::install([[maybe_unused]] engine::InstallContext& context) {}

void LoggingModule::start() {
  options_.apply();
  logger_.info("server starting");
}

void LoggingModule::stop() {}

}  // namespace rtype::server
