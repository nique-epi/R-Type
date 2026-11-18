#include "DeviceLinksService.hpp"
#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include "AccountsService.hpp"
#include "CreatedDeviceLink.hpp"
#include "CreatedSession.hpp"
#include "DeviceLinkDecision.hpp"
#include "DeviceLinkRefusal.hpp"
#include "DeviceLinkRepository.hpp"
#include "DeviceLinkSecret.hpp"
#include "DeviceLinkState.hpp"
#include "DeviceLinkStatus.hpp"
#include "DeviceLinkVerdict.hpp"
#include "DeviceLinksServiceConstants.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterException.hpp"
#include "NewDeviceLink.hpp"
#include "SessionOrigin.hpp"
#include "StoredDeviceLink.hpp"
#include "User.hpp"

namespace rtype::master {

namespace {

/** @returns Where @p link stands at @p now: a denial stays denied, an
 * approval only counts before the expiry. */
network::DeviceLinkStatus statusOf(const StoredDeviceLink& link,
                                   std::chrono::sys_seconds now) {
  if (link.decision == network::DeviceLinkDecision::Denied) {
    return network::DeviceLinkStatus::Denied;
  }
  if (link.expiresAt <= now) {
    return network::DeviceLinkStatus::Expired;
  }
  if (link.decision == network::DeviceLinkDecision::Approved) {
    return network::DeviceLinkStatus::Approved;
  }
  return network::DeviceLinkStatus::Pending;
}

network::DeviceLinkState stateOf(const StoredDeviceLink& link,
                                 std::chrono::sys_seconds now) {
  return network::DeviceLinkState{.userCode = link.userCode,
                                  .status = statusOf(link, now),
                                  .createdAt = link.createdAt,
                                  .expiresAt = link.expiresAt,
                                  .interval = DEVICE_LINK_INTERVAL};
}

/**
 * @brief Adds @p link under a short code no other link holds.
 * @returns The code it was added under.
 * @throws UserCodesExhaustedException when MAXIMUM_USER_CODE_DRAWS codes in a
 * row were taken.
 */
std::string insertedUserCode(DeviceLinkRepository& links, NewDeviceLink link) {
  for (std::size_t draw = 0; draw < MAXIMUM_USER_CODE_DRAWS; ++draw) {
    link.userCode = newDeviceLinkUserCode();
    if (links.insert(link)) {
      return link.userCode;
    }
  }
  throw UserCodesExhaustedException();
}

/** @returns Why a link that stands at @p status opens no session. */
DeviceLinkRefusal refusalOf(network::DeviceLinkStatus status) {
  switch (status) {
    case network::DeviceLinkStatus::Pending:
      return DeviceLinkRefusal::Pending;
    case network::DeviceLinkStatus::Denied:
      return DeviceLinkRefusal::Denied;
    case network::DeviceLinkStatus::Expired:
    case network::DeviceLinkStatus::Approved:
      break;
  }
  return DeviceLinkRefusal::Expired;
}

/** @returns Whether @p verdict is the one already written on @p link. */
bool isWrittenOn(const StoredDeviceLink& link, DeviceLinkVerdict verdict) {
  return link.decision == verdict.decision && link.decider.has_value() &&
         link.decider->id == verdict.userId;
}

/** @returns What reading back @p link after @p verdict was refused answers:
 * its state when it holds that very verdict, else why it holds another. */
std::variant<network::DeviceLinkState, DeviceLinkRefusal> answerToRepeat(
    const StoredDeviceLink& link, DeviceLinkVerdict verdict,
    std::chrono::sys_seconds now) {
  if (isWrittenOn(link, verdict)) {
    return stateOf(link, now);
  }
  return DeviceLinkRefusal::AlreadyDecided;
}

}  // namespace

DeviceLinksService::DeviceLinksService(MasterDatabase& database,
                                       AccountsService& accounts,
                                       std::function<std::string()> siteUrl)
    : database_(database),
      accounts_(accounts),
      siteUrl_(std::move(siteUrl)),
      logger_(std::string{MASTER_LOGGER_NAME}) {}

std::variant<network::CreatedDeviceLink, DeviceLinkRefusal>
DeviceLinksService::create(const std::string& address,
                           std::chrono::sys_seconds now) {
  std::string secret = newDeviceLinkSecret();
  const NewDeviceLink link{.userCode = {},
                           .secretFingerprint = deviceLinkFingerprintOf(secret),
                           .address = address,
                           .createdAt = now,
                           .expiresAt = now + DEVICE_LINK_LIFETIME};
  const std::optional<std::string> userCode = database_.transact(
      [&link, &address,
       now](SQLite::Database& connection) -> std::optional<std::string> {
        DeviceLinkRepository links{connection};
        links.forgetExpiredBefore(now - DEVICE_LINK_KEPT_AFTER_EXPIRY);
        if (links.createdSince(address, now - DEVICE_LINK_CREATION_WINDOW) >=
            MAXIMUM_DEVICE_LINKS_PER_ADDRESS) {
          return std::nullopt;
        }
        return insertedUserCode(links, link);
      });
  if (!userCode.has_value()) {
    return DeviceLinkRefusal::TooManyCreations;
  }
  network::CreatedDeviceLink created = pagesOf(*userCode);
  created.secret = std::move(secret);
  created.expiresAt = link.expiresAt;
  return created;
}

std::variant<network::DeviceLinkState, DeviceLinkRefusal>
DeviceLinksService::readOwn(const std::string& secret,
                            std::chrono::sys_seconds now) {
  const std::string fingerprint = deviceLinkFingerprintOf(secret);
  return database_.transact(
      [&fingerprint, now](SQLite::Database& connection)
          -> std::variant<network::DeviceLinkState, DeviceLinkRefusal> {
        DeviceLinkRepository links{connection};
        const std::optional<StoredDeviceLink> link =
            links.ofSecret(fingerprint);
        if (!link.has_value()) {
          return DeviceLinkRefusal::Unknown;
        }
        if (now - link->readAt < DEVICE_LINK_SHORTEST_READ_GAP) {
          return DeviceLinkRefusal::TooSoon;
        }
        links.markRead(fingerprint, now);
        return stateOf(*link, now);
      });
}

void DeviceLinksService::forgetOwn(const std::string& secret) {
  const std::string fingerprint = deviceLinkFingerprintOf(secret);
  database_.run([&fingerprint](SQLite::Database& connection) {
    DeviceLinkRepository{connection}.remove(fingerprint);
  });
}

std::optional<network::DeviceLinkState> DeviceLinksService::read(
    const std::string& userCode, std::chrono::sys_seconds now) {
  const std::optional<StoredDeviceLink> link =
      database_.run([&userCode](SQLite::Database& connection) {
        return DeviceLinkRepository{connection}.ofUserCode(userCode);
      });
  if (!link.has_value()) {
    return std::nullopt;
  }
  return stateOf(*link, now);
}

std::variant<network::DeviceLinkState, DeviceLinkRefusal>
DeviceLinksService::decide(const std::string& userCode,
                           DeviceLinkVerdict verdict,
                           std::chrono::sys_seconds now) {
  return database_.transact(
      [&userCode, verdict, now](SQLite::Database& connection)
          -> std::variant<network::DeviceLinkState, DeviceLinkRefusal> {
        DeviceLinkRepository links{connection};
        const std::optional<StoredDeviceLink> link = links.ofUserCode(userCode);
        if (!link.has_value()) {
          return DeviceLinkRefusal::Unknown;
        }
        if (link->decision.has_value() || link->expiresAt <= now ||
            !links.decide(userCode, verdict)) {
          return answerToRepeat(*link, verdict, now);
        }
        return stateOf(links.ofUserCode(userCode).value_or(*link), now);
      });
}

std::variant<network::CreatedSession, DeviceLinkRefusal>
DeviceLinksService::openSession(const std::string& secret,
                                const SessionOrigin& origin) {
  const std::string fingerprint = deviceLinkFingerprintOf(secret);
  const std::variant<network::User, DeviceLinkRefusal> claimed =
      database_.transact([&fingerprint, &origin](SQLite::Database& connection)
                             -> std::variant<network::User, DeviceLinkRefusal> {
        DeviceLinkRepository links{connection};
        const std::optional<StoredDeviceLink> link =
            links.ofSecret(fingerprint);
        if (!link.has_value()) {
          return DeviceLinkRefusal::Unknown;
        }
        const network::DeviceLinkStatus status =
            statusOf(*link, origin.openedAt);
        if (status != network::DeviceLinkStatus::Approved ||
            !link->decider.has_value()) {
          return refusalOf(status);
        }
        links.remove(fingerprint);
        return *link->decider;
      });
  if (const auto* refusal = std::get_if<DeviceLinkRefusal>(&claimed)) {
    return *refusal;
  }
  const auto& user = std::get<network::User>(claimed);
  logger_.info(user.nickname, SIGNED_IN_THROUGH_LINK_TEXT, origin.address);
  return accounts_.openSession(user, origin);
}

network::CreatedDeviceLink DeviceLinksService::pagesOf(
    const std::string& userCode) const {
  std::string page = siteUrl_() + std::string{DEVICE_LINK_PAGE_PATH};
  std::string pageWithCode =
      page + std::string{DEVICE_LINK_CODE_QUERY} + userCode;
  return network::CreatedDeviceLink{
      .secret = {},
      .userCode = userCode,
      .verificationUri = std::move(page),
      .verificationUriComplete = std::move(pageWithCode),
      .expiresAt = {},
      .interval = DEVICE_LINK_INTERVAL};
}

}  // namespace rtype::master
