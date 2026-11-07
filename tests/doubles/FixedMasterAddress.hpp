#pragma once

#include <string>
#include "IMasterAddress.hpp"

/** @brief A master that never changes, read from the url it was built with. */
class FixedMasterAddress : public rtype::client::IMasterAddress {
 public:
  explicit FixedMasterAddress(std::string url);

  [[nodiscard]] std::string url() const override;

 private:
  std::string url_;
};
