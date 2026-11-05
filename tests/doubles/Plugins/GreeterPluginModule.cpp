#include "GreeterPluginModule.hpp"
#include <string>
#include <vector>
#include "ILogJournal.hpp"
#include "InstallContext.hpp"
#include "TestPluginConstants.hpp"

std::string GreeterPluginModule::name() const {
  return std::string{GREETER_MODULE_NAME};
}

std::vector<std::string> GreeterPluginModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME_IN_TESTS}};
}

void GreeterPluginModule::install(rtype::engine::InstallContext& context) {
  logger_.emplace(std::string{GREETER_LOGGER_NAME},
                  context.require<rtype::logging::ILogJournal>());
}

void GreeterPluginModule::start() {
  if (logger_.has_value()) {
    logger_->info(GREETER_STARTED_TEXT);
  }
}

void GreeterPluginModule::stop() {
  if (logger_.has_value()) {
    logger_->info(GREETER_STOPPED_TEXT);
  }
}
