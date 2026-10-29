#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "DisplayMode.hpp"
#include "PlayerSettings.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingId.hpp"
#include "SettingsServiceFixture.hpp"
#include "SettingsServiceTestConstants.hpp"

using rtype::client::AUTO_FIRE_SETTING;
using rtype::client::BoundAction;
using rtype::client::COLOR_MODE_SETTING;
using rtype::client::ColorMode;
using rtype::client::DISPLAY_MODE_SETTING;
using rtype::client::DisplayMode;
using rtype::client::EFFECTS_VOLUME_SETTING;
using rtype::client::MUSIC_VOLUME_SETTING;
using rtype::client::MUTED_SETTING;
using rtype::client::PlayerSettings;
using rtype::client::SettingId;
using rtype::client::WINDOW_SIZE_SETTING;

/**
 * Given a service over the settings of a first launch
 * When the music volume is set to the lowest and to the highest volume
 * Then both values are applied
 */
TEST_F(SettingsServiceFixture, VolumeEndsAreAccepted) {
  const bool lowest = service_.apply(MUSIC_VOLUME_SETTING, LOWEST_VOLUME);
  const int storedAtLowest = settings_.musicVolume;
  const bool highest = service_.apply(MUSIC_VOLUME_SETTING, HIGHEST_VOLUME);

  EXPECT_TRUE(lowest);
  EXPECT_EQ(storedAtLowest, LOWEST_VOLUME);
  EXPECT_TRUE(highest);
  EXPECT_EQ(settings_.musicVolume, HIGHEST_VOLUME);
}

/**
 * Given a service over the settings of a first launch
 * When the music volume is set one below the lowest volume
 * Then it is refused and the volume is unchanged
 */
TEST_F(SettingsServiceFixture, VolumeBelowTheLowestIsRefused) {
  const PlayerSettings before = settings_;

  const bool applied =
      service_.apply(MUSIC_VOLUME_SETTING, BELOW_LOWEST_VOLUME);

  EXPECT_FALSE(applied);
  EXPECT_EQ(settings_, before);
}

/**
 * Given a service over the settings of a first launch
 * When the effects volume is set one above the highest volume
 * Then it is refused and the volume is unchanged
 */
TEST_F(SettingsServiceFixture, VolumeAboveTheHighestIsRefused) {
  const PlayerSettings before = settings_;

  const bool applied =
      service_.apply(EFFECTS_VOLUME_SETTING, ABOVE_HIGHEST_VOLUME);

  EXPECT_FALSE(applied);
  EXPECT_EQ(settings_, before);
}

/**
 * Given a service with a listener
 * When a volume out of range is refused
 * Then the listener is told nothing and there is nothing to save
 */
TEST_F(SettingsServiceFixture, ARefusedValueTellsNobodyAndSavesNothing) {
  const bool applied =
      service_.apply(MUSIC_VOLUME_SETTING, ABOVE_HIGHEST_VOLUME);

  EXPECT_FALSE(applied);
  EXPECT_TRUE(listener_.changes().empty());
  EXPECT_FALSE(service_.saveIfChanged().has_value());
}

/**
 * Given a service with a listener
 * When the effects volume is set
 * Then the listener is told the effects volume changed, once
 */
TEST_F(SettingsServiceFixture, AnAppliedVolumeTellsTheListenerOnce) {
  const bool applied = service_.apply(EFFECTS_VOLUME_SETTING, A_VOLUME);

  EXPECT_TRUE(applied);
  EXPECT_EQ(listener_.changes(),
            std::vector<SettingId>{SettingId::EffectsVolume});
}

/**
 * Given a service
 * When a switch and the color mode are set
 * Then the settings hold both values
 */
TEST_F(SettingsServiceFixture, SwitchesAndColorModeAreApplied) {
  const bool muted = service_.apply(MUTED_SETTING, true);
  const bool colored =
      service_.apply(COLOR_MODE_SETTING, ColorMode::Protanopia);

  EXPECT_TRUE(muted);
  EXPECT_TRUE(colored);
  EXPECT_TRUE(service_.settings().muted);
  EXPECT_EQ(service_.settings().colorMode, ColorMode::Protanopia);
}

/**
 * Given a service where the auto fire was set
 * When the service is asked to save twice
 * Then the first save reaches the storage and the second has nothing to save
 */
TEST_F(SettingsServiceFixture, AChangeIsSavedOnce) {
  const bool applied = service_.apply(AUTO_FIRE_SETTING, true);

  const std::optional<bool> first = service_.saveIfChanged();
  const std::optional<bool> second = service_.saveIfChanged();

  EXPECT_TRUE(applied);
  EXPECT_EQ(first, std::optional<bool>(true));
  EXPECT_FALSE(second.has_value());
  ASSERT_EQ(storage_.saves().size(), 1U);
  EXPECT_TRUE(storage_.saves().front().autoFire);
}

/**
 * Given a storage that refuses saves and a changed setting
 * When the service saves, then saves again without another change
 * Then the first save reports the refusal and the second has nothing to save
 */
TEST_F(SettingsServiceFixture, ARefusedSaveIsReportedAndNotRetried) {
  storage_.refuseSaves(true);
  const bool applied = service_.apply(MUSIC_VOLUME_SETTING, A_VOLUME);

  const std::optional<bool> first = service_.saveIfChanged();
  const std::optional<bool> second = service_.saveIfChanged();

  EXPECT_TRUE(applied);
  EXPECT_EQ(first, std::optional<bool>(false));
  EXPECT_FALSE(second.has_value());
}

/**
 * Given a service with a listener and sound settings away from their defaults
 * When the sound is reset
 * Then the settings are those of a first launch and the listener is told the
 * music, the effects and MUTE ALL changed
 */
TEST_F(SettingsServiceFixture, ResettingTheSoundTellsTheThreeSoundSettings) {
  const bool music = service_.apply(MUSIC_VOLUME_SETTING, A_VOLUME);
  const bool muted = service_.apply(MUTED_SETTING, true);
  listener_.forgetChanges();

  service_.resetSound();

  EXPECT_TRUE(music);
  EXPECT_TRUE(muted);
  EXPECT_EQ(settings_.musicVolume, PlayerSettings{}.musicVolume);
  EXPECT_FALSE(settings_.muted);
  EXPECT_EQ(
      listener_.changes(),
      (std::vector<SettingId>{SettingId::MusicVolume, SettingId::EffectsVolume,
                              SettingId::Muted}));
}

/**
 * Given a service with a listener
 * When a key is bound, the window size and the display mode are set
 * Then the settings hold the three and the listener is told Keys, WindowSize
 * and DisplayMode in that order
 */
TEST_F(SettingsServiceFixture, KeysWindowSizeAndDisplayModeAreAppliedAndTold) {
  service_.bindKey(BoundAction::Shoot, A_KEY);
  const bool sized =
      service_.apply(WINDOW_SIZE_SETTING, std::optional(A_WINDOW_SIZE));
  const bool displayed =
      service_.apply(DISPLAY_MODE_SETTING, DisplayMode::Borderless);

  EXPECT_TRUE(sized);
  EXPECT_TRUE(displayed);
  EXPECT_EQ(settings_.keys.keyOf(BoundAction::Shoot), A_KEY);
  EXPECT_EQ(settings_.windowSize, std::optional(A_WINDOW_SIZE));
  EXPECT_EQ(settings_.displayMode, DisplayMode::Borderless);
  EXPECT_EQ(listener_.changes(),
            (std::vector<SettingId>{SettingId::Keys, SettingId::WindowSize,
                                    SettingId::DisplayMode}));
}

/**
 * Given a service whose listener was subscribed and then unsubscribed
 * When a volume is set
 * Then the listener is told nothing
 */
TEST_F(SettingsServiceFixture, AnUnsubscribedListenerIsToldNothing) {
  service_.unsubscribe(listener_);

  const bool applied = service_.apply(MUSIC_VOLUME_SETTING, A_VOLUME);

  EXPECT_TRUE(applied);
  EXPECT_TRUE(listener_.changes().empty());
}
