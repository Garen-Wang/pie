#pragma once

#include <peg_parser/generator.h>

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

#include "config.hpp"
#include "rope.hpp"

using Attr = Style;

namespace shl {
    enum class LanguageType {
      CPP,
      JAVA,
      PYTHON,
      JSON,
    };

  extern std::set<std::string> identifiers;

  class SyntaxHighlightInfo {
  public:
    mutable int idx = 0;
    Attr attr;
    explicit SyntaxHighlightInfo(Attr attr);
    explicit SyntaxHighlightInfo(int idx, Attr attr);
  };

  typedef std::vector<SyntaxHighlightInfo> SyntaxHighlightInfos;
  using Parser = peg_parser::ParserGenerator<std::string>;
  using ParserBuilder = peg_parser::ParserGenerator<std::shared_ptr<SyntaxHighlightInfos>, Parser&>;

  class Colors {
  private:
    // not wise to use *unordered_set* here, try *unordered_map*
    std::unordered_map<std::string, QColor> colors;

  public:
    // get expression for PEG parser grammar
    std::string getExpr();
    // hope all string in `colors` are lowercase
    void append(const std::string& color, const QColor& _color);
    bool exist(const std::string& str);
    QColor get(const std::string& str);
  };

  ParserBuilder *generateParserBuilder(Colors& colors, Rope *);

  Colors getPredefinedColors();

  std::pair<bool, Parser> generateParserFromSHL(const std::string& filename, Rope *rope);

  bool renderSyntaxHighlightForSHL(const std::string& filename);

  void initParserBuilder(ParserBuilder& g, Colors& colors, Rope *, bool render = false);

  extern std::vector<int> indentDepth;
  extern std::vector<std::pair<int, int> > blockRecords;

  void initSHLGrammar(ParserBuilder &g);

  std::pair<long, long> getLineNumber(std::string_view input, std::pair<int, int> blockRecord);

  int getIdx(const std::string& str, int size);

  bool isLowercase(const std::string& str);
  std::pair<bool, shl::Parser> generateLanguageParser(LanguageType languageType, Rope *);
}  // namespace shl
