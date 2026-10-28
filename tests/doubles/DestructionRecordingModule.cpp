#include "DestructionRecordingModule.hpp"
#include <exception>
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"

DestructionRecordingModule::DestructionRecordingModule(
    std::string name, std::vector<std::string>& destroyed)
    : name_(std::move(name)), destroyed_(&destroyed) {}

DestructionRecordingModule::~DestructionRecordingModule() {
  try {
    destroyed_->push_back(name_);
  } catch (...) {
    std::terminate();
  }
}

std::string DestructionRecordingModule::name() const { return name_; }

std::vector<std::string> DestructionRecordingModule::dependencies() const {
  return {};
}

void DestructionRecordingModule::install(
    [[maybe_unused]] rtype::engine::InstallContext& context) {}

void DestructionRecordingModule::start() {}

void DestructionRecordingModule::stop() {}
