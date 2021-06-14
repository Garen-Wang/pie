/* The methods defined here are used to generate a *ParserBuilder*
 * and the way to generate *Parser* through generated *ParserBuilder*.
 */

#include "builtin.h"

namespace shl {
    SyntaxHighlightInfo::SyntaxHighlightInfo(Attr attr) {
        this->idx = -1;
        this->attr = attr;
    }

    SyntaxHighlightInfo::SyntaxHighlightInfo(int idx, Attr attr) {
        this->idx = idx;
        this->attr = attr;
    }

    std::string Colors::getExpr() {
        std::string expr = "(";
        for (const auto &it : colors) {
            expr += "'" + it.first + "'|";
        }
        expr.pop_back();
        expr += ")";
        return expr;
    }

    void Colors::append(const std::string &color, const QColor &_color) {
        colors[color] = _color;
    }
    bool Colors::exist(const std::string &str) {
        return colors.count(str) != 0;
    }
    QColor Colors::get(const std::string &str) {
        return colors[str];
    }

    ParserBuilder *generateParserBuilder(Colors &colors, Rope *rope) {
        auto *g = new ParserBuilder();
        initParserBuilder(*g, colors, rope);
        return g;
    }

    Colors getPredefinedColors() {
        Colors colors;
        colors.append("black", QColor(0, 0, 0));
        colors.append("gray", QColor(80, 80, 80));
        colors.append("shallow_yellow", QColor(204, 255, 0));
        colors.append("blue", QColor(0, 0, 255));
        colors.append("cyan", QColor(0, 255, 255));
        colors.append("purple", QColor(127, 0, 255));
        colors.append("green", QColor(0, 128, 0));
        colors.append("dark_purple", QColor(80, 80, 120));
        colors.append("orange", QColor(255, 20, 147));
        colors.append("white", QColor(255, 255, 255));
        colors.append("yellow", QColor(255, 255, 0));
        colors.append("pink", QColor(255, 182, 193));
        return colors;
    }

    std::pair<bool, Parser> generateParserFromSHL(const std::string &filename, Rope *rope) {
        Colors colors = getPredefinedColors();
        ParserBuilder *g = generateParserBuilder(colors, rope);
        std::ifstream ifs(filename);
        std::string shlFile((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        Parser gen;
        bool flag = true;
        try {
            auto output = g->run(shlFile, gen);
            // gen["Identifier"] >> [](auto) {
            //        std::cout << "11" << std::endl;
            //        return "Identifier";
            //      };
            //    std::cout << "Parsing result: " << output << std::endl;
        } catch (peg_parser::SyntaxError &e) {
            flag = false;
            std::cout << "ParserBuilder: Syntax error when parsing " << e.syntax->rule->name << std::endl;
        }
        return std::make_pair(flag, gen);
    }
}// namespace shl
