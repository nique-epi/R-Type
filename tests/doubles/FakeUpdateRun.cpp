#include "FakeUpdateRun.hpp"
#include "UpdatePhase.hpp"
#include "UpdateProgress.hpp"

using rtype::client::UpdatePhase;
using rtype::client::UpdateProgress;

UpdateProgress FakeUpdateRun::progress() const { return progress_; }

void FakeUpdateRun::advance() {}

bool FakeUpdateRun::restart() {
  ++restarts_;
  if (!started_) {
    progress_.phase = UpdatePhase::Failed;
  }
  return started_;
}

void FakeUpdateRun::set(UpdateProgress progress) { progress_ = progress; }

void FakeUpdateRun::answerRestart(bool started) { started_ = started; }

int FakeUpdateRun::restartCount() const { return restarts_; }
