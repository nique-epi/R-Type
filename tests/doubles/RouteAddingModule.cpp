#include "RouteAddingModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "ApiResponse.hpp"
#include "HttpConstants.hpp"
#include "InstallContext.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"

RouteAddingModule::RouteAddingModule(std::string path)
    : path_(std::move(path)) {}

std::string RouteAddingModule::name() const { return path_; }

std::vector<std::string> RouteAddingModule::dependencies() const {
  return {std::string{rtype::master::HTTP_MODULE_NAME}};
}

void RouteAddingModule::install(rtype::engine::InstallContext& context) {
  context.require<rtype::master::MasterHttpServer>().addGet(path_, [] {
    return rtype::master::ApiResponse{.status = rtype::master::OK_STATUS,
                                      .body = {}};
  });
}

void RouteAddingModule::start() {}

void RouteAddingModule::stop() {}
