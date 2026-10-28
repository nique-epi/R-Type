#include "WorkAddingModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "StageWork.hpp"

WorkAddingModule::WorkAddingModule(std::string name, std::string stage,
                                   rtype::engine::StageWork work)
    : name_(std::move(name)),
      stage_(std::move(stage)),
      work_(std::move(work)) {}

std::string WorkAddingModule::name() const { return name_; }

std::vector<std::string> WorkAddingModule::dependencies() const { return {}; }

void WorkAddingModule::install(rtype::engine::InstallContext& context) {
  context.addWork(stage_, work_);
}

void WorkAddingModule::start() {}

void WorkAddingModule::stop() {}
