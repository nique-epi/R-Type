#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <string>
#include <string_view>
#include "KeyPress.hpp"
#include "TextField.hpp"

using rtype::client::TextField;

namespace {

/** @brief Size of the fields of most tests, in bytes: the longest nickname of
 * the protocol. */
constexpr std::size_t SIXTEEN_BYTES = 16;

/** @brief Size of the fields that start with a longer text, in bytes. */
constexpr std::size_t FIVE_BYTES = 5;

/** @brief Fifteen letters without accent: fifteen bytes, one short of
 * SIXTEEN_BYTES. */
constexpr const char* FIFTEEN_LETTERS = "ABCDEFGHIJKLMNO";

/** @brief Sixteen letters without accent: a field of SIXTEEN_BYTES is full. */
constexpr const char* SIXTEEN_LETTERS = "ABCDEFGHIJKLMNOP";

/** @brief The letter e with an acute accent, and its two bytes in UTF-8. */
constexpr char32_t E_ACUTE = U'\xE9';
constexpr const char* E_ACUTE_BYTES = "\xC3\xA9";

/** @brief The euro sign, outside Basic Latin and Latin-1 Supplement. */
constexpr char32_t EURO_SIGN = U'\x20AC';

/** @brief The character Enter types along with its key. */
constexpr char32_t CARRIAGE_RETURN = U'\x0D';

/** @brief The character Backspace types along with its key on macOS and
 * Windows. */
constexpr char32_t BACKSPACE_CHARACTER = U'\x08';

/** @returns A field of SIXTEEN_BYTES that accepts every printable character,
 * starting with a text. */
TextField printableField(std::string_view text) {
  return {text, SIXTEEN_BYTES, TextField::Characters::Printable};
}

}  // namespace

/**
 * Given an empty field
 * When a digit is typed
 * Then the field holds that digit
 */
TEST(TextField, AnEmptyFieldTakesItsFirstCharacter) {
  TextField field("", FIVE_BYTES, TextField::Characters::Digits);

  field.type(U'7');

  EXPECT_EQ(field.text(), "7");
}

/**
 * Given a field holding AB, which accepts every printable character
 * When C is typed
 * Then the field holds ABC
 */
TEST(TextField, TypesAnAcceptedCharacterAtTheEnd) {
  TextField field = printableField("AB");

  field.type(U'C');

  EXPECT_EQ(field.text(), "ABC");
}

/**
 * Given a field of digits holding 42
 * When a letter is typed
 * Then the field still holds 42
 */
TEST(TextField, ADigitFieldRefusesALetter) {
  TextField field("42", FIVE_BYTES, TextField::Characters::Digits);

  field.type(U'a');

  EXPECT_EQ(field.text(), "42");
}

/**
 * Given a field of digits and dots holding 127
 * When a dot is typed
 * Then the field holds 127.
 */
TEST(TextField, AnAddressFieldTakesADot) {
  TextField field("127", SIXTEEN_BYTES, TextField::Characters::DigitsAndDots);

  field.type(U'.');

  EXPECT_EQ(field.text(), "127.");
}

/**
 * Given a field of digits and dots holding 127
 * When a colon is typed
 * Then the field still holds 127
 */
TEST(TextField, AnAddressFieldRefusesAColon) {
  TextField field("127", SIXTEEN_BYTES, TextField::Characters::DigitsAndDots);

  field.type(U':');

  EXPECT_EQ(field.text(), "127");
}

/**
 * Given a field holding AB, which accepts every printable character
 * When the character Enter types is typed
 * Then the field still holds AB
 */
TEST(TextField, RefusesTheCharacterOfEnter) {
  TextField field = printableField("AB");

  field.type(CARRIAGE_RETURN);

  EXPECT_EQ(field.text(), "AB");
}

/**
 * Given a field holding AB, which accepts every printable character
 * When the euro sign, outside Latin-1, is typed
 * Then the field still holds AB
 */
TEST(TextField, RefusesACharacterOutsideLatin1) {
  TextField field = printableField("AB");

  field.type(EURO_SIGN);

  EXPECT_EQ(field.text(), "AB");
}

/**
 * Given a field holding AB, which accepts every printable character
 * When an e with an acute accent is typed
 * Then the field holds AB followed by its two bytes
 */
TEST(TextField, TakesAnAccentedLetterAsItsTwoBytes) {
  TextField field = printableField("AB");

  field.type(E_ACUTE);

  EXPECT_EQ(field.text(), std::string("AB") + E_ACUTE_BYTES);
}

/**
 * Given a field of sixteen bytes holding fifteen letters
 * When one more letter is typed
 * Then the field holds sixteen letters
 */
TEST(TextField, ALetterFillsTheLastByte) {
  TextField field = printableField(FIFTEEN_LETTERS);

  field.type(U'P');

  EXPECT_EQ(field.text(), SIXTEEN_LETTERS);
}

/**
 * Given a field of sixteen bytes holding fifteen letters
 * When an accented letter of two bytes is typed
 * Then the field still holds the fifteen letters, never half a character
 */
TEST(TextField, RefusesALetterThatWouldNotFitWhole) {
  TextField field = printableField(FIFTEEN_LETTERS);

  field.type(E_ACUTE);

  EXPECT_EQ(field.text(), FIFTEEN_LETTERS);
}

/**
 * Given a full field of sixteen bytes
 * When a letter is typed
 * Then the field still holds its sixteen letters
 */
TEST(TextField, AFullFieldRefusesALetter) {
  TextField field = printableField(SIXTEEN_LETTERS);

  field.type(U'Q');

  EXPECT_EQ(field.text(), SIXTEEN_LETTERS);
}

/**
 * Given a field holding ABC
 * When a character is erased
 * Then the field holds AB
 */
TEST(TextField, ErasesTheLastCharacter) {
  TextField field = printableField("ABC");

  field.erase();

  EXPECT_EQ(field.text(), "AB");
}

/**
 * Given a field holding A and an accented letter
 * When a character is erased
 * Then the field holds A: both bytes of the accented letter are gone
 */
TEST(TextField, ErasesAnAccentedLetterWhole) {
  TextField field = printableField(std::string("A") + E_ACUTE_BYTES);

  field.erase();

  EXPECT_EQ(field.text(), "A");
}

/**
 * Given an empty field
 * When a character is erased
 * Then the field stays empty
 */
TEST(TextField, ErasingAnEmptyFieldLeavesItEmpty) {
  TextField field = printableField("");

  field.erase();

  EXPECT_EQ(field.text(), "");
}

/**
 * Given a field of digits built with a letter between two digits
 * When nothing else happens
 * Then the field holds the two digits only
 */
TEST(TextField, StartsWithTheCharactersItAccepts) {
  const TextField field("4a2", FIVE_BYTES, TextField::Characters::Digits);

  EXPECT_EQ(field.text(), "42");
}

/**
 * Given a field of five bytes built with six digits
 * When nothing else happens
 * Then the field holds the first five
 */
TEST(TextField, StartsWithWhatFitsOfItsText) {
  const TextField field("123456", FIVE_BYTES, TextField::Characters::Digits);

  EXPECT_EQ(field.text(), "12345");
}

/**
 * Given a field holding AB
 * When the window event of X typed reaches it
 * Then the field holds ABX
 */
TEST(TextField, ATypedCharacterEventTypesItsCharacter) {
  TextField field = printableField("AB");

  field.handleEvent(typedCharacter(U'X'));

  EXPECT_EQ(field.text(), "ABX");
}

/**
 * Given a field holding AB
 * When the window event of Backspace pressed reaches it
 * Then the field holds A
 */
TEST(TextField, ABackspacePressErases) {
  TextField field = printableField("AB");

  field.handleEvent(keyPress(sf::Keyboard::Key::Backspace));

  EXPECT_EQ(field.text(), "A");
}

/**
 * Given a field holding AB
 * When Backspace is pressed, and the character it types follows
 * Then the field holds A: one press erases one character
 */
TEST(TextField, BackspaceAndItsCharacterEraseOnce) {
  TextField field = printableField("AB");

  field.handleEvent(keyPress(sf::Keyboard::Key::Backspace));
  field.handleEvent(typedCharacter(BACKSPACE_CHARACTER));

  EXPECT_EQ(field.text(), "A");
}

/**
 * Given a field
 * When the character Enter types reaches it, which it refuses
 * Then the field still claims the event, so no menu reads it
 */
TEST(TextField, ClaimsATypedCharacterItRefuses) {
  TextField field = printableField("AB");

  EXPECT_TRUE(field.handleEvent(typedCharacter(CARRIAGE_RETURN)));
}

/**
 * Given a field
 * When the press of Enter reaches it
 * Then the field leaves the event to the menu
 */
TEST(TextField, LeavesOtherKeysToTheMenu) {
  TextField field = printableField("AB");

  EXPECT_FALSE(field.handleEvent(keyPress(sf::Keyboard::Key::Enter)));
}
