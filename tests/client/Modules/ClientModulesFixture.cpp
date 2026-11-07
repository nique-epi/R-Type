#include "ClientModulesFixture.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <span>
#include <string_view>
#include <vector>
#include "ClientModules.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"

using rtype::engine::IModule;

ClientModulesFixture::ClientModulesFixture()
    : loop_(clock_),
      options_(rtype::client::LaunchOptions::read(
          std::span<const std::string_view>{})) {}

std::vector<std::unique_ptr<IModule>> ClientModulesFixture::modules() {
  return rtype::client::clientModules(options_.logOptions(), options_, loop_);
}

std::vector<std::unique_ptr<IModule>> ClientModulesFixture::modulesWithout(
    std::string_view name) {
  std::vector<std::unique_ptr<IModule>> kept = modules();
  std::erase_if(kept, [name](const std::unique_ptr<IModule>& module) {
    return module->name() == name;
  });
  return kept;
}

std::size_t ClientModulesFixture::positionOf(std::string_view name) {
  const std::vector<std::unique_ptr<IModule>> listed = modules();
  const auto found = std::ranges::find_if(
      listed, [name](const std::unique_ptr<IModule>& module) {
        return module->name() == name;
      });
  return static_cast<std::size_t>(found - listed.begin());
}

bool ClientModulesFixture::lists(std::string_view name) {
  return positionOf(name) < modules().size();
}
