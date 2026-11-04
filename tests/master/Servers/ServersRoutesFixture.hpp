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
 * @brief The master, started on a free port and a database of its own, with
 * one server created through `POST /api/servers`, and a client of its routes.
 */
class ServersRoutesFixture : public ::testing::Test {
 protected:
  ServersRoutesFixture();

  /** @brief Stops the master. */
  void TearDown() override;

  /** @returns The id of the server the fixture created. */
  [[nodiscard]] std::int64_t createdId() const;

  /** @returns The key of the server the fixture created. */
  [[nodiscard]] const std::string& key() const;

  /** @returns The answer of the master to @p call. */
  [[nodiscard]] httplib::Result send(const RouteCall& call) const;

  /** @returns The answer to a creation of "Paris 1". */
  [[nodiscard]] httplib::Result createParis() const;

  /** @returns The answer to the registration of "Paris 1" with key(). */
  [[nodiscard]] httplib::Result registerParis() const;

 private:
  JournalFile journal_;
  MasterDatabaseFile databaseFile_;
  std::uint16_t port_;
  rtype::engine::ConfigurationReport report_;
  std::unique_ptr<rtype::engine::Application> application_;
  std::int64_t createdId_{};
  std::string key_;
};
