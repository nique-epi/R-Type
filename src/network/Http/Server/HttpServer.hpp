#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include "ApiHandler.hpp"
#include "Endpoint.hpp"
#include "ListeningLoss.hpp"
#include "Logger.hpp"
#include "RouteMethod.hpp"

namespace httplib {
class Server;
}  // namespace httplib

namespace rtype::network {

/**
 * @brief An HTTP server answering its routes on the threads of cpp-httplib,
 * for the master and the supervisor.
 *
 * Every route is added before open(). open() binds the address and the port,
 * then listens on a thread of its own until close(); close() stops it, lets
 * the requests in progress finish and frees the port. An answer of an error
 * status (400 or more) without a body gets a problem document of RFC 9457
 * (PROBLEM_CONTENT_TYPE), with the ApiResponse::detail of the route, so every
 * error is JSON: an unknown path gets 404, and a body over
 * MAX_REQUEST_BODY_BYTES sent to a route gets 413. A route that throws gets
 * 500, and only the journal reads why. A success to a request for a range of
 * bytes (a resumed download) gets 206 and those bytes only. No other socket
 * may listen on the port while the server holds it.
 */
class HttpServer {
 public:
  /**
   * @param loggerName The logger the journal lines of the server go to.
   * @param onLoss Told when the server stops listening without close() asking
   * for it.
   */
  HttpServer(const std::string& loggerName, ListeningLoss onLoss);

  /** @brief Stops listening, so the thread of open() ends; never throws. */
  ~HttpServer();

  HttpServer(const HttpServer&) = delete;
  HttpServer& operator=(const HttpServer&) = delete;
  HttpServer(HttpServer&&) = delete;
  HttpServer& operator=(HttpServer&&) = delete;

  /**
   * @brief Answers @p method on @p path with @p handler. Call it before
   * open(): the threads open() starts read the routes.
   */
  void add(RouteMethod method, const std::string& path, ApiHandler handler);

  /**
   * @brief Binds @p endpoint, an IPv4 address of the machine or
   * ANY_IPV4_ADDRESS and a TCP port, then listens on a thread of its own until
   * close().
   * @returns The port bound: the port of @p endpoint, or the one the system
   * chose for ANY_PORT. It returns once the server accepts connections.
   * @throws HttpPortUnavailableException when the address or the port cannot
   * be bound; nothing stays open then.
   */
  std::uint16_t open(const Endpoint& endpoint);

  /**
   * @brief Stops listening, waits for the requests in progress, and frees the
   * port; does nothing when the server is not open.
   */
  void close();

 private:
  /**
   * @brief Listens on the socket open() bound until close(); a stop close()
   * did not ask for, or an exception, is told to the ListeningLoss.
   */
  void listen();

  std::unique_ptr<httplib::Server> server_;
  ListeningLoss onLoss_;
  logging::Logger logger_;
  std::jthread listener_;
};

}  // namespace rtype::network
