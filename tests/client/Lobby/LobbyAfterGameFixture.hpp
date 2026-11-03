#pragma once

#include "LobbyScreenFixture.hpp"

/** @brief The waiting room opened alone after a game: the same fixture in
 * every use, built with LobbyPlace::AfterGame rather than after the
 * connection screen, so it derives from LobbyScreenFixture instead of
 * holding one. */
class LobbyAfterGameFixture : public LobbyScreenFixture {
 protected:
  LobbyAfterGameFixture();
};
