#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "tgraph.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <qvalidator.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TGraph graph;
    QPushButton qb;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAddr_Port_triggered();
    void on_pbMeasure_clicked();
    void on_pbConnect_clicked();
    void on_leFreq1_editingFinished();
    void on_leFreq2_editingFinished();
    void on_lePoints_editingFinished();
    void on_leIFBW_editingFinished();
    void on_cbSParamChrct_currentIndexChanged(int index);
    void on_cbSParams_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    QString addr;
    unsigned int port;
    int sock;
    struct sockaddr_in saddr = {0};
    QString strFreq1;
    QString strFreq2;
    QString strPoints;
    QString strBandWidth;
    QString graphSParam;
    QString graphSParamChrct;
    unsigned int numberPoints;
    std::vector<dataPoint> dataPoints;
    bool connected = false;
    //char *databuf;
    std::vector <char> databuf;
    char tempDataBuf[1000];
    QDoubleValidator freqValCheck;
    QIntValidator pointsValCheck;
    QIntValidator IFBWValCheck;

    QString AssembleCmd();
    int receiveBuf();
    void parseData();
    bool checkParams();
};
#endif // MAINWINDOW_H
