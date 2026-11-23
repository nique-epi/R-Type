#pragma once

#include <optional>
#include <string>
#include "ISessionTokenStorage.hpp"

/** @brief Token storage that holds the token in memory instead of a file,
 * and counts the writes asked, so a test sees one written each frame. */
class RecordingSessionTokenStorage final
    : public rtype::client::ISessionTokenStorage {
 public:
  [[nodiscard]] std::optional<std::string> load() const override;
  void keep(const std::string& token) override;
  void forget() override;

  /** @brief The token kept, as a file would hold it. Public so a test sets
   * what a previous launch kept. */
  std::optional<std::string> kept;
  /** @brief The number of keep() asked so far. */
  int keeps{};
};
