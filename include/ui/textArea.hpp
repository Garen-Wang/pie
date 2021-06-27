#pragma once

#include "libs.hpp"
#include "config.hpp"
#include "modes.hpp"
#include "actions.hpp"
#include "rope.hpp"

#include <builtin.h>

/* The text area class is the widget that display content
 * of a file */
class TextArea : public QWidget, public ActionVisitor {
Q_OBJECT
public:
    struct Selection {
        Interval interval;
        /* offset relative to [interval.begin] */
        int cursor;
    };

public:
    /* Open a file */
    TextArea(QWidget *parent, const QString &file);
    /* Using an existing rope */
    TextArea(QWidget *parent, Rope *content);

    /* return file name of this text area */
    const QString &file() const;

    /* save the opened file */
    void save() const;
    /* save to an alternative file, with name [file] */
    void save(const QString &file) const;

public:
    /* Implementation of methods in [ActionVisitor].
     * See [include/ui/actions.hpp] for more details. */
    Interval getObject(PrimitveObject) override;
    void insertChar(QChar c) override;
    void setSelection(Interval) override;
    void expandSelection(Interval) override;
    void delInterval(Interval) override;
    void pushCount(int) override;
    void undo() override;
    void redo() override;
    void setMode(const QString &) override;


private:
    /* resize the text area by number of characters. */
    void resizeByChar(int n_row, int n_col);

    /* re-run the parser on current content to update.
     * Should be called when file content changes */
    void reparse();

    /* setup pen of QPainter to make it draw characters
     * using the specified style. */
    static void setup_pen(QPainter &, const Style &);

    /* overriden event handlers from QWidget */
    void keyPressEvent(QKeyEvent *) override;
    void paintEvent(QPaintEvent *) override;

private:
    /* file name. [""] if none is specified */
    QString m_file;
    /* Content of the text area. Owned by the TextArea object */
    Rope *m_content;

    /* Current selection.
     * Several invariants should be guaranteed:
     * 1. [m_sel.interval.end - m_sel.interval.begin >= 1]
     * 2. [m_sel.interval.begin + m_sel.cursor < m_sel.interval.end]
     * 3. [m_sel.cursor > 0]
     * 4. [0 <= m_sel.interval.begin <  m_content.length()]
     * 5. [0 <  m_sel.interval.end   <= m_content.length()] */
    Selection m_sel;

    /* Information about current font */
    QFontMetrics *m_fm;
    /* The size of a single-width character under current font */
    QSize m_cell_size;

    /* current mode. See [include/ui/modes.hpp] for more details */
    Mode *m_mode;
    /* current count,
     * i.e. how many times the next command should be repeated */
    int m_count;

    /* If the file type of the text area is supported,
     * [m_parser] should point to a syntax highlight parser for the file type.
     * Otherwise, [m_parser] should be set to [nullptr].
     * See [include/shl/builtin.h] for more details */
    shl::Parser *m_parser;
};
