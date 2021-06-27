#pragma once

#include "libs.hpp"
#include "actions.hpp"

/* An object of [Mode] represents a text editing mode
 * that has a series of key bindings */
class Mode {
public:
    Mode(QString name);
    ~Mode();

    const QString &name();

    /* Add a new key binding to a mode */
    void addBinding(Qt::KeyboardModifiers mods, int key, Action *);
    /* Accept a [QKeyEvent], and executes corresponding actions
     * on the visitor if the event matches any of the mode's key bindings */
    void acceptKeyEvent(QKeyEvent *, ActionVisitor *);

public:
    /* Find a mode (globally) by its name.
     * Return [nullptr] if not found */
    static Mode *findMode(const QString &);
    /* Init default modes and their key bindings */
    static void initDefaultModes();
    /* Builtin "normal" mode */
    static Mode normal;
    /* Builtin "insert" mode */
    static Mode insert;

private:
    /* All defined modes */
    static QMap<QString, Mode*> all_modes;

private:
    const QString m_name;

    struct Binding {
        Qt::KeyboardModifiers mods;
        int key;
        Action *action;
    };
    QVector<Binding> m_bindings;
};
