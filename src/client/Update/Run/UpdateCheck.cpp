#include "UpdateCheck.hpp"
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include "ClientException.hpp"
#include "IReleaseUpdater.hpp"
#include "ReleaseOffer.hpp"
#include "UpdateConstants.hpp"

namespace rtype::client {

UpdateCheck::UpdateCheck(IReleaseUpdater& updater)
    : updater_(&updater),
      logger_(std::string{UPDATE_LOGGER_NAME}),
      thread_([this] { run(); }) {}

bool UpdateCheck::found() const {
  const std::scoped_lock lock{mutex_};
  return offer_.has_value();
}

std::optional<ReleaseOffer> UpdateCheck::offer() const {
  const std::scoped_lock lock{mutex_};
  return offer_;
}

void UpdateCheck::run() {
  std::optional<ReleaseOffer> found;
  try {
    found = updater_->newerRelease();
  } catch (const UpdateException& error) {
    logger_.warn(error.what());
    return;
  }
  if (!found.has_value()) {
    logger_.info("no newer release to install");
    return;
  }
  logger_.info("release ", found->versionText, " is newer: it is installed");
  const std::scoped_lock lock{mutex_};
  offer_ = std::move(found);
}

}  // namespace rtype::client
