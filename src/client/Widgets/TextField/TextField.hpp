#pragma once

#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace rtype::client {

/**
 * @brief Text the player types on the keyboard, kept to the characters it
 * accepts and to a size in bytes.
 *
 * The text is UTF-8. A typed character goes at the end when the field accepts
 * it and when all its bytes fit within the size; otherwise nothing changes, so
 * the text never ends with half a character. Erasing removes the last character
 * whole. The caret stays at the end: no key moves it.
 *
 * Whatever the characters it accepts, a field never holds one outside
 * FIRST_PRINTABLE_BASIC_LATIN to LAST_PRINTABLE_LATIN1_SUPPLEMENT, so never a
 * control character such as the ones Enter, Tab and Backspace type along with
 * their key.
 *
 * A field knows nothing of where it is drawn: a Button draws it in a box under
 * its label, and the Menu holding that button sends it the keyboard events
 * while it is selected.
 */
class TextField {
 public:
  /** @brief The characters a field accepts. */
  enum class Characters : std::uint8_t {
    /**
     * @brief Every character a field may hold: FIRST_PRINTABLE_BASIC_LATIN to
     * LAST_PRINTABLE_BASIC_LATIN, and FIRST_PRINTABLE_LATIN1_SUPPLEMENT to
     * LAST_PRINTABLE_LATIN1_SUPPLEMENT.
     */
    Printable,
    /** @brief The decimal digits, as a port is written. */
    Digits,
    /**
     * @brief The decimal digits and IPV4_ADDRESS_SEPARATOR, as an IPv4 address
     * is written.
     */
    DigitsAndDots,
  };

  /**
   * @param text UTF-8 text the field starts with: each of its characters is
   * typed in order, so the ones the field refuses are left out.
   * @param maximumSize Largest size of the text, in bytes.
   * @param accepted Characters the field accepts.
   */
  TextField(std::string_view text, std::size_t maximumSize,
            Characters accepted);

  /**
   * @brief Types the character of a TextEntered event, and erases on each press
   * of FIELD_ERASE_KEY, the ones the system repeats while the key is held
   * included.
   *
   * @returns Whether the event was one of those two, whether a character was
   * typed or erased or not: any other event is left to the caller.
   */
  bool handleEvent(const sf::Event& event);

  /** @brief Adds the character at the end of the text, if the field accepts
   * it and all its bytes fit. */
  void type(char32_t character);

  /** @brief Removes the last character of the text, whole; nothing when the
   * text is empty. */
  void erase();

  /** @returns The text, UTF-8. */
  [[nodiscard]] const std::string& text() const;

 private:
  [[nodiscard]] bool accepts(char32_t character) const;

  std::string text_;
  std::size_t maximumSize_{};
  Characters accepted_{};
};

}  // namespace rtype::client
