#pragma once

#include <cstdint>
#include "ApiHandler.hpp"
#include "Endpoint.hpp"
#include "FailureReport.hpp"
#include "HttpServer.hpp"
#include "MasterRoute.hpp"

namespace rtype::master {

/**
 * @brief The HTTP server of the master: the routes of its API, under
 * network::MASTER_API_PREFIX, answered in JSON by a network::HttpServer, which
 * says what an error, an unknown path or a body too large gets.
 */
class MasterHttpServer {
 public:
  /**
   * @param onFailure Told when the server stops listening without close()
   * asking for it: the master can no longer be reached.
   */
  explicit MasterHttpServer(FailureReport onFailure);

  /**
   * @brief Answers the method of @p route on network::MASTER_API_PREFIX and
   * its path with @p handler. Call it before open(): the threads open() starts
   * read the routes.
   */
  void add(network::MasterRoute route, network::ApiHandler handler);

  /**
   * @brief Binds @p endpoint and listens until close(), as
   * network::HttpServer::open() does.
   * @returns The port bound.
   * @throws network::HttpPortUnavailableException when the address or the
   * port cannot be bound; nothing stays open then.
   */
  std::uint16_t open(const network::Endpoint& endpoint);

  /**
   * @brief Stops listening, waits for the requests in progress, and frees the
   * port; does nothing when the server is not open.
   */
  void close();

 private:
  network::HttpServer server_;
};

}  // namespace rtype::master
