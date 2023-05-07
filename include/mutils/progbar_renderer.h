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


/**
 *
 * progbar_renderer.h
 *
 * Renderers for the terminal progress bar utility
 *
 */

#pragma once
#include "./ansi.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <math.h>
#include <string>
#ifdef WIN32

#  include <windows.h>

#else

#  include <sys/ioctl.h>
#  include <unistd.h>

#endif

using std::size_t;

namespace mutils::progbar {

//
// Internal base class for the Render
//
// DO NOT USE
//
class __AnyRenderer {
public:
  virtual ~__AnyRenderer() = default;
};

struct BaseRendererConfiguration {
  enum RenderAlignment {
    Left,
    Right,
    Center
  } alignment = Center;
};

//
// A renderer is a simple utility which generates a progress bar
//
template <class RendererStateType> class Renderer : __AnyRenderer {
protected:
  RendererStateType m_state;

public:
  using StateType = RendererStateType;

  RendererStateType get_state() {
    return m_state;
  }

  void set_state(RendererStateType st) {
    this->m_state = st;
  }

  /**
   * Render the bar with the given state, and yield
   */
  virtual std::string render(size_t max_steps, size_t current_steps) = 0;
  virtual int         render_height()                                = 0;
};

namespace renderers {

struct LineRendererState : BaseRendererConfiguration {
  char line_unit_active   = '=';
  char line_unit_inactive = '~';

  char box_corners     = '+';
  char box_horizontals = '-';
  char box_verticals   = '|';

  ansi::Color active_color   = ansi::Color::Green;
  ansi::Color inactive_color = ansi::Color::Red;

  ansi::Color title_color      = ansi::Color::BrightCyan;
  ansi::Color percentage_color = ansi::Color::BrightYellow;

  float width = 0.8; // The width of the bar as a fraction of the window width

  enum class NumericProgression {
    Fraction,
    Percentage,
    None
  } numeric_progression = NumericProgression::Percentage;

  enum class ProgressionDirection {
    Left,
    Right,
    Outwards,
    Inwards
  } progresses = ProgressionDirection::Outwards;

  std::string title      = "Testing";
  bool        show_title = true;
};

class BoxedLineRenderer : public Renderer<LineRendererState> {
public:
  int render_height() override {
    return 3;
  }

  std::string render(size_t max_steps, size_t current_steps) override {

    // Step 1: Determine the width of the terminal
#ifdef WIN32

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    size_t term_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;

#else

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    size_t term_width = w.ws_col;

#endif

    // 2. Determine the width of the bar
    size_t render_width = (float)term_width * m_state.width;

    // 3. Determine how much to pad the left of the bar by
    size_t padding_count = 0;

    if (m_state.alignment == BaseRendererConfiguration::RenderAlignment::Left) {
      padding_count = 0;
    } else if (m_state.alignment == BaseRendererConfiguration::RenderAlignment::Right) {
      padding_count = term_width - render_width;
    } else if (m_state.alignment == BaseRendererConfiguration::RenderAlignment::Center) {
      padding_count = (term_width - render_width) / 2;
    }

    std::string BAR = "";

    pad(BAR, padding_count);
    // Render the title

    if (!m_state.show_title) {

      //
      // No title, just padding
      //

      BAR += m_state.box_corners;
      pad(BAR, render_width - 2, m_state.box_horizontals);
      BAR += m_state.box_corners;

    } else {

      //
      // We draw a title centered above the bar
      //
      BAR += m_state.box_corners;
      put_centrally(BAR, render_width - 2, m_state.title, m_state.title_color);
      BAR += m_state.box_corners;
    }
    BAR += "\n";
    pad(BAR, padding_count);
    BAR += m_state.box_verticals;
    BAR += ' ';

    size_t bar_width    = render_width - 4;
    float  active_ratio = (float)current_steps / (float)max_steps;

    float factive_count   = active_ratio * bar_width;
    float finactive_count = (1 - active_ratio) * bar_width;

    int active_decimal   = (int)(factive_count * 100) % 100;
    int inactive_decimal = (int)(finactive_count * 100) % 100;

    if (active_decimal > inactive_decimal) {
      factive_count   = ceil(factive_count);
      finactive_count = floor(finactive_count);
    } else {
      factive_count   = floor(factive_count);
      finactive_count = ceil(finactive_count);
    }

    int active_count   = factive_count;
    int inactive_count = finactive_count;

    switch (m_state.progresses) {

      case LineRendererState::ProgressionDirection::Left: {

        // Paint inactives, then actives
        std::string progress_line = ansi::FormatBuilder()
                                        .fg(m_state.inactive_color)
                                        .bold()
                                        .write(std::string(inactive_count, m_state.line_unit_inactive))
                                        .fg(m_state.active_color)
                                        .write(std::string(active_count, m_state.line_unit_active))
                                        .str(true);
        BAR += progress_line;
      } break;

      case LineRendererState::ProgressionDirection::Right: {
        // Paint actives, then inactives
        std::string progress_line = ansi::FormatBuilder()
                                        .fg(m_state.active_color)
                                        .bold()
                                        .write(std::string(active_count, m_state.line_unit_active))
                                        .fg(m_state.inactive_color)
                                        .write(std::string(inactive_count, m_state.line_unit_inactive))
                                        .str(true);
        BAR += progress_line;
      } break;
      case LineRendererState::ProgressionDirection::Inwards: {
        int render_actives_right = active_count / 2;
        int render_actives_left  = render_actives_right;

        // Odd number of inactives, add an extra to the left as integer division
        // always rounds down
        if (active_count % 2 == 1) {
          inactive_count++;
          // render_inactives_left++;
        }
        std::string progress_line = ansi::FormatBuilder()
                                        .fg(m_state.active_color)
                                        .bold()
                                        .write(std::string(render_actives_left, m_state.line_unit_active))
                                        .fg(m_state.inactive_color)
                                        .write(std::string(inactive_count, m_state.line_unit_inactive))
                                        .fg(m_state.active_color)
                                        .write(std::string(render_actives_right, m_state.line_unit_active))
                                        .str(true);
        BAR += progress_line;
      } break;
      case LineRendererState::ProgressionDirection::Outwards: {
        int render_inactives_right = inactive_count / 2;
        int render_inactives_left  = render_inactives_right;

        // Odd number of inactives, add an extra to the left as integer division
        // always rounds down
        if (inactive_count % 2 == 1) {
          active_count++;
          // render_inactives_left++;
        }
        std::string progress_line = ansi::FormatBuilder()
                                        .fg(m_state.inactive_color)
                                        .bold()
                                        .write(std::string(render_inactives_left, m_state.line_unit_inactive))
                                        .fg(m_state.active_color)
                                        .write(std::string(active_count, m_state.line_unit_active))
                                        .fg(m_state.inactive_color)
                                        .write(std::string(render_inactives_right, m_state.line_unit_inactive))
                                        .str(true);
        BAR += progress_line;
      } break;
    }
    BAR += ' ';
    BAR += m_state.box_verticals;
    BAR += '\n';
    pad(BAR, padding_count);

    //
    //
    // Status text representation
    //
    //
    BAR += m_state.box_corners;
    switch (m_state.numeric_progression) {
      case LineRendererState::NumericProgression::None: {
        pad(BAR, render_width - 2, m_state.box_horizontals);
      } break;
      case LineRendererState::NumericProgression::Fraction: {
        std::string fraction;

        if (current_steps == max_steps) {
          put_centrally(BAR, render_width - 2, "Done", m_state.percentage_color);
          break;
        }
        fraction += std::to_string(current_steps);
        fraction += " / ";
        fraction += std::to_string(max_steps);

        put_centrally(BAR, render_width - 2, fraction, m_state.percentage_color);
      } break;
      case LineRendererState::NumericProgression::Percentage: {
        if (current_steps == max_steps) {
          put_centrally(BAR, render_width - 2, "Done", m_state.percentage_color);
          break;
        }
        std::string percent;
        percent += std::to_string((int)(active_ratio * 100));
        percent += "%";
        put_centrally(BAR, render_width - 2, percent, m_state.percentage_color);

      } break;
    }
    BAR += m_state.box_corners;
    return BAR;
  };

private:
  static void pad(std::string& txt, size_t by, char c = ' ') {
    for (size_t i = 0; i < by; i++) {
      txt += c;
    }
  }

  void put_centrally(std::string& txt, size_t max_width, std::string text, ansi::Color color = ansi::Color::White) {
    bool requires_title_correction = max_width % 2 != text.size() % 2;

    // We need to subtract one for titles where its even-ness doesnt match up
    // with the dimensions, this means that titles will be slightly off-center
    int title_width_correction = requires_title_correction ? 1 : 0;

    // Pad until the center - half the width of the text
    size_t pad_count = (max_width / 2) - (text.size() / 2);
    pad(txt, pad_count - title_width_correction, m_state.box_horizontals);

    // append the title
    txt += ansi::FormatBuilder().fg(color).write(text).str();

    // pad the other end
    pad(txt, pad_count, m_state.box_horizontals);
  }
};

}; // namespace renderers

}; // namespace mutils::progbar
