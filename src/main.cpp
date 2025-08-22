#include "../inc/MainWindow.h"
#include <QApplication>
#include <QScreen>
#include <QShortcut>
#include <QKeySequence>
#include <QCoreApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    // Enable high-DPI scaling before creating the application
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    // Global application font a bit larger for fullscreen readability
    QFont appFont = app.font();
    appFont.setPointSize(12);
    app.setFont(appFont);

    MainWindow w;

    // Force geometry to primary screen and show full screen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect geom = screen->geometry();
        w.setGeometry(geom);
    }

    w.showFullScreen();
    w.raise();
    w.activateWindow();

    // Allow exiting fullscreen with Esc
    QShortcut *esc = new QShortcut(QKeySequence(Qt::Key_Escape), &w);
    QObject::connect(esc, &QShortcut::activated, [&w]() {
        w.showNormal();
    });

    return app.exec();
}
