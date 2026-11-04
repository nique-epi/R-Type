#pragma once

#include "Application.hpp"
#include "ServerModuleFixture.hpp"

/** @brief The base of the tests of the logger module: an application made of
 * that module only, writing to the journal of the test. */
class ServerLoggerModuleFixture : public ServerModuleFixture {
 protected:
  /** @returns An application made of the logger module only. */
  [[nodiscard]] rtype::engine::Application applicationWithTheLogger() const;
};
