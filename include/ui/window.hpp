#pragma once

#include "libs.hpp"
#include "textArea.hpp"

class Window : public QMainWindow {
Q_OBJECT
public:
    explicit Window(QWidget *parent);

    void addTextArea(TextArea *, const QString &);

private:
    TextArea *currentTextArea();

signals:
    void exit();

private slots:
    void openFileMenu(bool);
    void doOpenFile(const QString &file);

    void saveFileMenu(bool);
    void saveAsMenu(bool);
    void doSaveFile(const QString &file);

    void exitMenu(bool);
    void undo(bool);
    void redo(bool);

private:
    QMenu *m_file_menu, *m_edit_menu;
    QAction *m_file_save_act, *m_file_saveas_act, *m_file_open_act, *m_exit_act;
    QAction *m_edit_undo_act, *m_edit_redo_act;
};
