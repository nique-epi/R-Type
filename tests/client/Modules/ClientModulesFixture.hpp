#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>
#include "FrameLoop.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "SimulatedClock.hpp"

/** @brief The modules of a client launched without any option, built and
 * never installed: building them opens no window and no socket. */
class ClientModulesFixture : public ::testing::Test {
 protected:
  ClientModulesFixture();

  /** @returns The modules clientModules() lists. */
  [[nodiscard]] std::vector<std::unique_ptr<rtype::engine::IModule>> modules();

  /** @returns The modules clientModules() lists, less the one named
   * @p name. */
  [[nodiscard]] std::vector<std::unique_ptr<rtype::engine::IModule>>
  modulesWithout(std::string_view name);

  /** @returns Where the module named @p name stands in clientModules(), or
   * the size of the list when it is not there. */
  [[nodiscard]] std::size_t positionOf(std::string_view name);

 private:
  SimulatedClock clock_;
  rtype::client::FrameLoop loop_;
  rtype::client::LaunchOptions options_;
};
