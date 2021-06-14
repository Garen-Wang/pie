#include "libs.hpp"
#include "window.hpp"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    auto tabs = new QTabWidget(this);
    this->setCentralWidget(tabs);

    m_file_menu = this->menuBar()->addMenu("File");

    m_file_open_act = m_file_menu->addAction("Open");
    m_file_open_act->setShortcut(QKeySequence("Ctrl+O"));
    QObject::connect(
        m_file_open_act, SIGNAL (triggered(bool)),
        this, SLOT (openFileMenu(bool))
    );

    m_file_save_act = m_file_menu->addAction("Save");
    m_file_save_act->setShortcut(QKeySequence("Ctrl+S"));
    QObject::connect(
        m_file_save_act, SIGNAL (triggered(bool)),
        this, SLOT (saveFileMenu(bool))
    );

    m_file_saveas_act = m_file_menu->addAction("Save As");
    m_file_saveas_act->setShortcut(QKeySequence("Ctrl+Shift+S"));
    QObject::connect(
        m_file_saveas_act, SIGNAL (triggered(bool)),
        this, SLOT (saveAsMenu(bool))
    );

    m_exit_act = m_file_menu->addAction("Exit");
    m_exit_act->setShortcut(QKeySequence("Ctrl+Shift+Q"));
    QObject::connect(
        m_exit_act, SIGNAL (triggered(bool)),
        this, SLOT (exitMenu(bool))
    );

    m_edit_menu = this->menuBar()->addMenu("Edit");

    m_edit_undo_act = m_edit_menu->addAction("Undo");
    m_edit_undo_act->setShortcut(QKeySequence("Ctrl+z"));
    QObject::connect(
        m_edit_undo_act, SIGNAL (triggered(bool)),
        this, SLOT (undo(bool))
    );

    m_edit_redo_act = m_edit_menu->addAction("Redo");
    m_edit_redo_act->setShortcut(QKeySequence("Ctrl+y"));
    QObject::connect(
        m_edit_redo_act, SIGNAL (triggered(bool)),
        this, SLOT (redo(bool))
    );

    this->setFocusPolicy(Qt::StrongFocus);
}


void Window::addTextArea(TextArea *ta, const QString &label) {
    auto sa = new QScrollArea(this);
    sa->setWidget(ta);

    sa->setAutoFillBackground(true);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window , Style::default_style.bg);
    sa->setPalette(pal);

    auto tabs = (QTabWidget*)(this->centralWidget());
    tabs->insertTab(tabs->currentIndex() + 1, sa, label);
    tabs->setCurrentIndex(tabs->currentIndex() + 1);
    ta->setFocus();
}

TextArea *Window::currentTextArea() {
    auto tabs = static_cast<QTabWidget*>(this->centralWidget());
    if (! tabs)
        return nullptr;

    auto scroll = static_cast<QScrollArea*>(tabs->currentWidget());
    if (! scroll)
        return nullptr;

    auto text_area = static_cast<TextArea*>(scroll->widget());
    return text_area;
}



void Window::openFileMenu(bool) {
    QFileDialog *dialog = new QFileDialog(this);
    QObject::connect(
        dialog, SIGNAL (fileSelected(const QString&)),
        this, SLOT (doOpenFile(const QString&))
    );
    dialog->open();
}

void Window::doOpenFile(const QString &filename) {
    if (filename == "")
        return;

    TextArea *ta = new TextArea(nullptr, filename);
    this->addTextArea(ta, ta->file());
    ta->repaint();
}



void Window::saveFileMenu(bool) {
    this->doSaveFile("");
}

void Window::saveAsMenu(bool) {
    this->doSaveFile(QFileDialog::getSaveFileName());
}

void Window::doSaveFile(const QString &file) {
    auto text_area = currentTextArea();
    if (! text_area)
        return;

    if (file == "")
        text_area->save();
    else
        text_area->save(file);
}


void Window::exitMenu(bool) {
    emit exit();
}


void Window::undo(bool) {
    auto text_area = currentTextArea();
    if (text_area)
        text_area->undo();
}

void Window::redo(bool) {
    auto text_area = currentTextArea();
    if (text_area)
        text_area->redo();
}
