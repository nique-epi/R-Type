#pragma once

#include <gtest/gtest.h>
#include "AssetLibrary.hpp"
#include "InstallContext.hpp"
#include "LoopStages.hpp"
#include "PlayerSettings.hpp"
#include "ServiceRegistry.hpp"
#include "TemporaryFolder.hpp"

/** @brief The services the sound module is installed over: an asset library
 * on an empty folder and the settings of a first launch, the loop stages of
 * the client, and a registry nothing was provided to. */
class ClientAudioModuleFixture : public ::testing::Test {
 protected:
  ClientAudioModuleFixture();

  /** @returns The context of the module named "audio" over the services. */
  [[nodiscard]] rtype::engine::InstallContext context();

  /** @returns The context of the module named "audio" over a registry that
   * holds no service. */
  [[nodiscard]] rtype::engine::InstallContext contextWithoutServices();

  [[nodiscard]] rtype::engine::ServiceRegistry& services();
  [[nodiscard]] rtype::engine::LoopStages& stages();

 private:
  TemporaryFolder folder_;
  rtype::client::AssetLibrary library_;
  rtype::client::PlayerSettings settings_;
  rtype::engine::ServiceRegistry services_;
  rtype::engine::ServiceRegistry noServices_;
  rtype::engine::LoopStages stages_;
};
