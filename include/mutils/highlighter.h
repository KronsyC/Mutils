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


//
// highlighter.h
//
// Construct highlighted text based on a file span
//
// This can be used for compiler error outlines and other things
//
//

#pragma once

#include "ansi.h"
#include "file.h"
#include <algorithm>
#include <cstdint>

namespace mutils {

struct SpanHighlighter {

  struct LineNumberConfig {
    bool show = true;
    std::string separator = " | ";

    ansi::FormatBuilder::FormatConfiguration appearance;
    ansi::FormatBuilder::FormatConfiguration separator_appearance;

    ansi::FormatBuilder::FormatConfiguration highlight_appearance;
  } line_numbers;

  struct ContextConfig {
    size_t lines_top = 3;
    size_t lines_bottom = 3;

    ansi::FormatBuilder::FormatConfiguration context_fmt;
  } ctx;

  struct LabelsConfig {
    bool show_comment = true;
    std::string comment_text = "Some Text";

    bool show_aside = true;
    std::string aside_text = "Some Elaboration";
  } labels;

  struct EmphasizerConfig {

    char underline_char = '-';
    ansi::FormatBuilder::FormatConfiguration underline_fmt;

    std::string sideline = "   |";
    std::string sideline_cap = "  <-";
    ansi::FormatBuilder::FormatConfiguration sideline_fmt;

    ansi::FormatBuilder::FormatConfiguration text_fmt;
  } emph;

  SpanHighlighter() {
    ctx.context_fmt = ansi::FormatBuilder().faint().italic().export_config();
    emph.text_fmt =
        ansi::FormatBuilder().bold().fg(ansi::Color::BrightRed).export_config();

    line_numbers.highlight_appearance =
        ansi::FormatBuilder().bold().export_config();
    line_numbers.separator_appearance = ansi::FormatBuilder()
                                            .italic()
                                            .fg(ansi::rgb(180, 180, 180))
                                            .export_config();

    emph.sideline_fmt =
        ansi::FormatBuilder().bold().fg(ansi::Color::BrightRed).export_config();
  }

  std::string highlight(TextFile::Span &span) {

    auto span_start = span.start_pos().line_no;
    auto span_stop = span.stop_pos().line_no;

    // Determine the number of context lines we can give in both directions

    auto top_hl_cnt = std::min(span_start - 1, ctx.lines_top);

    auto remaining_lines_below = span.file.lines().size() - span_stop - 1;
    auto btm_hl_cnt = std::min(remaining_lines_below, ctx.lines_bottom);

    // Generate the context lines

    auto lines_above =
        generate_context_lines(span, span_start - top_hl_cnt, top_hl_cnt);

    auto lines_below = generate_context_lines(span, span_stop + 1, btm_hl_cnt);

    std::string text;
    text += lines_above;

    if (span_start == span_stop) {
      // Single-line highlight, simple case

      text += generate_singleline_highlight(span);
    } else {
      // Multi-Line highlight, slightly more advanced
      text += generate_multiline_highlight(span);
    }

    text += "\n";
    text += lines_below;
    // text += top_context + "\n" + btm_context;
    return text;
  }

private:
  std::string format_text(std::string_view text,
                          ansi::FormatBuilder::FormatConfiguration cfg) {
    return ansi::FormatBuilder()
        .import_config(cfg)
        .write(std::string(text))
        .str();
  }

  std::string generate_singleline_highlight(TextFile::Span &span) {
    auto start = span.start_pos();
    auto stop = span.stop_pos();

    auto line = span.file.source_line_index(start.line_no);

    auto pre_context_count = (span.start - 1 - line.start_idx) + 1;
    auto post_context_count = line.end_idx + 1 - span.stop;

    auto highlight_count = span.length();
    std::string txt;

    txt += generate_line_number(span, start.line_no, true);

    if (pre_context_count != 0) {
      txt += format_text(span.file.data_between(line.start_idx, span.start - 1),
                         ctx.context_fmt);
    }

    txt += format_text(span.file.data_between(span.start, span.stop),
                       emph.text_fmt);

    if (post_context_count != 0) {
      txt += format_text(span.file.data_between(span.stop + 1, line.end_idx),
                         ctx.context_fmt);
    }

    txt += "\n";

    //
    // Underline
    //

    auto line_number_len =
        std::to_string(start.line_no).size() + line_numbers.separator.size();
    txt += std::string(line_number_len, ' ');

    txt += std::string(pre_context_count, ' ');

    txt += format_text(std::string(highlight_count, emph.underline_char),
                       emph.underline_fmt);

    txt += std::string(post_context_count, ' ');
    return txt;
  }
  std::string generate_multiline_highlight(TextFile::Span &span) {

    auto start = span.start_pos();
    auto stop = span.stop_pos();

    auto starting_line_info = span.file.source_line_index(start.line_no);
    auto final_line_info = span.file.source_line_index(stop.line_no);

    auto starting_line_len =
        starting_line_info.end_idx - starting_line_info.start_idx + 1;

    //
    // Find the length of the longest highlighted line
    //

    size_t max = 0;

    for (size_t line = start.line_no; line <= stop.line_no; line++) {
      auto cnt = span.file.get_line(line).size();

      max = std::max(max, cnt);
    }

    //
    // 1. First line
    //

    std::string data;
    data += generate_line_number(span, start.line_no, true);

    auto unhighlighted_text_cnt =
        span.start - 1 - starting_line_info.start_idx + 1;

    auto highlighted_text_cnt = starting_line_len - unhighlighted_text_cnt;

    auto unhighlighted_text_end = span.start - 1;
    auto highlighted_text_start =
        starting_line_info.end_idx - highlighted_text_cnt + 1;

    data += format_text(span.file.data_between(starting_line_info.start_idx,
                                               unhighlighted_text_end),
                        ctx.context_fmt);
    data += format_text(span.file.data_between(highlighted_text_start,
                                               starting_line_info.end_idx),
                        emph.text_fmt);

    auto cap_len = emph.sideline_cap.size();

    auto pad_count = max - starting_line_len;
    data += std::string(pad_count + 1, ' ');
    data += format_text(emph.sideline_cap, emph.sideline_fmt);
    //
    // Filler lines (guaranteed to contain all spanned characters)
    //

    auto first_filler_line = start.line_no + 1;
    auto last_filler_line = stop.line_no - 1;

    // If there are any filler lines
    if (first_filler_line < last_filler_line) {

      for (size_t line = first_filler_line; line <= last_filler_line; line++) {
        data += "\n";
        data += generate_line_number(span, line, true);

        auto text = std::string(span.file.get_line(line));

        data += format_text(text, emph.text_fmt);

        auto pad_count = max - text.size();

        data += std::string(pad_count + 1, ' ');
        data += format_text(emph.sideline, emph.sideline_fmt);
        // std::cout << "Filler @ line# " << line << std::endl;
      }
    }

    //
    // Final line
    //

    data += "\n";
    data += generate_line_number(span, stop.line_no, true);

    // Write final spanned text

    auto final_line_length =
        final_line_info.end_idx - final_line_info.start_idx + 1;

    auto final_line_spanned_count = span.stop - final_line_info.start_idx + 1;
    auto final_line_unspanned_count =
        final_line_length - final_line_spanned_count;

    auto spanned = std::string(span.file.data_between(
        final_line_info.start_idx,
        final_line_info.start_idx + final_line_spanned_count - 1));

    data += format_text(spanned, emph.text_fmt);

    data += format_text(
        span.file.data_between(span.stop + 1, final_line_info.end_idx),
        ctx.context_fmt);

    auto pad_count_end = max - final_line_length;

    data += std::string(pad_count_end + 1, ' ');

    data += format_text(emph.sideline_cap, emph.sideline_fmt);
    return data;
  }
  std::string generate_context_lines(TextFile::Span &span, size_t line_start,
                                     size_t n) {

    std::string lines;

    for (size_t i = 0; i < n; i++) {
      auto data = generate_context_line(span, line_start + i);
      lines += data + "\n";
    }

    return lines;
  }

  std::string generate_context_line(TextFile::Span &span, size_t line_number) {

    std::string data = "";

    data += generate_line_number(span, line_number, false);
    if (line_numbers.show) {

      // Insert line number
    }

    auto line = span.file.get_line(line_number);

    data += ansi::FormatBuilder()
                .import_config(ctx.context_fmt)
                .write(std::string(line))
                .str();
    return data;
  }

  std::string generate_line_number(TextFile::Span &span, size_t number,
                                   bool is_highlighted) {
    if (line_numbers.show) {

      auto reserved_digit_cont = max_line_number_length(span);
      std::string lno = std::to_string(number);
      int pad_cnt = reserved_digit_cont - lno.size();
      for (int i = 0; i < pad_cnt; i++) {
        lno += ' ';
      }
      return ansi::FormatBuilder()
          .import_config(is_highlighted ? line_numbers.highlight_appearance
                                        : line_numbers.appearance)
          .write(lno)
          .import_config(line_numbers.separator_appearance)
          .write(line_numbers.separator)
          .str();

    } else
      return "";
  }

  int max_line_number_length(TextFile::Span &span) {
    auto span_ends_on = span.stop_pos().line_no;
    auto context_max_ends_on = span_ends_on + ctx.lines_bottom;
    auto context_end = std::min(span.file.lines().size(), context_max_ends_on);
    return std::to_string(context_end).size();
  }
};

}; // namespace mutils
