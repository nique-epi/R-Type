#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "Application.hpp"
#include "IModule.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"

/** @brief Installs the module that provides the screen registry, then the
 * modules the test gives, and reads back which screens ended up registered. */
class ScreenModulesFixture : public ::testing::Test {
 protected:
  /** @brief Installs and starts the registry module followed by
   * @p screenModules, and finds the registry they filled.
   * @throws DuplicateScreenException if two of them register one identifier. */
  void install(
      std::vector<std::unique_ptr<rtype::engine::IModule>> screenModules);

  /** @returns Whether a module registered @p screen. */
  [[nodiscard]] bool isRegistered(rtype::client::ScreenId screen);

 private:
  rtype::client::ScreenRegistry* registry_{nullptr};
  std::unique_ptr<rtype::engine::Application> application_;
};
