//
// Created by garen on 6/14/21.
//

#ifndef PIE_PIERC_H
#define PIE_PIERC_H

#include <peg_parser/generator.h>

namespace pierc {
  typedef peg_parser::ParserGenerator<std::string> ConfigParser;

  extern std::string fontName;
  extern int fontSize;

  void config(ConfigParser& parser);

  // can use only once at the beginning when pie starts execution
  void gatherInfo();

  QFont getFont();
}

#endif //PIE_PIERC_H
