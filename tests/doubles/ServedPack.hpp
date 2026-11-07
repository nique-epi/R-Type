#pragma once

#include <string>

/** @brief What a FakePackServer serves: the path it answers, and the body it
 * answers there. */
struct ServedPack {
  std::string path;
  std::string body;
};
