#include "libs.hpp"
#include "textArea.hpp"
#include "pierc.h"

/* This class is a connector class,
 * used to feed the content of a rope to a parser.
 * The interface is that of [peg_parser::Input],
 * i.e. the overriden methods.
 *
 * The class is designed with the following things in mind:
 * 1. zero copy
 * 2. not dependent of the given row.
 *    That is, instances of [RopeParserInput] should still be
 *    valid after the given rope is modified */
class RopeParserInput : public peg_parser::Input {
public:
    explicit RopeParserInput(const Rope *rope);
    ~RopeParserInput() override = default;

    int length() const override;
    bool isAtEnd() const override;

    char current() const override;
    std::string slice(int begin, int len) override;

    void advance(int amount) override;
    int getPosition() const override;
    void setPosition(int) override;

    /* Return a copy of [this], with the same content.
     * The return value is dynamically allocated,
     * and the caller is responsible to free it. */
    Input *copy() override;

private:
    /* Used internally to implement [copy] */
    explicit RopeParserInput(const QList<BufferSlice> &content) : m_content(content) {}

private:
    /* The content of the rope. [Rope] provides zero copy API
     * to obtain this list. */
    const QList<BufferSlice> m_content;
    /* Points to the slice in which the current position lies,
     * or [m_content.constEnd()] if input reaches end. */
    QList<BufferSlice>::const_iterator m_current;
    /* Total length of input */
    int m_len;
    /* Offset from start of [*m_current] to current position */
    int m_offset;
    /* Current position, relative to start of [m_content] */
    int m_pos;
    /* The parser requires random seeking (setPosition(int)),
     * but seeking a random position in the list is an O(n) operation.
     * So here a cache is used to speed up random seeking,
     * especially backward seeking, which is extensively used in the parser.
     *
     * The cache is structured as follows:
     * key:
     * - absolute position relative to content start
     * value = QPair(pos, it):
     * - [pos] is the same as key. Due to limitation of the QMap API
     *   it must also be stored in the value
     * - [*it] is the slice whose start position in whole content
     *   is [pos]
     */
    using Cache = QMap<int, QPair<int, QList<BufferSlice>::const_iterator>>;
    /* The parser requires a [copy] function,
     * and it is more desirable to share the cache between inputs
     * with the same content.
     * So here [m_cache] is wrapped in a shared pointer. */
    std::shared_ptr<Cache> m_cache;
};


TextArea::TextArea(QWidget *parent, const QString &file) : QWidget(parent) {
    setFont(pierc::getFont());
    m_fm = new QFontMetrics(fontMetrics());
    m_cell_size = QSize {
        m_fm->horizontalAdvance('W'),
        m_fm->height()
    };

    m_mode = &Mode::normal;
    m_count = 0;

    /* So that the text area can get keyboard focus */
    this->setFocusPolicy(Qt::StrongFocus);

    m_file = file;

    m_content = new Rope();

    QFile f(m_file);
    if (f.open(QFile::ReadOnly)) {
        /* Thanks to piece table, file content doesn't have to be
         * split apart. This can improve locality.
         * For very large files, perhaps lazy loading and splitting
         * can speed up opening, though. */
        Buffer *buf = new Buffer { std::move(f.readAll()) };
        int len = buf->characters.size();
        /* A scan is necessary here,
         * as [Rope] requires that only the last char of
         * inserted buffer fragment can be newline. */
        for (int i = 0, j = 0; i < len; ++i) {
            if (i == len - 1 || buf->characters[i] == '\n') {
                m_content->insert(buf, j, i + 1 - j, j, Style::default_style, true);
                j = i + 1;
            }
        }
    }
    f.close();
    /* Store a more compact relative path, if possible */
    auto relative = QDir::current().relativeFilePath(m_file);
    if (relative.size() < m_file.size())
        m_file = relative;

    /* Make the text area fit its content */
    this->resizeByChar(m_content->lines() + 1, m_content->maxCol() + 1);

    /* Selection is placed at start of content initially */
    m_sel = { { 0, 1 }, 0 };
    m_content->maintainInterval(&(m_sel.interval));

    m_parser = nullptr;
    if (m_file.endsWith(".cpp")) {
        auto result = shl::generateLanguageParser(shl::LanguageType::CPP, m_content);
        if (result.first) {
            qDebug() << "filetype: Cpp";
            m_parser = new shl::Parser(std::move(result.second));
            /* Do the first parsing */
            this->reparse();
        }
    }
    else if (m_file.endsWith(".java")) {
        auto result = shl::generateLanguageParser(shl::LanguageType::JAVA, m_content);
        if (result.first) {
            qDebug() << "filetype: Java";
            m_parser = new shl::Parser(std::move(result.second));
            this->reparse();
        }
    }
    else if (m_file.endsWith(".py")) {
        auto result = shl::generateLanguageParser(shl::LanguageType::PYTHON, m_content);
        if (result.first) {
            qDebug() << "filetype: Python";
            m_parser = new shl::Parser(std::move(result.second));
            this->reparse();
        }
    }
}

TextArea::TextArea(QWidget *parent, Rope *content) : QWidget(parent) {
    setFont(pierc::getFont());
    m_file = "";
    m_content = content;
    this->resizeByChar(m_content->lines() + 1, m_content->maxCol() + 1);

    m_fm = new QFontMetrics(fontMetrics());
    m_cell_size = QSize {
        m_fm->horizontalAdvance('W'),
        m_fm->height()
    };

    this->setFocusPolicy(Qt::StrongFocus);

    m_sel = { { 0, 1 }, 0 };
    m_content->maintainInterval(&(m_sel.interval));

    m_mode = &Mode::normal;
    m_count = 0;
}


const QString &TextArea::file() const {
    return m_file;
}


void TextArea::resizeByChar(int n_row, int n_col) {
    int w = n_col * m_cell_size.width(); 
    int h = n_row * m_cell_size.height();
    if (w != width() || h != height())
        this->resize(w, h);
}


void TextArea::save() const {
    if (m_file == "")
        return;

    this->save(m_file);
}

void TextArea::save(const QString &file) const {
    QFile f(file);
    if (f.open(QFile::WriteOnly)) {
        for (BufferSlice &slice : m_content->query(0, m_content->length())) {
            f.write(slice.buf->characters.toUtf8().constData() + slice.begin, slice.len);
        }
    }
    f.close();
}


/* -------------------------------------------------
 * Implementation of [ActionVisitor] methods
 * ------------------------------------------------- */

/* Return the interval of a given text object */
Interval TextArea::getObject(PrimitveObject obj) {
    Interval i = m_sel.interval;
    switch (obj) {
        case PrimitveObject::Selection:
            break;
        /* [NextH] and [PrevH] are all single-character
         * text objects */
        case NextH:
            i.begin = i.begin + m_sel.cursor + 1,
            i.end = i.begin + 1;
            break;
        case PrevH:
            i.begin = i.begin + m_sel.cursor - 1,
            i.end = i.begin + 1;
            break;

        case NextV:
        case PrevV:
        {
            int offset = obj == NextV ? 1 : -1;
            auto cursor_pos = m_content->get_pos(i.begin + m_sel.cursor);
            int row = qMax(0, cursor_pos.first + offset);
            /* Try to go to the same column. End of line if not possible */
            i.begin = m_content->seek_pos(row, cursor_pos.second);
            i.end = i.begin + 1;
            break;
        }

        case NextW:
        {
            int pos = m_sel.interval.begin + m_sel.cursor;
            int begin = pos;
            auto it = std::move(m_content->iterAt(pos));
            QChar current = it.get().first;
            bool is_word = current.isLetterOrNumber();
            while (it.hasNext()) {
                ++pos;
                it.next();
                if (it.get().first.isLetterOrNumber() != is_word) {
                    i.begin = begin;
                    i.end = pos;
                    break;
                }
            }
            /* [NextW] should always advance at least one character,
             * so that the user won't get stuck in one place when
             * repeatedly entering [w] in normal mode.
             * Hence if we are not moving at all, move by one char. */
            if (pos == begin + 1) {
                i.begin = pos;
                i.end = pos + 1;
            }
            break;
        }

        case PrevW:
        {
            int pos = m_sel.interval.begin + m_sel.cursor;
            int end = pos + 1;
            auto it = std::move(m_content->iterAt(pos));
            QChar current = it.get().first;
            bool is_word = current.isLetterOrNumber();
            while (it.hasPrev()) {
                it.prev();
                if (it.get().first.isLetterOrNumber() != is_word) {
                    i.begin = pos;
                    i.end = end;
                    break;
                }
                --pos;
            }
            /* Similar to above */
            if (pos == end - 1) {
                i.begin = end - 2;
                i.end = end - 1;
            }
            break;
        }

        case CurLine:
        {
            /* In case current selection expand multiple lines,
             * the start postition and end position are seeked
             * with respect to [m_sel.interval.begin] and
             * [m_sel.interval.end]. */
            auto pos1 = m_content->get_pos(i.begin);
            auto pos2 = m_content->get_pos(i.end - 1);
            i.begin = m_content->seek_pos(pos1.first, 0);
            /* start of next row = position after end of current row. */
            i.end = m_content->seek_pos(pos2.first + 1, 0);
            break;
        }

        /* Select the next line */
        case Line:
        {
            auto cursor_pos = m_content->get_pos(i.begin + m_sel.cursor);
            auto row_start = m_content->seek_pos(cursor_pos.first, 0);
            auto next_row_start = m_content->seek_pos(cursor_pos.first + 1, 0);
            if (next_row_start < m_content->length()
                && row_start >= i.begin && next_row_start <= i.end) {
                i.begin = next_row_start,
                i.end = m_content->seek_pos(cursor_pos.first + 2, 0);
            }
            else {
                i.begin = row_start;
                i.end = next_row_start;
            }
            break;
        }

        case LineBeg:
        {
            auto cursor_pos = m_content->get_pos(i.begin + m_sel.cursor);
            auto row_s = m_content->seek_pos(cursor_pos.first, 0);
            i.begin = row_s;
            i.begin = row_s + 1;
            break;
        }

        case LineEnd:
        {
            auto cursor_pos = m_content->get_pos(i.begin + m_sel.cursor);
            auto next_row_s = m_content->seek_pos(cursor_pos.first + 1, 0);
            i.end = next_row_s;
            i.begin = next_row_s - 1;
            break;
        }

        case Match:
        {
            int pos = i.begin + m_sel.cursor;
            auto it = std::move(m_content->iterAt(pos));

            /* The char we search for whose match */
            QChar paren_char = '\0';
            /* The match of [paren_char] */
            QChar complement = '\0';
            /* Whether searching is backward, i.e. right to left */
            bool search_backward = false;

            /* First search for the first char that has a match */
            while (it.hasNext()) {
                QChar c = it.get().first;
                complement
                    = c == '(' ? ')'
                    : c == '[' ? ']'
                    : c == '{' ? '}'
                    : '\0';
                if (complement != '\0') {
                    paren_char = c;
                    search_backward = false;
                    break;
                }
                complement
                    = c == ')' ? '('
                    : c == ']' ? '['
                    : c == '}' ? '{'
                    : '\0';
                if (complement != '\0') {
                    paren_char = c;
                    search_backward = true;
                    break;
                }

                it.next();
                ++pos;
            }
            if (complement == '\0')
                break;

            /* Now find the match of [paren_char] */

            /* If we come up with [paren_char] again when searching
             * for its match, than the next [complement] should matches
             * the newly encountered [paren_char], instead of the one
             * we are searching for. So we use [recursion_count] to
             * capture the number of extra [paren_char]'es encountered
             * along the way. */
            int recursion_count = 0;

            /* search backward */
            if (search_backward) {
                int end = pos + 1;
                while (it.hasPrev()) {
                    it.prev();
                    --pos;
                    QChar c = it.get().first;
                    if (c == complement) {
                        /* The real match */
                        if (recursion_count == 0) {
                            i.begin = pos;
                            i.end = end;
                            break;
                        }
                        /* The match for some other [paren_char] */
                        else
                            --recursion_count;
                    }
                    else if (c == paren_char)
                        ++recursion_count;
                }
            }
            else {
                int begin = pos;
                while (it.hasNext()) {
                    it.next();
                    ++pos;
                    QChar c = it.get().first;
                    if (c == complement) {
                        if (recursion_count == 0) {
                            i.begin = begin;
                            i.end = pos + 1;
                            break;
                        }
                        else
                            --recursion_count;
                    }
                    else if (c == paren_char)
                        ++recursion_count;
                }
            }
            break;
        }
    }

    /* Make sure the resulting interval fits into current content */
    i.begin = qMax(0, qMin(i.begin, m_content->length() - 1));
    i.end = qMax(1, qMin(i.end, m_content->length()));
    return i;
}


void TextArea::insertChar(QChar c) {
    m_sel.interval.begin = m_sel.interval.end - 1;
    m_content->insert(c, m_sel.interval.end - 1, Style::default_style);
    this->resizeByChar(m_content->lines() + 1, m_content->maxCol() + 1);
    this->reparse();
    this->repaint();
}

void TextArea::setSelection(Interval i) {
    int cursor = m_sel.interval.begin + m_sel.cursor;
    /* Update cursor position based on the following heuristics:
     * 1. if selection extends to the right compared to previous one,
     *    set cursor to the right of selection
     * 2. otherwise, if selection extends to the left, set cursor to
     *    be the left of selection
     * 3. otherwise, toggle cursor position */
    if (cursor < i.end - 1)
        m_sel.cursor = i.end - i.begin - 1;
    else if (i.begin < cursor)
        m_sel.cursor = 0;
    else if (cursor == m_sel.interval.end - 1)
        m_sel.cursor = 0;
    else
        m_sel.cursor = i.end - i.begin - 1;
    m_sel.interval = i;
    this->repaint();
}


void TextArea::expandSelection(Interval i) {
    int cursor = m_sel.interval.begin + m_sel.cursor;
    /* The union of current selection and the interval to extend */
    i.begin = qMin(i.begin, m_sel.interval.begin);
    i.end = qMax(i.end, m_sel.interval.end);
    this->setSelection(i);
}

void TextArea::delInterval(Interval i) {
    int len = i.end - i.begin;

    /* Don't delete all of content and make it empty,
     * leave a newline anyway */
    if (len >= m_content->length()) {
        if (len == 1)
            return;

        m_content->remove(i.begin, len);
        m_content->insert('\n', 0, Style::default_style);
        m_sel.interval = Interval { 0, 1 };
    }
    else
        m_content->remove(i.begin, len);

    if (m_sel.interval.begin == m_sel.interval.end) {
        if (m_sel.interval.begin >= m_content->length())
            --(m_sel.interval.begin);
        m_sel.interval.end = m_sel.interval.begin + 1;
    }
    /* Deletion is a complete operation w.r.t. the undo and redo
     * system. So save current (unsaved) operations here. */
    m_content->save_operation();
    this->resizeByChar(m_content->lines() + 1, m_content->maxCol() + 1);
    this->reparse();
    this->setSelection(m_sel.interval);
}

void TextArea::pushCount(int count) {
    m_count = m_count * 10 + count;
}


void TextArea::undo() {
    m_content->undo();
    this->repaint();
}

void TextArea::redo() {
    m_content->redo();
    this->repaint();
}


void TextArea::setMode(const QString &mode_name) {
    Mode *mode = Mode::findMode(mode_name);
    if (mode != nullptr) {
        m_mode = mode;
        m_content->save_operation();
    }
}





void TextArea::reparse() {
    if (m_parser == nullptr)
        return;
    peg_parser::Input *input = new RopeParserInput(m_content);
    try {
        m_parser->run(input);
    } catch (peg_parser::SyntaxError &) {
        qDebug() << "Syntax Error";
    }
    delete input;
}

void TextArea::keyPressEvent(QKeyEvent *ev) {
    this->m_mode->acceptKeyEvent(ev, this);
}


void TextArea::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    /* fill background */
    p.fillRect(event->rect(), Style::default_style.bg);

    QPen pen = p.pen();
    /* so that underline and strikethrough are drawn with proper width */
    pen.setWidth(m_fm->lineWidth());

    /* offset from "ascent" position of characters
     * to the place to draw underline */
    int underline_offset     = m_fm->ascent() + m_fm->underlinePos();
    /* similar */
    int strikethrough_offset = m_fm->ascent() - m_fm->strikeOutPos();

    /* The rectangle to repaint.
     * Here to improve performance, only the portion necessary
     * to repaint are repainted. */
    QRect rect = event->rect();
    /* Due to the existence of double width characters (i.e. CJK ones),
     * content must be drawn in a per-line basis.
     * So only find out the first row and last row covered by [rect] */
    int row = qFloor((double)rect.y() / m_cell_size.height());
    int row_e = qCeil((double)(rect.y() + rect.height()) / m_cell_size.height());

    /* Current position to draw things, w.r.t. [this] */
    int x = 0;
    int y = row * m_cell_size.height();

    /* start and end index of characters that should be drawn
     * in [m_content] */
    int pos_s = m_content->seek_pos(row, 0);
    int pos_e = m_content->seek_pos(row_e + 1, 0);

    int pos = pos_s;
    /* cursor position */
    int cursor = m_sel.interval.begin + m_sel.cursor;

    RopeIter it = std::move(m_content->iterAt(pos));
    while (it.hasNext() && pos < pos_e) {
        auto entry = it.get();
        QChar c = entry.first;
        Style style = entry.second;
        it.next();

        /* Draw cursor and selection, no matter how they
         * are highlighted by the parser */
        if (pos == cursor)
            style = Style::cursor_style;
        else if (m_sel.interval.begin <= pos && pos < m_sel.interval.end)
            style = Style::selection_style;

        ++pos;
        if (c == '\n') {
            /* Draw something blank at the place of newline */
            p.fillRect(QRect {
                x, y,
                m_cell_size.width(),
                m_cell_size.height()
            }, style.bg);
            ++row;
            /* Advance drawing position to start ot next row */
            y += m_cell_size.height();
            x = 0;
            continue;
        }

        this->setup_pen(p, style);
        int cell_width = m_cell_size.width();
        /* Determine whether the character is double-width,
         * directly using its size in the font */
        if (m_fm->horizontalAdvance(c) > m_cell_size.width())
            cell_width *= 2;

        /* Rectangle of current character cell */
        QRect rect(
            x, y,
            cell_width,
            m_cell_size.height()
        );

        /* draw background */
        p.fillRect(rect, style.bg);

        /* Draw underline. Builtin underline and strikethrough mechanism
         * of Qt is error-prone with a composited font,
         * e.g. a font with ASCII characters and CJK characters coming
         * from different fonts */
        if (style.underline) {
            p.drawLine(
                x, y + underline_offset,
                x + cell_width, y + underline_offset
            );
        }
        /* Draw strikethrough */
        if (style.strikethrough) {
            p.drawLine(
                x, y + strikethrough_offset,
                x + cell_width, y + strikethrough_offset
            );
        }
        p.drawText(rect, Qt::AlignCenter, c);

        x += cell_width;
    }
}


void TextArea::setup_pen(QPainter &p, const Style &style) {
    QPen pen = p.pen();
    if (pen.color() != style.fg) {
        pen.setColor(style.fg);
        p.setPen(pen);
    }

    QFont ft = p.font();
    if (ft.bold() != style.bold)
        ft.setBold(style.bold);
    else if (ft.italic() != style.italic)
        ft.setItalic(style.italic);
    else
        return;
}


/* ------------------------------------------------
 *    Connector between Rope and parser
 * ------------------------------------------------ */

RopeParserInput::RopeParserInput(const Rope *rope)
/* copy constructor of [QList] is COW (Copy On Write),
 * so no copy would actually happen here */
: m_content(rope->query(0, rope->length()))
{
    m_len = rope->length();
    m_current = m_content.constBegin();
    m_offset = 0;
    m_pos = 0;
    m_cache = std::make_shared<Cache>();
    (*m_cache)[0] = { 0, m_current };
}


int RopeParserInput::length() const { return m_len; }

bool RopeParserInput::isAtEnd() const {
    return m_current == m_content.constEnd();
}


char RopeParserInput::current() const {
    char c;
    if (m_current == m_content.constEnd())
        c = '\0';
    else
        c = m_current->buf->characters[m_current->begin + m_offset].toLatin1();
    return c;
}

/* Return the substring [begin, begin + len) of the whole content */
std::string RopeParserInput::slice(int begin, int len) {
    /* Here we use [setPosition] to seek [begin],
     * however [slice] should not affect the state of [this],
     * so the current state is saved priorly and recovered later. */
    auto current_it = m_current;
    int current_offset = m_offset;
    int current_pos = m_pos;
    this->setPosition(begin);

    int copied = 0;
    /* Initialize result buffer */
    std::string result(len, '\0');
    /* Copy characters into [result] until [len] characters in copied */
    while (copied < len) {
        for (int i = 0; i < qMin(m_current->len - m_offset, len - copied); ++i)
            result[copied + i] = m_current->buf->characters[m_current->begin + m_offset + i].toLatin1();
        copied += m_current->len;
        ++m_current;
        m_offset = 0;
    }

    /* Recover state */
    m_current = current_it;
    m_offset = current_offset;
    m_pos = current_pos;

    return std::move(result);
}



void RopeParserInput::advance(int amount) {
    if (m_current == m_content.constEnd())
        return;

    /* Advancement can be done in current slice */
    if (m_offset + amount < m_current->len) {
        m_offset += amount;
        m_pos += amount;
        return;
    }
    /* Cannot be done in current slice.
     * Move to next slice, and advance recursively */
    amount = m_offset + amount - m_current->len;
    m_pos = m_pos + m_current->len - m_offset;
    m_offset = 0;
    ++m_current;
    /* Cache on each advancement incrementally */
    (*m_cache)[m_pos] = { m_pos, m_current };
    this->advance(amount);
}


int RopeParserInput::getPosition() const { return m_pos; }

void RopeParserInput::setPosition(int pos) {
    /* [*ub] is the first slice whose start position
     * is greater than [pos] */
    auto ub = m_cache->upperBound(pos);
    /* Now [*ub] is the first slice whose start position
     * is smaller than [pos], i.e. closest to [pos] */
    --ub;
    /* seek to start of [*ub] */
    m_pos = ub->first;
    m_current = ub->second;
    m_offset = 0;
    /* Advance slowly, build up cache along the way */
    this->advance(pos - m_pos);
}


peg_parser::Input *RopeParserInput::copy() {
    RopeParserInput *result = new RopeParserInput(m_content);
    result->m_current = m_current;
    result->m_len = m_len;
    result->m_offset = m_offset;
    result->m_pos = m_pos;
    /* [m_cache] is shared */
    result->m_cache = m_cache;
    return result;
}
