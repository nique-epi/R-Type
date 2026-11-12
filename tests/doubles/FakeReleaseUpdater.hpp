#pragma once

#include <atomic>
#include <functional>
#include <optional>
#include <string>
#include "IReleaseUpdater.hpp"
#include "ReleaseOffer.hpp"

/** @brief An updater whose answers the test sets: the release it offers, or
 * a failure at the check, the download or the apply, each an UpdateException
 * as VelopackUpdater throws them. The download reports half then all of the
 * package; the checks and the applies are counted, from any thread. */
class FakeReleaseUpdater : public rtype::client::IReleaseUpdater {
 public:
  [[nodiscard]] std::string currentVersion() override;
  [[nodiscard]] std::optional<rtype::client::ReleaseOffer> newerRelease()
      override;
  void download(const std::function<void(int)>& progress) override;
  void applyOnExit() override;
  void stop() override;

  void offer(std::optional<rtype::client::ReleaseOffer> release);
  void failCheck();
  void failDownload();
  void failApply();
  [[nodiscard]] int checkCount() const;
  [[nodiscard]] int applyCount() const;

 private:
  std::optional<rtype::client::ReleaseOffer> release_;
  bool checkFails_{false};
  bool downloadFails_{false};
  bool applyFails_{false};
  std::atomic<int> checks_{0};
  std::atomic<int> applies_{0};
};
