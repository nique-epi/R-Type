#include "ClientLoggerModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "ClientConstants.hpp"
#include "Endpoint.hpp"
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "ModuleConstants.hpp"

namespace rtype::client {

ClientLoggerModule::ClientLoggerModule(logging::LogLaunchOptions options,
                                       network::Endpoint server)
    : options_(std::move(options)),
      server_(std::move(server)),
      logger_(std::string{CLIENT_LOGGER_NAME}) {}

std::string ClientLoggerModule::name() const {
  return std::string{LOGGER_MODULE_NAME};
}

std::vector<std::string> ClientLoggerModule::dependencies() const { return {}; }

void ClientLoggerModule::install(
    [[maybe_unused]] engine::InstallContext& context) {}

void ClientLoggerModule::start() {
  options_.apply();
  logger_.info("client starting");
  logger_.info("server: ", server_.address, " port ", server_.port);
}

void ClientLoggerModule::stop() {}

}  // namespace rtype::client
