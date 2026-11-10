#pragma once

#include "IUpdateNotice.hpp"

/** @brief An update module whose answers the test sets. */
class FakeUpdateNotice : public rtype::client::IUpdateNotice {
 public:
  [[nodiscard]] bool updateAvailable() const override;
  [[nodiscard]] bool notesWaiting() const override;

  void offerUpdate(bool available);
  void leaveNotes(bool waiting);

 private:
  bool updateAvailable_{};
  bool notesWaiting_{};
};
