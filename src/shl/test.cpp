//
// Created by garen on 5/23/21.
//
#include <peg_parser/generator.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

using Attr = std::string;
class SyntaxHighlightInfo {
public:
  int idx = 0;
  Attr attr;
  explicit SyntaxHighlightInfo(Attr attr) {
    idx = -1;
    this->attr = std::move(attr);
  }
  explicit SyntaxHighlightInfo(int idx, Attr attr) {
    this->idx = idx;
    this->attr = std::move(attr);
  }
};
typedef std::vector<SyntaxHighlightInfo> SyntaxHighlightInfos;
typedef peg_parser::ParserGenerator<std::string> Parser;
typedef peg_parser::ParserGenerator<std::shared_ptr<SyntaxHighlightInfos>, Parser&> ParserBuilder;

class Colors {
private:
  std::unordered_set<std::string> colors;
public:
  std::string getExpr() {
    std::string expr = "(";
    for (const std::string& color : colors) {
      expr += "'" + color + "'|";
    }
    expr.pop_back();
    expr += ")";
    return expr;
  }
  // hope all string in `colors` are lowercase
  void append(const std::string& color) {
    colors.insert(color);
  }
};
Colors color;

void changeAttr(Attr attr, int begin, int end) {

}

int main() {
  color.append("black");
  color.append("grey");
  color.append("shallow_yellow");
  color.append("blue");
  color.append("cyan");
  color.append("purple");
  color.append("green");
  ParserBuilder g;
  g.setSeparator(g["Separators"] << "['\t''\n' ]");
  g["Identifier"] << "(([a-zA-Z] | '_') ([a-zA-Z0-9] | '_')*)" >> [](auto s, Parser&) {
    return nullptr;
//    return s.string();
  };
  g["GrammarExpr"] << "('\"' (!'\"' .)* '\"')" >> [](auto, Parser&) {
    return nullptr;
//    return "GrammarExpr";
  };
  g["Indent"] << "(['\t''\n' ]*)" >> [](auto, Parser&) {
    return nullptr;
//    return "Indent";
  };
  g["EmptyBlock"] << "('{' Indent '}')" >> [](auto, Parser&) {
    return nullptr;
//    return "EmptyBlock";
  };
  g["OnlyBlock"] << "('{' Indent Attr Indent '}')" >> [](auto s, Parser& gen) {
    auto ret = std::make_shared<SyntaxHighlightInfos>();
    Attr attr = s[1].evaluate(gen)->begin()->attr;
    ret->push_back(SyntaxHighlightInfo(-1, attr));
    return ret;
  };
  g["equals"] << "(Indent '=' Indent)" >> [](auto, Parser&) {
    return nullptr;
//    return "equals";
  };
  g["MultiBlock"] << "('{' ['\t''\n' ]* ('$' (Index equals Attr ['\t''\n' ]*))* '}')" >> [](auto s, Parser& gen) {
    auto ret = std::make_shared<SyntaxHighlightInfos>();
    for (int i = 0; i < s.size(); i += 3) {
      int idx = std::stoi(s[i].string());
      Attr attr = s[i + 2].evaluate(gen)->begin()->attr;
      // DEBUG
      std::cout << "$" << idx << " = " << s[i + 2].string() << std::endl; // since it contains newline

      ret->push_back(SyntaxHighlightInfo(idx, attr));
    }
    return ret;
//    return "MultiBlock";
  };
  g["Block"] << "(EmptyBlock | OnlyBlock | MultiBlock)";
  g["Index"] << "(('0') | ([1-9][0-9]*))" >> [](auto s, Parser&) {
    return nullptr;
//    return s.string();
  };
  g["underlined"] << "'underlined'" >> [](auto, Parser&) {
    return nullptr;
//    return "underlined";
  };
  g["bold"] << "'bold'" >> [](auto, Parser&) {
    return nullptr;
//    return "bold";
  };
  g["italic"] << "'italic'" >> [](auto, Parser&) {
    return nullptr;
//    return "italic";
  };
  g["Color"] << color.getExpr() >> [](auto s, Parser&) {
    return nullptr;
//    return s.string();
  };
  g["Attr"] << "(((underlined | bold | italic) Indent)* Color)" >> [](auto s, Parser&) {
    Attr attr;
    auto ret = std::make_shared<SyntaxHighlightInfos>();

    for (int i = 0; i < s.size(); i += 2) {
//      std::cout << s[i].evaluate() << " ";
      attr += s[i].string() + " ";
    }
    ret->push_back(SyntaxHighlightInfo(attr));
    return ret;
  };
  g["Comment"] << "('//' (!'\n' .)*)" >> [](auto, Parser&) {
    return nullptr;
//    return "Comment";
  };
  // integrated
  g["lparen"] << "('(')" >> [](auto, Parser&) {
    return nullptr;
//    return "lparen";
  };
  g["rparen"] << "('(')" >> [](auto, Parser&) {
    return nullptr;
//    return "rparen";
  };
  g["langle"] << "('<')" >> [](auto, Parser&) {
    return nullptr;
//    return "langle";
  };
  g["rangle"] << "('>')" >> [](auto, Parser&) {
    return nullptr;
//    return "rangle";
  };
  g["comma"] << "(',')" >> [](auto, Parser&) {
    return nullptr;
//    return "comma";
  };
  g["colon"] << "(':')" >> [](auto, Parser&) {
    return nullptr;
//    return "colon";
  };
  g["semicolon"] << "(';')" >> [](auto, Parser&) {
    return nullptr;
//    return "semicolon";
  };
  g["single_quote"] << "([\'])" >> [](auto, Parser&) {
    return nullptr;
//    return "single_quote";
  };
  g["double_quote"] << "('\"')" >> [](auto, Parser&) {
    return nullptr;
//    return "double_quote";
  };
  g["backslash"] << "('\\\\')" >> [](auto, Parser&) {
    return nullptr;
//    return "backslash";
  };
  g["single_equal"] << "('=')" >> [](auto, Parser&) {
    return nullptr;
//    return "single_equal";
  };
  g["double_equal"] << "('==')" >> [](auto, Parser&) {
    return nullptr;
//    return "double_equal";
  };
  g["newline"] << "('\n')" >> [](auto, Parser&) {
    return nullptr;
//    return "newline";
  };
  g["until_newline"] << "((!'\n' .)*)" >> [](auto, Parser&) {
    return nullptr;
//    return "until_newline";
  };
  g["tab"] << "('\t')" >> [](auto, Parser&) {
    return nullptr;
//    return "tab";
  };
  g["space"] << "(' ')" >> [](auto, Parser&) {
    return nullptr;
//    return "space";
  };

  g["InitParserBuilder"] << "''" >> [](auto, Parser& gen) {
    // DEBUG
    std::cout << "hahshfshsd" << std::endl;
    gen["lparen"] << "('(')" >> [](auto) {
      return "lparen";
    };
    gen["rparen"] << "('(')" >> [](auto) {
      return "rparen";
    };
    gen["langle"] << "('<')" >> [](auto) {
      return "langle";
    };
    gen["rangle"] << "('>')" >> [](auto) {
      return "rangle";
    };
    gen["comma"] << "(',')" >> [](auto) {
      return "comma";
    };
    gen["colon"] << "(':')" >> [](auto) {
      return "colon";
    };
    gen["semicolon"] << "(';')" >> [](auto) {
      return "semicolon";
    };
    gen["single_quote"] << "([\'])" >> [](auto) {
      return "single_quote";
    };
    gen["double_quote"] << "('\"')" >> [](auto) {
      return "double_quote";
    };
    gen["backslash"] << "('\\\\')" >> [](auto) {
      return "backslash";
    };
    gen["single_equal"] << "('=')" >> [](auto) {
      return "single_equal";
    };
    gen["double_equal"] << "('==')" >> [](auto) {
      return "double_equal";
    };
    gen["newline"] << "('\n')" >> [](auto) {
      return "newline";
    };
    gen["until_newline"] << "((!'\n' .)*)" >> [](auto) {
      return "until_newline";
    };
    gen["tab"] << "('\t')" >> [](auto) {
      return "tab";
    };
    gen["space"] << "(' ')" >> [](auto) {
      return "space";
    };
    return nullptr;
  };

  g["Grammar"] << "(Identifier ':' GrammarExpr Block)" >> [&](auto s, Parser& gen) {
    auto identifier = s[0].string();
    auto grammarExpr = s[1].string();
    auto syntaxHighlightInfos = s[2].evaluate(gen);
//    gen[identifier] << grammarExpr >> [&](auto ss) {
//      for (auto it = syntaxHighlightInfos->begin(); it != syntaxHighlightInfos->end(); ++it) {
//        changeAttr(it->attr, ss[it->idx].position(), ss[it->idx].position() + ss[it->idx].length());
//      }
//      return identifier;
//    };
    return nullptr;
  };
  g["Program"] << "(InitParserBuilder (Grammar | Comment)*)";
  g["Test"] << "Identifier ':' GrammarExpr MultiBlock";
  g.setStart(g["Program"]);
//  g.setStart(g["Program"]);

  std::ifstream ifs("../src/cpp/cpp_test.shl");
  std::string shlFile((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
//  std::cout << shlFile << std::endl;
  try {
    Parser gen;
    auto output = g.run(shlFile, gen);
    gen.setSeparator(gen["Separators"] << "['\t''\n' ]");
    gen["Program"] << "lparen" >> [](auto) { return "returnValue"; };
    gen.setStart(gen["Program"]);
    std::cout << "Parsing result: " << output << std::endl;
    std::string anotherInput;
    std::getline(std::cin, anotherInput);
    try {
      auto anotherOutput = gen.run(anotherInput);
      std::cout << "Parsing result: " << anotherOutput << std::endl;
    } catch (peg_parser::SyntaxError &ee) {
      std::cout << "GeneratedParser: Syntax error when parsing " << ee.syntax->rule->name << std::endl;
    }
  } catch (peg_parser::SyntaxError &e) {
    std::cout << "ParserBuilder: Syntax error when parsing " << e.syntax->rule->name << std::endl;
  }
  return 0;
}