#include "SavedStamp.hpp"
#include <string>
#include "OptionsScreenConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void SavedStamp::markSaved() {
  text_ = SAVED_STAMP;
  age_ = engine::Duration::zero();
}

void SavedStamp::markNotSaved() {
  text_ = NOT_SAVED_STAMP;
  age_.reset();
}

void SavedStamp::age(engine::Duration elapsed) {
  if (!age_.has_value()) {
    return;
  }
  *age_ += elapsed;
  if (*age_ >= SAVED_STAMP_DURATION) {
    text_.clear();
    age_.reset();
  }
}

const std::string& SavedStamp::text() const { return text_; }

}  // namespace rtype::client
