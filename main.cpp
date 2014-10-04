#include <QtGui/QApplication>
#include "Hamatuchi.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Hamatuchi hamatuchi;
    hamatuchi.show();
    return app.exec();
}
