#include "PluginHostModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "PluginHostState.hpp"

PluginHostModule::PluginHostModule(std::string name, PluginHostState& state)
    : name_(std::move(name)), state_(&state) {}

std::string PluginHostModule::name() const { return name_; }

std::vector<std::string> PluginHostModule::dependencies() const { return {}; }

void PluginHostModule::install(rtype::engine::InstallContext& context) {
  context.provide(state_->bus);
  context.provide(state_->delivered);
  context.provide(state_->lifetime);
}

void PluginHostModule::start() {}

void PluginHostModule::stop() {}
