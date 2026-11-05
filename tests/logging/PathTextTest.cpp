#include <gtest/gtest.h>
#include <filesystem>
#include "PathText.hpp"

using rtype::logging::genericText;

/**
 * Given a path of nested folders
 * When it is turned into text
 * Then its folders are separated by '/', on every platform
 */
TEST(PathText, GenericTextSeparatesFoldersWithASlash) {
  const std::filesystem::path nested =
      std::filesystem::path("sprites") / "enemies" / "bydo.png";

  EXPECT_EQ(genericText(nested), "sprites/enemies/bydo.png");
}
