#pragma once

namespace rtype::client {

/** @brief Where the book lies and how open it is: the column of its hinge,
 * the row of its top, from 0 (closed) to 1 (open). */
struct BookPose {
  int hinge{};
  int top{};
  float open{};
};

}  // namespace rtype::client
