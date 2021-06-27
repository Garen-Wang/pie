#pragma once

#include "libs.hpp"
#include "textArea.hpp"

/* The class for the main window of pie */
class Window : public QMainWindow {
Q_OBJECT
public:
    explicit Window(QWidget *parent);

    /* Add a new text area, opening the given file */
    void addTextArea(TextArea *, const QString &);

private:
    TextArea *currentTextArea();

signals:
    void exit();

private slots:
    /* "open" menu activated */
    void openFileMenu(bool);
    /* actually open the file */
    void doOpenFile(const QString &file);

    /* "save" menu activated */
    void saveFileMenu(bool);
    /* "save as" menu activated */
    void saveAsMenu(bool);
    /* actually save the file */
    void doSaveFile(const QString &file);

    void exitMenu(bool);

    /* "undo" menu activated */
    void undo(bool);
    /* "redo" menu activated */
    void redo(bool);

private:
    /* All the menus */
    QMenu *m_file_menu, *m_edit_menu;
    QAction *m_file_save_act, *m_file_saveas_act, *m_file_open_act, *m_exit_act;
    QAction *m_edit_undo_act, *m_edit_redo_act;
};
