#include "DropdownFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "Dropdown.hpp"
#include "DropdownParts.hpp"
#include "DropdownTestConstants.hpp"
#include "KeyPress.hpp"
#include "ValueBinding.hpp"

using rtype::client::Dropdown;
using rtype::client::DropdownParts;
using rtype::client::ValueBinding;
using rtype::client::testing::LARGEST_SIZE_INDEX;
using rtype::client::testing::SIZES_THAT_FIT;
using rtype::client::testing::WINDOW_SIZE_TEXTS;

DropdownFixture::DropdownFixture()
    : size_(LARGEST_SIZE_INDEX),
      list_(DropdownParts{
          .choices = std::vector<std::string>(WINDOW_SIZE_TEXTS.begin(),
                                              WINDOW_SIZE_TEXTS.end()),
          .available = [](std::size_t size) { return size < SIZES_THAT_FIT; },
          .value =
              ValueBinding<std::size_t>{
                  .read = [this] { return size_; },
                  .write = [this](std::size_t size) { size_ = size; }},
          .imposedText = [this] { return imposed_; }}) {}

bool DropdownFixture::press(sf::Keyboard::Key key) {
  return list_.keyPressed(keyPressOf(key));
}

std::size_t DropdownFixture::size() const { return size_; }

Dropdown& DropdownFixture::list() { return list_; }

void DropdownFixture::impose(std::string_view text) {
  imposed_ = std::string(text);
}
