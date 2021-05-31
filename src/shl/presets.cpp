//
// Created by garen on 5/29/21.
//

#include "shl/presets.h"

std::vector<int> indentDepth;
std::vector<std::pair<int, int> > blockRecords;

void initSHLGrammar(ParserBuilder& g) {
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
    gen["Indentation"] << "(' '*)" >> [](auto) {
      return "Indentation";
    };
    gen["lbracket"] << "('[')" >> [](auto) {
      return "lbracket";
    };
    gen["rbracket"] << "(']')" >> [](auto) {
      return "rbracket";
    };
    gen["question_mark"] << "('?')" >> [](auto) {
      return "question_mark";
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
    gen["CharAtomic"] << "(backslash? ([a-zA-Z0-9] | space | excl | question_mark | hash | dollar | percent | ampersand | lparen | rparen | asterisk | plus | comma | minus | dot | slash | lbracket | backslash | rbracket | caret | underscore | back_quote | lbrace | vertical_bar | rbrace | tilda | colon | semicolon))";


    return nullptr;
  };
  g["indent_aware"] << "('#indent_aware')" >> [](auto s, Parser& gen) {
    gen["InitBlocks"] << "''" << [&](auto&) -> bool {
      indentDepth.clear();
      return true;
    } >> [](auto) { return "InitBlocks"; };
    gen["SameIndentation"] << "Indentation" << [&](auto &s) -> bool {
      return s->length() == indentDepth.back();
    } >> [](auto) { return "SameIndentation"; };
    gen["SameIndentation"]->cacheable = false;

    gen["DeepIndentation"] << "Indentation" << [&](auto &s) -> bool {
      return s->length() > indentDepth.back();
    } >> [](auto) { return "DeepIndentation"; };
    gen["DeepIndentation"]->cacheable = false;

    gen["EnterBlock"] << "Indentation" << [&](auto &s) -> bool {
      if (indentDepth.empty() || s->length() > indentDepth.back()) {
        indentDepth.push_back(s->length());
        return true;
      } else return false;
    } >> [](auto) { return "EnterBlock"; };
    gen["EnterBlock"]->cacheable = false;

    gen["Line"] << "SameIndentation Grammar '\n'" >> [](auto) { return "Line"; };
    gen.getRule("Line")->cacheable = false;

    gen["EmptyLine"] << "Indentation '\n'" >> [](auto) { return "EmptyLine"; };

    gen["ExitBlock"] << "''" << [&](auto&) -> bool {
      indentDepth.pop_back();
      return true;
    } >> [](auto) { return "ExitBlock"; };
    gen.getRule("ExitBlock")->cacheable = false;

    gen["Block"] << "&EnterBlock Line (EmptyLine | Block | Line)* &ExitBlock" >> [&](auto e) {
      for (auto x : e) x.evaluate();
      //    blocks.push_back(Block{e.position(), e.length()});
      //    std::cout << "position: from " << e.position() << " to " << e.position() + e.length() << std::endl;
      blockRecords.push_back(std::make_pair(e.position(), e.length()));
      //    std::cout << "line from " << temp.first << " to " << temp.second << std::endl;
      return "Block";
    };
    return nullptr;
  };
}

void initParserBuilder(ParserBuilder& g, Colors& colors) {
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

  initSHLGrammar(g);
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
  // Shebang: to be continued...
  g["Shebang"] << "(indent_aware)";
  g["Program"] << "(InitParserBuilder Shebang? (Grammar | Comment)*)";
  g["Test"] << "Identifier ':' GrammarExpr MultiBlock";
  g.setStart(g["Program"]);
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
      auto temp = generateParserFromSHL("../src/python/python3.shl");
      if (temp.first) {
        auto gen = temp.second;
        gen["Grammar"] << "(Heads | Statement | Expr)";
        gen["Program"] << "((ImportStatement '\n')* InitBlocks Block)";
        gen.setStart(gen["Program"]);
        return std::make_pair(true, gen);
      } else return temp;
    }
    case LanguageType::JSON: {
      return generateParserFromSHL("../src/json/json.shl");
    }
    default: {
      throw std::runtime_error("Not implemented yet");
    }
  }
}

std::pair<long, long> getLineNumber(std::string_view input, std::pair<int, int> blockRecord) {
  return std::make_pair(
      std::count(input.begin(), input.begin() + blockRecord.first, '\n') + 1,
      std::count(input.begin(), input.begin() + blockRecord.first + blockRecord.second, '\n') + 1
  );
}
