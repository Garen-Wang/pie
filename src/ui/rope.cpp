#include "rope.hpp"

void Interval::ins(int pos, int len) {
	if(pos <= begin) {
		begin += len;
		end += len;
	} else if (pos < end) {
		end += len;
	}
}

void Interval::del(int pos, int len) {
    /*                    [begin, end)
     * [pos, pos + len) */
	if (pos + len <= begin) {
		begin -= len;
		end -= len;
		return;
	}

    /* [begin, end)
     *               [pos, pos + len) */
	if (pos >= end)
    	return;

    /* [begin, end)
     *    [pos, pos + len) */
	if (pos >= begin) {
    	end = qMax(begin, end - len);
    	return;
	}

    /*    [begin, end)
     * [pos, pos + len) */
    /* In this case, we guarantee the new [begin] equals [pos] */
    int deleted_len = pos + len - begin;
    begin = pos;
    end = begin + qMax(0, end - begin - deleted_len);
}

int Interval::length() const {
    return end - begin;
}


int Node::lines() const {
    return m_lines;
}

int Node::firstRow() const {
    return m_first_row;
}

int Node::lastRow() const {
    return m_last_row;
}

int Node::maxCol() const {
    return m_max_col;
}

int Node::size() const {
    return m_size;
}


int Node::length() const {
    return m_length;
}


RopeIter Node::iterAt(int pos) {
    return std::move(this->iterAt(pos, new QList<Node*>, new QList<Node*>));
}




Leaf::Leaf() {
    m_buf = nullptr;
    m_length = 0;
}

Leaf::Leaf(const Buffer *buf, int begin, int len, Attr attr) {
    m_buf = nullptr;
    m_length = 0;
    this->insert(buf, begin, len, 0, attr);
}

void Leaf::free() {
    delete this;
}



Node *Leaf::insert(const Buffer *buf, int pos, int len, int k, Attr attr) {
    int buf_lines = buf->characters[pos + len - 1] == '\n' ? 1 : 0;

    if (m_length == 0) {
        m_buf = buf;

        m_size = 1;
    	m_length = len;

    	m_begin = pos;
    	m_attr = attr;

    	this->updateRowInfo();

    	return this;
    }

    /* relative position between the position to insert and this,
     * -1: insert position on the left of this
     *  1: insert position on the right of this
     *  0: insert position in the middle of this */
    int relative_pos =
        k == 0
        ? -1
        : k == m_length
        ? 1 : 0;

    /* try to merge into one leaf, if possible */
    if (buf == m_buf && m_lines == 0 && attr == m_attr) {
        if (pos + len == m_begin && relative_pos == -1) {
            m_begin = pos;
            m_length += len;
            m_lines += buf_lines;
            m_last_row += m_lines > 0 ? len : 0;
            return this;
        }
        else if (m_begin + m_length == pos && relative_pos == 1) {
            m_length += len;
            m_lines += buf_lines;
            m_last_row += m_lines > 0 ? len : 0;
            return this;
        }
    }

    Leaf *new_leaf = new Leaf(buf, pos, len, attr);

    /* cases where there's no need to split */
    if (relative_pos == -1)
        return new Branch(new_leaf, this);
    else if (relative_pos == 1)
        return new Branch(this, new_leaf);

    /* inserting in the middle, must split current leaf */
    Leaf *left = new Leaf(m_buf, m_begin, k, m_attr);
    Leaf *right = new Leaf(m_buf, m_begin + k, m_length - k, m_attr);
    delete this;
    Branch *result = new Branch(left, new Branch(new_leaf, right));

    return result;
}


Node *Leaf::remove(int pos, int len) {
    if (pos == 0) {
        m_begin += len;
        m_length = qMax(0, m_length - len);
        this->updateRowInfo();
        return this;
    }
    else if (pos + len >= m_length) {
        m_length = pos;
        this->updateRowInfo();
        return this;
    }
    else {
        Leaf *left = new Leaf(m_buf, m_begin, pos, m_attr);
        Leaf *right = new Leaf(m_buf, pos + len, m_length - pos - len, m_attr);
        delete this;
        return new Branch(left, right);
    }
};

Node *Leaf::setAttr(int pos, int len, Attr attr) {
    if (attr == m_attr)
        return this;

    if (pos == 0 && len >= m_length) {
        m_attr = attr;
        return this;
    }

    if (pos == 0) {
        Leaf *new_leaf = new Leaf(m_buf, m_begin, len, attr);
        m_begin += len;
        m_length -= len;
        this->updateRowInfo();
        return new Branch(new_leaf, this);
    }

    if (pos + len >= m_length) {
        Leaf *new_leaf = new Leaf(m_buf, m_begin + pos, m_length - pos, attr);
        m_length = pos;
        this->updateRowInfo();
        return new Branch(this, new_leaf);
    }

    Leaf *left = new Leaf(m_buf, m_begin, pos, m_attr);
    Leaf *middle = new Leaf(m_buf, m_begin + pos, len, attr);
    Leaf *right = new Leaf(m_buf, m_begin + pos + len, m_length - pos - len, m_attr);
    delete this;

    Branch *result = new Branch(left, new Branch(middle, right));
    return result;
}



int Leaf::seek_pos(int row, int col) const {
    if (row > 0)
        return m_length;

    int last_pos = m_lines > 0 ? m_length - 2 : m_length - 1;
    return qMax(0, qMin(col, last_pos));
}

QPair<int, int> Leaf::get_pos(int idx) const {
    return { 0, qMin(m_length, idx) };
}

void Leaf::query(QList<BufferSlice> &collector, int pos, int len) const {
    int begin = m_begin + pos;
    len = qMin(len, m_length - pos);
    if (len > 0)
        collector.push_back({ m_buf, begin, len, m_attr });
}


RopeIter Leaf::iterAt(int pos, QList<Node*> *left, QList<Node*> *right) {
    BufferSlice s = {
        .buf = m_buf, .begin = m_begin,
        .len = m_length, .attr = m_attr
    };
    return std::move(RopeIter(left, s, qMin(pos, m_length - 1), right));
}

void Leaf::updateRowInfo() {
    if (m_length > 0 && m_buf->characters[m_begin + m_length - 1] == '\n') {
        m_lines = 1;
        m_first_row = m_last_row = m_length - 1;
    }
    else {
        m_lines = 0;
        m_first_row = -1;
        m_last_row = -1;
    }
    m_max_col = qMax(0, m_lines > 0 ? m_length - 2 : m_length - 1);
}



Branch::Branch(Node *l, Node *r) {
	m_left  = l;
	m_right = r;	
	this->pushup();
	this->maintain();
}

void Branch::free() {
    m_left->free();
    m_right->free();
    delete this;
}


void Branch::pushup() {
	m_size = m_left->size() + m_right->size();
	m_length = m_left->length() + m_right->length();

	m_lines = m_left->lines() + m_right->lines();
	int lfr = m_left->firstRow();
	int rfr = m_right->firstRow();
	if (lfr == -1)
    	m_first_row = rfr == -1 ? -1 : rfr + m_left->length();
	else
    	m_first_row = lfr;

	int llr = m_left->lastRow();
	int rlr = m_right->lastRow();
	if (rlr == -1)
    	m_last_row = llr == -1 ? -1 : llr;
	else
    	m_last_row = m_left->length() + rlr;

	m_max_col = qMax(m_left->maxCol(), m_right->maxCol());
	int mid_col_len
    	= (rfr == -1 ? m_length : m_left->length() + rfr)
    	- llr - 1;
    m_max_col = qMax(m_max_col, mid_col_len);
}

void Branch::maintain() {
	static const int BALANCE_FACTOR = 3;

	if(m_left->size() > m_right->size() * BALANCE_FACTOR) {
		auto orig_left = static_cast<Branch*>(m_left);
		Node *ll = orig_left->m_left;
		Node *lr = orig_left->m_right;
		delete orig_left;

		m_left = ll;
		m_right = new Branch(lr, m_right);
	}
	else if(m_left->size() * BALANCE_FACTOR < m_right->size()) {
		auto orig_right = static_cast<Branch*>(m_right);
		Node *rl = orig_right->m_left;
		Node *rr = orig_right->m_right;
		delete orig_right;

		m_left = new Branch(m_left, rl);
		m_right = rr;
	}
}

Node *Branch::insert(const Buffer *buf, int pos, int len, int k, Attr attr) {
    int left_len = m_left->length();

    if (k <= left_len)
        m_left = m_left->insert(buf, pos, len, k, attr);
    else
        m_right = m_right->insert(buf, pos, len, k - left_len, attr);

    this->pushup();
    this->maintain();
    return this;
}

Node *Branch::remove(int pos, int len) {
    int left_len = m_left->length();
    if (pos < left_len) {
        m_left = m_left->remove(pos, qMin(len, left_len - pos));
    }
    if (pos + len > left_len) {
        int start = qMax(0, pos - left_len);
        m_right = m_right->remove(start, pos + len - start - left_len);
    }

    if (m_left->length() == 0) {
        delete m_left;
        auto r = m_right;
        delete this;
        return r;
    }
    else if (m_right->length() == 0) {
        delete m_right;
        auto l = m_left;
        delete this;
        return l;
    }
    else {
        this->pushup();
        return this;
    }
}

Node *Branch::setAttr(int pos, int len, Attr attr) {
    int left_len = m_left->length();
    if (pos < left_len) {
        m_left = m_left->setAttr(pos, qMin(len, left_len - pos), attr);
    }
    if (pos + len >= left_len) {
        int new_pos = qMax(0, pos - left_len);
        m_right = m_right->setAttr(new_pos, pos + len - new_pos - left_len, attr);
    }
    this->pushup();
    return this;
}


int Branch::seek_pos(int row, int col) const {
    if (row < m_left->lines())
        return m_left->seek_pos(row, col);

    if (row > m_left->lines()) {
        int ridx = m_right->seek_pos(row - m_left->lines(), col);
        return ridx + m_left->length();
    }

    int row_s = m_left->lastRow() + 1;
    int row_e = m_right->firstRow();
    row_e = row_e == -1 ? m_length : m_left->length() + row_e;
    return row_s + qMax(0, qMin(col, row_e - row_s - 1));
}

QPair<int, int> Branch::get_pos(int idx) const {
    int left_len = m_left->length();

    if (idx < left_len)
        return m_left->get_pos(idx);

    auto rpos = m_right->get_pos(idx - left_len);
    if (rpos.first == 0) {
        int row_s = m_left->lastRow() + 1;
        rpos.second = idx - row_s;
    }
    rpos.first += m_left->lines();
    return rpos;
}

void Branch::query(QList<BufferSlice> &collector, int pos, int len) const {
    int left_len = m_left->length();
    if (pos < left_len) {
        m_left->query(collector, pos, qMin(len, left_len - pos));
    }
    if (pos + len > left_len) {
        m_right->query(collector, qMax(pos - left_len, 0), pos + len - left_len);
    }
}


RopeIter Branch::iterAt(int pos, QList<Node*> *left, QList<Node*> *right) {
    if (pos < m_left->length()) {
        right->push_front(m_right);
        return std::move(m_left->iterAt(pos, left, right));
    }
    else {
        left->push_front(m_left);
        return std::move(m_right->iterAt(pos - m_left->length(), left, right));
    }
}



RopeIter::RopeIter(QList<Node*> *left, BufferSlice slice, int cursor, QList<Node*> *right) {
    m_left = left;
    m_right = right;
    m_slice = slice;
    m_cursor = cursor;
}

RopeIter::RopeIter(RopeIter &&other) {
    *this = std::move(other);
}

RopeIter::~RopeIter() {
    delete m_left;
    delete m_right;
}


RopeIter &RopeIter::operator=(RopeIter &&other) {
    m_left = other.m_left;
    m_right = other.m_right;
    m_slice = other.m_slice;
    m_cursor = other.m_cursor;
    other.m_left = other.m_right = nullptr;
    return *this;
}


bool RopeIter::hasPrev() const {
    return m_cursor >= 0 || ! m_left->isEmpty();
}

bool RopeIter::hasNext() const {
    return m_cursor <= m_slice.len - 1 || ! m_right->isEmpty();
}


QPair<QChar, Attr> RopeIter::get() const {
    return {
        m_slice.buf->characters[m_slice.begin + m_cursor],
        m_slice.attr
    };
}


void RopeIter::prev() {
    if (m_cursor > 0) {
        --m_cursor;
        return;
    }

    if (m_left->isEmpty())
        return;

    Node *node = m_left->front();
    m_left->pop_front();

    *this = node->iterAt(node->length() - 1, m_left, m_right);
}

void RopeIter::next() {
    if (m_cursor < m_slice.len - 1) {
        ++m_cursor;
        return;
    }

    if (m_right->isEmpty())
        return;

    Node *node = m_right->front();
    m_right->pop_front();

    *this = node->iterAt(0, m_left, m_right);
}



RopeOperation RopeOperation::inverse() const {
    return {
        .kind = kind == Insert ? Remove : Insert,
        .pos = pos, .len = len, .text = text
    };
}



Rope::Rope() {
	m_root = new Leaf();
	m_temp_buffer = new Buffer { QString(temp_buffer_size, '\0') };
	m_temp_buffer_len = 0;
}

Rope::~Rope() {
    m_root->free();
}


int Rope::length() const {
    return m_root->length();
}

int Rope::lines() const {
    return m_root->lines();
}

int Rope::maxCol() const {
    return m_root->maxCol();
}



void Rope::insert(
    const Buffer *buf, int pos, int len,
    int k, Attr attr, bool forget
) {
    for (Interval *i : m_intervals)
        i->ins(pos, len);

    m_root = m_root->insert(buf, pos, len, k, attr);
    if (! forget) {
        m_pending.push_front({
            .kind = RopeOperation::Insert,
            .pos = k, .len = len,
            .text = { { buf, pos, len } }
        });
    }
}

void Rope::insert(const QString &str, int k, Attr attr) {
    const Buffer *buf = new Buffer { str };
    this->insert(buf, 0, str.size(), k, attr);
}

void Rope::insert(QChar c, int pos, Attr attr) {
    if (m_temp_buffer_len == temp_buffer_size) {
        m_temp_buffer = new Buffer { QString(temp_buffer_size, '\0') };
        m_temp_buffer_len = 0;
    }
    m_temp_buffer->characters[m_temp_buffer_len] = c;
    this->insert(m_temp_buffer, m_temp_buffer_len, 1, pos, attr);
    ++m_temp_buffer_len;
}

void Rope::remove(int pos, int len, bool forget) {
    for (Interval *i : m_intervals)
        i->del(pos, len);

    if (! forget) {
        auto text = this->query(pos, len);
        m_pending.push_front({
            .kind = RopeOperation::Remove,
            .pos = pos, .len = len,
            .text = std::move(text)
        });
    }
    m_root = m_root->remove(pos, len);
}

void Rope::setAttr(int pos, int len, Attr attr) {
    m_root = m_root->setAttr(pos, len, attr);
}



int Rope::seek_pos(int row, int col) const {
    return m_root->seek_pos(row, col);
}

QPair<int, int> Rope::get_pos(int idx) const {
    return m_root->get_pos(idx);
}

RopeIter Rope::iterAt(int pos) {
    return std::move(m_root->iterAt(pos));
}

QList<BufferSlice> Rope::query(int pos, int len) const {
    QList<BufferSlice> result;
    m_root->query(result, pos, len);
    return result;
}



void Rope::maintainInterval(Interval *i) {
    m_intervals.push_back(i);
}

void Rope::forgetInterval(Interval *i) {
    for (auto it = m_intervals.begin(); it != m_intervals.end(); ++it)
        if (*it == i)
            it = m_intervals.erase(it);
}


void Rope::undo() {
    if (! m_pending.isEmpty())
        this->save_operation();

    if (! m_undo.isEmpty()) {
        auto ops = m_undo.front();
        m_undo.pop_front();
        for (auto &op : ops)
            this->playOperation(op.inverse());
        m_redo.push_front(ops);
    }
}

void Rope::redo() {
    if (! m_pending.isEmpty())
        this->save_operation();

    if (! m_redo.isEmpty()) {
        auto ops = m_redo.front();
        m_redo.pop_front();
        for (auto &op : ops)
            this->playOperation(op);
        m_undo.push_front(ops);
    }
}

void Rope::save_operation() {
    if (m_pending.isEmpty())
        return;

    for (auto ops : m_redo)
        m_undo.push_front(ops);

    while (! m_redo.isEmpty()) {
        auto ops = m_redo.front();
        m_redo.pop_front();
        for (auto &op : ops)
            op = op.inverse();
        m_undo.push_front(ops);
    }

    m_undo.push_front(std::move(m_pending));
    m_pending.clear();
}


void Rope::playOperation(const RopeOperation &op) {
    if (op.kind == RopeOperation::Insert) {
        int pos = op.pos;
        for (auto &slice : op.text) {
            this->insert(
                slice.buf, slice.begin, slice.len,
                pos, slice.attr, true
            );
            pos += slice.len;
        }
    }
    else if (op.kind == RopeOperation::Remove) {
        this->remove(op.pos, op.len, true);
    }
}
