//
// Created by garen on 5/27/21.
//

#ifndef PYTHON_PARSER_PARSER_BUILDER_H
#define PYTHON_PARSER_PARSER_BUILDER_H

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

class _Color {
private:
  int r, g, b;

public:
  explicit _Color() : r(0), g(0), b(0) {}
  explicit _Color(int r, int g, int b) : r(r), g(g), b(b) {}

  friend std::ostream& operator<<(std::ostream& out, const _Color& _color) {
    return out << "(" << _color.r << ", " << _color.g << ", " << _color.b << ")";
  }
};

struct Style {
  _Color fg, bg;
  bool bold, italic;
  bool underlined, strikethrough;

  explicit Style()
      : fg(255, 255, 255),
        bg(255, 255, 255),
        bold(false),
        italic(false),
        underlined(false),
        strikethrough(false) {}

  friend std::ostream& operator<<(std::ostream& out, const Style& style) { return out << style.fg; }
};

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
  typedef peg_parser::ParserGenerator<std::string> Parser;
  typedef peg_parser::ParserGenerator<std::shared_ptr<SyntaxHighlightInfos>, Parser&> ParserBuilder;

  class Colors {
  private:
    // not wise to use *unordered_set* here, try *unordered_map*
    std::unordered_map<std::string, _Color> colors;

  public:
    // get expression for PEG parser grammar
    std::string getExpr();
    // hope all string in `colors` are lowercase
    void append(const std::string& color, const _Color& _color);
    bool exist(const std::string& str);
    _Color get(const std::string& str);
  };

  void changeAttr(Attr attr, int begin, int end);

  ParserBuilder generateParserBuilder(Colors& colors);

  Colors getPredefinedColors();

  std::pair<bool, Parser> generateParserFromSHL(const std::string& filename);

  void initParserBuilder(ParserBuilder& g, Colors& colors);
}

#endif //PYTHON_PARSER_PARSER_BUILDER_H