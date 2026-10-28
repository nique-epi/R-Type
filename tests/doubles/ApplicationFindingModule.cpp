#include "ApplicationFindingModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "InstallContext.hpp"

ApplicationFindingModule::ApplicationFindingModule(
    std::string name, const rtype::engine::Application*& found)
    : name_(std::move(name)), found_(&found) {}

std::string ApplicationFindingModule::name() const { return name_; }

std::vector<std::string> ApplicationFindingModule::dependencies() const {
  return {};
}

void ApplicationFindingModule::install(rtype::engine::InstallContext& context) {
  *found_ = &context.require<rtype::engine::Application>();
}

void ApplicationFindingModule::start() {}

void ApplicationFindingModule::stop() {}
