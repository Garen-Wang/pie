#include "config.hpp"


bool Style::operator==(const Style &other) const {
    return fg == other.fg
        && bg == other.bg
        && bold == other.bold
        && italic == other.italic
        && underline == other.underline
        && strikethrough == other.strikethrough;
}

bool Style::operator!=(const Style &other) const {
    return !(this->operator==(other));
}

Style Style::default_style = {
    .fg = QColor(255, 255, 255),
    .bg = QColor(64 , 64 , 64 ),
    .bold = false, .italic = false,
    .underline = false, .strikethrough = false
};

Style Style::cursor_style = {
    .fg = QColor(32 , 32 , 32 ),
    .bg = QColor(255, 255, 255),
    .bold = false, .italic = false,
    .underline = false, .strikethrough = false
};

Style Style::selection_style = {
    .fg = QColor(255, 255, 255),
    .bg = QColor(29 , 65 , 77 ),
    .bold = false, .italic = false,
    .underline = false, .strikethrough = false
};
