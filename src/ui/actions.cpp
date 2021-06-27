#include "actions.hpp"


/* ModifySelection */
ModifySelection::ModifySelection(bool expand, PrimitveObject obj) {
    m_expand = expand;
    m_obj = obj;
}

void ModifySelection::accept(ActionVisitor *visitor) {
    Interval i = visitor->getObject(m_obj);
    if (m_expand)
        visitor->expandSelection(i);
    else
        visitor->setSelection(i);
}


/* InsertChar */
InsertChar::InsertChar(QChar c) : m_char(c) {}

void InsertChar::accept(ActionVisitor *visitor) {
    visitor->insertChar(m_char);
}


/* Backspace */
void Backspace::accept(ActionVisitor *visitor) {
    auto i = visitor->getObject(PrevH);
    visitor->delInterval(i);
}


/* InsertNewline */
void InsertNewline::accept(ActionVisitor *visitor) {
    auto i = visitor->getObject(LineEnd);
    visitor->setSelection(i);
    visitor->insertChar('\n');
    visitor->setMode("insert");
}


/* DelSelection */
void DelSelection::accept(ActionVisitor *visitor) {
    visitor->delInterval(visitor->getObject(Selection));
}


/* PushCountChar */
PushCountChar::PushCountChar(int c) : m_count(c) {}

void PushCountChar::accept(ActionVisitor *visitor) {
    visitor->pushCount(m_count);
}


/* ReplaceSelection */
void ReplaceSelection::accept(ActionVisitor *visitor) {
    visitor->delInterval(visitor->getObject(Selection));
    visitor->setMode("insert");
}


/* Undo */
void Undo::accept(ActionVisitor *visitor) {
    visitor->undo();
}


/* Redo */
void Redo::accept(ActionVisitor *visitor) {
    visitor->redo();
}


/* SetMode */
SetMode::SetMode(QString mode) : m_mode(mode) {}

void SetMode::accept(ActionVisitor *visitor) {
    visitor->setMode(m_mode);
}
