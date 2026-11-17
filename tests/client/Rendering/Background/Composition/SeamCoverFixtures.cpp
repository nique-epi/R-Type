#include "SeamCoverFixtures.hpp"
#include <cstddef>
#include "SeamCover.hpp"
#include "SeamCoverTestConstants.hpp"

using rtype::client::hiddenBySeam;

std::size_t hiddenRowsOf(int fromSeam) {
  std::size_t hidden = 0;
  for (int row = 0; row < ROWS_READ; ++row) {
    if (hiddenBySeam({fromSeam, row})) {
      ++hidden;
    }
  }
  return hidden;
}
