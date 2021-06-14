#pragma once

#include "libs.hpp"
#include "config.hpp"
#include "modes.hpp"
#include "actions.hpp"
#include "rope.hpp"

#include <builtin.h>

class TextArea : public QWidget, public ActionVisitor {
Q_OBJECT
public:
    struct Selection {
        Interval interval;
        /* offset relative to [interval.begin] */
        int cursor;
    };

public:
    TextArea(QWidget *parent, const QString &file);
    TextArea(QWidget *parent, Rope *content);

    const QString &file() const;

    void save() const;
    void save(const QString &file) const;

    void resizeByChar(int n_row, int n_col);

public:
    Interval getObject(PrimitveObject) override;
    void insertChar(QChar c) override;
    void setSelection(Interval) override;
    void expandSelection(Interval) override;
    void delInterval(Interval) override;
    void pushCount(int) override;
    void undo() override;
    void redo() override;
    void setMode(const QString &) override;

    void setFont(const QFont &);


private:
    void reparse();

    static void setup_pen(QPainter &, const Style &);
    void keyPressEvent(QKeyEvent *) override;
    void paintEvent(QPaintEvent *) override;

private:
    QString m_file;
    Rope *m_content;
    Selection m_sel;

    QFontMetrics *m_fm;
    QSize m_cell_size;

    Mode *m_mode;
    int m_count;

    shl::Parser *m_parser;
};
