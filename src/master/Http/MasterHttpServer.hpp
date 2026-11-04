#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <thread>
#include "ApiHandler.hpp"
#include "Endpoint.hpp"
#include "FailureReport.hpp"
#include "Logger.hpp"

namespace httplib {
class Server;
}  // namespace httplib

namespace rtype::master {

/**
 * @brief The HTTP server of the master: the routes of its API, under
 * API_PATH_PREFIX, answered in JSON on the threads of cpp-httplib.
 *
 * Every route is added before open(). open() binds the address and the port,
 * then listens on a thread of its own until close(); close() stops it, lets
 * the requests in progress finish and frees the port. An answer of
 * an error status (400 or more) without a body gets a problem document of RFC
 * 9457 (PROBLEM_CONTENT_TYPE), with the ApiResponse::detail of the route, so
 * every error is JSON: an unknown path gets
 * 404, and a body over MAX_REQUEST_BODY_BYTES sent to a route gets 413. A route
 * that throws gets 500, and only the journal reads why. No other socket may
 * listen on the port while the server holds it.
 */
class MasterHttpServer {
 public:
  /**
   * @param onFailure Told when the server stops listening without close()
   * asking for it: the master can no longer be reached.
   */
  explicit MasterHttpServer(FailureReport onFailure);

  /** @brief Stops listening, so the thread of open() ends; never throws. */
  ~MasterHttpServer();

  MasterHttpServer(const MasterHttpServer&) = delete;
  MasterHttpServer& operator=(const MasterHttpServer&) = delete;
  MasterHttpServer(MasterHttpServer&&) = delete;
  MasterHttpServer& operator=(MasterHttpServer&&) = delete;

  /**
   * @brief Answers GET API_PATH_PREFIX + @p path with @p route. Call it before
   * open(): the threads open() starts read the routes.
   */
  void addGet(std::string_view path, ApiHandler route);

  /** @brief Answers PUT API_PATH_PREFIX + @p path with @p route, as addGet().
   */
  void addPut(std::string_view path, ApiHandler route);

  /**
   * @brief Binds @p endpoint, an IPv4 address of the machine or
   * network::ANY_IPV4_ADDRESS and a TCP port, then listens on a thread of its
   * own until close().
   * @returns The port bound: the port of @p endpoint, or the one the system
   * chose for network::ANY_PORT. It returns once the server accepts
   * connections.
   * @throws HttpPortUnavailableException when the address or the port cannot
   * be bound; nothing stays open then.
   */
  std::uint16_t open(const network::Endpoint& endpoint);

  /**
   * @brief Stops listening, waits for the requests in progress, and frees the
   * port; does nothing when the server is not open.
   */
  void close();

 private:
  /**
   * @brief Listens on the socket open() bound until close(); a stop close()
   * did not ask for, or an exception, is told to the FailureReport.
   */
  void listen();

  std::unique_ptr<httplib::Server> server_;
  FailureReport onFailure_;
  logging::Logger logger_;
  std::jthread listener_;
};

}  // namespace rtype::master
