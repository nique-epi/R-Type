#include "FakeReleaseUpdater.hpp"
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include "ClientException.hpp"
#include "FakeUpdateConstants.hpp"
#include "ReleaseOffer.hpp"
#include "UpdateConstants.hpp"

using rtype::client::FULL_DOWNLOAD_PERCENT;
using rtype::client::ReleaseOffer;
using rtype::client::UpdateException;

std::string FakeReleaseUpdater::currentVersion() {
  return std::string{FAKE_INSTALLED_VERSION};
}

std::optional<ReleaseOffer> FakeReleaseUpdater::newerRelease() {
  ++checks_;
  if (checkFails_) {
    throw UpdateException(FAKE_CHECK_STEP, FAKE_FAILURE);
  }
  return release_;
}

void FakeReleaseUpdater::download(const std::function<void(int)>& progress) {
  if (downloadFails_) {
    throw UpdateException(FAKE_DOWNLOAD_STEP, FAKE_FAILURE);
  }
  progress(FULL_DOWNLOAD_PERCENT / 2);
  progress(FULL_DOWNLOAD_PERCENT);
}

void FakeReleaseUpdater::applyOnExit() {
  ++applies_;
  if (applyFails_) {
    throw UpdateException(FAKE_APPLY_STEP, FAKE_FAILURE);
  }
}

void FakeReleaseUpdater::stop() {}

void FakeReleaseUpdater::offer(std::optional<ReleaseOffer> release) {
  release_ = std::move(release);
}

void FakeReleaseUpdater::failCheck() { checkFails_ = true; }

void FakeReleaseUpdater::failDownload() { downloadFails_ = true; }

void FakeReleaseUpdater::failApply() { applyFails_ = true; }

int FakeReleaseUpdater::checkCount() const { return checks_; }

int FakeReleaseUpdater::applyCount() const { return applies_; }
