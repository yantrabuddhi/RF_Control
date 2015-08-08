#include "rf_dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RF_Dialog w;
    w.show();

    return a.exec();
}
