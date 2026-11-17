#include "SiteVisitors.hpp"
#include <string>
#include <string_view>
#include "ApiRequest.hpp"
#include "HttpConstants.hpp"
#include "SettingValue.hpp"
#include "SiteClaim.hpp"
#include "SiteVisitorAddress.hpp"

namespace rtype::master {

namespace {

/** @returns The value of the header @p name of @p request, empty without
 * that header. */
std::string headerOf(const network::ApiRequest& request,
                     std::string_view name) {
  const auto header = request.headers.find(std::string{name});
  return header == request.headers.end() ? std::string{} : header->second;
}

/** @returns What @p request says of the web site and its visitor. */
SiteClaim siteClaimOf(const network::ApiRequest& request) {
  return SiteClaim{
      .key = headerOf(request, SITE_KEY_HEADER),
      .clientAddress = headerOf(request, SITE_CLIENT_ADDRESS_HEADER)};
}

}  // namespace

SiteVisitors::SiteVisitors(const engine::SettingValue& siteKey)
    : siteKey_(siteKey) {}

std::string SiteVisitors::addressOf(const network::ApiRequest& request) const {
  return visitorAddressOf(siteClaimOf(request), siteKey_.text())
      .value_or(request.remoteAddress);
}

}  // namespace rtype::master
