/**
 * progbar.h
 *
 * A simple utility for creating CLI progress bars
 *
 */

#pragma once

#include "ansi.h"
#include "progbar_renderer.h"
#include <iostream>
#include <stdexcept>
namespace mutils::progbar {

enum class DrawMode {

  Inline, // Continuously redraw the bar over the same area of the terminal
  Repeat, // Redraw new iterations of the bar on new lines of the terminal

};

template <class _Renderer> class ProgressBarBase {
  static_assert(std::is_base_of<__AnyRenderer, _Renderer>::value,
                "ProgressBarBase takes a renderer as its template argument");

  _Renderer m_renderer;
  DrawMode mode = DrawMode::Repeat;
  bool finished = false;

public:
  ProgressBarBase() : m_renderer(_Renderer()) {}

  typename _Renderer::StateType get_renderer_state() {
    return m_renderer.get_state();
  }

  void set_draw_mode(DrawMode m) { this->mode = m; }

  void set_renderer_state(typename _Renderer::StateType st) {
    m_renderer.set_state(st);
  }

  virtual void draw() = 0;
  ~ProgressBarBase() { this->m_finish(); }

protected:
  void m_draw(size_t max_steps, size_t current_steps) {

    auto text = m_renderer.render(max_steps, current_steps);
    std::cout << text << std::endl;

    std::cout << ansi::CursorBuilder()
                     .up(m_renderer.render_height() + 1)
                     .col(0)
                     .str();
  }

  void m_finish() {
    if (!finished)
      std::cout << ansi::CursorBuilder().down(m_renderer.render_height()).str()
                << std::endl;
    finished = true;
  }
};
//
// A simple bar to display progress as a fraction of known steps completed
//
template <class Renderer = renderers::BoxedLineRenderer>
class Stepped : public ProgressBarBase<Renderer> {

  size_t m_step_cnt;
  size_t m_current_step = 0;

public:
  Stepped(size_t step_count) { this->m_step_cnt = step_count; }

  void step() {

    if (m_current_step == m_step_cnt) {
      throw std::runtime_error("Cannot call step() on a progress bar which has "
                               "already reached 100%");
    }
    m_current_step++;
  }

  void draw() {
    this->m_draw(m_step_cnt, m_current_step);
    if (m_current_step == m_step_cnt)
      this->m_finish();
  }
};

}; // namespace mutils::progbar
