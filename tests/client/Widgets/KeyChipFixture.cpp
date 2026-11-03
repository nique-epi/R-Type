#include "KeyChipFixture.hpp"
#include <SFML/Window/Event.hpp>
#include "KeyChip.hpp"
#include "KeyChipParts.hpp"
#include "KeyChipTestConstants.hpp"
#include "PointerEvents.hpp"
#include "RecordingSoundPlayer.hpp"

using rtype::client::KeyChip;
using rtype::client::KeyChipParts;
using rtype::client::testing::CHIP_AREA;
using rtype::client::testing::CHIP_KEY;

KeyChipFixture::KeyChipFixture()
    : chip_(KeyChipParts{.text = {.label = "FILTER", .keyName = "F"},
                         .area = CHIP_AREA,
                         .key = CHIP_KEY,
                         .action = [this] { ++activations_; },
                         .sounds = sounds_}) {}

bool KeyChipFixture::give(const sf::Event& event) {
  return chip_.handleEvent(event, samePlayfieldPoint);
}

int KeyChipFixture::activations() const { return activations_; }

const RecordingSoundPlayer& KeyChipFixture::sounds() const { return sounds_; }

KeyChip& KeyChipFixture::chip() { return chip_; }
