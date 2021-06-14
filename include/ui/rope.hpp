#ifndef ROPE_H
#define ROPE_H

#include "libs.hpp"
#include "config.hpp"



using Attr = Style;

class Node;
class Leaf;
class Branch;
class Rope;
class RopeIter;


struct Interval {
	int begin, end;

	void ins(int pos, int len);
	void del(int pos, int len);
	int length() const;
};


struct Buffer {
    QString characters;
};


struct BufferSlice {
    const Buffer *buf;
    int begin;
    int len;
    Attr attr;
};




class Node {
public: 
    /* shallow free */
    virtual ~Node() = default;

    /* deep free. Also deletes [this] */
    virtual void free() = 0;

	int lines() const;
	int firstRow() const;
	int lastRow() const;
	int maxCol() const;
	int size() const;
	int length() const;

	virtual Node *insert(const Buffer *, int pos, int len, int k, Attr attr) = 0;
	virtual Node *remove(int pos, int len) = 0;
	virtual Node *setAttr(int pos, int len, Attr attr) = 0;

    virtual int seek_pos(int row, int col) const = 0;
    virtual QPair<int, int> get_pos(int idx) const = 0;
	virtual void query(QList<BufferSlice> &collector, int pos, int len) const = 0;
	RopeIter iterAt(int pos);

private:
	virtual RopeIter iterAt(int pos, QList<Node*> *left, QList<Node*> *right) = 0;

protected:
	int m_lines;
	int m_first_row;
	int m_last_row;
	int m_max_col;
	int m_size;
	int m_length;

friend class Leaf;
friend class Branch;
friend class RopeIter;
};  


class Leaf : public Node {
public:
    Leaf();
	Leaf(const Buffer *buf, int begin, int len, Attr attr);
    ~Leaf() = default;
    void free() override;

	Node *insert(const Buffer *, int pos, int len, int k, Attr attr) override;
	Node *remove(int pos, int len) override;
	Node *setAttr(int pos, int len, Attr attr) override;

    int seek_pos(int row, int col) const override;
    QPair<int, int> get_pos(int idx) const override;
	void query(QList<BufferSlice> &collector, int pos, int len) const override;

private:
	RopeIter iterAt(int pos, QList<Node*> *left, QList<Node*> *right) override;
	void updateRowInfo();

private:
	const Buffer *m_buf;
	int m_begin;
	Attr m_attr; 
};


class Branch : public Node {
public:
	Branch(Node *l, Node *r);
	~Branch() = default;
    void free() override;

	Node *insert(const Buffer *, int pos, int len, int k, Attr attr) override;
	Node *remove(int pos, int len) override;
	Node *setAttr(int pos, int len, Attr attr) override;

    int seek_pos(int row, int col) const override;
    QPair<int, int> get_pos(int idx) const override;
	void query(QList<BufferSlice> &collector, int pos, int len) const override;

	void maintain();

private:
	RopeIter iterAt(int pos, QList<Node*> *left, QList<Node*> *right) override;
	void pushup(); 

private:
	Node *m_left, *m_right;
};



class RopeIter {
private:
    RopeIter(QList<Node*> *left, BufferSlice slice, int cursor, QList<Node*> *right);
    RopeIter(const RopeIter &) = delete;

public:
    RopeIter(RopeIter &&);
    RopeIter &operator=(RopeIter &&);
    ~RopeIter();

    bool hasPrev() const;
    bool hasNext() const;

    QPair<QChar, Attr> get() const;

    void prev();
    void next();

private:
    QList<Node*> *m_left, *m_right;
    BufferSlice m_slice;
    int m_cursor;

friend class Leaf;
friend class Branch;
};



struct RopeOperation {
    enum { Insert, Remove } kind;
    int pos, len;
    QList<BufferSlice> text;

    RopeOperation inverse() const;
};


class Rope {
public:
	Rope();
	~Rope();

	int length() const;
	int lines() const;
	int maxCol() const;

	void insert(
    	const Buffer *,
    	int pos, int len,
    	int k, Attr attr,
    	bool forget = false
	);
	void insert(const QString &, int k, Attr attr);
	void insert(QChar c, int pos, Attr attr);

	void remove(int pos, int len, bool forget = false);

	void setAttr(int pos, int len, Attr attr);

    int seek_pos(int row, int col) const;
    QPair<int, int> get_pos(int idx) const;

    RopeIter iterAt(int pos);
	QList<BufferSlice> query(int pos, int len) const;

	void changeAttr(Attr attr, int begin, int end);

	void maintainInterval(Interval *);
	void forgetInterval(Interval *);

	void undo();
	void redo();
	void save_operation();

private:
    void playOperation(const RopeOperation &op);

private:
    static const int temp_buffer_size = 32;

private:
    Node *m_root;

    Buffer *m_temp_buffer;
    int m_temp_buffer_len;

    QList<QList<RopeOperation>> m_undo;
    QList<QList<RopeOperation>> m_redo;
    QList<RopeOperation> m_pending;

    QList<Interval*> m_intervals;
};

#endif 
