#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "connection.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    graph.setParent(this);
    ui->verticalLayout_3->addWidget(&graph);
    graph.show();
    graph.graphData = &dataPoints;
    freqValCheck.setParent(this);
    freqValCheck.setRange(1, 999999999999);
    freqValCheck.setNotation(QDoubleValidator::StandardNotation);
    freqValCheck.setDecimals(2);
    ui->leFreq1->setValidator(&freqValCheck);
    ui->leFreq2->setValidator(&freqValCheck);

    pointsValCheck.setParent(this);
    pointsValCheck.setRange(1, 9999);
    ui->lePoints->setValidator(&pointsValCheck);

    IFBWValCheck.setParent(this);
    IFBWValCheck.setRange(1, 999999);
    ui->leIFBW->setValidator(&IFBWValCheck);

    struct timeval timeout;
    timeout.tv_sec = 10;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    int reuseAddr = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));
}

MainWindow::~MainWindow()
{
    delete ui;
    if (connected)
        ::close(sock);
}

QString MainWindow::AssembleCmd()
{
    QString res;

    res.append(strFreq1);
    res.append(strFreq2);
    res.append(strPoints);
    res.append(strBandWidth);
    res.append(graphSParam);
    res.append(graphSParamChrct);
    return res + "\n";
}

int MainWindow::receiveBuf()
{
    std::string getDataCmd;
    size_t receivedBytes = 0;
    size_t bytesInPack;
    int bufIndex = 0;
    char *cpyAddr = databuf.data();

    memset(tempDataBuf, 0, sizeof(tempDataBuf));
    getDataCmd = "CALCulate:DATA:FDATa?\n";
    send(sock, getDataCmd.c_str(), getDataCmd.length(), 0);

    while (receivedBytes < numberPoints*36)
    {
        bytesInPack = recv(sock, tempDataBuf, 1000, 0);
        receivedBytes += bytesInPack;
        if (receivedBytes == 0)
            return 0;

        memcpy(cpyAddr, tempDataBuf, bytesInPack);
        cpyAddr = databuf.data() + receivedBytes;
    }
    return 1;
}

void MainWindow::parseData()
{
    QString stringData(databuf.data());
    stringData.removeLast();
    QStringList dataList = stringData.split(",", Qt::SkipEmptyParts);
    auto i = dataList.begin();
    while (i < dataList.end())
    {
        dataPoints.push_back(dataPoint({(*i).toDouble(), (*(i+1)).toDouble()}));
        i += 2;
    }
}

bool MainWindow::checkParams()
{
    if (ui->lePoints->text().isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Points field is empty", QMessageBox::Ok);
        return false;
    }

    if (ui->leFreq1->text().isEmpty() || ui->leFreq2->text().isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Frequency has not been set", QMessageBox::Ok);
        return false;
    }

    if (ui->leFreq1->text().toDouble() > ui->leFreq2->text().toDouble())
    {
        QMessageBox::warning(this, "Warning", "Frequency1 > Frequency2", QMessageBox::Ok);
        return false;
    }

    if (graphSParamChrct.isEmpty() || graphSParam.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Measured values have not been set", QMessageBox::Ok);
        return false;
    }

    if (numberPoints == 0)
    {
        QMessageBox::warning(this, "Warning", "Number of points is zero", QMessageBox::Ok);
        return false;
    }
    return true;
}

void MainWindow::on_pbMeasure_clicked()
{
    QString cmd;
    QString systemCmd;
    char opcBuf[8];

    if (checkParams())
        cmd = AssembleCmd(); //set params for measures
    else
        return;

    dataPoints.clear();

    //set system params
    systemCmd = ":TRIG:SOUR BUS;INIT:CONT ON\n";
    if (send(sock, systemCmd.toStdString().c_str(), systemCmd.length(), 0) < systemCmd.length())
    {
        QMessageBox::warning(this, "Warning", "System command has not been sent", QMessageBox::Ok);
        return;
    }

    //send params for measure
    if (send(sock, cmd.toStdString().c_str(), cmd.length(), 0) < cmd.length())
    {
        QMessageBox::warning(this, "Warning", "Params have not been sent", QMessageBox::Ok);
        return;
    }

    //run measures
    systemCmd = ":TRIG:SING\n";
    send(sock, systemCmd.toStdString().c_str(), systemCmd.length(), 0);
    //wait for measure finish
    systemCmd = "*OPC?\n";
    do
    {
        usleep(500000);
        send(sock, systemCmd.toStdString().c_str(), systemCmd.length(), 0);
        if (recv(sock, opcBuf, sizeof(opcBuf), 0) == 0)
        {
            QMessageBox::warning(this, "Warning", "Connection error", QMessageBox::Ok);
            return;
        }
    } while (opcBuf[0] != '1');

    //receive data
    if (!receiveBuf())
    {
        QMessageBox::warning(this, "Warning", "Connection error", QMessageBox::Ok);
        return;
    }

    parseData();
    graph.calcMinMax();
    graph.update();
}


void MainWindow::on_actionAddr_Port_triggered()
{
    auto tc = new TConnection(this);
    if (tc->exec() == QDialog::Accepted)
    {
        addr = tc->GetAddr();
        port = tc->GetPort();
    }
    delete tc;
}

void MainWindow::on_pbConnect_clicked()
{
    if (connected)
    {
        ::close(sock);
        connected = false;
        ui->pbConnect->setText("Connect");
        ui->pbMeasure->setEnabled(false);
        return;
    }

    if (addr.isEmpty())
    {
        on_actionAddr_Port_triggered();
        if (addr.isEmpty())
            return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(addr.toStdString().c_str());
    saddr.sin_port = htons(port);
    if (::connect(sock, (struct sockaddr*)&saddr, sizeof(saddr)) != -1)
    {
        QMessageBox::information(nullptr, "Info", "Connected");
        ui->pbMeasure->setEnabled(true);
        connected = true;
        ui->pbConnect->setText("Disconnect");
    }
    else
    {
        QMessageBox::information(nullptr, "Info", "No connection");
    }
}

void MainWindow::on_leFreq1_editingFinished()
{
    graph.SetFreq(ui->leFreq1->text().toDouble(), ui->leFreq2->text().toDouble());
    strFreq1.clear();
    strFreq1.append(":SENSe:FREQuency:STARt " + ui->leFreq1->text().toStdString() + ";");
}

void MainWindow::on_leFreq2_editingFinished()
{
    graph.SetFreq(ui->leFreq1->text().toDouble(), ui->leFreq2->text().toDouble());
    strFreq2.clear();
    strFreq2.append(":SENSe:FREQuency:STOP " + ui->leFreq2->text().toStdString() + ";");
}


void MainWindow::on_lePoints_editingFinished()
{
    strPoints.clear();
    strPoints.append(":SENSe:SWEep:POINts " + ui->lePoints->text().toStdString() + ";");
    numberPoints = ui->lePoints->text().toInt();
    dataPoints.clear();
    dataPoints.reserve(numberPoints);
    databuf.resize(numberPoints*36);
}


void MainWindow::on_leIFBW_editingFinished()
{
    strBandWidth.clear();
    strBandWidth.append(":SENSe:BANDwidth " + ui->leIFBW->text().toStdString() + ";");
}

void MainWindow::on_cbSParamChrct_currentIndexChanged(int index)
{
    graphSParamChrct.clear();
    graphSParamChrct.append(":CALCulate:FORMat ");
    switch (index)
    {
    case 0:
        graphSParamChrct.append("MLOGarithmic;");
        graph.setSParamChrctLbl("MLOGarithmic");
        break;
    case 1:
        graphSParamChrct.append("SWR;");
        graph.setSParamChrctLbl("SWR");
        break;
    case 2:
        graphSParamChrct.append("PHASe;");
        graph.setSParamChrctLbl("PHASE");
    }
}


void MainWindow::on_cbSParams_currentIndexChanged(int index)
{
    graphSParam.clear();
    graphSParam.append(":CALCulate:PARameter:DEFine ");
    switch (index)
    {
    case 0:
        graphSParam.append("S11;");
        graph.setSParamLbl("S11");
        break;
    case 1:
        graphSParam.append("S12;");
        graph.setSParamLbl("S12");
        break;
    case 2:
        graphSParam.append("S21;");
        graph.setSParamLbl("S21");
        break;
    case 3:
        graphSParam.append("S22;");
        graph.setSParamLbl("S22");
    }
}
