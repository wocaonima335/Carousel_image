#include <QApplication>
#include "Elliptical_background.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EllipseCornerCard w;
    w.show();
    return a.exec();
}
