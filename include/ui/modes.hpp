#pragma once

#include "libs.hpp"
#include "actions.hpp"

class Mode {
public:
    Mode(QString name);
    ~Mode();

    const QString &name();

    void addBinding(Qt::KeyboardModifiers mods, int key, Action *);
    void acceptKeyEvent(QKeyEvent *, ActionVisitor *);

public:
    static Mode *findMode(const QString &);
    static void initDefaultModes();
    static Mode normal;
    static Mode insert;

private:
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
