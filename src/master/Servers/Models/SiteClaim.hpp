#pragma once

#include <string>

namespace rtype::master {

/**
 * @brief What a request says to come from the web site: the key the site
 * names itself by, and the visitor it acts for. Both are whatever the client
 * wrote, empty without their header: nothing here is checked.
 */
struct SiteClaim {
  /** @brief The value of the SITE_KEY_HEADER. */
  std::string key;
  /** @brief The value of the SITE_CLIENT_ADDRESS_HEADER. */
  std::string clientAddress;
};

}  // namespace rtype::master
