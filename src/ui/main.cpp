
#include "libs.hpp"
#include "window.hpp"

int main(int argc, char **argv) {
    Mode::initDefaultModes();
    QApplication app(argc, argv);

    Window win(NULL);

    QObject::connect(&win, SIGNAL (exit()), &app, SLOT (quit()));

    win.show();
    return app.exec();
}
