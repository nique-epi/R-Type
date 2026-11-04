#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "MasterDatabase.hpp"
#include "ServersService.hpp"

namespace rtype::master {

/**
 * @brief The routes of the game servers: what reads their request and writes
 * their answer. A server is named by its key, never by a number in the path,
 * so no key reaches another server. Every refusal is a problem document: 400
 * naming the faulty member, 401 without a valid key (with `WWW-Authenticate:
 * Bearer`), 409 for a heartbeat before the registration, 429 over the quota of
 * creations.
 */
class ServersController {
 public:
  explicit ServersController(MasterDatabase& database);

  /**
   * @returns The answer of `POST /api/servers`: 201, `Location` naming the
   * server, and the server with its key, shown this once.
   */
  [[nodiscard]] ApiResponse createServer(const ApiRequest& request);

  /**
   * @returns The answer of `PUT /api/servers/me`: 200 and the server, its id
   * and its address included. A registration without an address gets the
   * address of the TCP peer.
   */
  [[nodiscard]] ApiResponse replaceOwnServer(const ApiRequest& request);

  /**
   * @returns The answer of `PUT /api/servers/me/heartbeat`: 200 and the
   * orders of the admins waiting for the server, an empty list until there are
   * orders.
   */
  [[nodiscard]] ApiResponse replaceOwnHeartbeat(const ApiRequest& request);

 private:
  ServersService service_;
};

}  // namespace rtype::master
