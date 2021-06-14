/* In this source code file, some preset grammar
 * of SHL (Syntax Highlighting Language) are defined.
 * The definitions here determine the fundamental grammar
 * of SHL, which will work to render syntax highlighting.
 */

#include "builtin.h"

#include <memory>

using namespace shl;

namespace shl {
    std::vector<int> indentDepth;
    std::vector<std::pair<int, int>> blockRecords;

    void initSHLGrammar(ParserBuilder &g) {
        g["InitParserBuilder"] << "''" >> [](auto, Parser &gen) {
            // preset grammar for parserBuilder
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
            // TODO: bug here, but have no idea to fix
            gen["single_quote"] << "[']" >> [](auto) {
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

            /* CharAtomic is defined here, independent of type of languages.
             * Here single and double quotation marks are not included.
             */
            gen["CharAtomic"]
                    << "(backslash? ([a-zA-Z0-9] | space | excl | question_mark | hash | dollar | "
                       "percent | ampersand | lparen | rparen | asterisk | plus | comma | minus | dot | "
                       "slash | lbracket | backslash | rbracket | caret | underscore | back_quote | "
                       "lbrace | vertical_bar | rbrace | tilda | colon | semicolon | langle | rangle))" >>
                [](auto) { return "CharAtomic"; };

            return nullptr;
        };
        g["indent_aware"] << "('#indent_aware')" >> [](auto s, Parser &gen) {
            gen["InitBlocks"] << "''" << [&](auto &) -> bool {
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
                } else
                    return false;
            } >> [](auto) { return "EnterBlock"; };
            gen["EnterBlock"]->cacheable = false;

            gen["Line"] << "SameIndentation Grammar '\n'" >> [](auto s) {
                s[1].evaluate();
                return "Line";
            };
            gen.getRule("Line")->cacheable = false;

            gen["EmptyLine"] << "Indentation '\n'" >> [](auto) { return "EmptyLine"; };

            gen["ExitBlock"] << "''" << [&](auto &) -> bool {
                indentDepth.pop_back();
                return true;
            } >> [](auto) { return "ExitBlock"; };
            gen.getRule("ExitBlock")->cacheable = false;

            gen["Block"] << "&EnterBlock Line (EmptyLine | Block | Line)* &ExitBlock" >> [&](auto e) {
                for (auto x : e) x.evaluate();
                // blocks.push_back(Block{e.position(), e.length()});
                // std::cout << "position: from " << e.position() << " to " << e.position() + e.length() << std::endl;
                blockRecords.push_back(std::make_pair(e.position(), e.length()));
                // std::cout << "line from " << temp.first << " to " << temp.second << std::endl;
                return "Block";
            };
            return nullptr;
        };
    }

    void initParserBuilder(ParserBuilder &g, Colors &colors, Rope *rope, bool render) {
        g.setSeparator(g["Separators"] << "['\t''\n' ]");
        g["Identifier"] << "(([a-zA-Z] | '_') ([a-zA-Z0-9] | '_')*)" >> [](auto s, Parser &) {
            return nullptr;
        };
        g["GrammarExpr"] << R"(('"' (!'"' .)* '"'))" >> [](auto, Parser &) {
            return nullptr;
        };
        g["Indent"] << "(['\t''\n' ]*)" >> [](auto, Parser &) {
            return nullptr;
        };
        g["EmptyBlock"] << "('{' Indent '}')" >> [](auto, Parser &) {
            return nullptr;
        };
        g["OnlyBlock"] << "('{' Indent Attr Indent '}')" >> [](auto s, Parser &gen) {
            return s[1].evaluate(gen);
        };
        g["equals"] << "(Indent '=' Indent)" >> [](auto, Parser &) {
            return nullptr;
        };
        g["for"] << "('for')" >> [](auto s, Parser &) {
            return nullptr;
        };
        g["ForStmt"] << "('for' ['\t' ]* '(' Identifier equals Index ';' ['\t' ]* Index ';' ['\t' ]* "
                        "Index ')' ['\t''\n' ]* '$' Identifier equals Attr)" >>
            [](auto s, Parser &gen) {
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
        g["SingleIndexStmt"] << "('$' Index equals Attr)" >> [](auto s, Parser &gen) {
            auto x = s[2].evaluate(gen);
            x->begin()->idx = std::stoi(s[0].string());
            return x;
        };
        g["MultiBlock"] << "('{' ['\t''\n' ]* ((ForStmt | SingleIndexStmt) ['\t''\n' ]*)* '}')" >>
            [](auto s, Parser &gen) {
                auto ret = new SyntaxHighlightInfos;
                for (int i = 0; i < s.size(); i++) {
                    // int idx = std::stoi(s[i].string());
                    // Attr attr = s[i + 2].evaluate(gen)->begin()->attr;
                    // DEBUG
                    // std::cout << "$" << idx << " = " << s[i + 2].string() << std::endl;

                    std::shared_ptr<SyntaxHighlightInfos> x = s[i].evaluate(gen);
                    ret->insert(ret->end(), x->begin(), x->end());
                }
                // std::cout << ret << std::endl;
                return std::shared_ptr<SyntaxHighlightInfos>(ret);
            };
        g["Block"] << "(EmptyBlock | OnlyBlock | MultiBlock)";
        g["Index"] << "(('+' | '-')? ('0' | ([1-9][0-9]*)))" >> [](auto s, Parser &) {
            return nullptr;
        };
        g["underline"] << "'underline'" >> [](auto, Parser &) {
            return nullptr;
        };
        g["bold"] << "'bold'" >> [](auto, Parser &) {
            return nullptr;
        };
        g["italic"] << "'italic'" >> [](auto, Parser &) {
            return nullptr;
        };
        g["strikethrough"] << "'strikethrough'" >> [](auto, Parser &) {
            return nullptr;
        };
        g["Color"] << colors.getExpr() >> [](auto s, Parser &) {
            return nullptr;
        };
        g["Attr"] << "(((bold | italic | underline | strikethrough | Color) Indent)* )" >>
            [&](auto s, Parser &) {
                Attr attr = Style::default_style;
                auto ret = new SyntaxHighlightInfos;

                for (int i = 0; i < s.size(); i += 2) {
                    auto str = s[i].string();
                    if (str == "bold") {
                        attr.bold = true;
                    } else if (str == "italic") {
                        attr.italic = true;
                    } else if (str == "underline") {
                        attr.underline = true;
                    } else if (str == "strikethrough") {
                        attr.strikethrough = true;
                    } else if (colors.exist(str)) {
                        attr.fg = colors.get(str);
                        // std::cout << "color: " << str << std::endl;
                    } else {
                        std::cerr << "unknown keyword" << std::endl;
                    }
                }
                ret->push_back(SyntaxHighlightInfo(attr));
                // std::cout << ret << std::endl;
                return std::shared_ptr<SyntaxHighlightInfos>(ret);
            };
        g["Comment"] << "('//' (!'\n' .)*)" >> [](auto, Parser &) {
            return nullptr;
        };

        initSHLGrammar(g);
        g["Grammar"] << "(Identifier ':' GrammarExpr Block)" >> [=](auto s, Parser &gen) {
            std::string identifier = s[0].string();
            auto grammarExpr = s[1].string().substr(1, s[1].string().length() - 2);
            // qDebug() << QString().fromStdString(identifier) << ", " << QString().fromStdString(grammarExpr);
            auto syntaxHighlightInfos = s[2].evaluate(gen);
            if (syntaxHighlightInfos != nullptr) {
                // std::cout << "---" << syntaxHighlightInfos << std::endl;
                // qDebug() << "!= nullptr";
                gen[identifier] << grammarExpr >> [=](auto ss) {
                    for (auto s : ss) s.evaluate();
                    // std::cout << "---" << syntaxHighlightInfos->size() << std::endl;
                    if (syntaxHighlightInfos->begin()->idx == -1) {
                        // qDebug() << "idx = -1";
                        if (rope)
                            rope->setAttr(
                                ss.position(), ss.length(),
                                syntaxHighlightInfos->begin()->attr);
                    } else {
                        // qDebug() << "idx != -1";
                        SyntaxHighlightInfos &infos = *syntaxHighlightInfos;
                        for (auto info : infos) {
                            if (info.idx >= ss.size()) continue;
                            if (rope)
                                rope->setAttr(
                                    ss[info.idx].position(), ss[info.idx].length(),
                                    info.attr);
                        }
                    }
                    return "test";
                };
            } else {
                // qDebug() << "== nullptr";
                gen[identifier] << grammarExpr >> [=](auto ss) {
                    for (auto s : ss) s.evaluate();

                    return "test";
                };
            }
            return nullptr;
        };

        g["Program"] << "(InitParserBuilder indent_aware? (Grammar | Comment)+)" >> [=](auto ss, Parser &gen) {
            for (const auto &s : ss) {
                s.evaluate(gen);
            }
            return nullptr;
        };
        g["Test"] << "Identifier Indentation" >> [=](auto ss, Parser &gen) {
            return nullptr;
        };
        g.setStart(g["Program"]);
    }

    std::pair<long, long> getLineNumber(std::string_view input, std::pair<int, int> blockRecord) {
        return std::make_pair(
            std::count(input.begin(), input.begin() + blockRecord.first, '\n') + 1,
            std::count(input.begin(), input.begin() + blockRecord.first + blockRecord.second, '\n') + 1);
    }
    int getIdx(const std::string &str, int size) {
        if (str[0] == '-')
            return size - std::stoi(str.substr(1));
        else if (str[0] == '+')
            return std::stoi(str.substr(1));
        else
            return std::stoi(str);
    }
    bool isLowercase(const std::string &str) {
        return std::all_of(str.begin(), str.end(), [](char x) { return islower(x); });
    }

    std::pair<bool, Parser> generateLanguageParser(LanguageType languageType, Rope *rope) {
        switch (languageType) {
            case LanguageType::CPP: {
                auto temp = generateParserFromSHL("../src/shl/rules/cpp.shl", rope);
                if (temp.first) {
                    auto gen = temp.second;
                    gen["Program"] << "Grammar+" >> [](auto ss) {
                        for (const auto &s : ss)
                            s.evaluate();
                        return "";
                    };
                    gen.setStart(gen["Program"]);
                    return std::make_pair(true, gen);
                } else
                    return temp;
            }
            case LanguageType::JAVA: {
                auto temp = generateParserFromSHL("../src/shl/rules/java.shl", rope);
                if (temp.first) {
                    auto gen = temp.second;
                    gen["Program"] << "ImportStatement* Class+ newline*" >> [](auto ss) {
                        for (const auto &s : ss)
                            s.evaluate();
                        return "";
                    };
                    gen.setStart(gen["Program"]);
                    return std::make_pair(true, gen);
                } else
                    return temp;
            }
            case LanguageType::PYTHON: {
                auto temp = generateParserFromSHL("../src/shl/rules/python3.shl", rope);
                if (temp.first) {
                    auto gen = temp.second;
                    gen["Grammar"] << "(Comment | ((Heads | Statement | Expr) Indentation Comment?))";
                    gen["Program"] << "(InitBlocks Block)" >> [](auto ss) {
                        for (auto s : ss) s.evaluate();
                        return "Program";
                    };
                    // gen["Program"] << "Expr";
                    gen.setStart(gen["Program"]);
                    return std::make_pair(true, gen);
                } else
                    return temp;
            }
            default: {
                throw std::invalid_argument("Not implemented yet");
            }
        }
    }
}// namespace shl
