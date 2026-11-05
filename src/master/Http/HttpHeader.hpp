#pragma once

#include <string>

namespace rtype::master {

/** @brief A header of an answer of the API, such as `Location`. */
struct HttpHeader {
  std::string name;
  std::string value;
};

}  // namespace rtype::master
