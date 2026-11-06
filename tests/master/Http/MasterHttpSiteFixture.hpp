#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <cstdint>
#include <filesystem>
#include <string_view>
#include "MasterHttpServer.hpp"
#include "TemporaryFolder.hpp"

/**
 * @brief An HTTP server of the master on the loopback address and a port the
 * system chooses, serving the web site the test writes in a folder of its
 * own, and a client that calls it.
 */
class MasterHttpSiteFixture : public ::testing::Test {
 protected:
  MasterHttpSiteFixture();

  /** @brief Writes @p content in @p file, below the folder of the site. */
  void writeSiteFile(std::string_view file, std::string_view content) const;

  /** @brief Writes @p content in @p file, beside the folder of the site. */
  void writeOutsideFile(std::string_view file, std::string_view content) const;

  /** @brief Adds TEST_GET_ROUTE, which answers EXPECTED_OK and TEST_ROUTE_BODY.
   */
  void addTestRoute();

  /** @brief Has the server serve the site of @p folder, below the test. */
  void serveSite(std::string_view folder);

  /** @brief Opens the server on a port the system chooses. */
  void openServer();

  /** @returns The answer of the server to GET @p path. */
  [[nodiscard]] httplib::Result get(std::string_view path) const;

 private:
  TemporaryFolder folder_;
  std::filesystem::path site_;
  rtype::master::MasterHttpServer server_;
  std::uint16_t port_{};
};
