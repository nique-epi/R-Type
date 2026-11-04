#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <memory>
#include <string>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "JournalFile.hpp"
#include "RouteCall.hpp"
#include "TemporaryFolder.hpp"

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

 private:
  [[nodiscard]] std::string issuedKey() const;

  JournalFile journal_;
  TemporaryFolder folder_;
  std::uint16_t port_;
  rtype::engine::ConfigurationReport report_;
  std::string key_;
  std::unique_ptr<rtype::engine::Application> application_;
};
