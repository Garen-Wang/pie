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

extern std::vector<int> indentDepth;
extern std::vector<std::pair<int, int> > blockRecords;

std::pair<bool, Parser> generateLanguageParser(LanguageType languageType);
void initSHLGrammar(ParserBuilder &g);

std::pair<long, long> getLineNumber(std::string_view input, std::pair<int, int> blockRecord);

#endif //PYTHON_PARSER_PRESETS_H
