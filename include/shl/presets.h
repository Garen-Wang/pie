//
// Created by garen on 5/29/21.
//

#ifndef PYTHON_PARSER_PRESETS_H
#define PYTHON_PARSER_PRESETS_H

#include "parser_builder.h"

enum class LanguageType {
  CPP,
  JAVA,
  PYTHON,
  JSON,
};

std::pair<bool, Parser> generateLanguageParser(LanguageType languageType);
void initParserBuilder(ParserBuilder& g, Colors& colors);
void initSHLGrammar(ParserBuilder &g);

#endif //PYTHON_PARSER_PRESETS_H
