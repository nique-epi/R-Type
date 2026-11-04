#include "ServersController.hpp"
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"
#include "RegisteredServer.hpp"
#include "ServerKeys.hpp"
#include "ServerRecord.hpp"
#include "ServerRegistration.hpp"
#include "ServersConstants.hpp"

namespace rtype::master {

namespace {

std::chrono::sys_seconds now() {
  return std::chrono::floor<std::chrono::seconds>(
      std::chrono::system_clock::now());
}

ApiResponse refusal(int status, const std::string& detail) {
  return ApiResponse{
      .status = status, .body = {}, .detail = detail, .header = std::nullopt};
}

ApiResponse unauthorizedResponse() {
  return ApiResponse{
      .status = UNAUTHORIZED_STATUS,
      .body = {},
      .detail = std::string{UNKNOWN_KEY_DETAIL},
      .header = HttpHeader{.name = std::string{WWW_AUTHENTICATE_HEADER},
                           .value = std::string{BEARER_SCHEME}}};
}

/** @returns The registration @p request sends, its address always known. */
network::ServerRegistration registrationOf(const ApiRequest& request) {
  network::ServerRegistration registration =
      network::registrationFromText(request.body);
  if (!registration.address.has_value()) {
    registration.address = request.remoteAddress;
  }
  return registration;
}

ApiResponse registeredResponse(
    const RegisteredServer& saved,
    const network::ServerRegistration& registration) {
  nlohmann::json server =
      nlohmann::json::parse(network::registrationText(registration));
  server[SERVER_ID_MEMBER] = saved.id;
  if (!saved.created) {
    return ApiResponse{.status = OK_STATUS,
                       .body = server.dump(),
                       .detail = std::nullopt,
                       .header = std::nullopt};
  }
  return ApiResponse{
      .status = CREATED_STATUS,
      .body = server.dump(),
      .detail = std::nullopt,
      .header = HttpHeader{.name = std::string{LOCATION_HEADER},
                           .value = std::string{SERVER_LOCATION_PREFIX} +
                                    std::to_string(saved.id)}};
}

ApiResponse heartbeatAnswer() {
  nlohmann::json answer;
  answer[network::COMMANDS_MEMBER] = nlohmann::json::array();
  return ApiResponse{.status = OK_STATUS,
                     .body = answer.dump(),
                     .detail = std::nullopt,
                     .header = std::nullopt};
}

}  // namespace

ServersController::ServersController(ServerKeys& keys, MasterDatabase& database)
    : keys_(keys), service_(database) {}

ApiResponse ServersController::registration(const ApiRequest& request) {
  const std::optional<std::int64_t> keyId =
      keys_.keyIdOf(request.bearerToken.value_or(std::string{}));
  if (!keyId.has_value()) {
    return unauthorizedResponse();
  }
  try {
    const network::ServerRegistration registration = registrationOf(request);
    return registeredResponse(
        service_.registerServer(ServerRecord{.keyId = *keyId,
                                             .registration = registration,
                                             .registeredAt = now()}),
        registration);
  } catch (const network::InvalidMasterMessageException& invalid) {
    return refusal(BAD_REQUEST_STATUS, invalid.what());
  }
}

ApiResponse ServersController::heartbeat(const ApiRequest& request) {
  const std::optional<std::int64_t> keyId =
      keys_.keyIdOf(request.bearerToken.value_or(std::string{}));
  if (!keyId.has_value()) {
    return unauthorizedResponse();
  }
  try {
    if (!service_.recordHeartbeat(
            *keyId, network::heartbeatFromText(request.body), now())) {
      return refusal(NOT_FOUND_STATUS, std::string{UNREGISTERED_SERVER_DETAIL});
    }
    return heartbeatAnswer();
  } catch (const network::InvalidMasterMessageException& invalid) {
    return refusal(BAD_REQUEST_STATUS, invalid.what());
  }
}

}  // namespace rtype::master
