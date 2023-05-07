/// Copyright (c) 2023 Samir Bioud
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
/// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
/// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
/// OR OTHER DEALINGS IN THE SOFTWARE.
/// 


#include "../include/mutils/ansi.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <ranges>
#include <termios.h>
#include <unistd.h>
using namespace mutils;

const int ANSI_FG_OFFSET = 30;
const int ANSI_BG_OFFSET = 40;
const char *ANSI_GENERAL_ESCAPE_SEQ = "\033[";
const char *ANSI_FORMATTING_ESCAPE_SEQ = "\033[0";

#define escape(c) ("\033[" c)

const int ANSI_CUSTOM_BG_CODE = 48;
const int ANSI_CUSTOM_FG_CODE = 38;
const int ANSI_CUSTOM_COLOR_INDICATE_RGB = 2;

ansi::RGBColorSequence ansi::rgb(uint8_t red, uint8_t green, uint8_t blue) {
  return {red, green, blue};
}

ansi::FormatBuilder &ansi::FormatBuilder::fg(ansi::AnsiColor color) {
  this->fmt.foreground = color;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::bg(ansi::AnsiColor color) {
  this->fmt.background = color;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::blink_fast() {
  fmt.m_effects.blink = FormatConfiguration::AnsiTextEffects::Fast;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::blink_slow() {
  fmt.m_effects.blink = FormatConfiguration::AnsiTextEffects::Slow;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::no_blink() {
  fmt.m_effects.blink = FormatConfiguration::AnsiTextEffects::None;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::bold() {
  fmt.m_effects.intensity = fmt.m_effects.Bold;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::faint() {
  fmt.m_effects.intensity = fmt.m_effects.Faint;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::no_intensity() {
  fmt.m_effects.intensity = fmt.m_effects.Normal;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::encircle() {
  fmt.m_effects.encircled = true;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::no_encircle() {
  fmt.m_effects.encircled = false;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::gothic() {
  fmt.m_effects.fraktur = true;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::no_gothic() {
  fmt.m_effects.fraktur = false;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::underline() {
  fmt.m_effects.underline = true;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::no_underline() {
  fmt.m_effects.underline = false;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::frame() {
  fmt.m_effects.framed = true;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::no_frame() {
  fmt.m_effects.framed = false;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::hide() {
  fmt.m_effects.hidden = true;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::show() {
  fmt.m_effects.hidden = false;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::italic() {
  fmt.m_effects.italic = true;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::no_italic() {
  fmt.m_effects.italic = false;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::strikethrough() {
  fmt.m_effects.crossout = true;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::no_strikethrough() {
  fmt.m_effects.crossout = false;
  return *this;
}

ansi::FormatBuilder &ansi::FormatBuilder::write(const std::string &str) {

  std::string modifier_str = ANSI_FORMATTING_ESCAPE_SEQ;

  // Font Color
  if (fmt.foreground.variant == AnsiColor::PRESET) {
    modifier_str +=
        ";" + std::to_string((int)fmt.foreground.data.preset + ANSI_FG_OFFSET);
  } else if (fmt.foreground.variant == AnsiColor::RGB) {
    modifier_str += ";38;2";
    modifier_str += ";" + std::to_string(fmt.foreground.data.rgb.Red);
    modifier_str += ";" + std::to_string(fmt.foreground.data.rgb.Green);
    modifier_str += ";" + std::to_string(fmt.foreground.data.rgb.Blue);
  }

  // Background Color
  if (fmt.background.variant == AnsiColor::PRESET) {
    modifier_str +=
        ";" + std::to_string((int)fmt.background.data.preset + ANSI_BG_OFFSET);
  } else if (fmt.background.variant == AnsiColor::RGB) {
    modifier_str += ";48;2";
    modifier_str += ";" + std::to_string(fmt.background.data.rgb.Red);
    modifier_str += ";" + std::to_string(fmt.background.data.rgb.Green);
    modifier_str += ";" + std::to_string(fmt.background.data.rgb.Blue);
  }

  // Simple Effects
  if (fmt.m_effects.italic)
    modifier_str += ";3";
  if (fmt.m_effects.hidden)
    modifier_str += ";8";
  if (fmt.m_effects.crossout)
    modifier_str += ";9";
  if (fmt.m_effects.fraktur)
    modifier_str += ";20";
  if (fmt.m_effects.framed)
    modifier_str += ";51";
  if (fmt.m_effects.encircled)
    modifier_str += ";52";
  if (fmt.m_effects.overlined)
    modifier_str += ";53";

  // Effects with multiple modes
  if (fmt.m_effects.blink == FormatConfiguration::AnsiTextEffects::Fast)
    modifier_str += ";6";
  else if (fmt.m_effects.blink == FormatConfiguration::AnsiTextEffects::Slow)
    modifier_str += ";5";

  if (fmt.m_effects.intensity == FormatConfiguration::AnsiTextEffects::Bold)
    modifier_str += ";1";
  else if (fmt.m_effects.intensity ==
           FormatConfiguration::AnsiTextEffects::Faint)
    modifier_str += ";2";

  m_data += modifier_str + "m" + str;
  return *this;
}
ansi::FormatBuilder &ansi::FormatBuilder::reset() {
  fmt.m_effects = FormatConfiguration::AnsiTextEffects();
  fmt.foreground = AnsiColor();
  fmt.background = AnsiColor();
  return *this;
}
std::string ansi::FormatBuilder::str(bool terminate) {
  if (terminate) {
    m_data += ANSI_FORMATTING_ESCAPE_SEQ + std::string("m");
  }
  return m_data;
}

ansi::FormatBuilder &
ansi::FormatBuilder::import_config(FormatConfiguration cfg) {
  fmt = cfg;
  return *this;
}
ansi::FormatBuilder::FormatConfiguration ansi::FormatBuilder::export_config() {
  return fmt;
}

//
//
// CURSOR BUILDER FUNCTIONS
//
//

ansi::CursorBuilder &ansi::CursorBuilder::up(unsigned int nlines) {
  data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(nlines) + "A";
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::down(unsigned int nlines) {
  data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(nlines) + "B";
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::left(unsigned int ncols) {
  data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(ncols) + "D";
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::right(unsigned int ncols) {
  data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(ncols) + "C";
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::col(unsigned int n) {
  data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(n) + "G";
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::goto_pos(unsigned int row,
                                                   unsigned int col) {
  data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(row) + ";" +
          std::to_string(col) + "f";
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::erase_line_from_cursor() {
  data += escape("K");
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::erase_line() {
  data += escape("2K");
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::erase_display_from_cursor() {
  data += escape("J");
  return *this;
}
ansi::CursorBuilder &ansi::CursorBuilder::erase_display_and_return_home() {
  data += escape("2J");
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::scroll_up(unsigned int nlines) {
  data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(nlines) + "S";
  return *this;
}

ansi::CursorBuilder &ansi::CursorBuilder::scroll_down(unsigned int nlines) {
  data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(nlines) + "T";
  return *this;
}
ansi::CursorBuilder &ansi::CursorBuilder::print(const std::string &text) {
  data += text;
  return *this;
}
std::string ansi::CursorBuilder::str() { return data; }

ansi::CursorBuilder::CursorPosition ansi::CursorBuilder::get_cursor_pos() {
  ansi::CursorBuilder::CursorPosition pos;

  // write(1, "\x1B[6n", 4);
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw);
  raw.c_lflag &= ~(ICANON | ECHO | IEXTEN | ISIG);
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  char buf[32];
  unsigned int i = 0;

  /* Read the response: ESC [ rows ; cols R */
  while (i < sizeof(buf) - 1) {
    if (read(0, buf + i, 1) != 1)
      break;
    if (buf[i] == 'R') {
      break;
    }
    i++;
  }
  buf[i] = '\0';
  struct termios def;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &def);
  std::string s((char *)buf);
  // std::cout << s << std::endl;
  // printf("\nResult:%s\n", buf);
  return pos;
}
