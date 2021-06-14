//
// Created by garen on 5/27/21.
//

#ifndef PYTHON_PARSER_PARSER_BUILDER_H
#define PYTHON_PARSER_PARSER_BUILDER_H


#include "config.hpp"
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

using Attr = Style;

namespace shl {
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

  void changeAttr(Attr attr, int begin, int end);

  ParserBuilder *generateParserBuilder(Colors& colors);

  Colors getPredefinedColors();

  std::pair<bool, Parser*> generateParserFromSHL(const std::string& filename);

  bool renderSyntaxHighlightForSHL(const std::string& filename);

  void initParserBuilder(ParserBuilder& g, Colors& colors, bool render = false);
}  // namespace shl

#endif //PYTHON_PARSER_PARSER_BUILDER_H
