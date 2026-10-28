#include "ServerJournalModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "ModuleConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

ServerJournalModule::ServerJournalModule(logging::LogLaunchOptions options)
    : options_(std::move(options)), logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerJournalModule::name() const {
  return std::string{JOURNAL_MODULE_NAME};
}

std::vector<std::string> ServerJournalModule::dependencies() const {
  return {};
}

void ServerJournalModule::install(
    [[maybe_unused]] engine::InstallContext& context) {}

void ServerJournalModule::start() {
  options_.apply();
  logger_.info("server starting");
}

void ServerJournalModule::stop() { logger_.info("server stopped"); }

}  // namespace rtype::server
