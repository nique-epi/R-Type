#include "SavedStamp.hpp"
#include <string>
#include "Language.hpp"
#include "OptionsScreenConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void SavedStamp::markSaved() {
  stamp_ = SAVED_STAMP;
  age_ = engine::Duration::zero();
}

void SavedStamp::markNotSaved() {
  stamp_ = NOT_SAVED_STAMP;
  age_.reset();
}

void SavedStamp::age(engine::Duration elapsed) {
  if (!age_.has_value()) {
    return;
  }
  *age_ += elapsed;
  if (*age_ >= SAVED_STAMP_DURATION) {
    stamp_.reset();
    age_.reset();
  }
}

std::string SavedStamp::text(Language language) const {
  if (!stamp_.has_value()) {
    return {};
  }
  return std::string(stamp_->in(language));
}

}  // namespace rtype::client
