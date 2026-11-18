#include "MasterTransport.hpp"
#include <httplib.h>
#include <memory>
#include <stdexcept>
#include <string>
#include "MasterClientConstants.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::network {

namespace {

/** @brief Sends @p credential as Bearer with @p request, unless it is empty.
 */
void authorize(httplib::Request& request, const std::string& credential) {
  if (!credential.empty()) {
    request.set_header(std::string{AUTHORIZATION_HEADER},
                       std::string{BEARER_SCHEME} +
                           std::string{BEARER_SCHEME_SEPARATOR} + credential);
  }
}

/**
 * @returns A client of the master at @p url.
 * @throws InvalidMasterUrlException for a url cpp-httplib refuses: it throws
 * for a scheme it does not speak, and builds a client with nothing inside,
 * which is_valid() reports, for a port outside 1 to 65535.
 */
std::unique_ptr<httplib::Client> clientOf(const std::string& url) {
  std::unique_ptr<httplib::Client> client;
  try {
    client = std::make_unique<httplib::Client>(url);
  } catch (const std::invalid_argument&) {
    throw InvalidMasterUrlException(url);
  }
  if (!client->is_valid()) {
    throw InvalidMasterUrlException(url);
  }
  return client;
}

}  // namespace

MasterTransport::MasterTransport(const std::string& url)
    : client_(clientOf(url)) {
  client_->set_keep_alive(true);
  client_->set_connection_timeout(MASTER_CONNECTION_TIMEOUT);
  client_->set_read_timeout(MASTER_TRANSFER_TIMEOUT);
  client_->set_write_timeout(MASTER_TRANSFER_TIMEOUT);
}

MasterTransport::~MasterTransport() = default;

std::string MasterTransport::send(httplib::Request request,
                                  const std::string& credential) {
  authorize(request, credential);
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
