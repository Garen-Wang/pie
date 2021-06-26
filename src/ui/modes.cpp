#include "modes.hpp"

Mode::Mode(QString name) : m_name(name) {
    all_modes.insert(name, this);
}

Mode::~Mode() {
    all_modes.remove(m_name);
    for (Binding &b : m_bindings)
        delete b.action;
}

const QString &Mode::name() {
    return m_name;
}

void Mode::addBinding(Qt::KeyboardModifiers mods, int key, Action *action) {
    m_bindings.push_back({ mods, key, action });
}

void Mode::acceptKeyEvent(QKeyEvent *ev, ActionVisitor *visitor) {
    for (Binding &b : m_bindings) {
        if (ev->modifiers() == b.mods && ev->key() == b.key) {
            b.action->accept(visitor);
            break;
        }
    }
}


Mode *Mode::findMode(const QString &name) {
    if (! all_modes.contains(name))
        return nullptr;

    return all_modes[name];
}

void Mode::initDefaultModes() {
    static const int left_key = Qt::Key_J;
    static const int right_key = Qt::Key_L;
    static const int up_key = Qt::Key_I;
    static const int down_key = Qt::Key_K;
    normal.addBinding(
        Qt::NoModifier, Qt::Key_U,
        new SetMode("insert")
    );
    normal.addBinding(
        Qt::NoModifier, Qt::Key_O,
        new InsertNewline()
    );
    normal.addBinding(
        Qt::NoModifier, right_key,
        new ModifySelection(false, NextH)
    );
    normal.addBinding(
        Qt::ShiftModifier, right_key,
        new ModifySelection(true, NextH)
    );
    normal.addBinding(
        Qt::NoModifier, left_key,
        new ModifySelection(false, PrevH)
    );
    normal.addBinding(
        Qt::ShiftModifier, left_key,
        new ModifySelection(true, PrevH)
    );
    normal.addBinding(
        Qt::NoModifier, up_key,
        new ModifySelection(false, PrevV)
    );
    normal.addBinding(
        Qt::ShiftModifier, up_key,
        new ModifySelection(true, PrevV)
    );
    normal.addBinding(
        Qt::NoModifier, down_key,
        new ModifySelection(false, NextV)
    );
    normal.addBinding(
        Qt::ShiftModifier, down_key,
        new ModifySelection(true, NextV)
    );
    normal.addBinding(
        Qt::NoModifier, down_key,
        new ModifySelection(false, NextV)
    );
    normal.addBinding(
        Qt::ShiftModifier, down_key,
        new ModifySelection(true, NextV)
    );

    normal.addBinding(
        Qt::NoModifier, Qt::Key_W,
        new ModifySelection(false, NextW)
    );
    normal.addBinding(
        Qt::ShiftModifier, Qt::Key_W,
        new ModifySelection(true, NextW)
    );

    normal.addBinding(
        Qt::NoModifier, Qt::Key_B,
        new ModifySelection(false, PrevW)
    );
    normal.addBinding(
        Qt::ShiftModifier, Qt::Key_B,
        new ModifySelection(true, PrevW)
    );

    normal.addBinding(
        Qt::NoModifier, Qt::Key_X,
        new ModifySelection(false, Line)
    );
    normal.addBinding(
        Qt::ShiftModifier, Qt::Key_X,
        new ModifySelection(true, Line)
    );

    normal.addBinding(
        Qt::NoModifier, Qt::Key_M,
        new ModifySelection(false, Match)
    );
    normal.addBinding(
        Qt::ShiftModifier, Qt::Key_M,
        new ModifySelection(true, Match)
    );

    normal.addBinding(
        Qt::NoModifier, Qt::Key_D,
        new DelSelection()
    );
    normal.addBinding(
        Qt::NoModifier, Qt::Key_C,
        new ReplaceSelection()
    );

    normal.addBinding(
        Qt::NoModifier, Qt::Key_Z,
        new Undo()
    );
    normal.addBinding(
        Qt::NoModifier, Qt::Key_Y,
        new Redo()
    );

    for (char c = 'a'; c <= 'z'; ++c) {
        insert.addBinding(
            Qt::NoModifier, Qt::Key_A + c - 'a',
            new InsertChar(c)
        );
        insert.addBinding(
            Qt::ShiftModifier, Qt::Key_A + c - 'a',
            new InsertChar(c - 'a' + 'A')
        );
    }
    for (char c = '0'; c <= 'z'; ++c) {
        insert.addBinding(
            Qt::NoModifier, Qt::Key_0 + c - '0',
            new InsertChar(c)
        );
    }

    static const QVector<QPair<char, int>> symb_keys = {
        { ' ', Qt::Key_Space },
        { '\n', Qt::Key_Return },
        { '`', Qt::Key_QuoteLeft },
        { '-', Qt::Key_Minus },
        { '=', Qt::Key_Equal },
        { '[', Qt::Key_BracketLeft },
        { ']', Qt::Key_BracketRight },
        { '\\', Qt::Key_Backslash },
        { ';', Qt::Key_Semicolon },
        { ',', Qt::Key_Comma },
        { '.', Qt::Key_Period },
        { '\'', Qt::Key_Apostrophe },
        { '/', Qt::Key_Slash }
    };
    for (auto &entry : symb_keys) {
        insert.addBinding(
            Qt::NoModifier, entry.second,
            new InsertChar(entry.first)
        );
    }

    static const QVector<QPair<char, int>> shift_keys = {
        { '~', Qt::Key_AsciiTilde },
        { '!', Qt::Key_Exclam },
        { '@', Qt::Key_At },
        { '#', Qt::Key_NumberSign },
        { '$', Qt::Key_Dollar },
        { '%', Qt::Key_Percent },
        { '^', Qt::Key_AsciiCircum },
        { '&', Qt::Key_Ampersand },
        { '*', Qt::Key_Asterisk },
        { '(', Qt::Key_ParenLeft },
        { ')', Qt::Key_ParenRight },
        { '_', Qt::Key_Underscore },
        { '+', Qt::Key_Plus },
        { '{', Qt::Key_BraceLeft },
        { '}', Qt::Key_BraceRight },
        { '|', Qt::Key_Bar },
        { ':', Qt::Key_Colon },
        { '"', Qt::Key_QuoteDbl },
        { '<', Qt::Key_Less },
        { '>', Qt::Key_Greater },
        { '?', Qt::Key_Question }
    };
    for (auto &entry : shift_keys) {
        insert.addBinding(
            Qt::ShiftModifier, entry.second,
            new InsertChar(entry.first)
        );
    }

    insert.addBinding(
        Qt::NoModifier, Qt::Key_Escape,
        new SetMode("normal")
    );

    insert.addBinding(
        Qt::NoModifier, Qt::Key_Backspace,
        new Backspace()
    );
}

QMap<QString, Mode*> Mode::all_modes;

Mode Mode::normal("normal");
Mode Mode::insert("insert");
