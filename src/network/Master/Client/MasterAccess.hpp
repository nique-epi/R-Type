#pragma once

#include <string>

namespace rtype::network {

/** @brief How a game server reaches the master, and with which key. */
struct MasterAccess {
  /**
   * @brief Where the master answers, as cpp-httplib reads it:
   * `http(s)://<host>[:<port>]`.
   */
  std::string url;
  /**
   * @brief The key of the server, `master_key` of server.cfg, sent as Bearer;
   * empty for a call that needs none, such as the creation of a server.
   */
  std::string key;
};

}  // namespace rtype::network
