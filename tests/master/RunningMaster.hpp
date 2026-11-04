#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "JournalFile.hpp"
#include "MasterDatabaseFile.hpp"

/**
 * @brief The modules of the master, started on a free port and a database of
 * their own, writing to a journal of their own and nothing on stderr.
 */
class RunningMaster {
 public:
  RunningMaster();

  /** @returns The TCP port its HTTP API listens on. */
  [[nodiscard]] std::uint16_t port() const;

  /** @returns Where a MasterClient reaches it: `http://127.0.0.1:<port>`. */
  [[nodiscard]] std::string url() const;

  /** @brief Stops the master. */
  void stop();

 private:
  JournalFile journal_;
  MasterDatabaseFile databaseFile_;
  std::uint16_t port_;
  rtype::engine::ConfigurationReport report_;
  std::unique_ptr<rtype::engine::Application> application_;
};
