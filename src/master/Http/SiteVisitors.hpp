#pragma once

#include <string>
#include "ApiRequest.hpp"
#include "SettingValue.hpp"

namespace rtype::master {

/**
 * @brief Who a request is counted under in a quota: the visitor the web site
 * names, or the client the request came from.
 *
 * The site calls the master from its own server, so every visitor would share
 * the quota of its address. It sends SITE_KEY_HEADER, the secret both hold,
 * and SITE_CLIENT_ADDRESS_HEADER, the address of its visitor; the master
 * believes the second only with the first (visitorAddressOf()).
 */
class SiteVisitors {
 public:
  /**
   * @param siteKey The `site-key` setting, read at each request and never
   * written anywhere; it must outlive this object.
   */
  explicit SiteVisitors(const engine::SettingValue& siteKey);

  /**
   * @returns The address of the visitor @p request names with the site key,
   * or its network::ApiRequest::remoteAddress: a wrong or missing key is no
   * error.
   */
  [[nodiscard]] std::string addressOf(const network::ApiRequest& request) const;

 private:
  const engine::SettingValue& siteKey_;
};

}  // namespace rtype::master
