#include <peg_parser/generator.h>
#include <iostream>
#include <vector>

int main() {
  struct Block {
    std::size_t begin, length;
  };

  using Blocks = std::vector<Block>;
  peg_parser::ParserGenerator<void, Blocks&> blockParser;
  blockParser["Indentation"] << "' '*";
  std::vector<int> indentDepth;
  blockParser["InitBlocks"] << "''" << [&](auto&) -> bool {
    indentDepth.resize(0);
    return true;
  };
  blockParser["SameIndentation"] << "Indentation" << [&](auto &s) -> bool {
    return s->length() == indentDepth.back();
  };
  blockParser["SameIndentation"]->cacheable = false;

  blockParser["DeepIndentation"] << "Indentation" << [&](auto &s) -> bool {
    return s->length() > indentDepth.back();
  };
  blockParser["DeepIndentation"]->cacheable = false;

  blockParser["EnterBlock"] << "Indentation" << [&](auto &s) -> bool {
    if (indentDepth.size() == 0 || s->length() > indentDepth.back()) {
      indentDepth.push_back(s->length());
      return true;
    } else return false;
  };
  blockParser["EnterBlock"]->cacheable = false;

  blockParser["Line"] << "SameIndentation (!'\n' .)+ '\n'"; 
  blockParser.getRule("Line")->cacheable = false;

  blockParser["EmptyLine"] << "Indentation '\n'";

  blockParser["ExitBlock"] << "''" << [&](auto&) -> bool {
    indentDepth.pop_back();
    return true;
  };
  blockParser.getRule("ExitBlock")->cacheable = false;

  blockParser["Block"] << "&EnterBlock Line (EmptyLine | Block | Line)* &ExitBlock" >> [](auto e, Blocks &blocks) {
    for (auto x : e) x.evaluate(blocks);
    blocks.push_back(Block{e.position(), e.length()});
  };

  blockParser["Start"] << "InitBlocks Block";
  blockParser.setStart(blockParser["Start"]);


  while (true) {
    std::string str, input;
    std::cout << "> ";
    std::getline(std::cin, str);
    if (str == "q" || str == "quit") break;
    do {
      input += str + '\n';
      std::cout << "- ";
      std::getline(std::cin, str);
    } while (str != "");
    
    try {
      Blocks blocks;
      blockParser.run(input, blocks);
      std::cout << "matched " << blocks.size() << "blocks." << std::endl;
      for (auto b : blocks) {
        std::cout << "- from line " << std::count(input.begin(), input.begin() + b.begin, '\n') + 1;
        std::cout << " to " << std::count(input.begin(), input.begin() + b.begin + b.length, '\n') << std::endl;
      }
    } catch (peg_parser::SyntaxError &error) {
      auto syntax = error.syntax;
      std::cout << "Syntax error at character " << syntax->end << " while parsing " << syntax->rule->name << std::endl;
    }
  }
  return 0;
}