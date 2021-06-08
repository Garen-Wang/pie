
#include "libs.hpp"
#include "window.hpp"

/*
class LineInput : public TextArea::Input {
public:
    LineInput(QString str = "", Style style = Style::default_style)
        : str(str), style(style), cursor(0) {}

    bool has_next() const override {
        return cursor < str.size();
    }

    int seek_to_row(int r) override {
        cursor = 0;
        return 0;
    }

    TextArea::Cell next() override {
        auto c = TextArea::Cell { str[cursor], style };
        ++cursor;
        return c;
    }

private:
    int cursor;
    QString str;
    Style style;
};

class BlockInput : public TextArea::Input {
public:
    BlockInput(QVector<TextArea::Input*> inputs) : inputs(inputs), cursor(0) {}

    bool has_next() const override {
        return cursor < inputs.size();
    }

    int seek_to_row(int r) override {
        cursor = qMin(r, inputs.size() - 1);
        for (int i = cursor; i < inputs.size(); ++i)
            inputs[i]->seek_to_row(0);
        return cursor;
    }

    TextArea::Cell next() override {
        auto c = inputs[cursor]->next();
        while (cursor < inputs.size() && ! inputs[cursor]->has_next() )
            ++cursor;
        return c;
    }

private:
    int cursor;
    QVector<TextArea::Input*> inputs;
};
*/


int main(int argc, char **argv) {
    Mode::initDefaultModes();
    QApplication app(argc, argv);

    Window win(NULL);

    QObject::connect(&win, SIGNAL (exit()), &app, SLOT (quit()));

    win.show();

    /*
    Rope *r = new Rope();
    Style s = Style::default_style;
    r->insert("pie text editor\n", 0, s);

    s.fg = QColor(255, 255, 0);
    s.bg = QColor(128, 128, 128);
    s.bold = s.italic = s.underline = s.strikethrough = true;
    r->insert("altogether", 16, s);

    auto ta = new TextArea(nullptr, r);
    // win.addTextArea(ta, "test");
    ta->resizeByChar(100, 100);
    ta->setEnabled(true);
    // ta->grabKeyboard();
    */

    /*
    Style s = Style::default_style;
    LineInput l1("pie text editor\n", s);

    s.bg = QColor(128, 128, 128);
    LineInput l2("colorful background\n", s);

    s = Style::default_style;
    s.fg = QColor(255, 255, 0);
    LineInput l3("colorful foreground\n", s);

    s = Style::default_style;
    s.bold = true;
    LineInput l4("bold text\n", s);

    s = Style::default_style;
    s.italic = true;
    LineInput l5("italic text\n", s);

    s = Style::default_style;
    s.underline = true;
    LineInput l6("underline\n", s);

    s = Style::default_style;
    s.strikethrough = true;
    LineInput l7("strikethrough\n", s);

    s.fg = QColor(255, 255, 0);
    s.bg = QColor(128, 128, 128);
    s.bold = s.italic = s.underline = s.strikethrough = true;
    LineInput l8("altogether\n", s);

    LineInput l9("CJK字符as well\n", s);

    s = Style::default_style;
    auto longest_line =
        QString("very very very very very very very very very ")
        + "very very very very very very very very very very very "
        + "very very very very very very very very very long line\n";
    LineInput l10(longest_line, s);

    QVector<TextArea::Input*> inputs = { &l1, &l2, &l3, &l4, &l5, &l6, &l7, &l8, &l9, &l10 };
    QVector<LineInput> lines(40);
    for (int i = 0; i < lines.size(); ++i) {
        lines[i] = LineInput("many many lines\n", s);
        inputs.push_back(&lines[i]);
    }

    BlockInput i(inputs);
    ta->resizeByChar(inputs.size(), longest_line.size());
    ta->setInput(&i);
    */

    /*

    ta->put(9, 0, "styled text abcde", s);
    ta->put(9, 12, "overridden");

    ta->put(10, 12, "overridden");
    ta->put(10, 0, "styled text abcde", s);

    ta->put(10, 12, "overridden");
    ta->put(10, 0, "styled text 一二三四五", s);
    */

    return app.exec();
}
