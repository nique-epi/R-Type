#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <variant>
#include "AccountsService.hpp"
#include "CreatedDeviceLink.hpp"
#include "CreatedSession.hpp"
#include "DeviceLinkRefusal.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkVerdict.hpp"
#include "Logger.hpp"
#include "MasterDatabase.hpp"
#include "SessionOrigin.hpp"

namespace rtype::master {

/**
 * @brief The rules of the device links, the OAuth 2.0 Device Authorization
 * Grant of RFC 8628: a game asks for a link and gets a secret and a short
 * code; a signed-in player approves or denies the code on the web site; the
 * game reads its link with its secret and, once approved, trades it for a
 * session, once. The master keeps the fingerprint of the secret only.
 */
class DeviceLinksService {
 public:
  /**
   * @param database It must outlive the service.
   * @param accounts Opens the session a link is traded for; it must outlive
   * the service.
   * @param siteUrl The address of the web site, read at each new link, which
   * its page of validation follows.
   */
  DeviceLinksService(MasterDatabase& database, AccountsService& accounts,
                     std::function<std::string()> siteUrl);

  /**
   * @brief Opens a link for the game at @p address, which expires
   * DEVICE_LINK_LIFETIME after @p now, and forgets the links expired long
   * before.
   * @returns The link with its secret, shown this once; or
   * DeviceLinkRefusal::TooManyCreations when @p address asked for
   * MAXIMUM_DEVICE_LINKS_PER_ADDRESS in the last DEVICE_LINK_CREATION_WINDOW.
   */
  std::variant<network::CreatedDeviceLink, DeviceLinkRefusal> create(
      const std::string& address, std::chrono::sys_seconds now);

  /**
   * @brief Reads the link of @p secret for its game, and notes the read.
   * @returns Its state at @p now; DeviceLinkRefusal::Unknown when no link has
   * that secret; DeviceLinkRefusal::TooSoon when the game read it less than
   * DEVICE_LINK_SHORTEST_READ_GAP before, which leaves the last read as it
   * was.
   */
  std::variant<network::DeviceLinkState, DeviceLinkRefusal> readOwn(
      const std::string& secret, std::chrono::sys_seconds now);

  /** @brief Forgets the link of @p secret; nothing when none has it. */
  void forgetOwn(const std::string& secret);

  /** @returns The state at @p now of the link of the short code @p userCode,
   * nothing when no link has it. */
  [[nodiscard]] std::optional<network::DeviceLinkState> read(
      const std::string& userCode, std::chrono::sys_seconds now);

  /**
   * @brief Writes @p verdict on the link of @p userCode, once: the same
   * verdict sent again by the same account changes nothing.
   * @returns The state of the link after it; DeviceLinkRefusal::Unknown when
   * no link has that code; DeviceLinkRefusal::AlreadyDecided when another
   * verdict was written, or the link expired undecided.
   */
  std::variant<network::DeviceLinkState, DeviceLinkRefusal> decide(
      const std::string& userCode, DeviceLinkVerdict verdict,
      std::chrono::sys_seconds now);

  /**
   * @brief Trades the approved link of @p secret for a session of the account
   * that approved it, opened as @p origin says, and forgets the link in the
   * same transaction it is read in, so a secret opens one session.
   * @returns The session; or DeviceLinkRefusal::Unknown, Pending, Denied or
   * Expired as the link stands, which keeps it.
   */
  std::variant<network::CreatedSession, DeviceLinkRefusal> openSession(
      const std::string& secret, const SessionOrigin& origin);

 private:
  /** @returns The pages of the web site that validate @p userCode, with the
   * secret and the expiry of a new link left to fill. */
  [[nodiscard]] network::CreatedDeviceLink pagesOf(
      const std::string& userCode) const;

  MasterDatabase& database_;
  AccountsService& accounts_;
  std::function<std::string()> siteUrl_;
  logging::Logger logger_;
};

}  // namespace rtype::master
