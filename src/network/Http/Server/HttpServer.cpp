#include "HttpServer.hpp"
#include <httplib.h>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include "ApiHandler.hpp"
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "ClientAddress.hpp"
#include "Endpoint.hpp"
#include "ForwardedAddress.hpp"
#include "HttpServerConstants.hpp"
#include "HttpStatusConstants.hpp"
#include "ListeningLoss.hpp"
#include "MasterHttpConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PeerAddresses.hpp"
#include "RouteMethod.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks,clang-analyzer-unix.BlockInCriticalSection)
namespace rtype::network {

namespace {

/**
 * @brief Makes @p socket the only one that may listen on its address and
 * port: cpp-httplib sets SO_REUSEPORT by default, which lets a second server
 * bind the same port with no error. SO_REUSEADDR still lets a server restart
 * at once while the connections of the previous one linger; on Windows,
 * SO_EXCLUSIVEADDRUSE refuses any other socket on the port. When the system
 * refuses the option, the socket keeps none, which shares the port with
 * nobody either.
 */
void claimPortAlone(socket_t socket) {
#if defined(_WIN32)
  // NOLINTNEXTLINE(misc-include-cleaner)
  httplib::set_socket_opt(socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, 1);
#else
  // NOLINTNEXTLINE(misc-include-cleaner)
  httplib::set_socket_opt(socket, SOL_SOCKET, SO_REUSEADDR, 1);
#endif
}

/**
 * @returns The port @p server bound for @p endpoint, or NO_PORT_BOUND when the
 * system refused the address or the port.
 */
int boundPortOf(httplib::Server& server, const Endpoint& endpoint) {
  if (endpoint.port == ANY_PORT) {
    return server.bind_to_any_port(endpoint.address);
  }
  return server.bind_to_port(endpoint.address, endpoint.port) ? endpoint.port
                                                              : NO_PORT_BOUND;
}

/**
 * @returns The problem document of RFC 9457 for an error of @p status:
 * `{"status":404,"title":"Not Found"}`, with @p detail when the route gives
 * one. It names no type, so the type is `about:blank`.
 */
std::string problemDetailsText(int status,
                               const std::optional<std::string>& detail) {
  nlohmann::json problem;
  problem[PROBLEM_STATUS_MEMBER] = status;
  problem[PROBLEM_TITLE_MEMBER] = httplib::status_message(status);
  if (detail.has_value()) {
    problem[PROBLEM_DETAIL_MEMBER] = *detail;
  }
  return problem.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
}

/**
 * @brief Gives an error answer that has no body the problem document of its
 * status, so a client always reads JSON.
 */
void writeProblemDocument(httplib::Response& response) {
  if (response.body.empty()) {
    response.set_content(problemDetailsText(response.status, std::nullopt),
                         std::string{PROBLEM_CONTENT_TYPE});
  }
}

/**
 * @returns The token after BEARER_SCHEME and its separator in @p
 * authorization, the scheme read in any case, or nothing for another scheme or
 * an empty token.
 */
std::optional<std::string> bearerTokenIn(std::string_view authorization) {
  const std::string prefix =
      std::string{BEARER_SCHEME} + std::string{BEARER_SCHEME_SEPARATOR};
  const bool hasPrefix =
      authorization.size() > prefix.size() &&
      std::ranges::equal(
          authorization.substr(0, prefix.size()), prefix,
          [](char letter, char expected) {
            return std::tolower(static_cast<unsigned char>(letter)) ==
                   std::tolower(static_cast<unsigned char>(expected));
          });
  if (!hasPrefix) {
    return std::nullopt;
  }
  return std::string{authorization.substr(prefix.size())};
}

/**
 * @returns The first value of each parameter of the query of @p request:
 * cpp-httplib keeps them in a std::multimap in the order of the query, and
 * emplace() keeps the first value of a name.
 */
std::unordered_map<std::string, std::string> queryParametersOf(
    const httplib::Request& request) {
  std::unordered_map<std::string, std::string> parameters;
  for (const auto& [name, value] : request.params) {
    parameters.emplace(name, value);
  }
  return parameters;
}

ApiRequest apiRequestOf(const httplib::Request& request,
                        ForwardedAddress forwarded) {
  return ApiRequest{.body = request.body,
                    .bearerToken = bearerTokenIn(request.get_header_value(
                        std::string{AUTHORIZATION_HEADER})),
                    .remoteAddress = clientAddressOf(
                        PeerAddresses{.peer = request.remote_addr,
                                      .forwardedFor = request.get_header_value(
                                          std::string{FORWARDED_FOR_HEADER})},
                        forwarded),
                    .pathParameters = request.path_params,
                    .queryParameters = queryParametersOf(request)};
}

/**
 * @brief Writes @p reply into @p response: its header, then its body, or the
 * problem document of its status when it is an error with no body. A success
 * of OK_STATUS leaves the status to cpp-httplib, which answers a request for
 * a range of bytes with 206 and those bytes (RFC 9110, section 14), and the
 * others with 200.
 */
void writeReply(ApiResponse reply, httplib::Response& response) {
  if (reply.status != OK_STATUS) {
    response.status = reply.status;
  }
  if (reply.header.has_value()) {
    response.set_header(reply.header->name, reply.header->value);
  }
  if (reply.status >= LOWEST_ERROR_STATUS && reply.body.empty()) {
    response.set_content(problemDetailsText(reply.status, reply.detail),
                         std::string{PROBLEM_CONTENT_TYPE});
    return;
  }
  response.set_content(std::move(reply.body), std::string{reply.contentType});
}

httplib::Server::Handler handlerOf(ApiHandler route,
                                   ForwardedAddress forwarded) {
  return [answer = std::move(route), forwarded](const httplib::Request& request,
                                                httplib::Response& response) {
    writeReply(answer(apiRequestOf(request, forwarded)), response);
  };
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

HttpServer::HttpServer(const std::string& loggerName, ListeningLoss onLoss,
                       ForwardedAddress forwarded)
    : server_(std::make_unique<httplib::Server>()),
      onLoss_(std::move(onLoss)),
      forwarded_(forwarded),
      logger_(loggerName) {
  server_->set_socket_options(claimPortAlone);
  server_->set_payload_max_length(MAX_REQUEST_BODY_BYTES);
  server_->set_error_handler(
      []([[maybe_unused]] const httplib::Request& request,
         httplib::Response& response) { writeProblemDocument(response); });
  server_->set_exception_handler(
      [this]([[maybe_unused]] const httplib::Request& request,
             httplib::Response& response, const std::exception_ptr& error) {
        logger_.error("a route of the HTTP server failed: ", reasonOf(error));
        response.status = httplib::StatusCode::InternalServerError_500;
      });
}

HttpServer::~HttpServer() { server_->stop(); }

void HttpServer::add(RouteMethod method, const std::string& path,
                     ApiHandler handler) {
  switch (method) {
    case RouteMethod::Get:
      server_->Get(path, handlerOf(std::move(handler), forwarded_));
      return;
    case RouteMethod::Post:
      server_->Post(path, handlerOf(std::move(handler), forwarded_));
      return;
    case RouteMethod::Put:
      server_->Put(path, handlerOf(std::move(handler), forwarded_));
      return;
  }
}

std::uint16_t HttpServer::open(const Endpoint& endpoint) {
  const int port = boundPortOf(*server_, endpoint);
  if (port == NO_PORT_BOUND) {
    throw HttpPortUnavailableException(endpoint.address, endpoint.port);
  }
  listener_ = std::jthread{[this] { listen(); }};
  server_->wait_until_ready();
  return static_cast<std::uint16_t>(port);
}

void HttpServer::close() {
  if (!listener_.joinable()) {
    return;
  }
  server_->stop();
  listener_.join();
}

void HttpServer::listen() {
  try {
    if (!server_->listen_after_bind()) {
      onLoss_(std::string{LISTENING_LOST_TEXT});
    }
  } catch (const std::exception& error) {
    onLoss_(error.what());
  }
}

}  // namespace rtype::network
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks,clang-analyzer-unix.BlockInCriticalSection)
