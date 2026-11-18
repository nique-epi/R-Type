#pragma once

#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "DeviceLinksService.hpp"
#include "ISessionAccounts.hpp"
#include "SiteVisitors.hpp"

namespace rtype::master {

/**
 * @brief The routes of the device links: what reads their request and writes
 * their answer. Each route is network::MasterRoute::httpsOnly. The game names
 * its link by its secret, sent as `Authorization: Bearer rtd_…`, never in a
 * path; a player names it by its short code, signed in with its session.
 * Every refusal is a problem document.
 */
class DeviceLinksController {
 public:
  /**
   * @param links Applies the rules of the links; it must outlive the
   * controller.
   * @param accounts Names the account of a session; it must outlive the
   * controller.
   * @param siteVisitors Who a creation is counted under; it must outlive the
   * controller.
   */
  DeviceLinksController(DeviceLinksService& links, ISessionAccounts& accounts,
                        const SiteVisitors& siteVisitors);

  /** @returns The answer of `POST /api/device-links`: 201, `Location` naming
   * the link of the secret, and the link; 429 over the quota. */
  [[nodiscard]] network::ApiResponse createDeviceLink(
      const network::ApiRequest& request);

  /** @returns The answer of `GET /api/device-links/me`: 200 and the state of
   * the link; 401 for no link of that secret; 429 with `Retry-After` for a
   * read before the interval. */
  [[nodiscard]] network::ApiResponse readOwnDeviceLink(
      const network::ApiRequest& request);

  /** @returns The answer of `DELETE /api/device-links/me`: 204 whether a link
   * had the secret or not, 401 without a secret. */
  [[nodiscard]] network::ApiResponse deleteOwnDeviceLink(
      const network::ApiRequest& request);

  /** @returns The answer of `GET /api/device-links/{code}`: 200 and the
   * state of the link; 401 without a session; 404 for no link of that code.
   */
  [[nodiscard]] network::ApiResponse readDeviceLink(
      const network::ApiRequest& request);

  /** @returns The answer of `PUT /api/device-links/{code}/decision`: 200 and
   * the state of the link; 400 for a body that is no decision; 401 without a
   * session; 404 for no link of that code; 409 when it was decided otherwise
   * or expired. */
  [[nodiscard]] network::ApiResponse replaceDeviceLinkDecision(
      const network::ApiRequest& request);

 private:
  DeviceLinksService& links_;
  ISessionAccounts& accounts_;
  const SiteVisitors& siteVisitors_;
};

}  // namespace rtype::master
