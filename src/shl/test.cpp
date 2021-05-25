//
// Created by garen on 5/23/21.
//
#include <peg_parser/generator.h>

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>

typedef peg_parser::ParserGenerator<std::string> Parser;
using Attr = std::string;
typedef peg_parser::ParserGenerator<Attr, Parser&> ParserBuilder;

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

void changeAttr(const Attr& attr, int begin, int end) {

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
    return s.string();
  };
  g["GrammarExpr"] << "('\"' (!'\"' .)* '\"')" >> [](auto, Parser&) {
    return "GrammarExpr";
  };
  g["Indent"] << "(['\t''\n' ]*)" >> [](auto, Parser&) {
    return "Indent";
  };
  g["EmptyBlock"] << "('{' Indent '}')" >> [](auto, Parser&) {
    return "EmptyBlock";
  };
  g["OnlyBlock"] << "('{' Indent Attr Indent '}')";
  g["equals"] << "(Indent '=' Indent)" >> [](auto, Parser&) {
    return "equals";
  };
  g["MultiBlock"] << "('{' ['\t''\n' ]* ('$' (Index equals Attr ['\t''\n' ]*))* '}')" >> [](auto s, Parser& gen) {
    for (int i = 0; i < s.size(); i += 3) {
      // DEBUG
      // std::cout << "$" << s[i].evaluate() << " = " << s[i + 2].evaluate() << std::endl;

      int idx = std::stoi(s[i].string());
      changeAttr(s[idx + 2].evaluate(gen), s[idx].position(), s[idx].position() + s[idx].length());
    }
    return "MultiBlock";
  };
  g["Index"] << "(('0') | ([1-9][0-9]*))" >> [](auto s, Parser&) {
    return s.string();
  };
  g["underlined"] << "'underlined'" >> [](auto, Parser&) {
    return "underlined";
  };
  g["bold"] << "'bold'" >> [](auto, Parser&) {
    return "bold";
  };
  g["italic"] << "'italic'" >> [](auto, Parser&) {
    return "italic";
  };
  g["Color"] << color.getExpr() >> [](auto s, Parser&) {
    return s.string();
  };
  g["Attr"] << "(((underlined | bold | italic) Indent)* Color)" >> [](auto s, Parser&) {
    std::string ret;
    for (int i = 0; i < s.size(); i += 2) {
//      std::cout << s[i].evaluate() << " ";
      ret += s[i].string() + " ";
    }
    return ret;
  };
  g["Comment"] << "('//' (!'\n' .)*)" >> [](auto, Parser&) {
    return "Comment";
  };
  // integrated basic keywords
  g["lparen"] << "('(')" >> [](auto, Parser&) {
    return "lparen";
  };
  g["rparen"] << "('(')" >> [](auto, Parser&) {
    return "rparen";
  };
  g["langle"] << "('<')" >> [](auto, Parser&) {
    return "langle";
  };
  g["rangle"] << "('>')" >> [](auto, Parser&) {
    return "rangle";
  };
  g["comma"] << "(',')" >> [](auto, Parser&) {
    return "comma";
  };
  g["colon"] << "(':')" >> [](auto, Parser&) {
    return "colon";
  };
  g["semicolon"] << "(';')" >> [](auto, Parser&) {
    return "semicolon";
  };
  g["single_quote"] << "([\'])" >> [](auto, Parser&) {
    return "single_quote";
  };
  g["double_quote"] << "('\"')" >> [](auto, Parser&) {
    return "double_quote";
  };
  g["backslash"] << "('\\\\')" >> [](auto, Parser&) {
    return "backslash";
  };
  g["single_equal"] << "('=')" >> [](auto, Parser&) {
    return "single_equal";
  };
  g["double_equal"] << "('==')" >> [](auto, Parser&) {
    return "double_equal";
  };
  g["newline"] << "('\n')" >> [](auto, Parser&) {
    return "newline";
  };
  g["until_newline"] << "((!'\n' .)*)" >> [](auto, Parser&) {
    return "until_newline";
  };
  g["tab"] << "('\t')" >> [](auto, Parser&) {
    return "tab";
  };
  g["space"] << "(' ')" >> [](auto, Parser&) {
    return "space";
  };


  g["GrammarWithMultiBlock"] << "(Identifier ':' GrammarExpr MultiBlock)" >> [&](auto s, Parser&) {
    // how to pass the Attr key-values to a lambda function????????????????????????
    // TODO: if this problem is not solved, we cannot dynamically generate parser with syntax highlighting feature
    return "GrammarWithMultiBlock";
  };
  g["GrammarWithOnlyBlock"] << "(Identifier ':' GrammarExpr OnlyBlock)" >> [&](auto s, Parser& gen) {
    auto attr = s[2].evaluate(gen);
    gen[s[0].string()] << s[1].string() >> [&](auto s) {
      for (int i = 0; i < s.size(); ++i) {
        changeAttr(attr, s[i].position(), s[i].position() + s[i].length());
      }
      return "test";
    };
    return "GrammarWithOnlyBlock";
  };
  g["Grammar"] << "(GrammarWithOnlyBlock | GrammarWithMultiBlock)";
  g["Program"] << "((Grammar | Comment)*)";
  g["Test"] << "Identifier ':' GrammarExpr MultiBlock";
  g.setStart(g["Program"]);
//  g.setStart(g["Program"]);

  std::ifstream ifs("../src/shl/cpp_test.shl");
  std::string shlFile((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
//  std::cout << shlFile << std::endl;
  try {
    Parser gen;
    auto output = g.run(shlFile, gen);
    std::cout << "Parsing result: " << output << std::endl;
  } catch (peg_parser::SyntaxError &e) {
    std::cout << "Syntax error when parsing " << e.syntax->rule->name << std::endl;
  }
  return 0;
}