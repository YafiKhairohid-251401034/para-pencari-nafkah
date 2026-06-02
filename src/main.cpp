#include <QApplication>
#include <QFont>
#include "mainwindow.h"
#include "theme.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Application metadata
    app.setApplicationName("Brew n Bites");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Brew n Bites Cafe");

    // Global font
    QFont defaultFont(Theme::FONT_BODY, Theme::FONT_SIZE_BASE);
    app.setFont(defaultFont);

    // Global application style — smooth scrolling, no focus rect
    app.setStyleSheet(R"(
        * {
            outline: none;
        }
        QScrollArea {
            border: none;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 6px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #DDD5CA;
            border-radius: 3px;
            min-height: 30px;
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical,
        QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical {
            background: none;
            height: 0;
        }
        QScrollBar:horizontal {
            height: 0;
        }
    )");

    MainWindow window;
    window.show();

    return app.exec();
}
