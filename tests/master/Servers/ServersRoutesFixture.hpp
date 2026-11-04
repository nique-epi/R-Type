#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <memory>
#include <string>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "JournalFile.hpp"
#include "MasterDatabaseFile.hpp"
#include "RouteCall.hpp"

/**
 * @brief The master, started on a free port and a database of its own that
 * holds one key, issued before the start, and a client of its routes.
 */
class ServersRoutesFixture : public ::testing::Test {
 protected:
  ServersRoutesFixture();

  /** @brief Stops the master. */
  void TearDown() override;

  /** @returns The key issued before the master started. */
  [[nodiscard]] const std::string& key() const;

  /** @returns The answer of the master to PUT @p call. */
  [[nodiscard]] httplib::Result put(const RouteCall& call) const;

  /** @returns The answer to the registration of "Paris 1" with key(). */
  [[nodiscard]] httplib::Result registerParis() const;

 private:
  [[nodiscard]] std::string issuedKey();

  JournalFile journal_;
  MasterDatabaseFile databaseFile_;
  std::uint16_t port_;
  rtype::engine::ConfigurationReport report_;
  std::string key_;
  std::unique_ptr<rtype::engine::Application> application_;
};
