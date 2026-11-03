#include "FrameLoopFixture.hpp"
#include <array>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "FrameLoopTestConstants.hpp"
#include "IModule.hpp"
#include "LoopStages.hpp"
#include "TimeConstants.hpp"
#include "WorkAddingModule.hpp"

void FrameLoopFixture::runPasses(
    std::span<const rtype::engine::Duration> passDurations) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<WorkAddingModule>(
      std::string{PACING_MODULE_NAME}, std::string{TESTED_STAGE},
      [this, passDurations](rtype::engine::Duration elapsed) {
        elapsedByPass_.push_back(elapsed);
        clock_.advance(passDurations[elapsedByPass_.size() - 1]);
        if (elapsedByPass_.size() == passDurations.size()) {
          loop_.stop();
        }
      }));
  const std::array<std::string_view, 1> stages{TESTED_STAGE};
  rtype::engine::Application application{rtype::engine::LoopStages{stages},
                                         std::move(modules)};
  application.start();
  loop_.run(application);
  application.stop();
}

void FrameLoopFixture::stopBeforeRunning() { loop_.stop(); }

const std::vector<rtype::engine::Duration>& FrameLoopFixture::elapsedByPass()
    const {
  return elapsedByPass_;
}
