#ifndef PIE_PIERC_H
#define PIE_PIERC_H

#include <peg_parser/generator.h>
#include <QFont>

namespace pierc {
    typedef peg_parser::ParserGenerator<std::string> ConfigParser;

    extern std::string fontName;
    extern int fontSize;

    void config(ConfigParser &parser);

    // can use only once at the beginning when pie starts execution
    void gatherInfo();

    QFont getFont();
}// namespace pierc

#endif//PIE_PIERC_H
