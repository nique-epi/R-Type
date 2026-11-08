#include "LineEditor.hpp"
#include <span>
#include <string>
#include <utility>
#include "AdminConsoleConstants.hpp"
#include "ConsoleChunk.hpp"
#include "EscapeState.hpp"
#include "LineEdit.hpp"

namespace rtype::server {

namespace {

/** @returns Whether @p byte is text to keep: a printable ASCII character or a
 * byte of a UTF-8 character. */
bool isText(char byte) {
  return static_cast<unsigned char>(byte) >=
             static_cast<unsigned char>(FIRST_PRINTABLE) &&
         byte != DELETE_CHARACTER;
}

/** @returns Whether @p byte continues a UTF-8 character. */
bool continuesCharacter(char byte) {
  return (static_cast<unsigned char>(byte) & UTF8_TOP_BITS) ==
         UTF8_CONTINUATION;
}

/** @returns Whether @p byte ends the sequence of a key. */
bool isFinalByte(char byte) {
  return byte >= FIRST_FINAL_BYTE && byte <= LAST_FINAL_BYTE;
}

}  // namespace

LineEdit LineEditor::feed(std::span<const char> bytes) {
  LineEdit edit;
  for (const char byte : bytes) {
    if (!skipSequence(byte)) {
      read(byte, edit);
    }
  }
  return edit;
}

ConsoleChunk LineEditor::finish() {
  ConsoleChunk chunk;
  if (!typed_.empty()) {
    chunk.lines.push_back(std::exchange(typed_, {}));
  }
  return chunk;
}

const std::string& LineEditor::typed() const { return typed_; }

bool LineEditor::skipSequence(char byte) {
  switch (escape_) {
    case EscapeState::None:
      return false;
    case EscapeState::Started:
      if (byte != CONTROL_SEQUENCE_OPENER && byte != FUNCTION_SEQUENCE_OPENER) {
        escape_ = EscapeState::None;
        return false;
      }
      escape_ = EscapeState::Sequence;
      return true;
    case EscapeState::Sequence:
      if (isFinalByte(byte)) {
        escape_ = EscapeState::None;
      }
      return true;
  }
  return false;
}

void LineEditor::read(char byte, LineEdit& edit) {
  const bool endOfAPair = byte == LINE_END && afterCarriageReturn_;
  afterCarriageReturn_ = byte == CARRIAGE_RETURN;
  if (endOfAPair) {
    return;
  }
  if (byte == CARRIAGE_RETURN || byte == LINE_END) {
    endLine(edit);
  } else if (byte == BACKSPACE || byte == DELETE_CHARACTER) {
    erase(edit);
  } else if (byte == ESCAPE) {
    escape_ = EscapeState::Started;
  } else if (isText(byte)) {
    keep(byte, edit);
  }
}

void LineEditor::endLine(LineEdit& edit) {
  edit.chunk.lines.push_back(std::exchange(typed_, {}));
  edit.echo += ROW_END;
  edit.echo += CONSOLE_PROMPT;
}

void LineEditor::erase(LineEdit& edit) {
  if (typed_.empty()) {
    return;
  }
  while (typed_.size() > 1 && continuesCharacter(typed_.back())) {
    typed_.pop_back();
  }
  typed_.pop_back();
  edit.echo += ERASE_CHARACTER;
}

void LineEditor::keep(char byte, LineEdit& edit) {
  if (typed_.size() >= MAXIMUM_CONSOLE_LINE_LENGTH) {
    return;
  }
  typed_.push_back(byte);
  edit.echo.push_back(byte);
}

}  // namespace rtype::server
