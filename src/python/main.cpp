#include <iostream>
#include <string>
#include <peg_parser/generator.h>

typedef peg_parser::ParserGenerator<std::string> Parser;

int main() {
  Parser blockParser;
  std::vector<int> indentDepth;
  blockParser["Indent"] << "(' '*)";
  blockParser["InitBlocks"] << "''" << [&](auto) -> bool {
    indentDepth.clear();
    return true;
  };

  blockParser["SameIndent"] << "Indent" << [&](auto s) -> bool {
    return s->length() == indentDepth.back();
  };
  blockParser["SameIndent"]->cacheable = false;

  blockParser["DeeperIndent"] << "Indent" << [&](auto s) -> bool {
    return s->length() == indentDepth.back() + indentDepth.front();
  };
  blockParser["DeeperIndent"]->cacheable = false;

  blockParser["EnterBlock"] << "Indent" << [&](auto s) -> bool {
    if (indentDepth.empty() || s->length() == indentDepth.back() + indentDepth.front()) {
      indentDepth.push_back(s->length());
      return true;
    } else return false;
  };
  blockParser["EnterBlock"]->cacheable = false;

  blockParser["ExitBlock"] << "''" << [&](auto s) -> bool {
    indentDepth.pop_back();
    return true;
  };
  blockParser["ExitBlock"]->cacheable = false;

  blockParser["Line"] << "SameIndent (!'\n' .)* '\n'";
  blockParser["EmptyLine"] << "SameIndent '\n'";
  blockParser["Block"] << "&EnterBlock Line (EmptyLine | Block | Line)* &ExitBlock" >> [](auto s) {

  };
}