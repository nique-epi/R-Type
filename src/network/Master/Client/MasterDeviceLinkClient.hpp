#pragma once

#include <string>
#include "CreatedDeviceLink.hpp"
#include "CreatedSession.hpp"
#include "DeviceLinkState.hpp"
#include "MasterTransport.hpp"

namespace rtype::network {

/**
 * @brief What the game client calls to sign in through the web site, the
 * OAuth 2.0 Device Authorization Grant of RFC 8628: one function per route of
 * the device links, the secret of a link sent as Bearer, never kept; and the
 * closing of a session.
 *
 * The routes answer over HTTPS only. Every call waits for the answer, as
 * MasterTransport does, so it is made away from the frame loop, and throws as
 * MasterTransport::send() does; InvalidMasterMessageException for an answer
 * that is not the message of its route. One call at a time.
 */
class MasterDeviceLinkClient {
 public:
  /** @throws InvalidMasterUrlException as MasterTransport does. */
  explicit MasterDeviceLinkClient(const std::string& url);

  /**
   * @returns A new link, its secret shown this once, `POST /api/device-links`.
   * @throws MasterRefusedException with status 429 when this address asked
   * for its quota of links in the last hour.
   */
  [[nodiscard]] CreatedDeviceLink createDeviceLink();

  /**
   * @returns The state of the link of @p secret, `GET /api/device-links/me`.
   * @throws MasterRefusedException with status 401 for a secret no link has,
   * 429 for a read before the interval.
   */
  [[nodiscard]] DeviceLinkState readOwnDeviceLink(const std::string& secret);

  /** @brief Forgets the link of @p secret, `DELETE /api/device-links/me`. */
  void deleteOwnDeviceLink(const std::string& secret);

  /**
   * @returns The session the approved link of @p secret is traded for,
   * `POST /api/sessions` with no body.
   * @throws MasterRefusedException with status 401 for a secret no link has,
   * 409 while it waits, 403 once refused, 410 once expired.
   */
  [[nodiscard]] CreatedSession createSessionOfDeviceLink(
      const std::string& secret);

  /** @brief Closes the session of @p token, `DELETE /api/sessions/me`;
   * closing it again succeeds. */
  void deleteOwnSession(const std::string& token);

 private:
  MasterTransport transport_;
};

}  // namespace rtype::network
