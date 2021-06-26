#pragma once

#include "libs.hpp"

struct Style {
    QColor fg, bg;
    bool bold, italic;
    bool underline, strikethrough;

    bool operator==(const Style &) const;
    bool operator!=(const Style &) const;

    static Style default_style;
    static Style cursor_style;
    static Style selection_style;
};
