#include <gtest/gtest.h>
#include "Language.hpp"
#include "TranslationTestConstants.hpp"

using rtype::client::Language;

/**
 * Given a text written in English and in French
 * When it is asked in English
 * Then it gives its English
 */
TEST(TranslationTest, GivesItsEnglishInEnglish) {
  EXPECT_EQ(SOME_TRANSLATION.in(Language::English), SOME_ENGLISH);
}

/**
 * Given a text written in English and in French
 * When it is asked in French
 * Then it gives its French
 */
TEST(TranslationTest, GivesItsFrenchInFrench) {
  EXPECT_EQ(SOME_TRANSLATION.in(Language::French), SOME_FRENCH);
}
