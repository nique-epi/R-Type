#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "IProbeSender.hpp"
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
  /**
   * @param service Applies the rules of the servers; it must outlive the
   * controller.
   * @param probeSender Where the probes of the accepted heartbeats go; it
   * must outlive the controller.
   */
  ServersController(ServersService& service, IProbeSender& probeSender);

  /**
   * @returns The answer of `POST /api/servers`: 201, `Location` naming the
   * server, and the server with its key, shown this once.
   */
  [[nodiscard]] network::ApiResponse createServer(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `PUT /api/servers/me`: 200 and the server, its id
   * and its address included. A registration without an address gets the
   * address of the TCP peer.
   */
  [[nodiscard]] network::ApiResponse replaceOwnServer(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `PUT /api/servers/me/heartbeat`: 200 and the
   * orders of the admins waiting for the server, an empty list until there are
   * orders; 409 for a server that registered before the probes, which must
   * register again. An accepted heartbeat makes the master send its server a
   * probe (ServersService::probeFor()).
   */
  [[nodiscard]] network::ApiResponse replaceOwnHeartbeat(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `GET /api/servers/{id}/games`: 200 and the games of
   * the server, open to everyone; 404 for an id that names no server, or that
   * is no number.
   */
  [[nodiscard]] network::ApiResponse listGames(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `GET /api/servers`: 200 and a page of the
   * registered servers by id, open to everyone; 400 when limit or offset is no
   * whole number within its bounds.
   */
  [[nodiscard]] network::ApiResponse listServers(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `GET /api/servers/{id}/samples`: 200 and a page of
   * the samples of the server, newest first, open to everyone; 404 for an id
   * that names no server, 400 for a page out of its bounds.
   */
  [[nodiscard]] network::ApiResponse listSamples(
      const network::ApiRequest& request);

  /**
   * @returns The answer of `GET /api/servers/{id}/statuses`: 200 and a
   * page of the statuses the server held, newest first, open to
   * everyone; 404 and 400 as listSamples().
   */
  [[nodiscard]] network::ApiResponse listStatuses(
      const network::ApiRequest& request);

 private:
  ServersService& service_;
  IProbeSender& probeSender_;
};

}  // namespace rtype::master
