//
// Created by garen on 5/23/21.
//
#include "shl/parser_builder.h"

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

// boundary

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
    // preset for parserBuilder
    gen["lparen"] << "('(')" >> [](auto) {
      return "lparen";
    };
    gen["rparen"] << "(')')" >> [](auto) {
      return "rparen";
    };
    gen["langle"] << "('<')" >> [](auto) {
      return "langle";
    };
    gen["rangle"] << "('>')" >> [](auto) {
      return "rangle";
    };
    gen["lbrace"] << "('{')" >> [](auto) {
      return "lbrace";
    };
    gen["rbrace"] << "('}')" >> [](auto) {
      return "rbrace";
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
    gen["single_equal"] << "('=')" >> [](auto) {
      return "single_equal";
    };
    gen["equals"] << "Indent single_equal Indent" >> [](auto) {
      return "equals";
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
    gen["Indent"] << "(['\t''\n' ]*)" >> [](auto) {
      return "Indent";
    };
    gen["lbracket"] << "('[')" >> [](auto) {
      return "lbracket";
    };
    gen["rbracket"] << "(']')" >> [](auto) {
      return "rbracket";
    };
    gen["question"] << "('?')" >> [](auto) {
      return "question";
    };
    gen["bell"] << "('\a')" >> [](auto) {
      return "bell";
    };
    gen["backspace"] << "('\b')" >> [](auto) {
      return "backspace";
    };
    gen["carriage_ret"] << "('\r')" >> [](auto) {
      return "carriage_ret";
    };

    gen["excl"] << "('!')" >> [](auto) {
      return "excl";
    };
    gen["space"] << "(' ')" >> [](auto) {
      return "space";
    };
    gen["hash"] << "('#')" >> [](auto) {
      return "hash";
    };
    gen["dollar"] << "('$')" >> [](auto) {
      return "dollar";
    };
    gen["percent"] << "('%')" >> [](auto) {
      return "percent";
    };
    gen["ampersand"] << "('&')" >> [](auto) {
      return "ampersand";
    };
    gen["asterisk"] << "('*')" >> [](auto) {
      return "asterisk";
    };
    gen["plus"] << "('+')" >> [](auto) {
      return "plus";
    };
    gen["minus"] << "('-')" >> [](auto) {
      return "minus";
    };
    gen["dot"] << "('.')" >> [](auto) {
      return "dot";
    };
    gen["slash"] << "('/')" >> [](auto) {
      return "slash";
    };
    gen["backslash"] << "('\\\\')" >> [](auto) {
      return "backslash";
    };
    gen["underscore"] << "('_')" >> [](auto) {
      return "underscore";
    };
    gen["caret"] << "('^')" >> [](auto) {
      return "caret";
    };
    gen["back_quote"] << "('`')" >> [](auto) {
      return "back_quote";
    };
    gen["tilda"] << "('~')" >> [](auto) {
      return "tilda";
    };
    gen["vertical_bar"] << "('|')" >> [](auto) {
      return "vertical_bar";
    };

    // CharAtomic is provided here
    // sorry, '\0' cannot be expressed in preset grammar
    // not contain single_quote and double_quote
    gen["CharAtomic"] << "(backslash? ([a-zA-Z0-9] | space | excl | hash | dollar | percent | ampersand | lparen | rparen | asterisk | plus | comma | minus | dot | slash | lbracket | backslash | rbracket | caret | underscore | back_quote | lbrace | vertical_bar | rbrace | tilda))";


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
  std::string shlFileName, testFileName;
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-f")) {
      if (i + 1 < argc) shlFileName = argv[i + 1];
      else throw std::invalid_argument("Arguments format error.");
    } else if (!strcmp(argv[i], "-t")) {
      if (i + 1 < argc) testFileName = argv[i + 1];
      else throw std::invalid_argument("Arguments format error.");
    }
  }
//  auto result = generateParserFromSHL(shlFileName);
  auto result = generateParserFromSHL("../src/java/java.shl");
  auto gen = result.second;
  gen["Program"] << "ImportStatement* Class+";
//  gen["Program"] << "Expr";
  gen.setStart(gen["Program"]);

  testFileName = "../src/java/tests/test.java";
  if (!testFileName.empty()) {
    std::ifstream ifs(testFileName);
    std::string testInput((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    try {
      auto testOutput = gen.run(testInput);
      std::cout << "Parsing result: " << testOutput << std::endl;
    } catch (peg_parser::SyntaxError &e) {
      std::cout << "Syntax error when parsing " << e.syntax->rule->name << std::endl;
    }
  }
  return 0;
}