#pragma once

#include "libs.hpp"
#include "rope.hpp"

/* primitive text objects that indicate some part of certain text */
enum PrimitveObject {
    /* current selected text */
    Selection,
    /* NextH(PrevH) the char on the right(left) of current cursor */
    NextH, PrevH,
    /* NextV(PrevV) the char on the bottom(top) of current cursor */
    NextV, PrevV,
    /* NextW(PrevW) the word on the right(left) of current cursor */
    NextW, PrevW,
    /* The whole line(s) containing current selection */
    CurLine,
    /* Current line, or the next one if current one is already
     * fully selected */
    Line,
    /* Begin/End of the line containing current cursor */
    LineBeg, LineEnd,
    /* The region between a char and its matching one,
     * for example '(' and ')', '[' and ']' */
    Match
};

/* An object of some derived class of [ActionVisitor] is something
 * capable to perform all allowed actions */
class ActionVisitor {
public:
    /* Return the interval corresponding to the text object */
    virtual Interval getObject(PrimitveObject) = 0;

    /* insert a character before current cursor position */
    virtual void insertChar(QChar c) = 0;
    /* set the selection to be the given interval */
    virtual void setSelection(Interval) = 0;
    /* expand the selection so that it contains the given interval */
    virtual void expandSelection(Interval) = 0;
    /* delete an interval from the text */
    virtual void delInterval(Interval) = 0;

    virtual void pushCount(int digit) = 0;

    virtual void undo() = 0;
    virtual void redo() = 0;

    virtual void setMode(const QString &) = 0;
};


/* An abstract base class for all possible actions.
 * Using the Visitor Model to implement */
class Action {
public:
    virtual void accept(ActionVisitor *) = 0;
    virtual ~Action() = default;
};


/* The action that modifies current selection */
class ModifySelection: public Action {
public:
    ModifySelection(bool expand, PrimitveObject);
    ~ModifySelection() = default;

    void accept(ActionVisitor *) override;

private:
    bool m_expand;
    PrimitveObject m_obj;
};

/* The action that insert a single char */
class InsertChar : public Action {
public:
    InsertChar(QChar c);
    ~InsertChar() = default;
    void accept(ActionVisitor *) override;

private:
    QChar m_char;
};

/* The action that deletes the char before current cursor */
class Backspace : public Action {
public:
    void accept(ActionVisitor *) override;
};

/* The action that inserts a newline at the end of current line,
 * and enter "insert" mode */
class InsertNewline : public Action {
public:
    void accept(ActionVisitor *) override;
};

/* The action that deletes current selection */
class DelSelection : public Action {
public:
    void accept(ActionVisitor *) override;
};

/* The action that deletes current selection and enters insert mode */
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

/* The action that undo the last operation */
class Undo : public Action {
public:
    void accept(ActionVisitor *) override;
};

/* The action that redo the last undone operation */
class Redo : public Action {
public:
    void accept(ActionVisitor *) override;
};

/* The action that switch current mode */
class SetMode : public Action {
public:
    SetMode(QString);
    ~SetMode() = default;
    void accept(ActionVisitor *) override;

private:
    const QString m_mode;
};
