#include "FakeUpdateNotice.hpp"

bool FakeUpdateNotice::updateAvailable() const { return updateAvailable_; }

bool FakeUpdateNotice::notesWaiting() const { return notesWaiting_; }

void FakeUpdateNotice::offerUpdate(bool available) {
  updateAvailable_ = available;
}

void FakeUpdateNotice::leaveNotes(bool waiting) { notesWaiting_ = waiting; }
