#include "../include/ansi.h"
#include<algorithm>
#include<ranges>
#include<map>
using namespace mutils;

const int ANSI_FG_OFFSET = 30;
const int ANSI_BG_OFFSET = 40;
const char* ANSI_GENERAL_ESCAPE_SEQ = "\033[";
const char* ANSI_FORMATTING_ESCAPE_SEQ = "\033[0";

#define escape(c) ("\033[" c)

const int ANSI_CUSTOM_BG_CODE = 48;
const int ANSI_CUSTOM_FG_CODE = 38;
const int ANSI_CUSTOM_COLOR_INDICATE_RGB = 2;

ansi::RGBColorSequence ansi::rgb(uint8_t red, uint8_t green, uint8_t blue){
    return {red, green, blue};
}


ansi::FormatBuilder& ansi::FormatBuilder::fg(ansi::Color color){
    foreground.variant = AnsiColor::PRESET;
    foreground.data.preset = color;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::bg(ansi::Color color){
    background.variant = AnsiColor::PRESET;
    background.data.preset = color;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::fg(ansi::RGBColorSequence rgb){
    foreground.variant = AnsiColor::RGB;
    foreground.data.rgb = rgb;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::bg(ansi::RGBColorSequence rgb){
    background.variant = AnsiColor::RGB;
    background.data.rgb = rgb;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::blink_fast(){
    m_effects.blink = FormatBuilder::AnsiTextEffects::Fast;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::blink_slow(){
    m_effects.blink = FormatBuilder::AnsiTextEffects::Slow;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::no_blink(){
    m_effects.blink = FormatBuilder::AnsiTextEffects::None;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::bold(){
    m_effects.intensity = m_effects.Bold;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::faint(){
    m_effects.intensity = m_effects.Faint;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::no_intensity(){
    m_effects.intensity = m_effects.Normal;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::encircle(){
    m_effects.encircled = true;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::no_encircle(){
    m_effects.encircled = false;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::gothic(){
    m_effects.fraktur = true;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::no_gothic(){
    m_effects.fraktur = false;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::underline(){
    m_effects.underline = true;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::no_underline(){
    m_effects.underline = false;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::frame(){
    m_effects.framed = true;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::no_frame(){
    m_effects.framed = false;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::hide(){
    m_effects.hidden = true;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::show(){
    m_effects.hidden = false;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::italic(){
    m_effects.italic = true;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::no_italic(){
    m_effects.italic = false;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::strikethrough(){
    m_effects.crossout = true;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::no_strikethrough(){
    m_effects.crossout = false;
    return *this;
}

ansi::FormatBuilder& ansi::FormatBuilder::write(const std::string& str){

    std::string modifier_str = ANSI_FORMATTING_ESCAPE_SEQ;

    // Font Color
    if(foreground.variant == AnsiColor::PRESET){
        modifier_str += ";" + std::to_string((int)foreground.data.preset + ANSI_FG_OFFSET);
    }
    else if(foreground.variant == AnsiColor::RGB){
        modifier_str += ";38;2";
        modifier_str += ";" + std::to_string(foreground.data.rgb.Red);
        modifier_str += ";" + std::to_string(foreground.data.rgb.Green);
        modifier_str += ";" + std::to_string(foreground.data.rgb.Blue);
    }

    // Background Color
    if(background.variant == AnsiColor::PRESET){
        modifier_str += ";" + std::to_string((int)background.data.preset + ANSI_BG_OFFSET);
    }
    else if(background.variant == AnsiColor::RGB){
        modifier_str += ";48;2";
        modifier_str += ";" + std::to_string(background.data.rgb.Red);
        modifier_str += ";" + std::to_string(background.data.rgb.Green);
        modifier_str += ";" + std::to_string(background.data.rgb.Blue);
    }

    // Simple Effects
    if(m_effects.italic)modifier_str += ";3";
    if(m_effects.hidden)modifier_str += ";8";
    if(m_effects.crossout)modifier_str += ";9";
    if(m_effects.fraktur)modifier_str += ";20";
    if(m_effects.framed)modifier_str += ";51";
    if(m_effects.encircled)modifier_str += ";52";
    if(m_effects.overlined)modifier_str += ";53";

    // Effects with multiple modes
    if(m_effects.blink == AnsiTextEffects::Fast)modifier_str += ";6";
    else if(m_effects.blink == AnsiTextEffects::Slow)modifier_str += ";5";

    if(m_effects.intensity == AnsiTextEffects::Bold)modifier_str += ";1";
    else if(m_effects.intensity == AnsiTextEffects::Faint)modifier_str += ";2";

    m_data += modifier_str + "m" + str;
    return *this;
}
ansi::FormatBuilder& ansi::FormatBuilder::reset(){
    m_effects = AnsiTextEffects();
    foreground = AnsiColor();
    background = AnsiColor();
    return *this;
}
std::string ansi::FormatBuilder::str(bool terminate){
    if(terminate){
        m_data += ANSI_FORMATTING_ESCAPE_SEQ + std::string("m");
    }
    return m_data;
}


ansi::CursorBuilder& ansi::CursorBuilder::up(unsigned int nlines){
    data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(nlines) + "A";
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::down(unsigned int nlines){
    data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(nlines) + "B";
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::left(unsigned int ncols){
    data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(ncols) + "D";
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::right(unsigned int ncols){
        data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(ncols) + "C";
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::col(unsigned int n){
    data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(n) + "G";
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::goto_pos(unsigned int row, unsigned int col){
    data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(row) + ";" + std::to_string(col) + "f";
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::erase_line_from_cursor(){
    data += escape("K");
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::erase_line(){
    data += escape("2K");
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::erase_display_from_cursor(){
    data += escape("J");
    return *this;
}
ansi::CursorBuilder& ansi::CursorBuilder::erase_display_and_return_home(){
    data += escape("2J");
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::scroll_up(unsigned int nlines){
    data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(nlines) + "S";
    return *this;
}

ansi::CursorBuilder& ansi::CursorBuilder::scroll_down(unsigned int nlines){
    data += ANSI_GENERAL_ESCAPE_SEQ + std::to_string(nlines) + "T";
    return *this;
}
ansi::CursorBuilder& ansi::CursorBuilder::print(const std::string& text){
    data += text;
    return *this;
}
std::string ansi::CursorBuilder::str(){
    return data;
}