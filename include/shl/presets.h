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

namespace shl {

  extern std::vector<int> indentDepth;
  extern std::vector<std::pair<int, int> > blockRecords;

  void initSHLGrammar(ParserBuilder &g);

  std::pair<long, long> getLineNumber(std::string_view input, std::pair<int, int> blockRecord);

  int getIdx(const std::string& str, int size);

  bool isLowercase(const std::string& str);
}  // namespace shl

std::pair<bool, shl::Parser> generateLanguageParser(LanguageType languageType);

#endif //PYTHON_PARSER_PRESETS_H
