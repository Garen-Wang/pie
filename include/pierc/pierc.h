#ifndef PIE_PIERC_H
#define PIE_PIERC_H

#include <peg_parser/generator.h>
#include <QFont>

namespace pierc {
    /* a special type of parser, only used to parse .pierc */
    typedef peg_parser::ParserGenerator<std::string> ConfigParser;

    /* store the name of the font */
    extern std::string fontName;

    /* store the size of the font */
    extern int fontSize;

    /* configure the parser after initialization */
    void config(ConfigParser &parser);

    /* used only once at the beginning when pie starts its execution */
    void gatherInfo();

    /* retrieve the font family in format of *QFont* */
    QFont getFont();
}// namespace pierc

#endif //PIE_PIERC_H
