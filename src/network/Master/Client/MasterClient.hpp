#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "GameListing.hpp"
#include "MasterAccess.hpp"
#include "MasterRoute.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerListing.hpp"
#include "ServerRegistration.hpp"

namespace httplib {
class Client;
struct Request;
}  // namespace httplib

namespace rtype::network {

/**
 * @brief What a game server calls to talk to the master: one function per
 * route, each sending the message of the route in JSON and reading its answer.
 *
 * Every call waits for the answer, within MASTER_CONNECTION_TIMEOUT and
 * MASTER_TRANSFER_TIMEOUT, so it is made away from the game loop. One call at
 * a time: two threads never call the same client together. The connection is
 * kept between calls, as long as the master keeps it open.
 *
 * Every call throws MasterRefusedException when the master answers an error
 * status (a key it does not know, a heartbeat before the registration, a
 * message it refuses), and MasterUnreachableException when no answer comes.
 */
class MasterClient {
 public:
  /**
   * @throws InvalidMasterUrlException when the url of @p access is not
   * `http(s)://<host>[:<port>]`, or its port is outside 1 to 65535. An
   * `https://` master is trusted only with a certificate the roots of the
   * system sign for its host.
   */
  explicit MasterClient(MasterAccess access);
  ~MasterClient();

  MasterClient(const MasterClient&) = delete;
  MasterClient& operator=(const MasterClient&) = delete;
  MasterClient(MasterClient&&) = delete;
  MasterClient& operator=(MasterClient&&) = delete;

  /**
   * @brief Creates a server, `POST /api/servers`, as the web site does for a
   * host; it needs no key.
   * @returns The server and its key, which the master never shows again.
   * @throws InvalidMasterMessageException when the answer is not a created
   * server.
   */
  [[nodiscard]] CreatedServer createServer(const ServerCreation& creation);

  /**
   * @brief Registers the server of the key, `PUT /api/servers/me`: what it
   * runs, where players reach it and how many it takes. A server registers
   * each time it starts.
   */
  void registerServer(const ServerRegistration& registration);

  /**
   * @brief Sends the state of the server of the key,
   * `PUT /api/servers/me/heartbeat`, once it registered.
   */
  void sendHeartbeat(const ServerHeartbeat& heartbeat);

  /**
   * @brief Sends @p game, which the server of the key drew @p gameId for,
   * `PUT /api/servers/me/results/{gameId}`. The master keeps a game once:
   * sending one it already keeps answers as the first time, so a result is
   * sent again safely.
   * @throws MasterRefusedException with status 400 for a game the master never
   * keeps: not vanilla, an id that is not GAME_ID_SIZE lowercase hexadecimal
   * digits, or a member out of its bounds.
   */
  void sendResult(const std::string& gameId, const FinishedGame& game);

  /**
   * @brief Reads the games the server @p serverId hosts,
   * `GET /api/servers/{id}/games`; it needs no key.
   * @throws MasterRefusedException with status 404 when no server has that id.
   * @throws InvalidMasterMessageException when the answer is no game listing.
   */
  [[nodiscard]] GameListing listGames(std::int64_t serverId);

  /**
   * @returns Every server the master lists, by id: it reads the pages of
   * `GET /api/servers`, MAXIMUM_PAGE_SIZE servers each, until one is not full,
   * MAXIMUM_LISTED_SERVER_PAGES pages at most; the route reads no key. It
   * blocks for as long as the pages take.
   * @throws MasterUnreachableException, MasterRefusedException as the other
   * calls do; InvalidMasterMessageException for a page it cannot read.
   */
  [[nodiscard]] ServerListing listServers();

 private:
  /** @returns The body the master answered to @p body sent on @p route. */
  [[nodiscard]] std::string answerOf(MasterRoute route,
                                     const std::string& body);

  /** @returns The body the master answered to @p request, which gets the key
   * of this client. */
  [[nodiscard]] std::string send(httplib::Request request);

  std::string key_;
  std::unique_ptr<httplib::Client> client_;
};

}  // namespace rtype::network
