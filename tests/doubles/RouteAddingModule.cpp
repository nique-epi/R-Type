#include "RouteAddingModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "HttpConstants.hpp"
#include "InstallContext.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "MasterRoute.hpp"
#include "RouteMethod.hpp"

RouteAddingModule::RouteAddingModule(std::string path)
    : path_(std::move(path)) {}

std::string RouteAddingModule::name() const { return path_; }

std::vector<std::string> RouteAddingModule::dependencies() const {
  return {std::string{rtype::master::HTTP_MODULE_NAME}};
}

void RouteAddingModule::install(rtype::engine::InstallContext& context) {
  context.require<rtype::master::MasterHttpServer>().add(
      rtype::network::MasterRoute{.method = rtype::network::RouteMethod::Get,
                                  .path = path_},
      []([[maybe_unused]] const rtype::master::ApiRequest& request) {
        return rtype::master::ApiResponse{.status = rtype::master::OK_STATUS,
                                          .body = {}};
      });
}

void RouteAddingModule::start() {}

void RouteAddingModule::stop() {}
