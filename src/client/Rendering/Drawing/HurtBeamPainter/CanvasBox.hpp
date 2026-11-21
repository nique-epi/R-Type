#pragma once

namespace rtype::client {

/** @brief The pixels of the canvas from (left, top) to (right, bottom), both
 * corners included. */
struct CanvasBox {
  int left{0};
  int top{0};
  int right{0};
  int bottom{0};
};

}  // namespace rtype::client
