//
// Created by garen on 5/29/21.
//

#include "shl/parser_builder.h"
#include "shl/presets.h"

SyntaxHighlightInfo::SyntaxHighlightInfo(Attr attr) {
  this->idx = -1;
  this->attr = std::move(attr);
}

SyntaxHighlightInfo::SyntaxHighlightInfo(int idx, Attr attr) {
  this->idx = idx;
  this->attr = std::move(attr);
}


std::string Colors::getExpr() {
  std::string expr = "(";
  for (const std::string& color : colors) {
    expr += "'" + color + "'|";
  }
  expr.pop_back();
  expr += ")";
  return expr;
}

void Colors::append(const std::string &color) {
  colors.insert(color);
}

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
  colors.append("black");
  colors.append("grey");
  colors.append("shallow_yellow");
  colors.append("blue");
  colors.append("cyan");
  colors.append("purple");
  colors.append("green");
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
    //    std::cout << "Parsing result: " << output << std::endl;
  } catch (peg_parser::SyntaxError &e) {
    flag = false;
    std::cout << "ParserBuilder: Syntax error when parsing " << e.syntax->rule->name << std::endl;
  }
  return std::make_pair(flag, gen);
}

std::pair<bool, Parser> generateLanguageParser(LanguageType languageType) {
  switch (languageType) {
    case LanguageType::CPP: {
      return generateParserFromSHL("../src/cpp/cpp.shl");
    }
    case LanguageType::JAVA: {
      auto temp = generateParserFromSHL("../src/java/java.shl");
      if (temp.first) {
        auto gen = temp.second;
        gen["Program"] << "ImportStatement* Class+ newline*";
        gen.setStart(gen["Program"]);
        return std::make_pair(true, gen);
      } else return temp;
    }
    case LanguageType::PYTHON: {
      return generateParserFromSHL("../src/python/python3.shl");
    }
    case LanguageType::JSON: {
      return generateParserFromSHL("../src/json/json.shl");
    }
    default: {
      throw std::runtime_error("Not implemented yet");
    }
  }
}

