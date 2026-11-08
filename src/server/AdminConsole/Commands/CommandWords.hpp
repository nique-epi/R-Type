#pragma once

#include <span>
#include <string>
#include <string_view>

namespace rtype::server {

/** @throws engine::CommandRefusedException "expected: <command>" when
 * @p words is not empty. */
void requireNoWord(std::span<const std::string> words,
                   std::string_view command);

}  // namespace rtype::server
