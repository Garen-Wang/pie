#pragma once


/* This file contains possible settings of pie */
#include "libs.hpp"

/* The [Style] type records how a character should be
 * displayed. */
struct Style {
    /* foreground and background color */
    QColor fg, bg;
    /* whether the character should be bold/italic */
    bool bold, italic;
    /* underline: a line is drawn on the bottom of the character
     * strikethrough: a line is drawn on the middle of the character */
    bool underline, strikethrough;

    bool operator==(const Style &) const;
    bool operator!=(const Style &) const;

    /* Builtin styles used by pie internally */
    /* default style for characters */
    static Style default_style;
    /* the style for the cursor */
    static Style cursor_style;
    /* the style for selected chars other than the cursor */
    static Style selection_style;
};
