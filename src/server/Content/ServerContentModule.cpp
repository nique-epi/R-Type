#include "ServerContentModule.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "ApiRequest.hpp"
#include "AssetFolder.hpp"
#include "ContentFile.hpp"
#include "ContentFiles.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentPack.hpp"
#include "ContentRouteConstants.hpp"
#include "Endpoint.hpp"
#include "IListeningPort.hpp"
#include "InstallContext.hpp"
#include "MasterRoutePath.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

ServerContentModule::ServerContentModule()
    : logger_(std::string{SERVER_LOGGER_NAME}),
      server_(std::string{SERVER_LOGGER_NAME},
              [this](const std::string& reason) {
                logger_.error("the content pack is no longer served: ", reason);
              }) {}

std::string ServerContentModule::name() const {
  return std::string{CONTENT_MODULE_NAME};
}

std::vector<std::string> ServerContentModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{NETWORK_MODULE_NAME}};
}

void ServerContentModule::install(engine::InstallContext& context) {
  port_ = &context.require<IListeningPort>();
  const std::vector<identifiers::ContentFile> files =
      game::contentFilesIn(engine::locateAssetFolder());
  desk_.emplace(network::encodeContentPack(files), clock_);
  server_.add(network::CONTENT_PACK_ROUTE.method,
              network::apiPathOf(network::CONTENT_PACK_ROUTE),
              [this](const network::ApiRequest& request) {
                return desk_->answer(request);
              });
}

void ServerContentModule::start() {
  if (!desk_.has_value()) {
    return;
  }
  const std::uint16_t port = port_->listeningPort();
  if (port == network::ANY_PORT) {
    logger_.warn(
        "the port is chosen by the system: the content pack is not "
        "served");
    return;
  }
  try {
    static_cast<void>(server_.open(network::Endpoint{
        .address = std::string{network::ANY_IPV4_ADDRESS}, .port = port}));
  } catch (const network::HttpPortUnavailableException& refused) {
    logger_.warn(refused.what(), ": the content pack is not served");
    return;
  }
  logger_.info("content pack ", network::fingerprintText(desk_->fingerprint()),
               " (", desk_->size(), " bytes) served on TCP port ", port);
}

void ServerContentModule::stop() { server_.close(); }

}  // namespace rtype::server
