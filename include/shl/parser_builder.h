//
// Created by garen on 5/27/21.
//

#ifndef PYTHON_PARSER_PARSER_BUILDER_H
#define PYTHON_PARSER_PARSER_BUILDER_H

#include <peg_parser/generator.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

// will be modified later
using Attr = std::string;

class SyntaxHighlightInfo {
public:
  int idx = 0;
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
  std::unordered_set<std::string> colors;
public:
  // get expression for PEG parser grammar
  std::string getExpr();
  // hope all string in `colors` are lowercase
  void append(const std::string& color);
};

void changeAttr(Attr attr, int begin, int end);

ParserBuilder generateParserBuilder(Colors& colors);

Colors getPredefinedColors();

std::pair<bool, Parser> generateParserFromSHL(const std::string& filename);

void initParserBuilder(ParserBuilder& g, Colors& colors);

int main(int argc, char** argv);

#endif //PYTHON_PARSER_PARSER_BUILDER_H