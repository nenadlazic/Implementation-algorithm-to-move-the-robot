#include "ui_controller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
        a.setApplicationName("BUG algorithm");
        MainWindow w;
        w.show();

        return a.exec();
}
