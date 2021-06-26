#pragma once

#include "libs.hpp"
#include "rope.hpp"

enum PrimitveObject {
    Selection,
    NextH, PrevH,
    NextV, PrevV,
    NextW, PrevW,
    CurLine,
    Line,
    LineBeg, LineEnd,
    Match
};

class ActionVisitor {
public:
    virtual Interval getObject(PrimitveObject) = 0;

    virtual void insertChar(QChar c) = 0;
    virtual void setSelection(Interval) = 0;
    virtual void expandSelection(Interval) = 0;
    virtual void delInterval(Interval) = 0;

    virtual void pushCount(int digit) = 0;

    virtual void undo() = 0;
    virtual void redo() = 0;

    virtual void setMode(const QString &) = 0;
};


class Action {
public:
    virtual void accept(ActionVisitor *) = 0;
    virtual ~Action() {};
};


class ModifySelection: public Action {
public:
    ModifySelection(bool expand, PrimitveObject);
    ~ModifySelection() = default;

    void accept(ActionVisitor *) override;

private:
    bool m_expand;
    PrimitveObject m_obj;
};


class InsertChar : public Action {
public:
    InsertChar(QChar c);
    ~InsertChar() = default;
    void accept(ActionVisitor *) override;

private:
    QChar m_char;
};

class Backspace : public Action {
public:
    void accept(ActionVisitor *) override;
};

class InsertNewline : public Action {
public:
    void accept(ActionVisitor *) override;
};

class DelSelection : public Action {
public:
    void accept(ActionVisitor *) override;
};

class ReplaceSelection : public Action {
public:
    void accept(ActionVisitor *) override;
};

class PushCountChar : public Action {
public:
    PushCountChar(int);
    void accept(ActionVisitor *) override;

private:
    int m_count;
};

class Undo : public Action {
public:
    void accept(ActionVisitor *) override;
};

class Redo : public Action {
public:
    void accept(ActionVisitor *) override;
};

class SetMode : public Action {
public:
    SetMode(QString);
    ~SetMode() = default;
    void accept(ActionVisitor *) override;

private:
    const QString m_mode;
};
