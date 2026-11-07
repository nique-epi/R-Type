#pragma once

#include <string_view>
#include "IConsoleOutput.hpp"

namespace rtype::server {

/** @brief The standard output of the supervisor: the journal writes to the
 * standard error, so the answers of the console never mix with its lines when
 * the journal is redirected. Each line is flushed at once. */
class StandardOutput final : public IConsoleOutput {
 public:
  void write(std::string_view line) override;
};

}  // namespace rtype::server
