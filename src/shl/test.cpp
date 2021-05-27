//
// Created by garen on 5/23/21.
//
#include <peg_parser/generator.h>
#include <cassert>
#include <cstring>
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
  // not wise to use *unordered_set* here, try *unordered_map*
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
  std::cout << "from " << begin << " to " << end << ", set" << attr << std::endl;
}

ParserBuilder generateParserBuilder(Colors& colors) {
  ParserBuilder g;
  g.setSeparator(g["Separators"] << "['\t''\n' ]");
  g["Identifier"] << "(([a-zA-Z] | '_') ([a-zA-Z0-9] | '_')*)" >> [](auto s, Parser&) {
    return nullptr;
  };
  g["GrammarExpr"] << R"(('"' (!'"' .)* '"'))" >> [](auto, Parser&) {
    return nullptr;
  };
  g["Indent"] << "(['\t''\n' ]*)" >> [](auto, Parser&) {
    return nullptr;
  };
  g["EmptyBlock"] << "('{' Indent '}')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["OnlyBlock"] << "('{' Indent Attr Indent '}')" >> [](auto s, Parser& gen) {
    auto ret = std::make_shared<SyntaxHighlightInfos>();
    Attr attr = s[1].evaluate(gen)->begin()->attr;
    ret->push_back(SyntaxHighlightInfo(-1, attr));
    return ret;
  };
  g["equals"] << "(Indent '=' Indent)" >> [](auto, Parser&) {
    return nullptr;
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
  };
  g["Block"] << "(EmptyBlock | OnlyBlock | MultiBlock)";
  g["Index"] << "(('0') | ([1-9][0-9]*))" >> [](auto s, Parser&) {
    return nullptr;
  };
  g["underlined"] << "'underlined'" >> [](auto, Parser&) {
    return nullptr;
  };
  g["bold"] << "'bold'" >> [](auto, Parser&) {
    return nullptr;
  };
  g["italic"] << "'italic'" >> [](auto, Parser&) {
    return nullptr;
  };
  g["Color"] << colors.getExpr() >> [](auto s, Parser&) {
    return nullptr;
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
  };

  // markers integrated
  g["lparen"] << "('(')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["rparen"] << "('(')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["langle"] << "('<')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["rangle"] << "('>')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["comma"] << "(',')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["colon"] << "(':')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["semicolon"] << "(';')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["single_quote"] << "([\'])" >> [](auto, Parser&) {
    return nullptr;
  };
  g["double_quote"] << "('\"')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["backslash"] << "('\\\\')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["single_equal"] << "('=')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["double_equal"] << "('==')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["newline"] << "('\n')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["until_newline"] << "((!'\n' .)*)" >> [](auto, Parser&) {
    return nullptr;
  };
  g["tab"] << "('\t')" >> [](auto, Parser&) {
    return nullptr;
  };
  g["space"] << "(' ')" >> [](auto, Parser&) {
    return nullptr;
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
    auto grammarExpr = s[1].string().substr(1, s[1].string().length() - 2);
    auto syntaxHighlightInfos = s[2].evaluate(gen);
    // DEBUG
    std::cout << identifier << ": " << grammarExpr << std::endl;
    // std::cout << identifier << std::endl;
    gen[identifier] << grammarExpr >> [&](auto ss) {
      //      std::cout << identifier << std::endl;
      return "test";
    };
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

enum class LanguageType {
  CPP,
  JAVA,
  PYTHON,
  JSON
};

std::pair<bool, Parser> generateParserFromSHL(const std::string& filename, bool test=false) {
  Colors colors = getPredefinedColors();
  ParserBuilder g = generateParserBuilder(colors);
  std::ifstream ifs(filename);
  std::string shlFile((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  Parser gen;
  bool flag = true;
  try {
    auto output = g.run(shlFile, gen);
    std::cout << "Parsing result: " << output << std::endl;

    gen.setSeparator(gen["Separators"] << "['\t''\n' ]");

    gen["Program"] << "Identifier";
    gen.setStart(gen["Program"]);

    if (test) {
      // test a line
      std::string anotherInput;
      std::getline(std::cin, anotherInput);
      try {
        auto anotherOutput = gen.run(anotherInput);
        std::cout << "Parsing result: " << anotherOutput << std::endl;
      } catch (peg_parser::SyntaxError &ee) {
        std::cout << "GeneratedParser: Syntax error when parsing " << ee.syntax->rule->name << std::endl;
      }
    }
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
      return generateParserFromSHL("../src/java/java.shl");
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

int main(int argc, char **argv) {
  std::string filename;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-f")) {
      if (i + 1 < argc) filename = argv[i + 1];
      else throw std::invalid_argument("Arguments format error.");
    }
  }
  auto result = generateParserFromSHL("../src/cpp/cpp.shl", true);
  return 0;
}