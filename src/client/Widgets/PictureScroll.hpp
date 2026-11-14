#pragma once

namespace rtype::client {

/** @brief How far a picture taller than the interface is scrolled down, and
 * how far it may be, in pixels of the interface picture. */
struct PictureScroll {
  int scroll{};
  int last{};
};

}  // namespace rtype::client
