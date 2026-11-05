#include "SheetTextFixtures.hpp"
#include <string>
#include <string_view>
#include "DataFileConstants.hpp"
#include "SheetText.hpp"

namespace rtype::game::testing {

SheetText sheetText(std::string_view name, std::string_view text) {
  return SheetText{
      .name = std::string{name},
      .source = std::string{name} + std::string{engine::DATA_FILE_EXTENSION},
      .text = std::string{text}};
}

}  // namespace rtype::game::testing
