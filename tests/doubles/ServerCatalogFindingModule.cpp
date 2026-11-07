#include "ServerCatalogFindingModule.hpp"
#include <string>
#include <vector>
#include "IServerCatalog.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ServerCatalogFindingModuleConstants.hpp"

ServerCatalogFindingModule::ServerCatalogFindingModule(
    rtype::client::IServerCatalog*& found)
    : found_(&found) {}

std::string ServerCatalogFindingModule::name() const {
  return std::string{SERVER_CATALOG_FINDING_MODULE_NAME};
}

std::vector<std::string> ServerCatalogFindingModule::dependencies() const {
  return {std::string{rtype::client::CONNECTION_MODULE_NAME}};
}

void ServerCatalogFindingModule::install(
    rtype::engine::InstallContext& context) {
  *found_ = &context.require<rtype::client::IServerCatalog>();
}

void ServerCatalogFindingModule::start() {}

void ServerCatalogFindingModule::stop() {}
