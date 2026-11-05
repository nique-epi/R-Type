#pragma once

#include <memory>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "PluginHostState.hpp"
#include "SharedLibrary.hpp"

/**
 * @brief A test plugin opened and asked for its module by hand: the library,
 * the entry function it exports, the version it answers and the module it
 * creates. The library outlives the module taken from it only if the test
 * declares this object before the application that holds the module.
 */
class HandLoadedPlugin {
 public:
  /** @param name The plugin to open, in the folder the tests build them in.
   * @throws SharedLibraryOpenException if the file cannot be opened. */
  explicit HandLoadedPlugin(const std::string& name);

  /** @returns The module the plugin created; null once it was taken. */
  [[nodiscard]] std::unique_ptr<rtype::engine::IModule> takeModule();

 private:
  rtype::engine::SharedLibrary library_;
  std::unique_ptr<rtype::engine::IModule> module_;
};

/** @returns The host module providing @p state, then @p plugin's module. */
std::vector<std::unique_ptr<rtype::engine::IModule>> hostThenPlugin(
    PluginHostState& state, HandLoadedPlugin& plugin);
