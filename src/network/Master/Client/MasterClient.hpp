#pragma once

#include <memory>
#include <string>
#include "CreatedServer.hpp"
#include "MasterAccess.hpp"
#include "MasterRoute.hpp"
#include "ServerCreation.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerRegistration.hpp"

namespace httplib {
class Client;
}  // namespace httplib

namespace rtype::network {

/**
 * @brief What a game server calls to talk to the master: one function per
 * route, each sending the message of the route in JSON and reading its answer.
 *
 * Every call waits for the answer, within MASTER_CONNECTION_TIMEOUT and
 * MASTER_TRANSFER_TIMEOUT, so it is made away from the game loop. One call at
 * a time: two threads never call the same client together.
 *
 * Every call throws MasterRefusedException when the master answers an error
 * status (a key it does not know, a heartbeat before the registration, a
 * message it refuses), and MasterUnreachableException when no answer comes.
 */
class MasterClient {
 public:
  /**
   * @throws InvalidMasterUrlException when the url of @p access is not
   * `http://<host>[:<port>]`: an `https://` url is refused while cpp-httplib
   * is built without TLS.
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

 private:
  /** @returns The body the master answered to @p body sent on @p route. */
  [[nodiscard]] std::string answerOf(MasterRoute route,
                                     const std::string& body);

  MasterAccess access_;
  std::unique_ptr<httplib::Client> client_;
};

}  // namespace rtype::network
