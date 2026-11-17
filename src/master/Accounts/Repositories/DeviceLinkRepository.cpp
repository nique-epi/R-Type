#include "DeviceLinkRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "DeviceLinkDecision.hpp"
#include "DeviceLinkRepositoryConstants.hpp"
#include "DeviceLinkVerdict.hpp"
#include "EpochSeconds.hpp"
#include "MasterMessageText.hpp"
#include "NewDeviceLink.hpp"
#include "StatementParts.hpp"
#include "StoredDeviceLink.hpp"
#include "User.hpp"
#include "UserRow.hpp"

namespace rtype::master {

namespace {

std::chrono::sys_seconds dateIn(const SQLite::Statement& query, int column) {
  return dateOfEpochSeconds(query.getColumn(column).getInt64());
}

/** @returns The decider of the row @p query stands on, nothing when no one
 * decided. */
std::optional<network::User> deciderIn(const SQLite::Statement& query) {
  if (query.getColumn(LINK_DECIDER_ID_COLUMN).isNull()) {
    return std::nullopt;
  }
  return userIn(query);
}

/** @returns The decision of the row @p query stands on, nothing when no one
 * decided or the text is no decision. */
std::optional<network::DeviceLinkDecision> decisionIn(
    const SQLite::Statement& query) {
  if (query.getColumn(LINK_DECISION_COLUMN).isNull()) {
    return std::nullopt;
  }
  return network::deviceLinkDecisionOf(
      query.getColumn(LINK_DECISION_COLUMN).getString());
}

/** @returns The link @p query finds, its parameters bound;
 * nothing when it finds none. */
std::optional<StoredDeviceLink> linkFoundBy(SQLite::Statement& query) {
  if (!query.executeStep()) {
    return std::nullopt;
  }
  return StoredDeviceLink{
      .userCode = query.getColumn(LINK_USER_CODE_COLUMN).getString(),
      .createdAt = dateIn(query, LINK_CREATED_AT_COLUMN),
      .expiresAt = dateIn(query, LINK_EXPIRES_AT_COLUMN),
      .readAt = dateIn(query, LINK_READ_AT_COLUMN),
      .decider = deciderIn(query),
      .decision = decisionIn(query)};
}

}  // namespace

DeviceLinkRepository::DeviceLinkRepository(SQLite::Database& connection)
    : connection_(connection) {}

bool DeviceLinkRepository::insert(const NewDeviceLink& link) {
  SQLite::Statement insert{connection_,
                           std::string{INSERT_DEVICE_LINK_STATEMENT}};
  insert.bind(std::string{LINK_USER_CODE_PARAMETER}, link.userCode);
  insert.bind(std::string{LINK_SECRET_FINGERPRINT_PARAMETER},
              link.secretFingerprint);
  insert.bind(std::string{LINK_ADDRESS_PARAMETER}, link.address);
  insert.bind(std::string{LINK_CREATED_AT_PARAMETER},
              epochSecondsOf(link.createdAt));
  insert.bind(std::string{LINK_EXPIRES_AT_PARAMETER},
              epochSecondsOf(link.expiresAt));
  return insert.exec() == 1;
}

std::int64_t DeviceLinkRepository::createdSince(
    const std::string& address, std::chrono::sys_seconds since) {
  SQLite::Statement query{connection_,
                          std::string{DEVICE_LINKS_CREATED_SINCE_QUERY}};
  query.bind(std::string{LINK_ADDRESS_PARAMETER}, address);
  query.bind(std::string{LINK_SINCE_PARAMETER}, epochSecondsOf(since));
  query.executeStep();
  return query.getColumn(LINK_COUNT_COLUMN).getInt64();
}

void DeviceLinkRepository::forgetExpiredBefore(
    std::chrono::sys_seconds oldest) {
  eraseOlderThan(connection_, FORGET_DEVICE_LINKS_STATEMENT, oldest);
}

std::optional<StoredDeviceLink> DeviceLinkRepository::ofSecret(
    const std::string& fingerprint) {
  SQLite::Statement query{connection_,
                          std::string{DEVICE_LINK_OF_SECRET_QUERY}};
  query.bind(std::string{LINK_SECRET_FINGERPRINT_PARAMETER}, fingerprint);
  return linkFoundBy(query);
}

std::optional<StoredDeviceLink> DeviceLinkRepository::ofUserCode(
    const std::string& userCode) {
  SQLite::Statement query{connection_,
                          std::string{DEVICE_LINK_OF_USER_CODE_QUERY}};
  query.bind(std::string{LINK_USER_CODE_PARAMETER}, userCode);
  return linkFoundBy(query);
}

void DeviceLinkRepository::markRead(const std::string& fingerprint,
                                    std::chrono::sys_seconds readAt) {
  SQLite::Statement update{connection_,
                           std::string{MARK_DEVICE_LINK_READ_STATEMENT}};
  update.bind(std::string{LINK_READ_AT_PARAMETER}, epochSecondsOf(readAt));
  update.bind(std::string{LINK_SECRET_FINGERPRINT_PARAMETER}, fingerprint);
  update.exec();
}

bool DeviceLinkRepository::decide(const std::string& userCode,
                                  DeviceLinkVerdict verdict) {
  SQLite::Statement update{connection_,
                           std::string{DECIDE_DEVICE_LINK_STATEMENT}};
  update.bind(std::string{LINK_USER_ID_PARAMETER}, verdict.userId);
  update.bind(std::string{LINK_DECISION_PARAMETER},
              std::string{network::deviceLinkDecisionText(verdict.decision)});
  update.bind(std::string{LINK_USER_CODE_PARAMETER}, userCode);
  return update.exec() == 1;
}

void DeviceLinkRepository::remove(const std::string& fingerprint) {
  SQLite::Statement remove{connection_,
                           std::string{DELETE_DEVICE_LINK_STATEMENT}};
  remove.bind(std::string{LINK_SECRET_FINGERPRINT_PARAMETER}, fingerprint);
  remove.exec();
}

}  // namespace rtype::master
