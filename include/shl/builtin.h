#ifndef SHL_BUILTIN_H
#define SHL_BUILTIN_H

#include <peg_parser/generator.h>

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

#include "config.hpp"
#include "rope.hpp"

using Attr = Style;

namespace shl {
    /* types of languages under syntax highlighting support */
    enum class LanguageType {
        CPP,
        JAVA,
        PYTHON,
    };

    /* a pair of syntax highlighting message, including indexes and corresponding style attributes */
    class SyntaxHighlightInfo {
      public:
        mutable int idx = 0;
        Attr attr;
        explicit SyntaxHighlightInfo(Attr attr);
        explicit SyntaxHighlightInfo(int idx, Attr attr);
    };

    /* act as the return type of each semantic action */
    typedef std::vector<SyntaxHighlightInfo> SyntaxHighlightInfos;
    using Parser = peg_parser::ParserGenerator<std::string>;
    using ParserBuilder = peg_parser::ParserGenerator<std::shared_ptr<SyntaxHighlightInfos>, Parser &>;

    class Colors {
      private:
        /* map color names to corresponding QFont objects */
        std::unordered_map<std::string, QColor> colors;

      public:
        // get expression in parsing expression grammar
        std::string getExpr();
        // all strings in `colors` are expected to be lowercase
        void append(const std::string &color, const QColor &_color);
        bool exist(const std::string &str);
        QColor get(const std::string &str);
    };

    /* return a ParserBuilder that have finished configuration */
    ParserBuilder *generateParserBuilder(Colors &colors, Rope *);

    /* check all colors under support */
    Colors getPredefinedColors();

    /* receive path of shl file, return a parser generated according to the grammar of shl file */
    std::pair<bool, Parser> generateParserFromSHL(const std::string &filename, Rope *rope);

    /* the initial configuration to a new ParserBuilder */
    void initParserBuilder(ParserBuilder &g, Colors &colors, Rope *, bool render = false);

    /* debug messages when indent-aware feature is enable */
    extern std::vector<int> indentDepth; // elements are the number of spaces as indent
    extern std::vector<std::pair<int, int>> blockRecords; // record the beginning and the end line of an indent block

    /* teach a ParserBuilder about the grammar of Syntax Highlighting Language */
    void initSHLGrammar(ParserBuilder &g);

    /* find out the positional interval in text, from which line to which line */
    std::pair<long, long> getLineNumber(std::string_view input, std::pair<int, int> blockRecord);

    /* return the real index described in shl language, given the size */
    int getIdx(const std::string &str, int size);

    /* check if all the letters in the string are lowercase */
    bool isLowercase(const std::string &str);

    /* generate the corresponding language parser */
    /* user can directly invoke this API to get the final parser */
    std::pair<bool, shl::Parser> generateLanguageParser(LanguageType languageType, Rope *);
}// namespace shl

/* FilePath is designed to make file path experience consistent across Windows and Linux,
 * without hardcoding the whole path */
class FilePath {
  private:
    std::string pathString; // stores the actual path on different system

    /* get separator, determined in runtime */
    const static char _separator = 
#if defined _WIN32 || defined __CYGWIN__
      '\\';
#else
      '/';
#endif

  public:
    /* allow multiple strings as arguments */
    explicit FilePath(int n_args, ...);

    ~FilePath() = default;

    /* reconstruct the path if the path needs to modifying */
    static std::string constructFileString(int n_args, ...);

    std::string getPathString() const;
};

#endif// SHL_BUILTIN_H
