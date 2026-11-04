#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "MasterDatabase.hpp"
#include "ServerKeys.hpp"
#include "ServersService.hpp"

namespace rtype::master {

/**
 * @brief The routes of a game server: what reads its request and writes its
 * answer. The server is named by its key, never by a number in the path, so
 * no key reaches another server. Every refusal is a problem document: 401
 * without a valid key (with `WWW-Authenticate: Bearer`), 400 naming the
 * faulty member, 404 for a heartbeat of a key that registered no server.
 */
class ServersController {
 public:
  ServersController(ServerKeys& keys, MasterDatabase& database);

  /**
   * @returns The answer of `PUT /api/servers/me`: 201 with `Location` the
   * first time a key registers, 200 after; both carry the server, its id and
   * its address included. A registration without an address gets the address
   * of the TCP peer.
   */
  [[nodiscard]] ApiResponse registration(const ApiRequest& request);

  /**
   * @returns The answer of `PUT /api/servers/me/heartbeat`: 200 and the
   * orders of the admins waiting for the server, an empty list until there are
   * orders.
   */
  [[nodiscard]] ApiResponse heartbeat(const ApiRequest& request);

 private:
  ServerKeys& keys_;
  ServersService service_;
};

}  // namespace rtype::master
