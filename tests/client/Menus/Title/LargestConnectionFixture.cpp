#include "LargestConnectionFixture.hpp"
#include "ConnectionGoal.hpp"
#include "ConnectionScreenFixture.hpp"
#include "ConnectionScreenTestConstants.hpp"

LargestConnectionFixture::LargestConnectionFixture()
    : ConnectionScreenFixture(rtype::client::ConnectionGoal::WaitingRoom,
                              rtype::client::testing::LARGEST_FORM_TEXT) {}
