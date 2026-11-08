#pragma once

#include <gtest/gtest.h>
#include <string_view>
#include "CommandInterpreter.hpp"
#include "ConfigurationRegistry.hpp"
#include "DatabaseLocation.hpp"
#include "ListeningPortModule.hpp"

/** @brief A database setting declared in a registry of its own, and the
 * location built from it, the folder LOCATION_FOLDER and LISTENING_PORT. */
class DatabaseLocationFixture : public ::testing::Test {
 protected:
  DatabaseLocationFixture();

  /** @brief Runs @p line, as a line of server.cfg, and expects it to run. */
  void run(std::string_view line);

  [[nodiscard]] const rtype::server::DatabaseLocation& location() const;

 private:
  rtype::engine::CommandInterpreter interpreter_;
  rtype::engine::ConfigurationRegistry registry_{interpreter_};
  ListeningPortModule port_;
  rtype::server::DatabaseLocation location_;
};
