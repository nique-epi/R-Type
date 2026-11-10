#pragma once

#include "IUpdateRun.hpp"
#include "UpdateProgress.hpp"

/** @brief An update whose progress the test sets; it counts the restarts the
 * screen asks for, and restart() answers what the test chose, failing the
 * update when it answers false, as UpdateRun does. */
class FakeUpdateRun : public rtype::client::IUpdateRun {
 public:
  [[nodiscard]] rtype::client::UpdateProgress progress() const override;
  [[nodiscard]] bool restart() override;

  void set(rtype::client::UpdateProgress progress);
  void answerRestart(bool started);
  [[nodiscard]] int restartCount() const;

 private:
  rtype::client::UpdateProgress progress_;
  bool started_{true};
  int restarts_{};
};
