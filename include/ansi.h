#pragma once


#include<inttypes.h>
#include<string>
#include "funcmod.h"

namespace mutils::ansi
{

    typedef struct{uint8_t Red; uint8_t Green; uint8_t Blue;} RGBColorSequence;

    /*
    The base colors defined by the ANSI standard
    */
    enum class Color
    {
        /* clang-format off */
        /**
         * Each value represents an offset from the base value
         * either 40 + X for backgrounds
         * or     30 + X for foregrounds
        */
        Black            = 0,
        Red              = 1,
        Green            = 2,
        Yellow           = 3,
        Blue             = 4,
        Magenta          = 5,
        Cyan             = 6,
        White            = 7,
        BrightBlack      = 60,
        BrightRed        = 61,
        BrightGreen      = 62,
        BrightYellow     = 63,
        BrightBlue       = 64,
        BrightMagenta    = 65,
        BrightCyan       = 66,
        BrightWhite      = 67,

        /* clang-format on */
    };

    /**
     * Create an RGBColorSequence to be used with the FormatBuilder
    */
    RGBColorSequence rgb(uint8_t red, uint8_t green, uint8_t blue);


    class FormatBuilder{
    private:
        std::string m_data;
        struct AnsiTextEffects{
            enum {
                Normal,
                Bold,
                Faint,
            } intensity = Normal;

            
            enum{
                Fast,
                Slow,
                None,
            } blink = None;

            bool italic = false;
            bool crossout = false;
            bool fraktur = false;
            bool underline = false;
            bool framed = false;
            bool encircled = false;
            bool overlined = false;
            bool hidden = false;

            //TODO: Add support for the multi-font feature (codes 11-19)
        } m_effects;


        struct AnsiColor{
            union{
                RGBColorSequence rgb;
                Color preset;
            } data;
            enum{
                RGB,
                PRESET,
                DEFAULT,
            } variant = DEFAULT;
        };


        AnsiColor foreground;
        AnsiColor background;

    public:


        /**
         * Set the text foreground color to one of the preset ansi colors
        */
        FormatBuilder& fg(Color color);

        /**
         * Set the text background color to one of the preset ansi colors
        */
        FormatBuilder& bg(Color color);

        /**
         * Reset the formatting back to default (no formatting)
        */
        FormatBuilder& reset();

        /**
         * Set the text foreground color to a custom RGB value created by
         * the rgb() function
        */
        FormatBuilder& fg(RGBColorSequence color);

        /**
         * Set the text background color to a custom RGB value created by
         * the rgb() function
        */
        FormatBuilder& bg(RGBColorSequence color);


        /**
         * Make the text appear italic (slanted text)
        */
        FormatBuilder& italic();

        /**
         * Reset the italic effect
        */
        FormatBuilder& no_italic();

        /**
         * Add a strikethrough to the text (crossed-out text)
        */
        FormatBuilder& strikethrough();

        /**
         * Reset the strikethrough effect
        */
        FormatBuilder& no_strikethrough();

        /**
         * Make the text appear gothic (fraktur)
        */
        DEPRECATED("This feature is not supported by most terminals") FormatBuilder& gothic();

        /**
         * Reset the gothic effect
        */
        DEPRECATED("This feature is not supported by most terminals") FormatBuilder& no_gothic();

        /**
         * Add an underline to the text
        */
        FormatBuilder& underline();

        /**
         * Reset the underline effect
        */
        FormatBuilder& no_underline();

        DEPRECATED("This feature is not supported by most terminals") FormatBuilder& frame();
        DEPRECATED("This feature is not supported by most terminals") FormatBuilder& no_frame();

        DEPRECATED("This feature is not supported by most terminals") FormatBuilder& encircle();
        DEPRECATED("This feature is not supported by most terminals") FormatBuilder& no_encircle();

        FormatBuilder& hide();
        FormatBuilder& show();

        FormatBuilder& bold();
        FormatBuilder& faint();
        FormatBuilder& no_intensity();

        FormatBuilder& blink_fast();
        FormatBuilder& blink_slow();
        FormatBuilder& no_blink();


        /**
         * Write custom text into the builder, this text will have the effects
         * of the previously called constructions applied to it
        */
        FormatBuilder& write(const std::string& str);

        /**
         * Get the resulting text from the FormatBuilder
        */
        std::string str(bool terminate = true);
    };


    /**
     * The CursorBuilder is responsible for orchestrating motion of the cursor
     * this can be used for things like terminal graphics
    */
    class CursorBuilder{
    private:
        std::string data;
    public:
        /**
         * Move the cursor up by n lines 
         */
        CursorBuilder& up(unsigned int nlines);

        /**
         * Move the cursor down by n lines
        */
        CursorBuilder& down(unsigned int nlines);

        /**
         * Move the cursor left by n columns
        */
        CursorBuilder& left(unsigned int ncols);

        /**
         * Move the cursor right by n columns
        */
        CursorBuilder& right(unsigned int ncols);

        /**
         * Move the cursor to the column n of the current
         * line
        */
        CursorBuilder& col(unsigned int n);

        /**
         * Move the cursor to the position y, x
        */
        CursorBuilder& goto_pos(unsigned int row, unsigned int col);

        /**
         * Erase the contents of a line from the cursor onwards
         * including the character that the cursor is on
        */
        CursorBuilder& erase_line_from_cursor();

        /**
         * Erase the entire line that the cursor is on
        */
        CursorBuilder& erase_line();

        /**
         * Erase the display from the cursor onwards
         * including the character that the cursor is on
        */
        CursorBuilder& erase_display_from_cursor();

        /**
         * Erase the entire display, and move the
         * cursor to the position (0, 0)
        */
        CursorBuilder& erase_display_and_return_home();


        /**
         * Scroll the whole page up by n lines
         * new lines are inserted into the bottom
        */
        CursorBuilder& scroll_up(unsigned int nlines);

        /**
         * Scroll the whole page down by n lines
         * new lines are inserted into the top
        */
        CursorBuilder& scroll_down(unsigned int nlines);

        /**
         * Write text from the current cursor position
        */
        CursorBuilder& print(const std::string& text);

        /**
         * Get the output sequences from the builder
        */
        std::string str();
    };

};