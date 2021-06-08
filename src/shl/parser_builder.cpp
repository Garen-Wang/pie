//
// Created by garen on 5/29/21.
//

#include "parser_builder.h"

namespace shl {
  std::set<std::string> identifiers;
  SyntaxHighlightInfo::SyntaxHighlightInfo(Attr attr) {
    this->idx = -1;
    this->attr = attr;
  }

  SyntaxHighlightInfo::SyntaxHighlightInfo(int idx, Attr attr) {
    this->idx = idx;
    this->attr = attr;
  }

  std::string Colors::getExpr() {
    std::string expr = "(";
    for (const auto& it : colors) {
      expr += "'" + it.first + "'|";
    }
    expr.pop_back();
    expr += ")";
    return expr;
  }

  void Colors::append(const std::string& color, const _Color& _color) { colors[color] = _color; }
  bool Colors::exist(const std::string& str) { return colors.count(str) != 0; }
  _Color Colors::get(const std::string& str) { return colors[str]; }

  void changeAttr(Attr attr, int begin, int end) {
    std::cout << "from " << begin << " to " << end << ", set" << attr << std::endl;
  }

  ParserBuilder generateParserBuilder(Colors& colors) {
    ParserBuilder g;
    initParserBuilder(g, colors);
    return g;
  }

  Colors getPredefinedColors() {
    Colors colors;
    colors.append("black", _Color(0, 0, 0));
    colors.append("gray", _Color(220, 220, 220));
    colors.append("shallow_yellow", _Color(204, 255, 0));
    colors.append("blue", _Color(0, 0, 255));
    colors.append("cyan", _Color(0, 255, 255));
    colors.append("purple", _Color(127, 0, 255));
    colors.append("green", _Color(0, 128, 0));
    colors.append("dark_purple", _Color(230, 230, 250));
    colors.append("orange", _Color(255, 165, 0));
    colors.append("white", _Color(255, 255, 255));
    colors.append("yellow", _Color(255, 255, 0));
    colors.append("pink", _Color(255, 182, 193));
    return colors;
  }

  std::pair<bool, Parser> generateParserFromSHL(const std::string& filename) {
    Colors colors = getPredefinedColors();
    ParserBuilder g = generateParserBuilder(colors);
    std::ifstream ifs(filename);
    std::string shlFile((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    Parser gen;
    bool flag = true;
    try {
      auto output = g.run(shlFile, gen);
//      gen["Identifier"] >> [](auto) {
      //        std::cout << "11" << std::endl;
      //        return "Identifier";
      //      };
      //    std::cout << "Parsing result: " << output << std::endl;
    } catch (peg_parser::SyntaxError& e) {
      flag = false;
      std::cout << "ParserBuilder: Syntax error when parsing " << e.syntax->rule->name << std::endl;
    }
    return std::make_pair(flag, gen);
  }

  bool renderSyntaxHighlightForSHL(const std::string& filename) { return false; }
}  // namespace shl
