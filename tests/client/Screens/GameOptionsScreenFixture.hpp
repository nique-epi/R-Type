#pragma once

#include "OptionsScreenFixture.hpp"

/** @brief The options over a game: the same fixture in every use, opened over
 * the game rather than the title, so it derives from OptionsScreenFixture
 * instead of holding one. */
class GameOptionsScreenFixture : public OptionsScreenFixture {
 protected:
  GameOptionsScreenFixture();
};
