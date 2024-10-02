#include "connection.h"
#include "ui_connection.h"

static QString addr = "";
static int port = 0;

TConnection::TConnection(QWidget *parent) : QDialog(parent),
                                            ui(new Ui::TConnection)
{
    ui->setupUi(this);
    ui->leAddr->setText(addr);
    ui->lePort->setText(QString::number(port));
}

TConnection::~TConnection()
{
    delete ui;
}

QString TConnection::GetAddr()
{
    addr = ui->leAddr->text();
    return addr;
}

unsigned int TConnection::GetPort()
{
    port = ui->lePort->text().toInt();
    return port;
}


