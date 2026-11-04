#include "MasterHttpServer.hpp"
#include <httplib.h>
#if !defined(_WIN32)
#include <sys/socket.h>
#endif
#include <cstdint>
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include "ApiHandler.hpp"
#include "ApiResponse.hpp"
#include "FailureReport.hpp"
#include "HttpConstants.hpp"
#include "HttpEndpoint.hpp"
#include "MasterConstants.hpp"
#include "MasterException.hpp"
#include "NetworkConstants.hpp"
#include "ProblemDetails.hpp"

namespace rtype::master {

namespace {

/**
 * @brief Makes @p socket the only one that may listen on its address and
 * port: cpp-httplib sets SO_REUSEPORT by default, which lets a second master
 * bind the same port with no error. SO_REUSEADDR still lets a master restart
 * at once while the connections of the previous one linger; on Windows,
 * SO_EXCLUSIVEADDRUSE refuses any other socket on the port. When the system
 * refuses the option, the socket keeps none, which shares the port with
 * nobody either.
 */
void claimPortAlone(socket_t socket) {
  const int enabled = 1;
#if defined(_WIN32)
  setsockopt(socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
             reinterpret_cast<const char*>(&enabled), sizeof(enabled));
#else
  setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));
#endif
}

/**
 * @returns The port @p server bound for @p endpoint, or NO_PORT_BOUND when the
 * system refused the address or the port.
 */
int boundPortOf(httplib::Server& server, const HttpEndpoint& endpoint) {
  if (endpoint.port == network::ANY_PORT) {
    return server.bind_to_any_port(endpoint.address);
  }
  return server.bind_to_port(endpoint.address, endpoint.port) ? endpoint.port
                                                              : NO_PORT_BOUND;
}

/**
 * @brief Gives an error answer that has no body the problem document of its
 * status, so a client always reads JSON.
 */
void writeProblemDocument(httplib::Response& response) {
  if (response.body.empty()) {
    response.set_content(
        problemDetailsText(response.status,
                           httplib::status_message(response.status)),
        std::string{PROBLEM_CONTENT_TYPE});
  }
}

/** @returns What @p error says, for the journal. */
std::string reasonOf(const std::exception_ptr& error) {
  try {
    std::rethrow_exception(error);
  } catch (const std::exception& thrown) {
    return thrown.what();
  } catch (...) {
    return std::string{UNKNOWN_ERROR_TEXT};
  }
}

}  // namespace

MasterHttpServer::MasterHttpServer(FailureReport onFailure)
    : server_(std::make_unique<httplib::Server>()),
      onFailure_(std::move(onFailure)),
      logger_(std::string{MASTER_LOGGER_NAME}) {
  server_->set_socket_options(claimPortAlone);
  server_->set_payload_max_length(MAX_REQUEST_BODY_BYTES);
  server_->set_error_handler(
      []([[maybe_unused]] const httplib::Request& request,
         httplib::Response& response) { writeProblemDocument(response); });
  server_->set_exception_handler(
      [this]([[maybe_unused]] const httplib::Request& request,
             httplib::Response& response, const std::exception_ptr& error) {
        logger_.error("a route of the API failed: ", reasonOf(error));
        response.status = httplib::StatusCode::InternalServerError_500;
      });
}

MasterHttpServer::~MasterHttpServer() { server_->stop(); }

void MasterHttpServer::addGet(std::string_view path, ApiHandler route) {
  server_->Get(std::string{API_PATH_PREFIX} + std::string{path},
               [answer = std::move(route)](
                   [[maybe_unused]] const httplib::Request& request,
                   httplib::Response& response) {
                 const ApiResponse reply = answer();
                 response.status = reply.status;
                 response.set_content(reply.body,
                                      std::string{JSON_CONTENT_TYPE});
               });
}

std::uint16_t MasterHttpServer::open(const HttpEndpoint& endpoint) {
  std::promise<int> bound;
  std::future<int> boundPort = bound.get_future();
  listener_ =
      std::jthread{[this, endpoint, bound = std::move(bound)]() mutable {
        listen(endpoint, bound);
      }};
  const int port = boundPort.get();
  if (port == NO_PORT_BOUND) {
    listener_.join();
    throw HttpPortUnavailableException(endpoint.address, endpoint.port);
  }
  server_->wait_until_ready();
  return static_cast<std::uint16_t>(port);
}

void MasterHttpServer::close() {
  if (!listener_.joinable()) {
    return;
  }
  server_->stop();
  listener_.join();
}

void MasterHttpServer::listen(const HttpEndpoint& endpoint,
                              std::promise<int>& boundPort) {
  const int port = boundPortOf(*server_, endpoint);
  boundPort.set_value(port);
  if (port != NO_PORT_BOUND && !server_->listen_after_bind()) {
    onFailure_(std::string{LISTENING_LOST_TEXT});
  }
}

}  // namespace rtype::master
