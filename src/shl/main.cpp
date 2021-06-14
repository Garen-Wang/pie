#include "builtin.h"

int main(int argc, char **argv) {
    std::string shlFileName, testFileName;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-f")) {
            if (i + 1 < argc) shlFileName = argv[i + 1];
            else
                throw std::invalid_argument("Arguments format error.");
        } else if (!strcmp(argv[i], "-t")) {
            if (i + 1 < argc) testFileName = argv[i + 1];
            else
                throw std::invalid_argument("Arguments format error.");
        }
    }
    testFileName = "../test/python/test.py";
    std::ifstream ifs(testFileName);
    std::string testInput((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    std::cout << testInput << std::endl;
    //  auto result = generateParserFromSHL(shlFileName);

    auto temp = shl::generateLanguageParser(shl::LanguageType::PYTHON, nullptr);
    assert(temp.first);
    auto gen = temp.second;

    try {
        auto testOutput = gen.run(testInput);
        std::cout << "Parsing result: " << testOutput << std::endl;
        // std::cout << blockRecords.size() << std::endl;
        // for (auto x : blockRecords) {
        //   std::cout << x.first << ", " << x.second << std::endl;
        //   auto t = getLineNumber(testInput, x);
        //   std::cout << t.first << ", " << t.second << std::endl;
        // }
    } catch (peg_parser::SyntaxError &e) {
        std::cout << "Syntax error when parsing " << e.syntax->rule->name << std::endl;
    }
    return 0;
}
