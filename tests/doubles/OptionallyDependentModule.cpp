#include "OptionallyDependentModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"

OptionallyDependentModule::OptionallyDependentModule(std::string name,
                                                     std::string optional)
    : name_(std::move(name)), optional_(std::move(optional)) {}

std::string OptionallyDependentModule::name() const { return name_; }

std::vector<std::string> OptionallyDependentModule::dependencies() const {
  return {};
}

std::vector<std::string> OptionallyDependentModule::optionalDependencies()
    const {
  return {optional_};
}

void OptionallyDependentModule::install(
    [[maybe_unused]] rtype::engine::InstallContext& context) {}

void OptionallyDependentModule::start() {}

void OptionallyDependentModule::stop() {}
