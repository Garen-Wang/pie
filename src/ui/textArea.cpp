#include "libs.hpp"
#include "textArea.hpp"


TextArea::TextArea(QWidget *parent, const QString &file) : QWidget(parent) {
    m_fm = new QFontMetrics(fontMetrics());
    m_cell_size = QSize {
        m_fm->horizontalAdvance('W'),
        m_fm->height()
    };

    m_mode = &Mode::normal;
    m_count = 0;

    this->setFocusPolicy(Qt::StrongFocus);

    m_file = file;

    m_content = new Rope();

    QFile f(m_file);
    if (f.open(QFile::ReadOnly)) {
        Buffer *buf = new Buffer { std::move(f.readAll()) };
        int len = buf->characters.size();
        for (int i = 0, j = 0; i < len; ++i) {
            if (i == len - 1 || buf->characters[i] == '\n') {
                m_content->insert(buf, j, i + 1 - j, j, Style::default_style, true);
                j = i + 1;
            }
        }
    }
    f.close();
    this->resizeByChar(m_content->lines() + 1, m_content->maxCol() + 1);

    m_sel = { { 0, 1 }, 0 };
    m_content->maintainInterval(&(m_sel.interval));
}

TextArea::TextArea(QWidget *parent, Rope *content) : QWidget(parent) {
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


Interval TextArea::getObject(PrimitveObject obj) {
    Interval i = m_sel.interval;
    switch (obj) {
        case PrimitveObject::Selection:
            break;
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
            if (pos == end - 1) {
                i.begin = end - 2;
                i.end = end - 1;
            }
            break;
        }

        case CurLine:
        {
            auto pos1 = m_content->get_pos(i.begin);
            auto pos2 = m_content->get_pos(i.end - 1);
            i.begin = m_content->seek_pos(pos1.first, 0);
            i.end = m_content->seek_pos(pos2.first + 1, 0);
            break;
        }

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

            QChar complement = '\0';
            bool search_backward = false;
            while (it.hasNext()) {
                QChar c = it.get().first;
                complement
                    = c == '(' ? ')'
                    : c == '[' ? ']'
                    : c == '{' ? '}'
                    : '\0';
                if (complement != '\0') {
                    search_backward = false;
                    break;
                }
                complement
                    = c == ')' ? '('
                    : c == ']' ? '['
                    : c == '}' ? '{'
                    : '\0';
                if (complement != '\0') {
                    search_backward = true;
                    break;
                }

                it.next();
                ++pos;
            }

            // search backward
            if (complement == '\0')
                break;

            if (search_backward) {
                int end = pos + 1;
                while (it.hasPrev()) {
                    it.prev();
                    --pos;
                    if (it.get().first == complement) {
                        i.begin = pos;
                        i.end = end;
                        break;
                    }
                }
            }
            else {
                int begin = pos;
                while (it.hasNext()) {
                    it.next();
                    ++pos;
                    if (it.get().first == complement) {
                        i.begin = begin;
                        i.end = pos + 1;
                        break;
                    }
                }
            }
            break;
        }
    }

    i.begin = qMax(0, qMin(i.begin, m_content->length() - 1));
    i.end = qMax(1, qMin(i.end, m_content->length()));
    return i;
}


void TextArea::insertChar(QChar c) {
    m_sel.interval.begin = m_sel.interval.end - 1;
    m_content->insert(c, m_sel.interval.end - 1, Style::default_style);
    this->resizeByChar(m_content->lines() + 1, m_content->maxCol() + 1);
    this->repaint();
}

void TextArea::setSelection(Interval i) {
    int cursor = m_sel.interval.begin + m_sel.cursor;
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
    i.begin = qMin(i.begin, m_sel.interval.begin);
    i.end = qMax(i.end, m_sel.interval.end);
    this->setSelection(i);
}

void TextArea::delInterval(Interval i) {
    int len = i.end - i.begin;

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
    m_content->save_operation();
    this->resizeByChar(m_content->lines() + 1, m_content->maxCol() + 1);
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




void TextArea::keyPressEvent(QKeyEvent *ev) {
    this->m_mode->acceptKeyEvent(ev, this);
}


void TextArea::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.fillRect(event->rect(), Style::default_style.bg);

    QPen pen = p.pen();
    pen.setWidth(m_fm->lineWidth());
    int underline_offset     = m_fm->ascent() + m_fm->underlinePos();
    int strikethrough_offset = m_fm->ascent() - m_fm->strikeOutPos();

    QRect rect = event->rect();
    int row = qFloor((double)rect.y() / m_cell_size.height());
    int row_e = qCeil((double)(rect.y() + rect.height()) / m_cell_size.height());

    int x = 0;
    int y = row * m_cell_size.height();

    int pos_s = m_content->seek_pos(row, 0);
    int pos_e = m_content->seek_pos(row_e + 1, 0);

    int pos = pos_s;
    int cursor = m_sel.interval.begin + m_sel.cursor;

    RopeIter it = std::move(m_content->iterAt(pos));
    while (it.hasNext() && pos < pos_e) {
        auto entry = it.get();
        QChar c = entry.first;
        Style style = entry.second;
        it.next();

        if (pos == cursor)
            style = Style::cursor_style;
        else if (m_sel.interval.begin <= pos && pos < m_sel.interval.end)
            style = Style::selection_style;

        ++pos;
        if (c == '\n') {
            p.fillRect(QRect {
                x, y,
                m_cell_size.width(),
                m_cell_size.height()
            }, style.bg);
            ++row;
            y += m_cell_size.height();
            x = 0;
            continue;
        }

        this->setup_pen(p, style);
        int cell_width = m_cell_size.width();
        if (m_fm->horizontalAdvance(c) > m_cell_size.width())
            cell_width *= 2;

        QRect rect(
            x, y,
            cell_width,
            m_cell_size.height()
        );

        p.fillRect(rect, style.bg);

        if (style.underline) {
            p.drawLine(
                x, y + underline_offset,
                x + cell_width, y + underline_offset
            );
        }

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

    p.setFont(ft);
}
