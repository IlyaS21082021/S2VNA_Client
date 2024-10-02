#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    /////////////
    std::vector <QString> vv;
    vv.resize(0);
    QStringList dataList;
    dataList.push_back("123");
    dataList.push_back("456");
    dataList.push_back("789");
    dataList.push_back("777");
    auto i = dataList.begin();
    while (i < dataList.end())
    {
        vv.emplace_back(*i);
        vv.emplace_back(*(i+1));
        i += 2;
    }
    ///////////
    w.show();
    return a.exec();
}
