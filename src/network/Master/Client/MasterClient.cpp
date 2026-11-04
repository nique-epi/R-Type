#include "MasterClient.hpp"
#include <httplib.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "CreatedServer.hpp"
#include "MasterAccess.hpp"
#include "MasterClientConstants.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRoute.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "NetworkException.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerRegistration.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::network {

namespace {

/** @returns The request of @p route carrying @p body in JSON. */
httplib::Request requestOf(MasterRoute route, const std::string& body) {
  httplib::Request request;
  request.method = std::string{methodNameOf(route.method)};
  request.path = apiPathOf(route);
  request.body = body;
  request.set_header(std::string{CONTENT_TYPE_HEADER},
                     std::string{JSON_CONTENT_TYPE});
  return request;
}

/** @brief Sends @p key as Bearer with @p request, unless it is empty. */
void authorize(httplib::Request& request, const std::string& key) {
  if (!key.empty()) {
    request.set_header(std::string{AUTHORIZATION_HEADER},
                       std::string{BEARER_SCHEME} +
                           std::string{BEARER_SCHEME_SEPARATOR} + key);
  }
}

/**
 * @returns A client of the master at @p url.
 * @throws InvalidMasterUrlException for a url cpp-httplib refuses.
 */
std::unique_ptr<httplib::Client> clientOf(const std::string& url) {
  try {
    return std::make_unique<httplib::Client>(url);
  } catch (const std::invalid_argument&) {
    throw InvalidMasterUrlException(url);
  }
}

}  // namespace

MasterClient::MasterClient(MasterAccess access)
    : key_(std::move(access.key)), client_(clientOf(access.url)) {
  client_->set_keep_alive(true);
  client_->set_connection_timeout(MASTER_CONNECTION_TIMEOUT);
  client_->set_read_timeout(MASTER_TRANSFER_TIMEOUT);
  client_->set_write_timeout(MASTER_TRANSFER_TIMEOUT);
}

MasterClient::~MasterClient() = default;

CreatedServer MasterClient::createServer(const ServerCreation& creation) {
  return createdServerFromText(
      answerOf(CREATE_SERVER_ROUTE, serverCreationText(creation)));
}

void MasterClient::registerServer(const ServerRegistration& registration) {
  static_cast<void>(
      answerOf(REPLACE_OWN_SERVER_ROUTE, registrationText(registration)));
}

void MasterClient::sendHeartbeat(const ServerHeartbeat& heartbeat) {
  static_cast<void>(
      answerOf(REPLACE_OWN_HEARTBEAT_ROUTE, heartbeatText(heartbeat)));
}

std::string MasterClient::answerOf(MasterRoute route, const std::string& body) {
  httplib::Request request = requestOf(route, body);
  authorize(request, key_);
  const httplib::Result result = client_->send(request);
  if (!result) {
    throw MasterUnreachableException(httplib::to_string(result.error()));
  }
  if (result->status >= LOWEST_ERROR_STATUS) {
    throw MasterRefusedException(result->status, problemDetailOf(result->body));
  }
  return result->body;
}

}  // namespace rtype::network
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
