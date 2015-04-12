#include <QCoreApplication>

#include <QStringList>

#include "syncer.h"


int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    QString qstr_source("C:\\Test");
    QString qstr_dest("F:\\Test");

    Syncer SyncTest(qstr_source, qstr_dest, QStringList("~conficts"),QStringList("~*"));

    //return a.exec();
}
