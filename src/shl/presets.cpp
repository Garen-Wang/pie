//
// Created by garen on 5/29/21.
//

#include "presets.h"

#include <memory>

using namespace shl;

namespace shl {
  std::vector<int> indentDepth;
  std::vector<std::pair<int, int> > blockRecords;

  void initSHLGrammar(ParserBuilder& g) {
    g["InitParserBuilder"] << "''" >> [](auto, Parser& gen) {
      // preset for parserBuilder
      gen["lparen"] << "('(')" >> [](auto, Rope*) { return "lparen"; };
      gen["rparen"] << "(')')" >> [](auto, Rope*) { return "rparen"; };
      gen["langle"] << "('<')" >> [](auto, Rope*) { return "langle"; };
      gen["rangle"] << "('>')" >> [](auto, Rope*) { return "rangle"; };
      gen["lbrace"] << "('{')" >> [](auto, Rope*) { return "lbrace"; };
      gen["rbrace"] << "('}')" >> [](auto, Rope*) { return "rbrace"; };
      gen["comma"] << "(',')" >> [](auto, Rope*) { return "comma"; };
      gen["colon"] << "(':')" >> [](auto, Rope*) { return "colon"; };
      gen["semicolon"] << "(';')" >> [](auto, Rope*) { return "semicolon"; };
      // TODO: bug here, but have no idea to fix
      gen["single_quote"] << "[']" >> [](auto, Rope*) { return "single_quote"; };
      gen["double_quote"] << "('\"')" >> [](auto, Rope*) { return "double_quote"; };
      gen["single_equal"] << "('=')" >> [](auto, Rope*) { return "single_equal"; };
      gen["equals"] << "Indent single_equal Indent" >> [](auto, Rope*) { return "equals"; };
      gen["double_equal"] << "('==')" >> [](auto, Rope*) { return "double_equal"; };
      gen["newline"] << "('\n')" >> [](auto, Rope*) { return "newline"; };
      gen["until_newline"] << "((!'\n' .)*)" >> [](auto, Rope*) { return "until_newline"; };
      gen["tab"] << "('\t')" >> [](auto, Rope*) { return "tab"; };
      gen["space"] << "(' ')" >> [](auto, Rope*) { return "space"; };
      gen["Indent"] << "(['\t''\n' ]*)" >> [](auto, Rope*) { return "Indent"; };
      gen["Indentation"] << "(' '*)" >> [](auto, Rope*) { return "Indentation"; };
      gen["lbracket"] << "('[')" >> [](auto, Rope*) { return "lbracket"; };
      gen["rbracket"] << "(']')" >> [](auto, Rope*) { return "rbracket"; };
      gen["question_mark"] << "('?')" >> [](auto, Rope*) { return "question_mark"; };
      gen["bell"] << "('\a')" >> [](auto, Rope*) { return "bell"; };
      gen["backspace"] << "('\b')" >> [](auto, Rope*) { return "backspace"; };
      gen["carriage_ret"] << "('\r')" >> [](auto, Rope*) { return "carriage_ret"; };

      gen["excl"] << "('!')" >> [](auto, Rope*) { return "excl"; };
      gen["space"] << "(' ')" >> [](auto, Rope*) { return "space"; };
      gen["hash"] << "('#')" >> [](auto, Rope*) { return "hash"; };
      gen["dollar"] << "('$')" >> [](auto, Rope*) { return "dollar"; };
      gen["percent"] << "('%')" >> [](auto, Rope*) { return "percent"; };
      gen["ampersand"] << "('&')" >> [](auto, Rope*) { return "ampersand"; };
      gen["asterisk"] << "('*')" >> [](auto, Rope*) { return "asterisk"; };
      gen["plus"] << "('+')" >> [](auto, Rope*) { return "plus"; };
      gen["minus"] << "('-')" >> [](auto, Rope*) { return "minus"; };
      gen["dot"] << "('.')" >> [](auto, Rope*) { return "dot"; };
      gen["slash"] << "('/')" >> [](auto, Rope*) { return "slash"; };
      gen["backslash"] << "('\\\\')" >> [](auto, Rope*) { return "backslash"; };
      gen["underscore"] << "('_')" >> [](auto, Rope*) { return "underscore"; };
      gen["caret"] << "('^')" >> [](auto, Rope*) { return "caret"; };
      gen["back_quote"] << "('`')" >> [](auto, Rope*) { return "back_quote"; };
      gen["tilda"] << "('~')" >> [](auto, Rope*) { return "tilda"; };
      gen["vertical_bar"] << "('|')" >> [](auto, Rope*) { return "vertical_bar"; };

      // CharAtomic is provided here
      // sorry, '\0' cannot be expressed in preset grammar
      // not contain single_quote and double_quote
      gen["CharAtomic"]
              << "(backslash? ([a-zA-Z0-9] | space | excl | question_mark | hash | dollar | "
                 "percent | ampersand | lparen | rparen | asterisk | plus | comma | minus | dot | "
                 "slash | lbracket | backslash | rbracket | caret | underscore | back_quote | "
                 "lbrace | vertical_bar | rbrace | tilda | colon | semicolon | langle | rangle))"
          >> [](auto, Rope*) { return "CharAtomic"; };

      return nullptr;
    };
    g["indent_aware"] << "('#indent_aware')" >> [](auto s, Parser& gen) {
      gen["InitBlocks"] << "''" << [&](auto&) -> bool {
        indentDepth.clear();
        return true;
      } >> [](auto, Rope*) { return "InitBlocks"; };
      gen["SameIndentation"] << "Indentation" << [&](auto& s) -> bool {
        return s->length() == indentDepth.back();
      } >> [](auto, Rope*) { return "SameIndentation"; };
      gen["SameIndentation"]->cacheable = false;

      gen["DeepIndentation"] << "Indentation" << [&](auto& s) -> bool {
        return s->length() > indentDepth.back();
      } >> [](auto, Rope*) { return "DeepIndentation"; };
      gen["DeepIndentation"]->cacheable = false;

      gen["EnterBlock"] << "Indentation" << [&](auto& s) -> bool {
        if (indentDepth.empty() || s->length() > indentDepth.back()) {
          indentDepth.push_back(s->length());
          return true;
        } else
          return false;
      } >> [](auto, Rope*) { return "EnterBlock"; };
      gen["EnterBlock"]->cacheable = false;

      gen["Line"] << "SameIndentation Grammar '\n'" >> [](auto, Rope*) { return "Line"; };
      gen.getRule("Line")->cacheable = false;

      gen["EmptyLine"] << "Indentation '\n'" >> [](auto, Rope*) { return "EmptyLine"; };

      gen["ExitBlock"] << "''" << [&](auto&) -> bool {
        indentDepth.pop_back();
        return true;
      } >> [](auto, Rope*) { return "ExitBlock"; };
      gen.getRule("ExitBlock")->cacheable = false;

      gen["Block"] << "&EnterBlock Line (EmptyLine | Block | Line)* &ExitBlock" >>
          [&](auto e, Rope*) {
            for (auto x : e) x.evaluate();
            //    blocks.push_back(Block{e.position(), e.length()});
            //    std::cout << "position: from " << e.position() << " to " << e.position() +
            //    e.length() << std::endl;
            blockRecords.push_back(std::make_pair(e.position(), e.length()));
            //    std::cout << "line from " << temp.first << " to " << temp.second << std::endl;
            return "Block";
          };
      return nullptr;
    };
  }

  void initParserBuilder(ParserBuilder& g, Colors& colors, bool render) {
    // it seems that setSeparator cannot work properly, maybe need to be removed
    g.setSeparator(g["Separators"] << "['\t''\n' ]");
    g["Identifier"] << "(([a-zA-Z] | '_') ([a-zA-Z0-9] | '_')*)" >>
        [](auto s, Parser&) { return nullptr; };
    g["GrammarExpr"] << R"(('"' (!'"' .)* '"'))" >> [](auto, Parser&) { return nullptr; };
    g["Indent"] << "(['\t''\n' ]*)" >> [](auto, Parser&) { return nullptr; };
    g["EmptyBlock"] << "('{' Indent '}')" >> [](auto, Parser&) { return nullptr; };
    g["OnlyBlock"] << "('{' Indent Attr Indent '}')" >>
        [](auto s, Parser& gen) { return s[1].evaluate(gen); };
    g["equals"] << "(Indent '=' Indent)" >> [](auto, Parser&) { return nullptr; };
    g["for"] << "('for')" >> [](auto s, Parser&) { return nullptr; };
    g["ForStmt"] << "('for' ['\t' ]* '(' Identifier equals Index ';' ['\t' ]* Index ';' ['\t' ]* "
                    "Index ')' ['\t''\n' ]* '$' Identifier equals Attr)"
        >> [](auto s, Parser& gen) {
            assert(s[0].string() == s[5].string());
            int size = s.size();
            int left = getIdx(s[2].string(), size);
            int right = getIdx(s[3].string(), size);
            int inc = getIdx(s[4].string(), size);
            auto ret = new SyntaxHighlightInfos;
            std::shared_ptr<SyntaxHighlightInfos> x = s[7].evaluate(gen);
            for (int i = left; i < right; i += inc) {
              ret->emplace_back(i, x->begin()->attr);
            }
            return std::shared_ptr<SyntaxHighlightInfos>(ret);
          };
    // for loop
    g["SingleIndexStmt"] << "('$' Index equals Attr)" >> [](auto s, Parser& gen) {
      auto x = s[2].evaluate(gen);
      x->begin()->idx = std::stoi(s[0].string());
      return x;
    };
    g["MultiBlock"] << "('{' ['\t''\n' ]* ((ForStmt | SingleIndexStmt) ['\t''\n' ]*)* '}')" >>
        [](auto s, Parser& gen) {
          auto ret = new SyntaxHighlightInfos;
          for (int i = 0; i < s.size(); i++) {
            // int idx = std::stoi(s[i].string());
            // Attr attr = s[i + 2].evaluate(gen)->begin()->attr;
            // DEBUG
            // std::cout << "$" << idx << " = " << s[i + 2].string() << std::endl; // since it
            // contains newline

            std::shared_ptr<SyntaxHighlightInfos> x = s[i].evaluate(gen);
            ret->insert(ret->end(), x->begin(), x->end());
          }
          // std::cout << ret << std::endl;
          return std::shared_ptr<SyntaxHighlightInfos>(ret);
        };
    g["Block"] << "(EmptyBlock | OnlyBlock | MultiBlock)";
    g["Index"] << "(('+' | '-')? ('0' | ([1-9][0-9]*)))" >> [](auto s, Parser&) { return nullptr; };
    g["underlined"] << "'underlined'" >> [](auto, Parser&) { return nullptr; };
    g["bold"] << "'bold'" >> [](auto, Parser&) { return nullptr; };
    g["italic"] << "'italic'" >> [](auto, Parser&) { return nullptr; };
    g["strikethrough"] << "'strikethrough'" >> [](auto, Parser&) { return nullptr; };
    g["Color"] << colors.getExpr() >> [](auto s, Parser&) { return nullptr; };
    g["Attr"] << "(((bold | italic | underlined | strikethrough | Color) Indent)* )" >>
        [&](auto s, Parser&) {
          Attr attr;
          auto ret = new SyntaxHighlightInfos;

          for (int i = 0; i < s.size(); i += 2) {
            std::string str = s[i].string();
            if (str == "bold") {
              attr.bold = true;
            } else if (str == "italic") {
              attr.italic = true;
            } else if (str == "underlined") {
              attr.underline = true;
            } else if (str == "strikethrough") {
              attr.strikethrough = true;
            } else if (colors.exist(str)) {
              attr.fg = QColor(str.c_str());
              // std::cout << "color: " << str << std::endl;
            } else {
              std::cerr << "unknown keyword" << std::endl;
            }
          }
          ret->emplace_back(attr);
          // std::cout << ret << std::endl;
          return std::shared_ptr<SyntaxHighlightInfos>(ret);
        };
    g["Comment"] << "('//' (!'\n' .)*)" >> [](auto, Parser&) {
      return nullptr;
    };

    initSHLGrammar(g);
    g["Grammar"] << "(Identifier ':' GrammarExpr Block)" >> [&](auto s, Parser& gen) {
      std::string identifier = s[0].string();
      auto grammarExpr = s[1].string().substr(1, s[1].string().length() - 2);
      auto syntaxHighlightInfos = s[2].evaluate(gen);
      if (syntaxHighlightInfos != nullptr) {
        // std::cout << "---" << syntaxHighlightInfos << std::endl;
        gen[identifier] << grammarExpr >> [=](auto ss, Rope* rope) {
          if (identifier == "Identifier" || isLowercase(identifier)) {
            identifiers.insert(ss.string());
          }
          for (auto s : ss) s.evaluate(rope);
          // std::cout << "---" << syntaxHighlightInfos->size() << std::endl;
          if (syntaxHighlightInfos->begin()->idx == -1) {
            // std::cout << "dss" << std::endl;
            // changeAttr(syntaxHighlightInfos->begin()->attr, ss.position(),
            //            ss.position() + ss.length());
            rope->setAttr(ss.position(), ss.length(), syntaxHighlightInfos->begin()->attr);
          } else {
            SyntaxHighlightInfos& infos = *syntaxHighlightInfos;
            for (auto info : infos) {
              if (info.idx >= ss.size()) continue;
              // changeAttr(info.attr, ss[info.idx].position(),
              //            ss[info.idx].position() + ss[info.idx].length());
              rope->setAttr(ss[info.idx].position(), ss[info.idx].length(), info.attr);
            }
          }
          return "test";
        };
      } else {
        gen[identifier] << grammarExpr >> [=](auto ss, Rope* rope) {
          // std::cout << identifier << std::endl;
          if (identifier == "Identifier") {
            identifiers.insert(ss.string());
          }
          for (auto s : ss) s.evaluate(rope);

          return "test";
        };
      }
      return nullptr;
    };

    // Shebang: to be continued...
    g["Shebang"] << "(indent_aware)";
    g["Program"] << "(InitParserBuilder Shebang? (Grammar | Comment)*)";
    g["Test"] << "Identifier ':' GrammarExpr MultiBlock";
    g.setStart(g["Program"]);
  }

  std::pair<long, long> getLineNumber(std::string_view input, std::pair<int, int> blockRecord) {
    return std::make_pair(
        std::count(input.begin(), input.begin() + blockRecord.first, '\n') + 1,
        std::count(input.begin(), input.begin() + blockRecord.first + blockRecord.second, '\n')
            + 1);
  }
  int getIdx(const std::string& str, int size) {
    if (str[0] == '-')
      return size - std::stoi(str.substr(1));
    else if (str[0] == '+')
      return std::stoi(str.substr(1));
    else
      return std::stoi(str);
  }
  bool isLowercase(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](char x) { return islower(x); });
  }
}  // namespace shl

std::pair<bool, Parser> generateLanguageParser(LanguageType languageType) {
  switch (languageType) {
    case LanguageType::CPP: {
      auto temp = generateParserFromSHL("../src/shl/rules/cpp.shl");
      if (temp.first) {
        auto gen = temp.second;
        gen["Program"] << "Grammar*";
        gen.setStart(gen["Program"]);
        return std::make_pair(true, gen);
      } else return temp;
    }
    case LanguageType::JAVA: {
      auto temp = generateParserFromSHL("../src/shl/rules/java.shl");
      if (temp.first) {
        auto gen = temp.second;
        gen["Program"] << "ImportStatement* Class+ newline*";
        gen.setStart(gen["Program"]);
        return std::make_pair(true, gen);
      } else return temp;
    }
    case LanguageType::PYTHON: {
      auto temp = generateParserFromSHL("../src/shl/rules/python3.shl");
      if (temp.first) {
        auto gen = temp.second;
        gen["Grammar"] << "(Comment | ((Heads | Statement | Expr) Comment?))";
        gen["Program"] << "(InitBlocks Block)";

        //        gen["Program"] << "Expr";
        gen.setStart(gen["Program"]);
        return std::make_pair(true, gen);
      } else return temp;
    }
    default: {
      throw std::invalid_argument("Not implemented yet");
    }
  }
}
